/******************************************************************************
*
* Filename: 	vme_driver.h
* 
* Description:	Header file for VME device driver
*
* $Revision: 1.1.1.1 $
*
* $Date: 2005/05/24 14:34:02 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme/vme_driver.h,v $
*
* Copyright 2000-2005 Concurrent Technologies.
*
******************************************************************************/

#ifndef __INCvme_driver
#define __INCvme_driver


typedef struct 
{
	INT32 fileOpen; 		/* whether device is open */
	uid_t owner;			/* current owner */
	int mmapCount;			/* memory mapping counter */

} VMECTRL;


int vmedrv_getInfo( char *buffer );

#endif	/* __INCvme_driver */
