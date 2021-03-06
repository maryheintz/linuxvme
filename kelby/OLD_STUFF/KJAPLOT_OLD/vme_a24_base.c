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
int devHandle;
PCI_IMAGE_DATA idata;
                                                                                    
unsigned long vme_a24_base()
  {int result;
    unsigned long i;
    UINT32 userMemAddrs;
    unsigned long wbuffer,rbuffer,*adr;

    devHandle = vme_openDevice( "lsi0" );
    if ( devHandle < 0 )
    { printf("Error - failed to open lsi0\n");
      exit(0);    
    }

    idata.pciAddress = 0xd0000000;
    idata.vmeAddress = 0;     
    idata.size = 0x1000000;
    idata.dataWidth = VME_D32;  /* 32 bit data */
    idata.addrSpace = VME_A24;  /* 24 bit address */
    idata.postedWrites = 1;
    idata.type = LSI_DATA;      /* data AM code */
    idata.mode = LSI_USER;      /* non-privileged */
    idata.vmeCycle = 0; /* no BLT's on VME bus */
    idata.pciBusSpace = 0;  /* PCI bus memory space */
    idata.ioremap = 0;  /* no ioremap - vme_mmap used */

    result = vme_enablePciImage( devHandle, &idata );
    if(result < 0)
      { printf("Error - failed to enable PCI image 0 (%s)\n", 
                        strerror(errno));
        exit(0);
      }
    printf("PCI image 0 enabled, accessing  data...\n");
    result = vme_mmap(devHandle,0,0x1000000,&userMemAddrs);
    adr = (unsigned long)userMemAddrs;
    return(adr);
  }   
	    
    vme_close()
       { vme_closeDevice( devHandle );
       }

