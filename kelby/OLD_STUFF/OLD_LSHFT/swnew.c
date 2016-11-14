
#include <stdio.h>
#include <math.h>
#include <time.h>
unsigned int sw_base,fermi_base,vme_base;
main()
{ int mode,swtst,i,c1,c2,f1,f2;
  unsigned short *adr,kkk;
  unsigned long *adrl;
  vme_base = vme_a24_base();
  sw_base = vme_base + 0xaa0000;
restart:
   printf(" select desired test - loop\n");
   printf("  1 = read switches and latch\n");
   printf("  2 = pulse timers (internal)\n");
   printf("  3 = reset latch\n");
   printf("  4 = toggle f1\n");
   printf("  5 = toggle f2\n");
   printf("  6 = pulse timers (external)\n");
   printf("  7 = read fermi status\n");
   fscanf(stdin,"%d",&mode);

   if(mode == 1)
    {  for(i=1;i<9;i++)
        {swtst = sw(i);
         printf("i=%d  sw=%d\n",i,swtst);
        }
    swtst = latch();
    printf("latch bit=%d\n",swtst);
    }

   if(mode == 2)
    {
     c1=20;
     c2=30;
     f1=0;
     f2=0;
loop2:
     setime(c1,f1,c2,f2);
     stvme();
     sleep(1);
     restim();
     goto loop2;
    }

   if(mode == 3)
    {  rlatch();
    }

   if(mode == 4)
    { (unsigned short *)adr = (unsigned short *)(sw_base + 0x0a);
    loop4:
       *adr = 0x55;
       *adr = 0xaa;
    goto loop4;
    }

   if(mode == 5)
    { (unsigned short *)adr = (unsigned short *)(sw_base + 0x12);
    loop5:
       *adr = 0x55;
       *adr = 0xaa;
    goto loop5;
    }

   if(mode == 6)
    {
loop6:
     printf("enter c1 c2 f1 f2\n");
     fscanf(stdin,"%d %d %d %d",&c1,&c2,&f1,&f2);
     printf("c1=%d c2=%d f1=%d f2=%d\n",c1,c2,f1,f2);
     setime(c1,f1,c2,f2);
    goto loop6;
    }

   if(mode == 7)
    { (unsigned short *)adr = (unsigned short *)(fermi_base + 0x24);
      *adr = 0;  /* issue reset */
      (unsigned short *)adr = (unsigned short *)(fermi_base + 0x22);
      *adr = 0;
      kkk = *adr;
      printf(" 0 fermi status = %x  %x\n",kkk,adr);
      *adr = 0xf;
      kkk = *adr;
      printf(" 1 fermi status = %x  %x\n",kkk,adr);
      (unsigned short *)adr = (unsigned short *)(fermi_base + 0x20);
      *adr = 0xaa;
      kkk = *adr;
      printf(" aa fermi sample reg = %x  %x\n",kkk,adr);
      *adr = 0x55;
      kkk = *adr;
      printf(" 55 fermi sample reg = %x  %x\n",kkk,adr);
    }

goto restart;

}
sw(i)
int i;
{ int k,l,shift;
  unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)(sw_base + 0x0c);
  k = *adr;
  printf("k=%x\n",k);
  shift = i-1;
  l = (k>>shift) & 1;
  return(l);
}
latch()
{ int k,l;
  unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)(sw_base + 0x0c);
  k = *adr;
  l = (k>>15) & 1;
  return(l);
}
setime(c1,f1,c2,f2)
int c1,f1,c2,f2;
{unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)(sw_base + 0x0a);
 *adr = (unsigned short)f1;               /* latch fine timer 1 value */
  (unsigned short *)adr = (unsigned short *)(sw_base + 0x12);
 *adr = (unsigned short)f2;             /* latch fine timer 2 value */
  (unsigned short *)adr = (unsigned short *)(sw_base + 0x08);
 *adr = (unsigned short)c1;               /* write coarse timer 1 to latch */
  (unsigned short *)adr = (unsigned short *)(sw_base + 0x10);
 *adr = (unsigned short)c2;               /* write coarse timer 1 to latch */
  (unsigned short *)adr = (unsigned short *)(sw_base + 0x04);
 *adr = 0;                        /* load fine timers with latched values */
  (unsigned short *)adr = (unsigned short *)(sw_base + 0x00);
 *adr = 0;                        /* xfer latched to counters (coarse) */
}
restim()
{unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)(sw_base + 0x06);
 *adr = 0;              /* clear fine timer done bits */
  (unsigned short *)adr = (unsigned short *)(sw_base + 0x00);
 *adr = 0;              /* reset timers back to latched values */
}
stvme()
{unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)(sw_base + 0x0e);
 *adr = 0;              /* start the timers from a vme null write */
}
rlatch()
{unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)(sw_base + 0x02);
 *adr = 0;              /* reset the latch bit */
}

