#include <stdio.h>
#include <sys/types.h>

unsigned long vmebadr;
main()
{  int i,l,offst;
   unsigned long *adr;
   unsigned long kk;
    
   vmebadr = 0xde570000;

   adr = (unsigned long *)(vmebadr+0x1fc);
   kk = *adr;
   printf(" code version number=%x\n",kk);

   adr = (unsigned long *)vmebadr;
   kk = *adr;
   for(i=0;i<6;i++)
    { l = (kk>>i) & 1;
      printf(" front panel sw%d = %d\n",i,l);
    }
   printf("\n");
   for(i=0;i<10;i++)
    { offst=i+12;
      l = (kk>>offst);
      printf(" sw1 dip%d = %d\n",i,l);
    }
   printf("\n");
   for(i=0;i<10;i++)
    { offst=i+22;
      l = (kk>>offst);
      printf(" sw2 dip%d = %d\n",i,l);
    }
   printf("\n");
/* now test the read/write register */
   adr = (unsigned long *)(vmebadr + 0x14);
   *adr=0xaaaaaaaa;
   adr = (unsigned long *)(vmebadr + 0x10);
   kk = *adr;
   printf(" test register 0xaaaaaaaa written %x read back\n",kk);
   adr = (unsigned long *)(vmebadr + 0x14);
   *adr=0x55555555;
   adr = (unsigned long *)(vmebadr + 0x10);
   kk = *adr;
   printf(" test register 0x55555555 written %x read back\n",kk);
}

