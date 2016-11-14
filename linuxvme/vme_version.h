/******************************************************************************
*
* Filename: 	vme_version.h
* 
* Description:	Header file for version information
*
* $Revision: 1.6 $
*
* $Date: 2007/02/09 13:53:01 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme/vme_version.h,v $
*
* Copyright 2000-2005 Concurrent Technologies.
*
******************************************************************************/

#ifndef __INCvme_version
#define __INCvme_version

static const char *creationDate  = __DATE__ ", " __TIME__;
static const char *copyright = "Copyright 2001-2007 Concurrent Technologies Plc";


#ifdef VME_DRIVER
char kernel_version[] = UTS_RELEASE;
static const char *vmeDriverVersion	= "Version V2.06-01";
#endif

#ifdef VME_LIB
static const char *vmeLibVersion = "Version V2.06-01";
#endif

#ifdef VME_API
static const char *vmeApiVersion = "Version V2.06-01";
#endif

#endif	/* __INCvme_version */
