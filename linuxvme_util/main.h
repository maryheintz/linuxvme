/******************************************************************************
*
* Filename: 	main.h
* 
* Description:	Header file for the Linux VME Utility Program.
*
* $Revision: 1.1.1.1 $
*
* $Date: 2005/05/24 14:34:03 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme_util/main.h,v $
*
* Copyright 2000 - 2005 Concurrent Technologies.
*
******************************************************************************/
#ifndef __INCmain
#define __INCmain

#include "vme_api.h"

#define MAX_STRING 		81		/* longest allowed response + null terminator */
#define MAX_SIZE 		256		/* max number of bytes to read & display */
#define MAX_BUFFER_SIZE 1024

#define MAX_COLUMN		80		/* screen width */

#define START_LINE	 	2		/* Messages start here					*/
#define STATUS_LINE  	21		/* The line to use for status/errors	*/
#define PROMPT_LINE  	23		/* Line for prompting on 				*/
#define DEVICE_INDENT 	60		/* Device name printed here				*/


typedef struct 
{
	char name[41];
	char format;
	UINT32 size;
	UINT32 value;
	UINT32 min;
	UINT32 max;
	char **sPtr;

} ENTRYITEM; 


/* Function Prototypes */
void printBuf( WINDOW *wPtr, int startrow, UINT32 addr, UINT8 *buf, UINT32 size );
void getReturn( int column );
int getConfirm( int column );
void getString( char *string );
void clearAllScreen( const char *message );
void drawItems( ENTRYITEM *items, int highlight, int spacing, int itemsPerCol );

#endif	/* __INCmain */
