/******************************************************************************
*
* Filename: 	example5.c
* 
* Description:	10.5 Example 5 - Using a VME Image Window 
*               The following routine shows how to use a memory mapped 
*               VME image window.
*
* $Revision: 1.1.1.1 $
*
* $Date: 2005/05/24 14:34:03 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme_examples/example5.c,v $
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

/*void enable_vme_image( void )*/
int main( void )
{
    int devHandle;
    int result;
    int i;
    UINT8 buffer[8];
    UINT32 userMemAddrs;
    VME_IMAGE_DATA idata;


    devHandle = vme_openDevice( "vsi0" );
    if ( devHandle < 0 )
    {   
        /*printf("Error - failed to open lsi0\n");*/    
        printf("Error - failed to open vsi0\n");    
    }
    else
    {
        idata.vmeAddress = 0x2000000;       
        idata.size = 0x10000;
        idata.addrSpace = VME_A32;  /* 32 bit address */
        idata.postedWrites = 1;
        idata.prefetchRead = 1;
        idata.type = VSI_BOTH;  /* both program and data */
        idata.mode = VSI_BOTH;  /* both supervisor and non-privileged */
        idata.pciBusLock = 0;   /* PCI bus lock disable */
        idata.pciBusSpace = 0;  /* PCI bus memory space */
        idata.ioremap = 0;      /* don,t ioremap */


        result = vme_enableVmeImage( devHandle, &idata );
        if ( result < 0 )
        {
            printf("Error - failed to enable VME image 0 (%s)\n", 
                        strerror(errno));
        }
        else
        {
            result = vme_mmap( devHandle, 0, PAGE_SIZE, &userMemAddrs );

            if ( result < 0 )
            {
                 printf("Error - failed to memory map VME image 0 (%s)\n",  
                         strerror(errno));
            
            }
            else
            {
                 printf("VME image 0 enabled and memory mapped, reading data...\n");

                memcpy( buffer, (UINT8 *) userMemAddrs, 8 );
            
                for ( i = 0; i < 8; i++ )
                {
                    printf("0x%02X\n", buffer[i] );
                }
        
                result = vme_unmap( devHandle, userMemAddrs, PAGE_SIZE );

                if ( result < 0 )
                {
                    printf("Error - failed to  unmap VME image 0 (%s)\n", 
                            strerror(errno));
            
                }
            }

            result = vme_disableVmeImage( devHandle );
            if ( result < 0 )
            {
                printf("Error - failed to disable VME image 0 (%s)\n", 
                        strerror(errno));
            }
            
        }

        vme_closeDevice( devHandle );
    }

    return( 0 );
}

