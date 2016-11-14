/******************************************************************************
*
* Filename: 	example3.c
* 
* Description:	10.3 Example 3 - Wait For An Interrupt
*               The following routine shows how a Mailbox could be used.
*
* $Revision: 1.1.1.1 $
*
* $Date: 2005/05/24 14:34:03 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme_examples/example3.c,v $
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

/*void wait_vme_interrupt( void )*/
int main( void )
{
    int devHandle;
    int result;
    UINT32 reg;
    UINT32 intNum;
    UINT32 selectedInts;
    VME_IMAGE_ACCESS idata;

    devHandle = vme_openDevice( "ctl" );
    if ( devHandle >= 0 )
    {
        idata.vmeAddress = 0x2000000;
        idata.addrSpace = VME_A32;
        idata.type = VSI_BOTH;  /* both Program and Data */
        idata.mode = VSI_BOTH;  /* both Supervisor and non-privileged */

        result = vme_enableRegAccessImage( devHandle, &idata );
        if ( result < 0 )
        {
            printf("Error - failed to enable Register Access (%s)\n", 
                        strerror(errno));
        }
        else
        {
            printf("Register Access enabled\n");
            printf("Waiting for someone to write to Mailbox...\n");

            /* wait for mailbox interrupt to occur
               note: vme_waitInterrupt makes sure the interrupt is enabled
               so there no need to call vme_enableInterrupt first 
           */
            selectedInts = 1L <<LINT_MBOX0;
            intNum = 0;
            result = vme_waitInterrupt( devHandle, selectedInts, 0, &intNum );
            if ( result < 0 )
            {
                printf("Error - failed to receive interrupt (%s)\n", 
                        strerror(errno));
            }
            else
            {
     
                printf ("LINT_MBOX0 RECEIVED (0x%08X)\n", intNum);
                printf("Interrupt %u received\n", LINT_MBOX0);
                result = vme_readRegister( devHandle, MBOX0, &reg );
                if ( result < 0 )
                {
                    printf("Error - failed to read register (%s)\n", 
                                strerror(errno));
                }
                else
                {
                    printf("MBOX0: 0x%08X\n", reg);
                }
            }

            result = vme_disableRegAccessImage( devHandle );
            if ( result < 0 )
            {
                printf("Error - failed to disable Register Access (%s)\n", 
                        strerror(errno));
            }

        }
        vme_closeDevice( devHandle );
    }

return ( 0 );
}


