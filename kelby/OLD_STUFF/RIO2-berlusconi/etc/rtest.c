#include <stdio.h>
#include <sys/types.h>

unsigned long vmebadr;
main()
{  int i,l,offst;
   unsigned long *adr;
   unsigned long kk,limit;
    
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

/* ============================================================== */
   adr = (unsigned long *)(vmebadr + 0x30);
   *adr=0;  /* disable random numbers and reset seeds */

   adr = (unsigned long *)(vmebadr + 0x34);
   *adr=10737418;  /* set threshold = 100KHz */

   adr = (unsigned long *)(vmebadr + 0x38);
   kk = *adr;  /* read back threshold */
   printf(" threshold = %d %x\n",kk,kk);

   adr = (unsigned long *)(vmebadr + 0x2c);
   *adr=0;  /* enable random numbers and reset seeds */

loop:
   adr = (unsigned long *)(vmebadr + 0x3c);
   kk = *adr;  /* read random number */
   printf(" random = %d %x\n",kk,kk);
   sleep(1);
   if(fpsw(1) ==1)
     { fscanf(stdin,"%d",&limit);
       adr = (unsigned long *)(vmebadr + 0x34);
       *adr=limit;  /* set threshold */

       adr = (unsigned long *)(vmebadr + 0x38);
       kk = *adr;  /* read back threshold */
       printf(" threshold = %d %x\n",kk,kk);
     }

     
   goto loop;
   
}

fpsw(int k)
  {  unsigned long *adr,kk;
     int l;
     adr = (unsigned long *)vmebadr;
     kk = *adr;
     l = (kk>>k) & 1;
     return(l);
  }
