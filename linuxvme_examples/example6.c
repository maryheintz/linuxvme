/******************************************************************************
*
* Filename: 	example6.c
* 
* Description:	10.6 Example 6 - Using Direct Mode DMA 
*               The following routine shows how to read some data using 
*               direct mode DMA.
*
* $Revision: 1.1.1.1 $
*
* $Date: 2005/05/24 14:34:03 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme_examples/example6.c,v $
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

/*void do_direct_dma( void )*/
int main( void )
{
int devHandle;
    UINT8 buffer[8];
    VME_DIRECT_TXFER tdata;
    UINT32 size;
    int result;
    int i;

    devHandle = vme_openDevice( "dma" );
    if ( devHandle >= 0 )
    {
        size = 4096;
        result = vme_allocDmaBuffer( devHandle, &size );
        if ( result < 0 )
        {
            printf("Error - failed to allocate DMA buffer (%s)\n", 
                        strerror(errno));
        }
        else
        {
            printf("DMA buffer allocted, %u bytes available\n",
                    size);

            tdata.direction = UNI_DMA_READ;
            tdata.offset = 0;       /* start of DMA buffer */
            tdata.size = 4096;  /* read 4KB */
            tdata.vmeAddress = 0x1000000;

            tdata.txfer.timeout = 200;  /* 2 second timeout */
            tdata.txfer.ownership = 0;  /* hold VME bus until done */

            tdata.access.dataWidth = VME_D64;
            tdata.access.addrSpace = VME_A32;
            tdata.access.type = LSI_DATA;   /* data AM code */
            tdata.access.mode = LSI_USER;   /* non-privileged */
            tdata.access.vmeCycle = 1;  /* single BLT's */


            result = vme_dmaDirectTransfer( devHandle, &tdata );
            if ( result < 0 )
            {
                printf("Error - DMA transfer failed (%s)\n", 
                        strerror(errno));
            }
            else
            {
                printf("DMA transfer successful\n");

                /* read and display the first 8 bytes of the buffer */
                result = vme_read( devHandle, 0, buffer, 8 );

                if ( result > 0 )
                {
                    for ( i = 0; i < 8; i++ )
                    {
                        printf("0x%02X\n", buffer[i] );
                    }
                }
            }

            result = vme_freeDmaBuffer( devHandle );
            if ( result < 0 )
            {
                printf("Error - failed to free DMA buffer (%s)\n", 
                        strerror(errno));
            }

        }

        vme_closeDevice( devHandle );
    }

    return( 0 );
}

