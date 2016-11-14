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
    map_3in1();
    zone = 1;
    sector = 1;
    multi_low();
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
    tim2_set(tf);   /* fine */
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
      card = i;
      goto found;
    }
    printf("no card found\n");
    return(0);
found:
    printf("card %d found\n",card);
    send12_3in1(2,0,0);   /* s1=s2=s3=s4=0 */
    send4_3in1(1,0);   /* itr=0 */
    send4_3in1(0,0);   /* ire=0 */
    send4_3in1(3,0);   /* mse=0 */
/* ---------------- do logain 220pF -------------------------------- */
    printf("which capacitor 0=small 1=large\n");
    fscanf(stdin,"%d",&icap);
    if(icap == 0)
     { send4_3in1(4,1);   /* enable small capacitor */
       send4_3in1(5,0);   /* disable large capacitor */
       capacitor = 5.2;
     }
    if(icap == 1)
     { send4_3in1(4,0);   /* disable small capacitor */
       send4_3in1(5,1);   /* enable large capacitor */
       capacitor = 100.0;
     }
    printf("input charge\n");
    fscanf(stdin,"%lf",&charge);
    dacval = (charge * 1023.0 ) / (2.0*4.096 * capacitor);
    if(dacval > 1023.0) dacval = 1023.0;
    dac = (int) dacval;
    charge = ( 2.0 * 4.096 * capacitor *(double)dac ) / 1023.0;
    printf(" charge=%f  dacval=%f  dac=%d  cap=%f  icap=%d\n",
           charge,dacval,dac,capacitor,icap);
loop:      
        set_dac(dac);
        rtime();
	usec = 2;
        usleep(usec);
        inject();
        goto loop;
}  /* end of main */
inject()
  { tp_high();
    usec = 15;
    usleep(usec); 
    stime();
    usec = 2;
    usleep(usec);
/*    rtime(); */
  }
    double hstfit(i,l,x)
    int i,l;
    double x;
    {double y;
     y=0.0;
     return(y); 
    }  
