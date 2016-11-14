/******************************************************************************
*
* Filename: 	example4_bis.c
* 
* Description:	10.4 Example 4 - Using a PCI Image Window 
*               The following routine shows how to use an ioremapped 
*               PCI image window.
*
* $Revision: 1.1.1.1 $
*
* $Date: 2005/05/24 14:34:03 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme_examples/example4.c,v $
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
                                                                                
/*void enable_pci_image( void )*/
int main( void )
{
    int devHandle;
    int result;
    int i;
    unsigned long wbuffer,rbuffer;
    PCI_IMAGE_DATA idata;

    devHandle = vme_openDevice( "lsi0" );
    if ( devHandle < 0 )
    {   
        printf("Error - failed to open lsi0\n");    }
    else
    {
        idata.pciAddress = 0xd0000000;
        idata.vmeAddress = 0x00570000;     
        idata.size = 0x10000;
        idata.dataWidth = VME_D32;  /* 32 bit data */
        idata.addrSpace = VME_A24;  /* 32 bit address */
        idata.postedWrites = 1;
        idata.type = LSI_DATA;      /* data AM code */
        idata.mode = LSI_USER;      /* non-privileged */
        idata.vmeCycle = 0; /* no BLT's on VME bus */
        idata.pciBusSpace = 0;  /* PCI bus memory space */
        idata.ioremap = 1;  /* use ioremap */

        result = vme_enablePciImage( devHandle, &idata );
        if ( result < 0 )
        {
            printf("Error - failed to enable PCI image 0 (%s)\n", 
                        strerror(errno));
        }
        else
        {
            printf("PCI image 0 enabled, accessing  data...\n");
	
            wbuffer=0x12345678;
            result = vme_write( devHandle, 0x14, &wbuffer, 4 );
            /* eventually check for any error */

            result = vme_read( devHandle, 0x10, &rbuffer, 4 );
            /* eventually check for any error */
            printf(" rbuffer=%x\n",rbuffer);


            result = vme_disablePciImage( devHandle );
            if ( result < 0 )
            {
                printf("Error - failed to disable PCI image 0 (%s)\n", 
                        strerror(errno));
            }
        }
        vme_closeDevice( devHandle );
    }

    return( 0 );
}