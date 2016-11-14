#include <stdio.h>
#include <math.h>
#include <time.h>
#include <types.h>
time_t usec;
main()
  { FILE *fp;
    int timc,timf,i,l;
    int zone,sector,card;
    double asum,axsum,axxsum,ped,dped,sig2,aval;
    int time1,time2,time3,time4;
    int dac,icap;
    double charge,dacval,capacitor;
    char xstring[80],ystring[80],string[80],err[80];
    unsigned short cadr,tc,tf;
    unsigned short sval,dval;
    map_3in1();  /* just needed for the RIO cpu */
    multi_low();  /* must start with this to find cards */
    zone = 1;
    sector = 1;
    card = 0;
    l = (zone<<12) | (sector<<6) | card;
    cadr = (unsigned short)l;
    card_sel(cadr);
    time1 = 0;
    time2 = 0;
    time3 = 0;
    time4 = 0;
    set_drawer_timer(zone,sector,1,time1);
    set_drawer_timer(zone,sector,2,time2);
    set_drawer_timer(zone,sector,3,time3);
    set_drawer_timer(zone,sector,4,time4);  
    timc=10;
    timf=100;
    tc = (unsigned short)timc;
    tim1_set(tc);   /* coarse */
    tf = (unsigned short)timf;
   for(i=0;i<49;i++)
    { card = i;
      l = (zone<<12) | (sector<<6) | card;
      cadr = (unsigned short)l;
      card_sel(cadr);
      sval = (unsigned short)0x555;
      wrt_shift(12,sval);
      read_shift(&dval);
      if(sval != (dval&0xfff) ) continue;
      sval = (unsigned short)0xaaa;
      wrt_shift(12,sval);
      read_shift(&dval);
      if(sval != (dval&0xfff) ) continue;
      send4_3in1(3,1);   /* mse = 1 <================ important line */
      set_dac(0);
      send4_3in1(4,0);   /* disable small capacitor */
      send4_3in1(5,1);   /* disable large capacitor */
      printf("found card %d\n",i);
    }
    multi_high();    /* <================== important line */
/* this stuff is just to put the integrators in some standard state */
    send12_3in1(2,0,0);   /* s1=s2=s3=s4=0 */
    send4_3in1(1,0);   /* itr=0 */
    send4_3in1(0,0);   /* ire=0 */
loop:
        for(dac=0;dac<1023;dac=dac+16)
          { set_dac(dac);
            usec = 100;
	    usleep(usec);
            inject();
	  }
	goto loop;
}  /* end of main */
inject()
  { rtime();
    tp_high();
    usec = 15;
    usleep(usec); 
    stime();
    usec = 2;
    usleep(usec);
  }
