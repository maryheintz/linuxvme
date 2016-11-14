/******************************************************************************
*
* Filename:	 vme_os.c
*
* Description:	OS wrapper functions.
*
* $Revision: 1.7 $
*
* $Date: 2006/08/18 13:58:15 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme/vme_os.c,v $
*
* Copyright 2000-2005 Concurrent Technologies.
*
******************************************************************************/

#ifndef __KERNEL__
#define __KERNEL__
#endif

#define __NO_VERSION__
#include <linux/config.h>
#include <linux/module.h>

#ifndef LINUX_VERSION_CODE
#include <linux/version.h>
#endif

#include <linux/kernel.h>

#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <asm/page.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>

#include "vme_types.h"


typedef struct
{
	struct semaphore wait;

} OSSEMA;

typedef struct
{
	wait_queue_head_t wait;
	int wait_active;

} OSWQUEUE;

typedef struct
{
	struct timer_list timer;

} OSTLIST;


/* Default free PCI memory space */
/* Can be overridden by using module parameters pciAddr and pciSize */
#define PCI_SPACE_START	0xb0000000
#define PCI_SPACE_SIZE	0x10000000

#define TUNDRA_PCI_VENDOR_ID	0x10e3
#define UNIVERSE_PCI_DEVICE_ID	0x0000


#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
extern void unv_isr_wr( int irq, void *dev_id, struct pt_regs *regs );
#define unv_interrupt unv_isr_wr 
#else
extern irqreturn_t unv_isr( int irq, void *dev_id, struct pt_regs *regs );
#define unv_interrupt unv_isr
#endif


/******************************************************************************
*
* os_inb
*
* IO read byte.
*
*
*
*
* RETURNS: IO port value.
*
******************************************************************************/
unsigned char os_inb( unsigned int address )
{
	return inb( address );
}


/******************************************************************************
*
* os_outb
*
* IO write byte.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_outb( unsigned char value, unsigned int address )
{
	outb_p( value, address );
}


/******************************************************************************
*
* os_inb_p
*
* IO read byte with pause.
*
*
*
*
* RETURNS: IO port value.
*
******************************************************************************/
unsigned char os_inb_p( unsigned int address )
{
	return inb_p( address );
}


/******************************************************************************
*
* os_outb_p
*
* IO write byte with pause.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_outb_p( unsigned char value, unsigned int address )
{
	outb_p( value, address );
}


#if defined(__powerpc__)
/******************************************************************************
*
* os_readPvr
*
* Get processor version.
*
*
*
*
* RETURNS: Processor version register value.
*
******************************************************************************/
unsigned int os_readPvr( void )
{
	return mfspr( PVR );
}
#endif


/******************************************************************************
*
* os_readb
*
* Read byte.
*
*
*
*
* RETURNS: Byte value.
*
******************************************************************************/
unsigned char os_readb( void *addr )
{
	return readb( addr );
}


/******************************************************************************
*
* os_writeb
*
* Write byte value.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_writeb( unsigned char val, void *addr )
{
	writeb( val, addr );
}


/******************************************************************************
*
* os_readw
*
* Read word.
*
*
*
*
* RETURNS: Word value.
*
******************************************************************************/
unsigned short os_readw( void *addr )
{
	return readw( addr );
}


/******************************************************************************
*
* os_writew
*
* Write word value.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_writew( unsigned short val, void *addr )
{
	writew( val, addr );
}


/******************************************************************************
*
* os_readl
*
* Read long.
*
*
*
*
* RETURNS: Long value.
*
******************************************************************************/
unsigned int os_readl( void *addr )
{
	return readl( addr );
}


/******************************************************************************
*
* os_writel
*
* Write long value.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_writel( unsigned int val, void *addr )
{
	writel( val, addr );
}


/******************************************************************************
*
* os_disable_interrupts
*
* Disable interrupts.
*
*
*
*
* RETURNS: flags.
*
******************************************************************************/
unsigned long os_disable_interrupts( void )
{
	unsigned long flags;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
	save_flags( flags );
	cli();
#else
	local_save_flags( flags );
	local_irq_disable();
#endif

	return flags;
}


/******************************************************************************
*
* os_enable_interrupts
*
* Enable interrupts.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_enable_interrupts( unsigned long flags )
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
	restore_flags( flags ); 
#else
	local_irq_restore( flags ); 
#endif
}


/******************************************************************************
*
* os_kmalloc
*
* Allocate kernel memory.
*
*
*
*
* RETURNS: Pointer to the allocated memory or NULL.
*
******************************************************************************/
void *os_kmalloc( unsigned int size )
{
	return kmalloc( size, GFP_KERNEL );
}


/******************************************************************************
*
* os_kfree
*
* Free kernel memory.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_kfree( const void *objp )
{
	if ( objp != NULL )
	{
		kfree( objp );
	}
}


/******************************************************************************
*
* os_alloc_wqueue
*
* Allocate and initialize a wait queue.
*
*
*
*
* RETURNS: Pointer to the allocated wait queue or NULL.
*
******************************************************************************/
void *os_alloc_wqueue( void )
{
    OSWQUEUE *pWqueue;


	pWqueue = (OSWQUEUE *) kmalloc( sizeof( OSWQUEUE ), GFP_KERNEL );
    
    if (pWqueue != NULL)
    {
		init_waitqueue_head( &pWqueue->wait );
		pWqueue->wait_active = 0;
    }

    return pWqueue;
}


/******************************************************************************
*
* os_schedule
*
* Relinquish the processor temporarily.
*
*
*
*
* RETURNS: 1.
*
******************************************************************************/
int os_schedule( void )
{
	schedule();

	return 1;
}


/******************************************************************************
*
* os_signal_pending
*
* Check if signal pending.
*
*
*
*
* RETURNS: True if signal pending else false.
*
******************************************************************************/
int os_signal_pending( void )
{
	return signal_pending( current );
}


/******************************************************************************
*
* interruptible_sleep_on
*
* Put process to sleep and make interruptible.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_interruptible_sleep_on( void *pWqueue )
{
	OSWQUEUE *os_wqueue = (OSWQUEUE *)pWqueue;


	os_wqueue->wait_active = 0;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
	interruptible_sleep_on( &os_wqueue->wait );
#else
	wait_event_interruptible( os_wqueue->wait, os_wqueue->wait_active );
#endif
}


/******************************************************************************
*
* os_wake_up_interruptible
*
* Wake up sleeping interruptible process.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_wake_up_interruptible( void *pWqueue )
{
	OSWQUEUE *os_wqueue = (OSWQUEUE *)pWqueue;


	os_wqueue->wait_active = 1;
	wake_up_interruptible( &os_wqueue->wait );
}


/******************************************************************************
*
* os_waitqueue_active
*
* Check if wait queue is available.
*
*
*
*
* RETURNS: True if active else false.
*
******************************************************************************/
int os_waitqueue_active( void *pWqueue )
{
	OSWQUEUE *os_wqueue = (OSWQUEUE *)pWqueue;

	return waitqueue_active( &os_wqueue->wait );
	
}


/******************************************************************************
*
* os_alloc_timer
*
* Allocate and initialize a timer.
*
*
*
*
* RETURNS: Pointer to the allocated timer or NULL.
*
******************************************************************************/
void *os_alloc_timer( void )
{
    OSTLIST *pTimer;


	pTimer = (OSTLIST *) kmalloc( sizeof( OSTLIST ), GFP_KERNEL );
    
    if (pTimer != NULL)
    {
		init_timer( &pTimer->timer );
    }

    return pTimer;
}


/******************************************************************************
*
* os_start_timer
*
* Setup and start timer.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_start_timer( void *pTimer, void (*function)( unsigned long ), 
							unsigned long arg, unsigned long timeout )
{
    OSTLIST *os_timer = (OSTLIST *)pTimer;
    
    init_timer( &os_timer->timer );
	os_timer->timer.function = function;
	os_timer->timer.data = arg;
	os_timer->timer.expires = jiffies + timeout; 
	add_timer( &os_timer->timer );
}


/******************************************************************************
*
* os_del_timer
*
* Delete timer.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_del_timer( void *pTimer )
{
    OSTLIST *os_timer = (OSTLIST *)pTimer;
    
	del_timer( &os_timer->timer );
}


/******************************************************************************
*
* os_alloc_sema
*
* Allocate and initialize a semaphore.
*
*
*
*
* RETURNS: Pointer to the allocated semaphore or NULL.
*
******************************************************************************/
void *os_alloc_sema( void )
{
    OSSEMA *pSema;


	pSema = (OSSEMA *) kmalloc( sizeof( OSSEMA ), GFP_KERNEL );
    
    if (pSema != NULL)
    {
    	sema_init( &pSema->wait, 1 );
    }

    return pSema;
}


/******************************************************************************
*
* os_acquire_sema
*
* Acquire a semaphore.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_acquire_sema( void *pSema )
{
    OSSEMA *os_sema = (OSSEMA *)pSema;
    
    down( &os_sema->wait );
}


/******************************************************************************
*
* os_release_sema
*
* Release a semaphore.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_release_sema( void  *pSema )
{
    OSSEMA *os_sema = (OSSEMA *)pSema;
    
    up( &os_sema->wait );
}


/******************************************************************************
*
* os_ioremap
*
* ioremap memory.
*
*
*
*
* RETURNS: Pointer to the remapped memory or NULL.
*
******************************************************************************/
void *os_ioremap( unsigned long offset, unsigned long size )
{
	return ioremap( offset, size);
}


/******************************************************************************
*
* os_iounmap
*
* iounmap memory.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_iounmap( void *addr )
{
	iounmap( addr );
}


/******************************************************************************
*
* os_remap_page_range
*
* Remap page range for given physical address.
*
*
*
*
* RETURNS: 0 or error code.
*
******************************************************************************/
int os_remap_page_range( void *pVma, unsigned long phys_add, unsigned long size )
{
	struct vm_area_struct *vma = (struct vm_area_struct *) pVma;

#ifdef REMAP_PFN_RANGE
	return remap_pfn_range( vma, vma->vm_start, (phys_add >> PAGE_SHIFT),
			     					size, vma->vm_page_prot );
#else

#ifdef REMAP_PAGE_RANGE_4
	return remap_page_range( vma->vm_start, phys_add,
			     					size, vma->vm_page_prot );
#else
	return remap_page_range( vma, vma->vm_start, phys_add,
			     					size, vma->vm_page_prot );
#endif
#endif

}


/******************************************************************************
*
* os_memcpy
*
* Memory copy.
*
*
*
*
* RETURNS: Pointer to the destination.
*
******************************************************************************/
void *os_memcpy( void *to, const void *from, unsigned int n )
{
	return memcpy( to, from, n );
}


/******************************************************************************
*
* os_memset
*
* Memory set.
*
*
*
*
* RETURNS: Pointer to the memory being set.
*
******************************************************************************/
void *os_memset( void *s, char c, unsigned int count)
{
	return memset( s, c, count);
}


/******************************************************************************
*
* os_strcpy
*
* String copy.
*
*
*
*
* RETURNS: Pointer to the destination.
*
******************************************************************************/
char *os_strcpy( char *dest, const char *src )
{
	return strcpy( dest, src );
}


/******************************************************************************
*
* os_strcmp
*
* String compare.
*
*
*
*
* RETURNS: An integer greater than, equal to, or less than 0.
*
******************************************************************************/
int os_strcmp( const char *cs, const char *ct )
{

	return strcmp( cs, ct );
}


/******************************************************************************
*
* os_page_align
*
* Get a page aligned size.
*
*
*
*
* RETURNS: Page aligned size.
*
******************************************************************************/
unsigned int os_page_align( unsigned int size )
{
	return PAGE_ALIGN( size );
}


/******************************************************************************
*
* os_getMemInfo
*
* Get system memory information.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_getMemInfo( UMEMINFO *pmi )
{
	struct sysinfo si;


	si_meminfo( &si );	/* get memory information */

	pmi->totalram = si.totalram;
	pmi->totalhigh = si.totalhigh;
	pmi->freeram = si.freeram;
	pmi->freehigh = si.freehigh;
}


/******************************************************************************
*
* os_high_memory
*
* Get the virtual high memory address.
*
*
*
*
* RETURNS: Pointer to the virtual high memory address.
*
******************************************************************************/
void *os_high_memory( void )
{
	return high_memory;
}


/******************************************************************************
*
* os_virt_to_phys
*
* Convert a virtual address to a physical address.
*
*
*
*
* RETURNS: A physical address value.
*
******************************************************************************/
unsigned long os_virt_to_phys( volatile void *address )
{
	return virt_to_phys( address );
}


/******************************************************************************
*
* os_page_shift
*
* Get the page shift value.
*
*
*
*
* RETURNS: The page shift value.
*
******************************************************************************/
int os_page_shift( void )
{
	return PAGE_SHIFT;
}


/******************************************************************************
*
* os_page_size
*
* Get the page size value.
*
*
*
*
* RETURNS: The page size value.
*
******************************************************************************/
unsigned long os_page_size( void )
{
	return PAGE_SIZE;
}


/******************************************************************************
*
* os_sprintf
*
* Write a formatted string to a buffer.
*
*
*
*
* RETURNS: The number of characters copied to buffer.
*
******************************************************************************/
int os_sprintf( char *buf, const char *fmt, ... )
{
    va_list arglist;
    int result;


    va_start( arglist, fmt );
    result = vsprintf( buf, fmt, arglist );
    va_end( arglist );

    return result;
}


/******************************************************************************
*
* os_logMsg
*
* Formats and sends the message to the kernel log.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_logMsg( char *fmt, ... )
{
	va_list args;
	char *msg;


	msg = (char *) kmalloc( PAGE_SIZE, GFP_KERNEL );

	if ( msg != NULL )
	{
		/* format message */
		va_start( args, fmt );
		vsprintf( msg, (const char *)fmt, args );
		va_end( args );

		printk( KERN_DEBUG "%s\n", msg );

		kfree( msg );
	}
}


/******************************************************************************
*
* os_copy_from_user
*
* Copy data from user space.
*
*
*
*
* RETURNS: 0 or error code.
*
******************************************************************************/
unsigned long os_copy_from_user( void *dest, void *src, unsigned long size )
{
	return copy_from_user( dest, src, size );
}


/******************************************************************************
*
* os_copy_from_user_nc
*
* Copy data from user space with no check.
*
*
*
*
* RETURNS: 0 or error code.
*
******************************************************************************/
unsigned long os_copy_from_user_nc( void *dest, const void *src, unsigned long size )
{
	return __copy_from_user( dest, src, size );
}


/******************************************************************************
*
* os_copy_to_user
*
* Copy data to user space.
*
*
*
*
* RETURNS: 0 or error code.
*
******************************************************************************/
unsigned long os_copy_to_user( void *dest, void *src, unsigned long size )
{
	return copy_to_user( dest, src, size );
}


/******************************************************************************
*
* os_copy_to_user_nc
*
* Copy data to user space with no check.
*
*
*
*
* RETURNS: 0 or error code.
*
******************************************************************************/
unsigned long os_copy_to_user_nc( void *dest, const void *src, unsigned long size )
{
	return __copy_to_user( dest, src, size );
}


/******************************************************************************
*
* os_put_user8_nc
*
* Copy a single 8 bit value to user space with no check.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_put_user8_nc( UINT8 value, UINT8 *src )
{
	__put_user( value, src );
}


/******************************************************************************
*
* os_put_user16_nc
*
* Copy a single 16 bit value to user space with no check.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_put_user16_nc( UINT16 value, UINT16 *src )
{
	__put_user( value, src );
}


/******************************************************************************
*
* os_put_user32_nc
*
* Copy a single 32 bit value to user space with no check.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_put_user32_nc( UINT32 value, UINT32 *src )
{
	__put_user( value, src );
}


/******************************************************************************
*
* os_get_user8_nc
*
* Copy a single 8 bit value from user space with no check.
*
*
*
*
* RETURNS: 0 or error code.
*
******************************************************************************/
unsigned long os_get_user8_nc( UINT8 *value, const UINT8 *src )
{
	UINT8 tmp;
	unsigned long result;

	result = __get_user( tmp, src );
	*value = tmp;

	return result;
}


/******************************************************************************
*
* os_get_user16_nc
*
* Copy a single 16 bit value from user space with no check.
*
*
*
*
* RETURNS: 0 or error code.
*
******************************************************************************/
unsigned long os_get_user16_nc( UINT16 *value, const UINT16 *src )
{
	UINT16 tmp;
	unsigned long result;

	result = __get_user( tmp, src );
	*value = tmp;

	return result;
}


/******************************************************************************
*
* os_get_user32_nc
*
* Copy a single 32 bit value from user space with no check.
*
*
*
*
* RETURNS: 0 or error code.
*
******************************************************************************/
unsigned long os_get_user32_nc( UINT32 *value, const UINT32 *src )
{
	UINT32 tmp;
	unsigned long result;


	result = __get_user( tmp, src );
	*value = tmp;

	return result;
}


/******************************************************************************
*
* os_get_user32
*
* Copy a single 32 bit value from user space.
*
*
*
*
* RETURNS: 0 or error code.
*
******************************************************************************/
unsigned long os_get_user32( UINT32 *value, const UINT32 *src )
{
	UINT32 tmp;
	unsigned long result;


	result = get_user( tmp, src );
	*value = tmp;

	return result;
}


/******************************************************************************
*
* os_memcpy_fromio
*
* Copy data from memory mapped IO space.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_memcpy_fromio( void *dest, void *src, unsigned long count )
{
	memcpy_fromio( dest, src, count );
}


/******************************************************************************
*
* os_memcpy_toio
*
* Copy data tp memory mapped IO space.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_memcpy_toio( void *dest, const void *src, unsigned long count )
{
	memcpy_toio( dest, src, count );
}



/******************************************************************************
*
* os_cpu_to_le32
*
* Convert CPU to little-endian byte order.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
UINT32 os_cpu_to_le32( UINT32 value )
{
	return __cpu_to_le32( value );
}


/******************************************************************************
*
* os_cpu_to_le32
*
* Convert little-endian to CPU byte order.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
UINT32 os_le32_to_cpu( UINT32 value )
{
	return __le32_to_cpu( value );
}


/******************************************************************************
*
* os_virt_to_bus
*
* Convert virtual address to bus (physical) address.
*
*
*
*
* RETURNS: The bus (physical) address.
*
******************************************************************************/
unsigned long os_virt_to_bus( void *address )
{
	return virt_to_bus( address );
}


/******************************************************************************
*
* os_request_irq
*
* Attach an Universe interrupt service routine.
*
*
*
*
* RETURNS: 0 or error code.
*
******************************************************************************/
int os_request_irq( unsigned int irq, const char *devname )
{
	return request_irq( irq, unv_interrupt, (SA_INTERRUPT | SA_SHIRQ),
						devname, unv_interrupt );	
}


/******************************************************************************
*
* os_free_irq
*
* Release an interrupt service routine.
*
*
*
*
* RETURNS: None.
*
******************************************************************************/
void os_free_irq( unsigned int irq )
{
	free_irq( irq, unv_interrupt );
}


/******************************************************************************
*
* os_find_universe
*
* Finds the Universe device.
*
*
*
*
* RETURNS: VME_SUCCESS or error code.
*
******************************************************************************/
int os_find_universe( UNIPCIINFO *pUni )
{
	int result;
	UINT32 temp;
	struct pci_dev *pciDev = NULL;

	
	pciDev = pci_find_device( TUNDRA_PCI_VENDOR_ID,
								UNIVERSE_PCI_DEVICE_ID,
								pciDev );

	if ( pciDev != NULL )
	{
		/* For ACPI systems , before you do anything with the device we've found,
		 * we need to enable it by calling pci_enable_device() which enables
		 * I/O and memory regions of the device, allocates an IRQ if necessary,
		 * assigns missing resources if needed and wakes up the device if it 
		 * was in suspended state. 
		 */
		result = pci_enable_device( pciDev );
		if (  result == VME_SUCCESS )
		{
			/* Get device information */
			pUni->vendor = (UINT16) pciDev->vendor;
			pUni->device = (UINT16) pciDev->device;
			pUni->class = (UINT32) pciDev->class;

			/* Get base address and IRQ */
			pci_read_config_dword(pciDev, PCI_BASE_ADDRESS_0, &temp);
			pUni->baseAddress = temp;

			/* CGD following code only works on non APIC/multiprocessor
			 * enabled kernels. Use irq from supplied device
			 * structure instead
			 *
			 * pci_read_config_dword( pciDev, PCI_INTERRUPT_LINE, &temp );
			 * pUni->irq = (temp & 0x000000FF);
			 */
			pUni->irq = pciDev->irq;

			pUni->pciDev = (void *) pciDev;

			/* Turn latency off */
			pci_write_config_byte(pciDev, PCI_LATENCY_TIMER, 0);
		}
		else
		{
			pUni->pciDev = NULL;
		}
	}
	else
	{
		result = (-VME_ENODEV);
	}

	return result;
}


/******************************************************************************
*
* os_disable_universe
*
* Disable the Universe device.
*
*
*
*
* RETURNS: VME_SUCCESS
*
******************************************************************************/
int os_disable_universe( UNIPCIINFO *pUni )
{

	if ( pUni->pciDev != NULL )
	{
		pci_disable_device( (struct pci_dev *) pUni->pciDev );
	}
	
	return VME_SUCCESS;
}


/******************************************************************************
*
* os_getPciSpace
*
* Gets PCI memory base address and size.
*
* Users may change the default values or add their own code to determine the
* values appropriate for their system.
*
* See also module parameters pciAddr and pciSize. These can be used to override
* the defaults at module load time.
*
*
* RETURNS: VME_SUCCESS or an error code may be returned to the driver.
*
******************************************************************************/
int os_getPciSpace( unsigned int *baseAddress, unsigned int *size )
{

	*baseAddress = PCI_SPACE_START;
	*size = PCI_SPACE_SIZE;

	return VME_SUCCESS;
}
