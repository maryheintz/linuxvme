#include <stdio.h>
#include <math.h>
main()
{ int mode,swtst,i,c1,c2,f1,f2;
  unsigned short *adr;
restart:
   printf(" select desired test - loop\n");
   printf("  1 = read switches and latch\n");
   printf("  2 = pulse timers (internal)\n");
   printf("  3 = reset latch\n");
   printf("  4 = toggle f1\n");
   printf("  5 = toggle f2\n");
   printf("  6 = pulse timers (external)\n");
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
    { (unsigned short *)adr = (unsigned short *)0xdeaa000a;
    loop4:
       *adr = 0x55;
       *adr = 0xaa;
    goto loop4;
    }

   if(mode == 5)
    { (unsigned short *)adr = (unsigned short *)0xdeaa0012;
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

goto restart;

}
sw(i)
int i;
{ int k,l,shift;
  unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xdeaa000c;
  k = *adr;
  printf("k=%x\n",k);
  shift = i-1;
  l = (k>>shift) & 1;
  return(l);
}
latch()
{ int k,l;
  unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xdeaa000c;
  k = *adr;
  l = (k>>15) & 1;
  return(l);
}
setime(c1,f1,c2,f2)
int c1,f1,c2,f2;
{unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xdeaa000a;
 *adr = (unsigned short)f1;               /* latch fine timer 1 value */
  (unsigned short *)adr = (unsigned short *)0xdeaa0012;
 *adr = (unsigned short)f2;             /* latch fine timer 2 value */
  (unsigned short *)adr = (unsigned short *)0xdeaa0008;
 *adr = (unsigned short)c1;               /* write coarse timer 1 to latch */
  (unsigned short *)adr = (unsigned short *)0xdeaa0010;
 *adr = (unsigned short)c2;               /* write coarse timer 1 to latch */
  (unsigned short *)adr = (unsigned short *)0xdeaa0004;
 *adr = 0;                        /* load fine timers with latched values */
  (unsigned short *)adr = (unsigned short *)0xdeaa0000;
 *adr = 0;                        /* xfer latched to counters (coarse) */
}
restim()
{unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xdeaa0006;
 *adr = 0;              /* clear fine timer done bits */
  (unsigned short *)adr = (unsigned short *)0xdeaa0000;
 *adr = 0;              /* reset timers back to latched values */
}
stvme()
{unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xdeaa000e;
 *adr = 0;              /* start the timers from a vme null write */
}
rlatch()
{unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xdeaa0002;
 *adr = 0;              /* reset the latch bit */
}

