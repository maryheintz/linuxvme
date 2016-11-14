/******************************************************************************
*
* Filename: 	example1.c
* 
* Description:	10.1 Example 1 - Generating A VME Bus Interrupt From Software 
*               The following routine shows how to set the Status ID, then  
*               VIRQ1 interrupt.
*
* $Revision: 1.1.1.1 $
*
* $Date: 2005/05/24 14:34:03 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme_examples/example1.c,v $
*
* Copyright 2000 - 2005 Concurrent Technologies.
*
******************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <asm/page.h>

#include "vme_api.h"

extern int errno;

/* void generate_vme_interrupt( void ) */
int main ( void )
{
    int devHandle;
    int result;
    devHandle = vme_openDevice( "ctl" );
    if ( devHandle >= 0 )
    {
        result = vme_setStatusId( devHandle, 0x2 );
        if ( result < 0 )
        {
            printf("Error - failed to set status ID (%s)\n", 
                        strerror(errno));
        }
        else
        {
            printf("status ID set, generating VIRQ1\n");

            result = vme_generateInterrupt( devHandle, 1 );
            if ( result < 0 )
            {
                printf("Error - failed to generate interrupt (%s)\n", 
                        strerror(errno));
            }
            else
            {
                printf("VIRQ1 generated\n");
            }
        }
        vme_closeDevice( devHandle );
    }

    return ( 0 );
}
