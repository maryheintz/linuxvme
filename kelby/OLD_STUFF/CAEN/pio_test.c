#include <stdio.h>
#include <time.h>
 unsigned long vmebase,swadr=0x570000;

main()
 { unsigned long *wadr,*radr,k,l;
   int i;
    printf("pio_test entered\n");
    vmebase = vme_a24_base();
    printf(" vme_a24_base=%x\n",vmebase);
    vmebase = vmebase + swadr;     
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
    wadr = (unsigned long *)(vmebase + 0x14);
    radr = (unsigned long *)(vmebase + 0x10);
    *wadr=0x12345678;
    k = *radr;
    printf("sent 12345678 read back %x\n",k);

    for(l=0;l<0xffffffff;l++)
      { *wadr = l;
        k = *radr;
        if(fpsw(0) == 1) printf(" test reg %x written   %x read back\n",l,k); 
	if(l != k) exit(0);
      }	
/* send pulses */
    wadr = (unsigned long *)(vmebase + 0x14);
    *wadr = 1;
  }
fpsw(int i)
  { int ent=0,l;
    unsigned long *adr,kk;
    adr = (unsigned long *)vmebase;
    kk = *adr;
    l = (kk>>i) & 1;
    return(l);
  }
  sw1(int i)
  { int ent=0,l,m;
    unsigned long *adr,kk;
    adr = (unsigned long *)vmebase;
    kk = *adr;
    m = i+12;
    l = (kk>>m) & 1;
    return(l);
  }
  sw2(int i)
  { int ent=0,l,m;
    unsigned long *adr,kk;
    adr = (unsigned long *)vmebase;
    kk = *adr;
    m = i+22;
    l = (kk>>m) & 1;
    return(l);
  }
  
