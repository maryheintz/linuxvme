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
    unsigned long wbuffer,rbuffer,*wadr,*radr,wd;
    unsigned short *sadr,sbuffer;
    PCI_IMAGE_DATA idata;
    extern unsigned long swap32(unsigned long wd);

    devHandle = vme_openDevice( "lsi0" );
    if(devHandle < 0)
      { printf("Error - failed to open lsi0\n");
        exit(0);
      }

    idata.pciAddress = 0xd0000000;
    idata.vmeAddress = 0x0;     
    idata.size = 0x1000000;
    idata.dataWidth = VME_D32;  /* 32 bit data */
    idata.addrSpace = VME_A24;  /* 32 bit address */
    idata.postedWrites = 1;
    idata.type = LSI_DATA;      /* data AM code */
    idata.mode = LSI_USER;      /* non-privileged */
    idata.vmeCycle = 0; /* no BLT's on VME bus */
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

/*    printf("userMemAddrs=%x\n",userMemAddrs);  */
    wadr = (unsigned long *)(userMemAddrs+0x570014);
    radr = (unsigned long *)(userMemAddrs+0x570010);
    wbuffer=0x12345678;
    *wadr = wbuffer;
    rbuffer = *radr;
    printf(" rbuffer=%x\n",rbuffer);
    wbuffer=0x9abcdef0;
    *wadr = wbuffer;
    rbuffer = *radr;
    printf(" rbuffer=%x\n",rbuffer);
    
    sadr = (short long *)(userMemAddrs+0xaa0000);
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

  unsigned long swap32(unsigned long wd)
    { unsigned long b1,b2,b3,b4,b5,b6,b7,b8;
      unsigned long reorder;
      b1 = (wd>>28) & 0xf;
      b2 = (wd>>24) & 0xf;
      b3 = (wd>>16) & 0xf;
      b4 = (wd>>20) & 0xf;
      b5 = (wd>>12) & 0xf;
      b6 = (wd>>8) & 0xf;
      b7 = (wd>>4) & 0xf;
      b8 = wd & 0xf;
      reorder = b7<<28 | b8<<24 | b5<<20 | b6<<16 
             | b4<<12 | b3<<8 | b1<<4 | b2;
      return(reorder);
    } 
	  
