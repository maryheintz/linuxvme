/* ------------------------------------------------------------------
    This is a small program to pulse all cards alternating between
    a large and half height pulse.
    tsleep(0X80000002) -  this is a delay of about 2usec.  It takes
    about 2.5usec to execute a TTC command that sends data to a
    3in1 card.  The TTC system has no flow control so you must
    wait or commands will be ignored if the system is busy.
    See ttcvi.c for low level TTCvi commands
    See lib3in1.c for higher level commands                  
----------------------------------------------------------------------  */
#include <stdio.h>
#include <math.h>
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{ 
   int zone,sector,tube,mode,i,phase,dac,hdac;
   double fdac,charge,capacitor;
   int icap,card,k;
   ttcvi_map();   /* initialize the TTCvi */
   can_init();    /* initialize the Can bus - TIP810 */
   zone = 0;
   sector = 0;
/*   printf(" enter zone sector tube\n");
   fscanf(stdin,"%d %d %d",&zone,&sector,&tube);  */
   set_zone(zone);
   tsleep(0x80000002);
   set_sector(sector);
   tsleep(0x80000002);
   multi_low();
   tsleep(0x80000002);
   rxw_low();
   tsleep(0x80000002);
   back_low();
   tsleep(0x80000002);
/* initialize the drawer delays */
   set_mb_time(1,0);
   set_mb_time(2,0);
   set_mb_time(3,0);
   set_mb_time(4,0);

/* loop through cards and set up for standard data taking */
   for(k=1;k<49;k++)
     { 
         if(card_init(k)==0) 
              printf(" not card at tube slot %d\n",k);
         else
              printf(" card %d set up OK\n",k);
     }

/* now set to do commands in parallel */
   multi_high();
   tsleep(0x80000002);

/* now setup to inject charge */
   printf(" enter capacitor  0=small 1=large\n");
   fscanf(stdin,"%d",&icap);
   capacitor = 100.0;
   large_cap_enable();
   tsleep(0x80000002);
   small_cap_disable();
   tsleep(0x80000002);
   if(icap==0)
     { capacitor = 5.1;
       large_cap_disable();
       tsleep(0x80000002);
       small_cap_enable();
       tsleep(0x80000002);}
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
   tsleep(0x80000005);
   tp_low();
/* injection delay relative to the clock edge - it is circular */
/* scan the phase to center the injection on the adc sample timing */
/* one can also hold this fixed and vary the digitizer clock phase */
   printf(" enter clock phase 0-239\n");
   fscanf(stdin,"%d",&phase);
   SetTimeDes2(0,phase);  
   printf(" hdac=%d\n",hdac);
loop:
   set_dac(dac);
   tsleep(0x80000005);
   inject();
   sleep(1);
   set_dac(hdac);
   tsleep(0x80000005);
   inject();
   sleep(1);

   if(sw(8) != 1) goto loop;
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
     inject()
       { static int entry=0;
         int c1,c2,f1,f2,swtst,i,i1,i2,i0time,i3time;
         if(entry == 0)
	   {/* setup inhibit1 to set tp high */
	     SetBMode(1,0xc);
	     SetInhibitDelay(1,10);
	     SetInhibitDuration(1,10);
	     BPutLong(1,0,1,0x80,0x00);  /* load set tp_high into fifo1 */
	     /* set inhibit2 to drop tp low (timed broadcast mode */
	     SetBMode(2,0xc);
	     SetInhibitDelay(2,2000);
	     SetInhibitDuration(2,10);
	    /* BPutLong(2,0,1,0x80,0x01);  */ /* load set tp_low into fifo2 */
	     BPutShort(2,0xc0);   /* set tp low bcast command  */
	     /* set inhibit3 to set latch                     */
	     /* use latch-bar and inhibit3 to start fermi     */
             /* will use inhibit 3 to set l1a with digitizers */
             i3time=2025;
/*	     printf("input i3time\n");
             fscanf(stdin,"%d",&i3time);     */
	     SetInhibitDelay(3,i3time);
	     SetInhibitDuration(3,10);
	     SetInhibitDelay(0,1);
	     SetInhibitDuration(3,10);
	     /* setup programable pulser and latch module */
             /* the latch bit signals a level 1 event */
             /* the pulser output clears a gate generator after the
                event is processed to allow another event through
                this needs to be replaced by corbo logic or whatever */
	     c1=1;
             c2=1;
             f1=5;
             f2=5; 
             setime(c1,f1,c2,f2);  /* sets pulser delay and duration */
	     restim();  /* clears the pulser */
             rlatch();  /* reset the latch bit */  
     	     entry = 1;
	    }
           tsleep(0x80000005);
loop:      stvme();  /* send pulses to clear the event gate */
           tsleep(0x80000002);
           swtst = latch();  /* wait for the latch bit to set */
           if(swtst == 0) 
             { tsleep(0x80000002);
               goto loop;
             }
           restim();  /* reset the pulser */
           rlatch();    /* reset the latch bit */
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
   tsleep(0x80000002);
   set_intg_gain(0);   /* s1=s3=1 s2=s4=0 */
   tsleep(0x80000002);
   itr_low();           /* itr=0 */
   tsleep(0x80000002);
   intg_rd_low();        /* ire=0 */
   tsleep(0x80000002);
   mse_high();           /* mse=1 */
   tsleep(0x80000002);
   small_cap_disable();  /* tplo=0 */
   tsleep(0x80000002);
   large_cap_disable();   /* tphi=0 */
   tsleep(0x80000002);
   trig_enable();       /* trig=1 */
   tsleep(0x80000002);
   can_bkld_high();
   tsleep(0x80000002);
   can_bkld_low();
   tsleep(0x80000002);
   status=can_3in1set(0x9c00);   /* fetch from 3in1 and load can output reg */
   tsleep(0x80000005);
   status=can_3in1get(&shrt);
   tsleep(0x80000005);
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
