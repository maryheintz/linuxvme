/******************************************************************************
*
* Filename: 	vme_driver.c
* 
* Description:	VME device driver for Linux.
*
* $Revision: 1.2 $
*
* $Date: 2005/08/17 10:20:05 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme/vme_driver.c,v $
*
* Copyright 2000-2005 Concurrent Technologies.
*
******************************************************************************/
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/config.h>
#include <linux/module.h>

#ifndef MODULE
#include <linux/init.h>
#define INIT_FUNC 		static int init_vmedriver
#define CLEANUP_FUNC 	static void cleanup_vmedriver
#else
#define INIT_FUNC 		int init_module
#define CLEANUP_FUNC 	void cleanup_module
#endif

#ifndef LINUX_VERSION_CODE
#include <linux/version.h>
#endif

#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <asm/uaccess.h>

#include "vme_types.h"
#include "vme_driver.h"
#include "vme_os.h"
#include "vme_api.h"
#include "vme_proc.h"


#define VME_DRIVER
#include "vme_version.h"

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,10))
MODULE_LICENSE("Proprietary");
#endif

extern int umem_init( int mem_size );
extern void umem_cleanup( void );

extern int unv_init( int vecBufSize, int pciAddr, int pciSize );
extern void unv_cleanup( void );
extern int unv_readRegs( long currPos, UINT8 *buf, unsigned int count );
extern int unv_writeRegs( long currPos, const UINT8 *buf, unsigned int count );
extern int unv_readImage( UINT32 minorNum, long currPos, UINT8 *buf, unsigned int length );
extern int unv_writeImage( UINT32 minorNum, long currPos, const UINT8 *buf, unsigned int length );
extern int unv_readDma( long currPos, UINT8 *buf, unsigned int length );
extern int unv_writeDma( long currPos, const UINT8 *buf, unsigned int length );
extern long unv_seek( UINT32 minorNum, long currPos, unsigned int offset, int whence );
extern int unv_ioctlPciImage( UINT32 minorNum, UINT32 cmd, UINT32 arg );
extern int unv_ioctlVmeImage( UINT32 minorNum, UINT32 cmd, UINT32 arg);
extern int unv_ioctlDma( UINT32 cmd, UINT32 arg );
extern int unv_ioctl( UINT32 cmd, UINT32 arg );
extern int unv_mmapImage( UINT32 minorNum, void *pVma, UINT32 size, UINT32 offset );
extern int unv_mmapDma( void *pVma, UINT32 size, UINT32 offset );
extern void unv_closePciImage( UINT32 minorNum );
extern void unv_closeVmeImage( UINT32 minorNum );
extern void unv_closeDma( void );
extern void unv_closeCtl( void );

extern int getVmeDriverLibInfo( char *buffer );

#ifdef USE_CONSOLE
extern void os_dbgOpenConsole( void );
void os_dbgCloseConsole( void );
#endif

static int vmeMajor = 0; /* VME_DRIVER_MAJOR; 0 = use dynamic allocation */
static const char *vmeName	= "Linux VME device driver";


static loff_t vmedrv_llseek( struct file *file, loff_t offset, int whence );
static ssize_t vmedrv_read( struct file *file, char *buf, size_t count, 
							loff_t *ppos );
static ssize_t vmedrv_write( struct file *file, const char *buf, 
							size_t count, loff_t *ppos );
static int vmedrv_ioctl( struct inode *inode, struct file *file,
						unsigned int cmd, unsigned long arg );
static int vmedrv_mmap( struct file *file, struct vm_area_struct *vma );
static int vmedrv_open( struct inode *inode, struct file *file );
static int vmedrv_close( struct inode *inode, struct file *file );

static void vmedrv_vmaClose( struct vm_area_struct *vma );

/* device methods declared using tagged format */
/* note: GNU compiler specific */ 
static struct file_operations vme_fops = 
{
	llseek:		vmedrv_llseek,
	read:		vmedrv_read,
	write:		vmedrv_write,
	ioctl:		vmedrv_ioctl,
	mmap:		vmedrv_mmap,   
	open:		vmedrv_open,
	release:	vmedrv_close
};

struct vm_operations_struct vme_vm_ops = 
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
	close: vmedrv_vmaClose
#else
	.close = vmedrv_vmaClose,
#endif
};

/* device status record */
static VMECTRL vmeCtrl[VME_MAX_MINORS];

char *boardName = NULL;
static int resMemSize = 0;
static int vecBufSize = 32;
static int pciAddr = 0;
static int pciSize = 0;

#ifdef MODULE
/* Module parameters, passed to module at load time */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,5))
MODULE_PARM( boardName, "s" );  /* Can be used to override board type auto detection */

MODULE_PARM( resMemSize, "i" ); /* Can be used to specify the amount of reserved memory in MB's. 
                                   resMemSize values:  = 0  probe for user reserved memory
                                                       > 0  number of MB's of user reserved memory
                                                       < 0  disabled
								*/

MODULE_PARM( vecBufSize, "i" ); /* Can be used to increase the number of VME vectors
								   captured before buffer wraps, range from 32-128.
								   When number is > 32 extended vector capture mode
                                   is used.	
								*/

MODULE_PARM ( pciAddr, "i" );   /* Can be used to specify free PCI space for VME PCI
                                   window allocation.
                                */

MODULE_PARM ( pciSize, "i" );   /* Can be used to specify the total amount of PCI space
                                   to use for VME PCI window allocation.
                                */

#else
module_param( boardName, charp, 0 );
MODULE_PARM_DESC(boardName, "board name");
module_param( resMemSize, int, 0 );
MODULE_PARM_DESC(resMemSize, "specify the amount of reserved memory");
module_param( vecBufSize, int, 0 );
MODULE_PARM_DESC(vecBufSize, "increase the number of VME vectors");
module_param( pciAddr, int, 0 );
MODULE_PARM_DESC(pciAddr, "specify free PCI space start address");
module_param( pciSize, int, 0 );
MODULE_PARM_DESC(pciSize, "specify size of free PCI space");

#endif

#endif


/******************************************************************************
*
* vmedrv_vmaClose
*
* Just keep track of how many times the device is mapped, to avoid releasing it.
* mmap() calls MOD_INC_USE_COUNT, vmedrv_vmaClose() excutes when munmap() called.
* Need to do it like this because driver is unaware of munmap().
*  
*
* RETURNS: None.
*
******************************************************************************/
static void vmedrv_vmaClose( struct vm_area_struct *vma )
{
	VMECTRL *cPtr;
	UINT32 minorNum;


	minorNum = MINOR(vma->vm_file->f_dentry->d_inode->i_rdev);

	cPtr = &vmeCtrl[minorNum];
	cPtr->mmapCount--;

	DBMSG( "vmedrv_vmaClose minorNum: %u mmapCount %d", minorNum, cPtr->mmapCount );

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
	MOD_DEC_USE_COUNT;
#else
	module_put( THIS_MODULE );
#endif
}


/******************************************************************************
*
* vmedrv_read
*
* Read from VME device driver file.
*
*
*  
*
* RETURNS: Number of bytes read if successful else an error code.
*
******************************************************************************/
static ssize_t vmedrv_read( struct file *file, char *buf, size_t count, loff_t *ppos )
{
	UINT32 minorNum;
	ssize_t result;
	char *nPtr;  
	 

	minorNum = MINOR(file->f_dentry->d_inode->i_rdev);
	nPtr = (char *) file->f_dentry->d_iname;

	/* check for invalid device file */
	if (minorNum >= VME_MAX_MINORS) 
	{
		ERRMSG( "attempt to read invalid device %s", nPtr );
		result = (-VME_ENODEV);
	}
	else
	{
		DBMSG( "%s read %u f_pos 0x%X %d bytes", nPtr, minorNum, 
				(UINT32) file->f_pos, (int) count );

		/* check user data buffer address is valid */
		if ( access_ok( VERIFY_READ, (UINT32) buf, (UINT32) count ) == 1 )
		{
			if (minorNum == VME_CONTROL_MINOR) 
			{
				result = unv_readRegs( file->f_pos, buf, (count / 4) );
			}
			else if ( (minorNum > VME_CONTROL_MINOR) && (minorNum <= VME_VSI7_MINOR) ) 
			{
				result = unv_readImage( minorNum, file->f_pos, buf, count );
			}
			else if ( minorNum == VME_DMA_MINOR )
			{
				result = unv_readDma( file->f_pos, buf, count );
			}
			else
			{
				ERRMSG( "%s", "read function not supported" );
				result = (-VME_EPERM);
			}
		}
		else
		{
			ERRMSG( "%s read failed, user buffer invalid", nPtr);
			result = (-VME_EFAULT);
		}
	}

	if ( result > 0 )
	{
		*ppos += result;
	}

	return result;
}


/******************************************************************************
*
* vmedrv_write
*
* Write to VME device driver file.
*
*
*  
*
* RETURNS: Number of bytes written if successful else an error code.
*
******************************************************************************/
static ssize_t vmedrv_write( struct file *file, const char *buf, size_t count, loff_t *ppos )
{
	UINT32 minorNum;
	ssize_t result;  
	char *nPtr;
	 

	minorNum = MINOR(file->f_dentry->d_inode->i_rdev);
	nPtr = (char *) file->f_dentry->d_iname;

	/* check for invalid device file */
	if (minorNum >= VME_MAX_MINORS) 
	{
		ERRMSG( "attempt to write invalid device", nPtr );
		result = (-VME_ENODEV);
	}
	else
	{
		DBMSG( "%s write %u f_pos 0x%X %d bytes", nPtr, minorNum, 
				(UINT32) file->f_pos, (int) count );

		/* check user data buffer address is valid */
		if ( access_ok( VERIFY_WRITE, (UINT32) buf, (UINT32) count ) == 1 )
		{
			if (minorNum == VME_CONTROL_MINOR) 
			{
				result = unv_writeRegs( file->f_pos, buf, (count / 4) );
			}
			else if ( (minorNum > VME_CONTROL_MINOR) && (minorNum <= VME_VSI7_MINOR) ) 
			{
				result = unv_writeImage( minorNum, file->f_pos, buf, count );
			}
			else if ( minorNum == VME_DMA_MINOR )
			{
				result = unv_writeDma( file->f_pos, buf, count );
			}
			else
			{
				ERRMSG( "%s", "write function not supported" );
				result = (-VME_EPERM);
			}
		}
		else
		{
			ERRMSG( "%s write failed, user buffer invalid", nPtr);
			result = (-VME_EFAULT);
		}
	}

	if ( result > 0 )
	{
		*ppos += result;
	}

	return result;
}


/******************************************************************************
*
* vmedrv_ioctl
*
* Perform ioctl operations on a VME device driver file.
*
*
*  
*
* RETURNS: VME_SUCCESS if successful else an error code.
*
******************************************************************************/
static int vmedrv_ioctl( struct inode *inode, struct file *file,
						unsigned int cmd, unsigned long arg )
{
	UINT32 minorNum;
	int result;
	char *nPtr;


	minorNum = MINOR(inode->i_rdev);
	nPtr = (char *) file->f_dentry->d_iname;
  
	/* check for invalid device file */
	if (minorNum >= VME_MAX_MINORS) 
	{
		DBMSG( "attempt to ioctl invalid device %s", nPtr );
		result = (-VME_ENODEV);
	}
	else
	{
		DBMSG( "%s ioctl %u 0x%X", nPtr, cmd, arg );

		if ( cmd == IOCTL_GET_POS )
		{
			/* use checking version, calls access_ok() internally */ 
			if ( put_user( file->f_pos, (int *)arg ) )
			{
				ERRMSG( "%s", "ioctl failed to copy data to user space" );
				result = (-VME_EFAULT);
			}
			else
			{
				result = VME_SUCCESS;
			}
		}
		else
		{
			if ( (minorNum > VME_CONTROL_MINOR) && (minorNum <= VME_LSI7_MINOR) ) 
			{
				result = unv_ioctlPciImage( minorNum, cmd, (UINT32) arg );
			}
			else if ( (minorNum > VME_LSI7_MINOR) && (minorNum <= VME_VSI7_MINOR) )
			{
				result = unv_ioctlVmeImage( minorNum, cmd, (UINT32) arg );
			}
			else if ( minorNum == VME_DMA_MINOR )
			{
				result = unv_ioctlDma( cmd, (UINT32) arg );
			}
			else
			{
				result = unv_ioctl( cmd, (UINT32) arg );
			}
		}
	}
	
	return result;
}


/******************************************************************************
*
* vmedrv_mmap
*
* Maps VME device driver memory to user memory. 
*
*
*  
*
* RETURNS: VME_SUCCESS if successful else an error code.
*
******************************************************************************/
static int vmedrv_mmap( struct file *file, struct vm_area_struct *vma )
{
	int result;
	char *nPtr;
	VMECTRL *cPtr;
	UINT32 size;
	UINT32 minorNum;
	UINT32 offset;


	minorNum = MINOR(file->f_dentry->d_inode->i_rdev);
	nPtr = (char *) file->f_dentry->d_iname;

	/* check for invalid device file */
	if (minorNum >= VME_MAX_MINORS) 
	{
		ERRMSG( "attempt to mmap invalid device", nPtr );
		result = (-VME_ENODEV);
	}
	else
	{
		DBMSG( "%s mmap", nPtr );
		size = vma->vm_end - vma->vm_start;
		cPtr = &vmeCtrl[minorNum];

		offset = vma->vm_pgoff << PAGE_SHIFT;

		/* offset must be page aligned */ 
		if ( ((offset & (PAGE_SIZE-1)) == 0) &&
				((size % PAGE_SIZE) == 0) )  
		{
			/* dont't allow mmap of control device */
			if ( (minorNum > VME_CONTROL_MINOR) && (minorNum <= VME_VSI7_MINOR) ) 
			{
				result = unv_mmapImage( minorNum, (void *) vma, size, offset );
			}
			else if ( minorNum == VME_DMA_MINOR )
			{
				result = unv_mmapDma( (void *) vma, size, offset );
			}
			else
			{
				result = (-VME_EPERM);
			}
		
			/* if successful */
			if ( result == VME_SUCCESS )
			{

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
				vma->vm_file = file;
				vma->vm_ops = &vme_vm_ops;

				cPtr->mmapCount++;

				MOD_INC_USE_COUNT;	 /* done here because open(vma) is not called */

				DBMSG( "%s mmap OK", nPtr );
#else
				if ( try_module_get( THIS_MODULE ) )
				{
					vma->vm_file = file;
					vma->vm_ops = &vme_vm_ops;

					cPtr->mmapCount++;

					DBMSG( "%s mmap OK", nPtr );
				}
				else
				{
					DBMSG( "%s mmap failed", nPtr );
					result = (-VME_EINVAL);
				}
#endif	
			}
			else
			{
				DBMSG( "%s mmap failed", nPtr );
			}
		}
		else
		{
			result = (-VME_EINVAL);
		}
	}
	
	return result;
}


/******************************************************************************
*
* vmedrv_llseek
*
* Seek given postion in VME device driver file.
*
*
*  
*
* RETURNS: position if successful else an error code.
*
******************************************************************************/
static loff_t vmedrv_llseek( struct file *file, loff_t offset, int whence )
{
	UINT32 minorNum;
	loff_t result; 
	char *nPtr; 


	minorNum = MINOR(file->f_dentry->d_inode->i_rdev);
	nPtr = (char *) file->f_dentry->d_iname;

	/* check for invalid device file */
	if (minorNum >= VME_MAX_MINORS) 
	{
		ERRMSG( "attempt to seek invalid device %s", nPtr );
		result = (-VME_ENODEV);
	}
	else
	{
		DBMSG( "%s seek %d offset: 0x%X", 
				nPtr, whence, (UINT32) offset );

		result = unv_seek( minorNum, file->f_pos, offset, whence );
		
		if (  result >= 0  )
		{
			file->f_pos = offset;
		}
		
		DBMSG( "f_pos: 0x%X", (UINT32) file->f_pos );
	}

	return result;
}


/******************************************************************************
*
* vmedrv_open
*
* Opens a VME device driver file.
*
*
*  
*
* RETURNS: VME_SUCCESS if successful else an error code.
*
******************************************************************************/
static int vmedrv_open( struct inode *inode, struct file *file )
{
	UINT32 minorNum;
	VMECTRL *cPtr;
	int result;
	char *nPtr;


	minorNum = MINOR(inode->i_rdev);
	nPtr = (char *) file->f_dentry->d_iname;
  
	/* check for invalid device file */
	if (minorNum >= VME_MAX_MINORS) 
	{
		ERRMSG( "attempt to open invalid device %s", nPtr );
		result = (-VME_ENODEV);
	}
	else
	{
		cPtr = &vmeCtrl[minorNum];

/* define for debug use only */  
#ifdef MULTI_PROCESS
		/* if device already open or not permitted user return busy */
		if ( (cPtr->fileOpen != 0) &&
			(cPtr->owner != current->uid) &&  	/* allow user */
        	(cPtr->owner != current->euid) && 	/* allow whoever did su */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
			!suser() ) 							/* still allow root suser()*/
#else
        	!capable(CAP_SYS_ADMIN))
#endif
#else
		/* only allow single process access */
		/* if device already open return busy */
		if ( cPtr->fileOpen != 0 )
#endif
		{
			ERRMSG( "%s", "device busy" );
			result = (-VME_EBUSY);
  		} 
		else
		{ 
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
    		cPtr->fileOpen++;
			cPtr->owner = current->uid;
			DBMSG( "%s device open successful", nPtr );

			MOD_INC_USE_COUNT;

			result = VME_SUCCESS;
#else
			if ( try_module_get( THIS_MODULE ) )
			{
				cPtr->fileOpen++;
				cPtr->owner = current->uid;
				DBMSG( "%s device open successful", nPtr );

				result = VME_SUCCESS;
			}
			else
			{
				result = (-VME_EINVAL);
			}

#endif
		}
	}
	
	return result;
}


/******************************************************************************
*
* vmedrv_close
*
* Closes a VME device driver file.
*
*
*  
*
* RETURNS: VME_SUCCESS if successful else an error code.
*
******************************************************************************/
static int vmedrv_close( struct inode *inode, struct file *file )
{
  	UINT32 minorNum;
	VMECTRL *cPtr;
	int result;
	char *nPtr;


	minorNum = MINOR(inode->i_rdev);
	nPtr = (char *) file->f_dentry->d_iname;

	/* check for invalid device file */
	if (minorNum >= VME_MAX_MINORS) 
	{
		ERRMSG( "attempt to close invalid device %s", nPtr );
		result = (-VME_ENODEV);
	}
	else
	{
		cPtr = &vmeCtrl[minorNum];

		if ( (cPtr->owner != current->uid) &&  	/* allow user */
        	(cPtr->owner != current->euid) && 	/* allow whoever did su */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
			!suser() ) 							/* still allow root suser()*/
#else
        	!capable(CAP_SYS_ADMIN))
#endif
		{
  			ERRMSG( "%s", "close attempt by invalid user" );
			result = (-VME_EPERM);
  		} 
		else
		{ 
			if ( (minorNum > VME_CONTROL_MINOR) && (minorNum <= VME_LSI7_MINOR) ) 
			{
				unv_closePciImage( minorNum );
			}
			else if ( (minorNum > VME_LSI7_MINOR) && (minorNum <= VME_VSI7_MINOR) )
			{
				unv_closeVmeImage( minorNum );
			}
			else if ( minorNum == VME_DMA_MINOR )
			{
				unv_closeDma();
			}
			else
			{
				unv_closeCtl();
			}

			if ( cPtr->fileOpen > 0 )
			{
  				cPtr->fileOpen--;
			}

			DBMSG( "%s device closed", nPtr );

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
			MOD_DEC_USE_COUNT;
#else
			module_put( THIS_MODULE );
#endif
			result = VME_SUCCESS;
		}
	}

  	return result;
}


/******************************************************************************
*
* vmedrv_getInfo
*
* Copy VME device driver information into given buffer.
*
*
*  
*
* RETURNS: number of bytes copied.
*
******************************************************************************/
int vmedrv_getInfo( char *buffer )
{
	int len = 0;


	if( buffer != NULL )
	{
		len += sprintf( buffer+len, "%s\n", vmeName );
		len += sprintf( buffer+len, "%s %s\n", vmeDriverVersion, creationDate);
		len += sprintf( buffer+len, "%s\n", copyright);
		len += sprintf( buffer+len, "Compiled with Kernel Version %s\n",  kernel_version);
		len += getVmeDriverLibInfo( buffer+len );
	}

	return len;
}


/******************************************************************************
*
* INIT_FUNC
*
* Initialization function.
* Called when module is loaded.
*
*
*  
*
* RETURNS: VME_SUCCESS if successful else an error code.
*
******************************************************************************/
INIT_FUNC( void )
{
	int result;
	char *sPtr;


	memset( vmeCtrl, 0, sizeof( vmeCtrl ) );

#ifdef USE_CONSOLE
	os_dbgOpenConsole();	/* for debug use */
#endif

	sPtr = (char *) kmalloc( 200, GFP_KERNEL );
	vmedrv_getInfo( sPtr );
	MSG( "%s", sPtr );
	kfree( sPtr );

	/* Show module parameters */
	MSG("resMemSize: %d", resMemSize);
	MSG("vecBufSize: %d", vecBufSize);
	MSG("pciAddr   : 0x%x", pciAddr);
	MSG("pciSize   : 0x%x", pciSize);

	/* initialise Universe device */
	result = unv_init( vecBufSize, pciAddr, pciSize );

	if ( result == VME_SUCCESS )
	{
		/* register device with kernel */
		result = register_chrdev( vmeMajor, "vmedriver", &vme_fops );
		if ( result >= 0 ) 
		{
			/* if major number allocated dynamically */
			if ( vmeMajor == 0 )
			{
				vmeMajor = result;
			}

			/* register with proc file system */
			vme_procInit();

			/* see if there is any user reserved memory */
			umem_init( resMemSize );

			MSG( "VME device driver (MAJOR %d) initialised", vmeMajor );

			result = 0;	/* must return zero if successful */
		}
		else
		{
			MSG( "Failed to register VME device driver with kernel" );
			unv_cleanup();
		}
	}
	else
	{
		MSG( "VME device driver failed to initialise" );
	} 

	return result;
}


/******************************************************************************
*
* CLEANUP_FUNC
*
* Cleanup function, free's resources.
* Called when module is unloaded.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
CLEANUP_FUNC( void )
{
	/* tidy up Universe device */
	unv_cleanup();

	/* free user reserved memory */
	umem_cleanup();

	/* unregister device */
	unregister_chrdev( vmeMajor, "vmedriver" );

	/* remove proc file system entries */
	vme_procCleanup();

	MSG( "VME device driver removed" );

#ifdef USE_CONSOLE
	os_dbgCloseConsole();	/* for debug use */
#endif
}

#ifndef MODULE
module_init( init_vmedriver );
module_exit( cleanup_vmedriver );
#endif
