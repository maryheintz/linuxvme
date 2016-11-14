/******************************************************************************
*
* Filename: 	example7.c
* 
* Description:	10.7 Example 7 - Using Linked-list Mode DMA 
*               The following routine shows how to read some data using 
*               linked list mode DMA.
*
* $Revision: 1.1.1.1 $
*
* $Date: 2005/05/24 14:34:03 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme_examples/example7.c,v $
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

/*void do_list_dma( void )*/
int main( void )
{
    int devHandle;
    UINT8 buffer[8];
    VME_CMD_DATA cmddata;
    VME_TXFER_PARAMS tdata;
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

            /* clear any existing command packets */
            result = vme_clearDmaCmdPkts( devHandle );
            if ( result < 0 )
            {
                printf("Error - failed clear list (%s)\n", 
                        strerror(errno));
            }
            else
            {
                cmddata.direction = UNI_DMA_READ;
                cmddata.offset = 0; /* start of DMA buffer */
                cmddata.size = 4096;    /* read 4KB */
                cmddata.vmeAddress = 0x1000000;

                cmddata.access.dataWidth = VME_D64;
                cmddata.access.addrSpace = VME_A32;
                cmddata.access.type = LSI_DATA;  /* data AM code */
                cmddata.access.mode = LSI_USER;  /* non-privileged */
                cmddata.access.vmeCycle = 1;    /* single BLT's */

                result = vme_addDmaCmdPkt( devHandle, &cmddata );
                if ( result < 0 )
                {
                     printf("Error - failed to add command packet (%s)\n",
                             strerror(errno));

                }
                else
                {
                    /* 2 second timeout, hold VME bus until done */
                    tdata.timeout = 200;  
                    tdata.ownership = 0;  
                    result = vme_dmaListTransfer( devHandle, &tdata );
                    if ( result < 0 )
                    {
                        printf("Error - DMA transfer failed (%s)\n", 
                                strerror(errno));
                    }
                    else
                    {
                        printf("DMA transfer successful\n");

                        /* read and display the first 8 bytes */
                        result = vme_read( devHandle, 0, buffer, 8 );

                        if ( result > 0 )
                        {
                            for ( i = 0; i < 8; i++ )
                            {
                                printf("0x%02X\n", buffer[i] );
                            }
                        }
                    }
                }
            }

            vme_clearDmaCmdPkts( devHandle );

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
