#include <stdio.h>
#include <math.h>
#include <time.h>
#include <types.h>
#include <sys/timers.h>
#include <ces/vmelib.h>
static struct pdparam_master param=
      { 1,   /* VME Iack (must be 1 for all normal pages) */
        0,   /* VME Read Prefetch Option */
        0,   /* VME Write Posting Option */
        1,   /* VME Swap Option */
        0,   /* page number return */
        0,   /* reserved */
        0,   /* reserved */
        0 };
unsigned long *swtch, *adr;
time_t usec = 1;
main()
  { int zone,sector,tube,l;
    int dac,time1,time2,time3,time4;
    double fdac,charge,capacitor;
    l = sw(0); /* initialize switch reading */
    ttcvi_map();
    init_old();
    zone=0;
    sector=0;
    set_zone(zone);
    set_sector(sector);
    set_swln2(1);
    set_swln1(0);
    time1 = 0;
    time2 = 0;
    time3 = 0;
    time4 = 0;
    set_mb_time(1,time1);
    set_mb_time(2,time2);
    set_mb_time(3,time3);
    set_mb_time(4,time4);
    capacitor = 100.0;
    charge=800.0;
    fdac = (1023.0*charge) / (2.0 * 4.096 * capacitor);
    dac = (int)fdac;
    charge = 2.0 * 4.096 * capacitor * (double)dac / 1023.0;
    printf("logain charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    for(tube=37;tube<49;tube++)
       { set_tube(tube);
         multi_low();
         rxw_low();
         intg_rd_low();
         itr_low();
         trig_enable();
         small_cap_disable();
         usleep(10);
         large_cap_enable();
         usleep(10);
	 mse_high();
         set_dac(dac);
         usleep(10);
       }
    charge=800.0;
    fdac = (1023.0*charge) / (2.0 * 4.096 * capacitor);
    dac = (int)fdac;
    multi_high();
    set_dac(dac);   
loop:
    while(sw(8) == 0) inject();
}  /* end of main */
     inject()
       { static int entry=0;
         int c1,c2,f1,f2,swtst,i,i1,i2,i3time;
         if(entry == 0)
	   { /* setup inhibit1 to set tp high */
	     SetBMode(1,0xc);
	     SetInhibitDelay(1,10);
	     SetInhibitDuration(1,10);
	     BPutLong(1,0,1,0x80,0x00);  /* load set tp_high into fifo1 */
	     /* set inhibit2 to drop tp low (timed broadcast mode */
	     SetBMode(2,0xc);
	     SetInhibitDelay(2,3500);
	     SetInhibitDuration(2,10);
	    /* BPutLong(2,0,1,0x80,0x01);  */ /* load set tp_low into fifo2 */
	     BPutShort(2,0xc0);   /* set tp low bcast command  */
	     /* set inhibit3 to set latch                     */
	     /* use latch-bar and inhibit3 to start fermi     */
             /* will use inhibit 3 to set l1a with digitizers */
             i3time=3545;
/*	     printf("input i3time\n");
             fscanf(stdin,"%d",&i3time);  */
	     SetInhibitDelay(3,i3time);
	     SetInhibitDuration(3,1);
	     /* setup the switch box timers */
	     c1=1;
             c2=1;
             f1=5;
             f2=5; 
             setime(c1,f1,c2,f2);
	     restim();
	     rlatch_old();  
	     entry = 1;
	    }
	 /* send pulse out switch box through fanout and
	          into b_go1, b_go2, and b_go3 inputs
            send inhibit1 output into the switch box latch
	    send inhibit3 output into l1a0 or fermi start input */  

	   stvme();  /* send pulses from the switch box */
loop:
	   swtst = latch_old();  /* wait for the latch bit to set */
           if(swtst == 0) goto loop; 
           restim();  /* reset the timer for next trigger */
           rlatch_old();    /* reset the latch bit */
	   trigger_old();
      }
sw(i)
  int i;
  {  unsigned short *adr,k;
     int shft,l;
     (unsigned short *)adr = (unsigned short *)0xdeaa000c;
      k = *adr;
        shft=i-1;
        l=(k>>shft)&1;
	return(l); 
  }      
kjasleep(i)
int i;
{int k;
 double d,f=2.0;
 for (k=0;k<i;k++)
   { d = sqrt(f);}
}
  init_old()
  { unsigned long am,vmead,offset,size,len,k;
    int l,shft;
       am=0x39;
       vmead=0xcf0000;
       offset=0;
       size=0;
       len=400;
       swtch = 
         (unsigned long *)( find_controller(vmead,len,am,offset,size,&param) );
       if( swtch == (unsigned long *)(-1) ) 
        { printf("unable to map vme address\n");
          exit(0);}
       return(0);	  
  }
   latch_old()
     { unsigned long k;
       int l;
       k = *swtch;
       l = k & 1;
       return(l);
     }
   rlatch_old()
     { unsigned long k;
       k = (unsigned long)swtch | 8;
       (unsigned long *)adr = (unsigned long *)k;
       *adr = 0;
     }
   trigger_old()
     { unsigned long k;
       k = (unsigned long)swtch | 4;
       (unsigned long *)adr = (unsigned long *)k;
       *adr = 0;
     }
