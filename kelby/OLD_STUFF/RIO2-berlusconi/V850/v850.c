#include <stdio.h>
#include <time.h>
/*#include "vmebase.h"*/ 
unsigned long V850adr=0xde00f000;
unsigned long vmebase,sw_base;

main()
 { unsigned short k,i,*adr,il,iwave,busy;
   double dly1,dly2,dly3,dly4,dly5,dly6;
   unsigned long longtime,l1,l2;
   int c1,c2,f1,f2;
    printf("Highland V850_test entered\n");
      printf("V850adr=%x\n",V850adr);
      vmebase = V850adr;  /* set up vme access */
      sw_base = 0xdeaa0000;

/* ========== read board ID - should be 0xFEEE ======= */ 
     adr = (unsigned short *)(vmebase);
     k = *adr;
     if(k != 0xfeee) 
       { printf(" bad board id=%x should be 0xfeee\n",k);
         exit(0);
       }

/* ========== read the module type - should be 0x5943 ======= */ 
     adr = (unsigned short *)(vmebase+0x02);
     k = *adr;
     if(k != 0x5943) 
       { printf(" bad module type=%x should be 0x5943\n",k);
         exit(0);
       }

/* ==================== set DLY1 LO register ====================== */ 
     dly1=25.0;
     longtime=(unsigned long)(dly1/0.0390625);
     l1 = (longtime>>16);
     l2 = (longtime & 0xffff);
     adr = (unsigned short *)(vmebase+0x20);
     *adr= l1;
     adr = (unsigned short *)(vmebase+0x22);
     *adr= l2;
     
/* ==================== set DLY2 LO register ====================== */ 
     dly2=25.0;
     longtime=(unsigned long)(dly2/0.0390625);
     l1 = (longtime>>16);
     l2 = (longtime & 0xffff);
     adr = (unsigned short *)(vmebase+0x24);
     *adr= l1;
     adr = (unsigned short *)(vmebase+0x26);
     *adr= l2;
     
/* ==================== set DLY3 LO register ====================== */ 
     dly3=25.0;
     longtime=(unsigned long)(dly3/0.0390625);
     l1 = (longtime>>16);
     l2 = (longtime & 0xffff);
     adr = (unsigned short *)(vmebase+0x28);
     *adr= l1;
     adr = (unsigned short *)(vmebase+0x2a);
     *adr= l2;
     
/* ==================== set DLY4 LO register ====================== */ 
     dly4=25.0;
     longtime=(unsigned long)(dly4/0.0390625);
     l1 = (longtime>>16);
     l2 = (longtime & 0xffff);
     adr = (unsigned short *)(vmebase+0x2c);
     *adr= l1;
     adr = (unsigned short *)(vmebase+0x2e);
     *adr= l2;
     
/* ==================== set DLY5 LO register ====================== */ 
     dly5=25.0;
     longtime=(unsigned long)(dly5/0.0390625);
     l1 = (longtime>>16);
     l2 = (longtime & 0xffff);
     adr = (unsigned short *)(vmebase+0x30);
     *adr= l1;
     adr = (unsigned short *)(vmebase+0x32);
     *adr= l2;
     
/* ==================== set DLY6 LO register ====================== */ 
     dly6=25.0;
     longtime=(unsigned long)(dly6/0.0390625);
     l1 = (longtime>>16);
     l2 = (longtime & 0xffff);
     adr = (unsigned short *)(vmebase+0x34);
     *adr= l1;
     adr = (unsigned short *)(vmebase+0x36);
     *adr= l2;
     
/* ########################################################### */
/* ########################################################### */
/*                                                             */
/* these settings give ttl levels on the n0-n6 outputs         */
/* and nim levels on n0-n6  watch the scope termination        */
/*  50ohm to look at n0-n6 and not 50ohm for t0-t6             */
/*                                                             */
/* ########################################################### */
/* ########################################################### */

/* =================set waves ================================ */
     iwave = 0x22;
     adr = (unsigned short *)(vmebase+0x38);
     *adr= iwave;
     adr = (unsigned short *)(vmebase+0x3a);
     *adr= iwave;
     adr = (unsigned short *)(vmebase+0x3c);
     *adr= iwave;

/* ========== set the VOUT LO register to 0V ============== */ 
     adr = (unsigned short *)(vmebase+0x8);
     *adr= 0;
     
/* ========== set the VOUT HI register to 5V ============== */ 
     adr = (unsigned short *)(vmebase+0xa);
     *adr= 0xff00;

/* ========== set the trigger level to 2.5V =============== */ 
     adr = (unsigned short *)(vmebase+0xe);
     *adr= 0xff00;


/* ========== toggle XFER to latch values ================= */ 
     adr = (unsigned short *)(vmebase+0xc);
     *adr= 0x80;  /* ACTION Register */

/* ========== set the Control register  positive edge ===== */ 
     adr = (unsigned short *)(vmebase+0x10);
     *adr= 0x000;

/* ########################################################### */
/* ########################################################### */

     c1=20;
     c2=30;
     f1=0;
     f2=0;
     setime(c1,f1,c2,f2);
     dly1=25.0;
loop:
/* ==================== step through delay times on channel 1 */
     il=il+0x100;
     dly1=dly1+25.0;;
     longtime=(unsigned long)(dly1/0.0390625);
     l1 = (longtime>>16);
     l2 = (longtime & 0xffff);
     adr = (unsigned short *)(vmebase+0x20);
     *adr= l1;
     usleep(10);
     adr = (unsigned short *)(vmebase+0x22);
     *adr= l2;
     usleep(10);

/*   set chanel 2 500ns longer than chanel 1 to make channel 1 pulse longer */
     dly2=dly1+500.0;
     longtime=(unsigned long)(dly2/0.0390625);
     l1 = (longtime>>16);
     l2 = (longtime & 0xffff);
     adr = (unsigned short *)(vmebase+0x24);
     *adr= l1;
     usleep(10);
     adr = (unsigned short *)(vmebase+0x26);
     *adr= l2;

     adr = (unsigned short *)(vmebase+0xc);
     *adr= 0x80;  /* ACTION Register - XFR */ 
     usleep(10);
     
/*   send a pulse */
     stvme();
     usleep(1000);
     restim();


wait:
     adr = (unsigned short *)(vmebase+0x4);
     k = *adr;  /* STATUS Register */ 
     busy = (k>>8) & 0x3;
/*     printf(" STATUS Register=%x  busy=%x\n",k,busy); */
     if(busy != 0) goto wait;
     usleep(10);

     if(dly1>60000.0) dly1=0.0;
/*     printf("longtime=%x  l1=%x  l2=%x\n",longtime,l1,l2); */
     usleep(10000);
     goto loop;
     

     exit(0);
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

