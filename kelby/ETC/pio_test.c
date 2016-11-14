#include <stdio.h>
#include <time.h>
#include <math.h>
/* #include "vmebase.h" */
 unsigned int vme_base,pio_base;

main()
 { unsigned long *wadr,*radr,k,l,vvv,*pulse,*sn;
   int i;
    printf("pio_test entered\n");
    vme_base = vme_a24_base();
    printf(" vme_base=%x\n",vme_base);
    pio_base = vme_base + 0x570000;
    sn = (unsigned long *)pio_base;
    k = *sn;
    printf(" serial number=%d\n",k);     
   for(i=0;i<6;i++) 
     { k = fpsw(i);
       printf(" fpsw(%d) = %d\n",i,k);
     }
   for(i=0;i<10;i++)
     { k=sw1(i);
       printf(" sw1(%d) = %d\n",i,k);
     }
   for(i=0;i<10;i++)
     { k=sw2(i);
       printf(" sw2(%d) = %d\n",i,k);
     }
    wadr = (unsigned long *)(pio_base + 0xc);
    radr = (unsigned long *)(pio_base + 0x8);
    *wadr=0x12345678;
    k = *radr;
    printf("sent 12345678 read back %x\n",k);

    for(l=0;l<0xfff;l++)
      { *wadr = l;
        k = *radr;
        printf(" test reg %x written   %x read back\n",l,k); 
	if(l != k) exit(0);
      }	
    pulse = (unsigned long *)(pio_base + 0x1c);
    *pulse = 1; 
  }
  
fpsw(int i)
  { int ent=0,l;
    unsigned long *adr,kk;
    adr = (unsigned long *)(pio_base + 0x4);
    kk = *adr;
    l = (kk>>i) & 1;
    return(l);
  }
  sw1(int i)
  { int ent=0,l,m;
    unsigned long *adr,kk;
    adr = (unsigned long *)(pio_base + 0x4);
    kk = *adr;
    m = i+12;
    l = (kk>>m) & 1;
    return(l);
  }
  sw2(int i)
  { int ent=0,l,m;
    unsigned long *adr,kk;
    adr = (unsigned long *)(pio_base + 0x4);
    kk = *adr;
    m = i+22;
    l = (kk>>m) & 1;
    return(l);
  }
  
