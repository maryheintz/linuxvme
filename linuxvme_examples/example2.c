/******************************************************************************
*
* Filename: 	example2.c
* 
* Description:	10.2 Example 2 - Reading VME Bus Interrupt Information 
*               The following routine shows how read VME bus interrupt 
*               information for VIRQ1.
*
* $Revision: 1.1.1.1 $
*
* $Date: 2005/05/24 14:34:03 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme_examples/example2.c,v $
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

/*void read_vme_interrupt( void )*/
int main ( void )
{
    int devHandle;
    int result;
    int i;
    VME_INT_INFO info;

    devHandle = vme_openDevice( "ctl" );
    if ( devHandle >= 0 )
    {
        info.intNum = 1; /* get info for VIRQ1 */

        result = vme_readInterruptInfo( devHandle, &info );
        if ( result < 0 )
        {
            printf("Error - failed to read interrupt info (%s)\n", 
                        strerror(errno));
        }
        else
        {
            printf("Number of interrupts: %u\n", info.numOfInts);

            for ( i = 0; i < info.vecCount; i++ )
            {
                printf("%02d     0x%02X\n", i, info.vectors[i]);
            }
        }
        vme_closeDevice( devHandle );
    }

return ( 0 );
}
