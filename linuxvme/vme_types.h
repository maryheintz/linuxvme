/******************************************************************************
*
* Filename: 	vme_types.h
* 
* Description:	Header file for VME types
*
* $Revision: 1.2 $
*
* $Date: 2006/08/18 14:00:12 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme/vme_types.h,v $
*
* Copyright 2000-2005 Concurrent Technologies.
*
******************************************************************************/

#ifndef __INCvme_types
#define __INCvme_types

typedef unsigned char	UINT8;
typedef unsigned short	UINT16;
typedef unsigned int	UINT32;
typedef char			INT8;
typedef short			INT16;
typedef int				INT32;


typedef struct
{
	UINT16 vendor;
	UINT16 device;
	UINT32 class;
	UINT32 baseAddress;
	UINT32 irq;
	void *pciDev;

} UNIPCIINFO;

typedef struct
{
	unsigned long totalram;
	unsigned long totalhigh;
	unsigned long freeram;
	unsigned long freehigh;

} UMEMINFO;

typedef struct
{
	long tv_sec;
	long tv_usec;

} UNITIME;


#define VME_CONTROL_MINOR 	0	/* minor device numbers */
#define VME_LSI0_MINOR		1
#define VME_LSI1_MINOR		2
#define VME_LSI2_MINOR		3
#define VME_LSI3_MINOR		4
#define VME_LSI4_MINOR		5
#define VME_LSI5_MINOR		6
#define VME_LSI6_MINOR		7
#define VME_LSI7_MINOR		8
#define VME_VSI0_MINOR		9
#define VME_VSI1_MINOR		10
#define VME_VSI2_MINOR		11
#define VME_VSI3_MINOR		12
#define VME_VSI4_MINOR		13
#define VME_VSI5_MINOR		14
#define VME_VSI6_MINOR		15
#define VME_VSI7_MINOR		16
#define VME_DMA_MINOR		17
#define VME_MAX_MINORS		18	/* number of device files */

/* operating modes */
#define MODE_UNDEFINED	0
#define MODE_IOREMAPPED	1

#define SEEK_SET 0	/* set current offset to given value */
#define SEEK_CUR 1	/* add given value to current offset */

#ifndef NULL
#define NULL (void *) 0
#endif

#define VME_SUCCESS		0
#define	VME_EPERM		1	/* Operation not permitted */
#define	VME_EINTR		4	/* Interrupted system call */
#define	VME_EIO		 	5	/* I/O error */
#define	VME_ENOMEM		12	/* Out of memory */
#define	VME_EFAULT		14	/* Bad address */
#define	VME_EBUSY		16	/* Device or resource busy */
#define	VME_ENODEV		19	/* No such device */
#define	VME_EINVAL		22	/* Invalid argument */
#define	VME_ETIME		62	/* Timer expired */

#if defined( DEBUG ) || defined( ERROR_MSG )
extern void os_dbgMsg( char *fmt, ... );
#endif
#ifdef DEBUG
#define DBMSG(msg, args...) os_dbgMsg( "dbgVME %03d: " msg, __LINE__, ##args )
#else
#define DBMSG(msg, args...)
#endif

#ifdef ERROR_MSG
#define ERRMSG(msg, args...) os_dbgMsg( "errVME %03d: " msg, __LINE__, ##args )
#else
#define ERRMSG(msg, args...)
#endif

#define MSG(msg, args...) os_logMsg( msg, ##args )

#endif	/* __INCvme_types */
