/* ------------------------------------------------------------------
    This is a small program to pulse all cards alternating between
    a large and half height pulse.
----------------------------------------------------------------------  */
#include <stdio.h>

main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{ 
   int zone,sector,tube,mode,i,phase,dac,hdac;
   double fdac,charge,capacitor;
   int icap,card,k;
   ttcvi_map();   /* initialize the TTCvi */
   set_TTCadr(0x1234);
   set_digitizer();  /* set up digitizer */
   zone = 0;
   sector = 0;
/*   printf(" enter zone sector tube\n");
   fscanf(stdin,"%d %d %d",&zone,&sector,&tube);  */
   set_zone(zone);
   set_sector(sector);
   multi_low();
   rxw_low();
   back_low();
/* initialize the drawer delays */
   set_mb_time(1,0);
   set_mb_time(2,0);
   set_mb_time(3,0);
   set_mb_time(4,0);

/* loop through cards and set up for standard data taking */
   can_init();    /* initialize the Can bus - TIP810 */
   for(k=1;k<49;k++)
     { 
         if(card_init(k)==0) 
              printf(" not card at tube slot %d\n",k);
         else
              printf(" card %d set up OK\n",k);
     }
   can_clear();    /* free up the can bus */

/* now set to do commands in parallel */
   multi_high();
/* now setup to inject charge */
   printf(" enter capacitor  0=small 1=large\n");
   fscanf(stdin,"%d",&icap);
   capacitor = 100.0;
   large_cap_enable();
   small_cap_disable();
   if(icap==0)
     { capacitor = 5.1;
       large_cap_disable();
       small_cap_enable(); }
   charge=700.0;
   printf(" enter charge  0-800 large C   0-40 small C\n");
   fscanf(stdin,"%lf",&charge);
   fdac = (1023.0*charge) / (2.0 * 4.096 * capacitor);
   dac = (int)fdac;
   hdac = dac/2;
   charge = 2.0 * 4.096 * capacitor * (double)dac / 1023.0;
   printf("logain charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
   set_dac(dac);
   tp_low();
/* injection delay relative to the clock edge - it is circular */
/* scan the phase to center the injection on the adc sample timing */
/* one can also hold this fixed and vary the digitizer clock phase */
   printf(" enter clock phase 0-239\n");
   fscanf(stdin,"%d",&phase);
   SetTimeDes2(0x1234,phase);  
   printf(" hdac=%d\n",hdac);
   set_dac(dac);        
   inject_start();      /* start the charge injection once per orbit     */
                        /*  till ResetFIFO(1,2,and 3) are called         */
                        /*  FIFO 1 must be cleared before FIFO 2         */
                        /*  or the injection is left open - a no no      */
                        /*  safest to call tp_low() after FIFO;s cleared */

loop:
      set_dac(dac);        
      sleep(1);
      set_dac(hdac);
      sleep(1);
     goto loop;
/*     printf(" calling inject_stop\n");
     inject_stop(); */     
}
/* ------------------------------------------------------------------------
 this code sets up the ttc system for continuous injections once
   per orbit 
           small_cap_disable();
           large_cap_disable();
   prevent the pulsing from having an effect

           small_cap_enable();
   or
           large_cap_enable();
   lets the pulse through 
--------------------------------------------------------------------------- */
     inject_start()
       { int c1,c2,f1,f2,swtst,i,i1,i2,i0time,i3time;
         /* setup inhibit1 to set tp high */
	     SetBMode(1,0xc);
	     SetInhibitDelay(1,10);
	     SetInhibitDuration(1,10);
	     BPutLong(1,0,1,0x80,0x00);  /* load set tp_high into fifo1 */
	 /* set inhibit2 to drop tp low (timed broadcast mode */
	     SetBMode(2,0xc);
	     SetInhibitDelay(2,1900);
	     SetInhibitDuration(2,10);
	     BPutLong(1,0,1,0x80,0x01);   /* load set tp_low into fifo1 */
	     /* BPutShort(2,0xc0); */   /* set tp low bcast command  */
	 /* set inhibit3 to set latch                     */
	 /* use latch-bar and inhibit3 to start fermi     */
         /* will use inhibit 3 to set l1a with digitizers */
             i3time=2025;
	     printf("input i3time\n");
             fscanf(stdin,"%d",&i3time);     
	     SetInhibitDelay(3,i3time);
	     SetInhibitDuration(3,10);
	     SetInhibitDelay(0,1);
	     SetInhibitDuration(3,10);
 }

inject_stop()
  { SetInhibitDuration(1,0);
    SetInhibitDuration(2,0);
    SetInhibitDuration(2,0);
    tp_low();
  }    

/* ------------------------------------------------------------------
   setup card k for standard data taking with mse=1 to allow parallel
   commands to execute when multi_high() is set 
   ------------------------------------------------------------------- */ 
card_init(k)
int k;
  {int i,status;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
   status=0;
   set_tube(k);
   set_intg_gain(0);   /* s1=s3=1 s2=s4=0 */
   itr_low();           /* itr=0 */
   intg_rd_low();        /* ire=0 */
   mse_high();           /* mse=1 */
   small_cap_disable();  /* tplo=0 */
   large_cap_disable();   /* tphi=0 */
   trig_enable();       /* trig=1 */
   can_bkld_high();
   can_bkld_low();
   status=can_3in1set(0x9c00);   /* fetch from 3in1 and load can output reg */
   status=can_3in1get(&shrt);
   i = (int)shrt;
   trig = i & 1;
   s1 = (i>>1) & 1;
   s2 = (i>>2) & 1;
   s3 = (i>>3) & 1;
   s4 = (i>>4) & 1;
   itr = (i>>5) & 1;
   ire = (i>>6) & 1;
   mse = (i>>7) & 1;
   tplo = (i>>8) & 1;
   tphi = (i>>9) & 1;
   if((s1==0) && (s2==0)  && (s3==0) && (s4==0) && (itr==0) &&
      (ire==0) && (mse==1) && (tplo==0) && (tphi==0) && 
      (trig==1) ) status=1;
   return(status);
  }
set_digitizer()
  {
     SendLongCommand(0,1,0x01,0x0f);
     SendLongCommand(0,1,0x02,100);
     SendLongCommand(0,1,0x08,170);
     SendLongCommand(0,1,0x09,170);
     SendLongCommand(0,1,0x0a,0x18);
     SendLongCommand(0,1,0x01,0x0f);
     SendLongCommand(0,1,0xf,8);
     SendLongCommand(0,1,0xf,0);
     BroadcastShortCommand(0x40);
     
  }
