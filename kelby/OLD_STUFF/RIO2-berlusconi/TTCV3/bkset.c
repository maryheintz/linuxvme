#include <stdio.h>
#include <math.h>
#include <time.h>
#include <ces/vmelib.h>

#include <sys/types.h>
#include <unistd.h>
#include <mem.h>


#include "ttcvi.h"
int ttcadr,id;
time_t usec = 1;
int code;
static int nsamp;
/* ttcvi parameter */
static char *ttcvi_address;
static int icap;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { int tim,i,l,k,kkk,lll,pipe,drawer,dig,kk,tube;
    int dac,alarm;
    double charge,dacval,capacitor;
    char xstring[80],ystring[80],string[80],err[80];
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    unsigned short *adr;
    tube=0;
    kk = 0;
/*    printf("input 0=600pC  1=7pC\n ");
    fscanf(stdin,"%d",&icap);  */
    nsamp = 6;
loop4:
    icap = 1;
    id=1;

    usleep(2);
    ttcadr=0x300a;
    ttcvi_map(ttcadr);

    set_mb_time(ttcadr,1,156);
    set_mb_time(ttcadr,2,104);
    set_mb_time(ttcadr,3,92);
    set_mb_time(ttcadr,4,40);

    set_mb_time(ttcadr,1,156);
    set_mb_time(ttcadr,2,104);
    set_mb_time(ttcadr,3,92);
    set_mb_time(ttcadr,4,40);

    multi_low(ttcadr);
    rxw_low(ttcadr);
    for(k=1;k<11;k++)
      {set_tube(ttcadr,k);
       intg_rd_low(ttcadr);
       itr_low(ttcadr);
       set_intg_gain(ttcadr,0);  /* s1=s2=s3=s4=0 */
       trig_enable(ttcadr);
       large_cap_disable(ttcadr);
       small_cap_enable(ttcadr);
       mse_low(ttcadr);         
       charge = 0.5;
       capacitor = 5.2;
       dacval = (charge * 1023.0 ) / (2.0 * 4.096* capacitor);
       if(dacval > 1023.0) dacval = 1023.0;
       dac = (int) dacval;
       charge = ( 2.0 * 4.096 * capacitor *(double)dac ) / 1023.0;
       set_dac(ttcadr,dac);
      }
    
    SetTimeDes2(ttcadr,216);
 
/*	inject_setup();  */
  }  /* end of main */
   
   
     inject_setup()
       { static int entry=0;
         int c1,c2,f1,f2,swtst,i,i1,i2,i0time,i3time;
	 double a;
	 unsigned short *adr;
/* set ready low */
      (unsigned short *)adr = (unsigned short *)0xdede0002;
      *adr = 0;
             usleep(100);
             ResetFIFO(0);
             ResetFIFO(1);
             ResetFIFO(2);
             ResetFIFO(3);
	     SetBMode(0,0xa);
	     SetBMode(1,0xa);
	     SetBMode(2,0xa);
	     BPutShort(0,1);   /* send orbit into bgo(0) reset bunch cntr */
	     BPutShort(2,0xc4);   /* set tp high bcast command  */
	     BPutShort(2,0xc0);   /* set tp low bcast command  */
        /* setup inhibits */
	     SetInhibitDelay(0,1);
	     SetInhibitDuration(0,2);
	     SetInhibitDelay(1,1940);
	     SetInhibitDuration(1,2);
	     SetInhibitDelay(2,1990);
	     SetInhibitDuration(2,2);
/* ================= fan in inh1 and inh2 and send output to bgo2 ======*/	     
	     /* set inhibit3 to set L1A2                     */
             i3time=2048;
	   /*  printf("input i3time\n");
	     fscanf(stdin,"%d",&i3time);  */   
	     SetInhibitDelay(3,i3time);
	     SetInhibitDuration(3,2);
	     sleep(1);
 /* set ready high */
      (unsigned short *)adr = (unsigned short *)0xdede0000;
      *adr = 0;

	     /* setup the switch box timers */
	     c1=1;
             c2=1;
             f1=5;
             f2=5; 
             setime(c1,f1,c2,f2);
	     restim();
             rlatch();
	    }
	     

