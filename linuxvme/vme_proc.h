/******************************************************************************
*
* Filename: 	vme_proc.h
* 
* Description:	Header file VME device driver proc file system interface.
*
* $Revision: 1.1.1.1 $
*
* $Date: 2005/05/24 14:34:02 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme/vme_proc.h,v $
*
* Copyright 2000-2005 Concurrent Technologies.
*
******************************************************************************/

#ifndef __INCvme_proc
#define __INCvme_proc


typedef struct 
{
	char *name;
	read_proc_t *read_proc;
	void *data;

} VME_PROC_ENTRY;


void vme_procInit( void );
void vme_procCleanup( void );

#endif	/* __INCvme_proc */
