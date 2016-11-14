#include <stdio.h>
#include <math.h>
#include <types.h>
#include <time.h>
time_t usec;
main()
{ 
   int zone,sector,tube,mode;
   ttcvi_map();
   printf(" enter zone sector tube\n");
   fscanf(stdin,"%d %d %d",&zone,&sector,&tube);
   set_zone(zone);
   set_sector(sector);
   set_tube(tube);
   set_swln2(1);
   set_swln1(0);
   usleep(10);
   large_cap_enable();
   usleep(10);
   small_cap_disable();
   usleep(10);
   set_dac(0x3ff);
   usleep(10);
   tp_low();
loop:
   inject();
   usleep(1000);
   if(sw(1) != 1) goto loop;
}
     inject()
       { static int entry=0;
         int c1,c2,f1,f2,swtst,i,i1,i2;
         if(entry == 0)
	   { /* setup inhibit1 to set tp high */
	     SetBMode(1,0xc);
	     SetInhibitDelay(1,10);
	     SetInhibitDuration(1,10);
	     BPutLong(1,0,1,0x80,0x00);  /* load set tp_high into fifo1 */
	     /* set inhibit2 to drop tp low (timed broadcast mode */
	     SetBMode(2,0xc);
	     SetInhibitDelay(2,4000);
	     SetInhibitDuration(2,10);
	    /* BPutLong(2,0,1,0x80,0x01);  */ /* load set tp_low into fifo2 */
	     BPutShort(2,3);   /* set tp low bcast command  */
	     /* set inhibit3 to set latch                     */
	     /* use latch-bar and inhibit3 to start fermi     */
             /* will use inhibit 3 to set l1a with digitizers */
	     SetInhibitDelay(3,4000);
	     SetInhibitDuration(3,10);
	     /* setup the switch box timers */
	     c1=1;
             c2=1;
             f1=5;
             f2=5; 
             setime(c1,f1,c2,f2);
	     restim();
	     rlatch();
	     entry = 1;
	    }
	 /* send pulse out switch box through fanout and
	          into b_go1, b_go2, and b_go3 inputs
            send inhibit1 output into the switch box latch
	    send inhibit3 output into l1a0 or fermi start input */  

           tp_high();
loop:
	   stvme();  /* send pulses from the switch box */
           usleep(5);
	   swtst = latch();  /* wait for the latch bit to set */
           if(swtst == 0) goto loop;
           restim();  /* reset the timer for next trigger */
           rlatch();  /* reset the latch bit */
           usleep(1);
      }
      


