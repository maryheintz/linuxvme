/******************************************************************************
*
* vmetst.c
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
    unsigned long i;
    UINT32 userMemAddrs;
    unsigned short *sadr,sbuffer;
    PCI_IMAGE_DATA idata;

    devHandle = vme_openDevice( "lsi1" );
    if(devHandle < 0)
      { printf("Error - failed to open lsi1\n");
        exit(0);
      }

    idata.pciAddress = 0xd0000000;
    idata.vmeAddress = 0x0;     
    idata.size = 0x1000000;
    idata.dataWidth = VME_D16;  /* 16 bit data */
    idata.addrSpace = VME_A24;  /* 24 bit address */
    idata.postedWrites = 1;
    idata.type = LSI_DATA;      /* data AM code */
    idata.mode = LSI_USER;      /* non-privileged */
    idata.vmeCycle = 1; /* no BLT's on VME bus */
    idata.pciBusSpace = 0;  /* PCI bus memory space */
    idata.ioremap = 0;  /* use ioremap */
    result = vme_enablePciImage( devHandle, &idata );
    if(result < 0)
      { printf("Error - failed to enable PCI image 0 (%s)\n", 
                        strerror(errno));
        exit(0);
      }

/*    printf("PAGE_SIZE=%x\n",PAGE_SIZE);  */
/*    printf("PCI image 0 enabled, accessing  data...\n");  */
    result = vme_mmap(devHandle,0,0x1000000,&userMemAddrs);
    if(result < 0)
      { printf("Error - failed to map memory (%s)\n", 
                        strerror(errno));
        exit(0);
      }

    
    sadr = (unsigned short *)(userMemAddrs+0xaa0000);
    sbuffer = *sadr;
    printf("sbuffer=%x\n",sbuffer);


    result = vme_disablePciImage( devHandle );
    if(result < 0)
      { printf("Error - failed to disable PCI image 0 (%s)\n", 
                strerror(errno));
      }
    vme_closeDevice( devHandle );

    return( 0 );
  }
