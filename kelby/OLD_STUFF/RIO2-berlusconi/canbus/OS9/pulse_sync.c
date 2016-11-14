#include <stdio.h>
#include <math.h>
main()
{ 
   int zone,sector,tube,mode,i,phase,dac;
   double fdac,charge,capacitor;
   int icap;
   ttcvi_map();  /* set up the ttcvi and ttcrx */
   zone = 0;
   sector = 0;
   set_sector(sector);
   set_zone(zone);
   for(tube=0;tube<49;tube++)
    { set_tube(tube);
      large_cap_disable();
      small_cap_disable();
    }
   set_swln2(1);  /* turn on the reference voltage */
   set_swln1(0);  /* turn off the second ref voltage source */

   set_mb_time(1,0); /* set motherboard time delays */
   set_mb_time(2,0);
   set_mb_time(3,0);
   set_mb_time(4,0);

   tube = 45;
   set_tube(tube);

   printf(" enter capacitor  0=small 1=large\n");
   fscanf(stdin,"%d",&icap);
   capacitor = 100.0;
   large_cap_enable();
   small_cap_disable();
   if(icap==0)
     { capacitor = 5.1;
       large_cap_disable();
       small_cap_enable();}

   printf(" enter charge\n");
   fscanf(stdin,"%lf",&charge);
   fdac = (1023.0*charge) / (2.0 * 4.096 * capacitor);
   dac = (int)fdac;
   charge = 2.0 * 4.096 * capacitor * (double)dac / 1023.0;
   printf("logain charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
   set_dac(dac);
   tsleep(0x80000010);
   tp_low();
   printf(" enter clock phase 0-239\n");
   fscanf(stdin,"%d",&phase);
   SetTimeDes2(0,phase);  
loop:
   inject();
   if(sw(8) != 1) goto loop;
}
     inject()
       { static int entry=0;
         int c1,c2,f1,f2,swtst,i,i1,i2,i0time,i3time;
         if(entry == 0)
	   {/* setup inhibit1 to set tp high */
	     SetBMode(1,0xe);
	     SetInhibitDelay(1,10);
	     SetInhibitDuration(1,2);
	     BPutLong(1,0,1,0x80,0x00);  /* load set tp_high into fifo1 */
	     /* set inhibit2 to drop tp low (timed broadcast mode */
	     SetBMode(2,0xe);
	     SetInhibitDelay(2,2000);
	     SetInhibitDuration(2,2);
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
	     /* setup pulser */
	     c1=1;
             c2=1;
             f1=5;
             f2=5; 
             setime(c1,f1,c2,f2);
	     restim();  /* reset the pulser */
             rlatch();  /* reset the latch bit */
     	     entry = 1;
	    }
loop:      stvme();  /* send pulses to clear event gate generator */
           tsleep(0x80000002);
           swtst = latch();  /* wait for the latch bit to set */
           if(swtst == 0) 
             { tsleep(0x80000002);
               goto loop;
             }
           restim();  /* reset the pulser for next trigger */
           rlatch();    /* reset the latch bit */
  }
