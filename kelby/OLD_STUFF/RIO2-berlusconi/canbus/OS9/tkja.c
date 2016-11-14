#include <stdio.h>
#include <math.h>
main()
  { unsigned short shrt;
    int zone,sector,tube,l,i,phase;
    int dac,time1,time2,time3,time4;
    int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
    double fdac,charge,capacitor;
    ttcvi_map();
    phase = 116;
    SetTimeDes2(0,phase);  
    zone=0;
    sector=0;
    set_zone(zone);
    set_sector(sector);
    time1 = 0;
    time2 = 0;
    time3 = 0;
    time4 = 0;
    set_mb_time(1,time1);
    set_mb_time(2,time2);
    set_mb_time(3,time3);
    set_mb_time(4,time4);
    multi_low();
    rxw_low();
    for(tube=1;tube<49;tube++)
       { set_tube(tube);
         intg_rd_low();
         itr_low();
         set_intg_gain(0xf);
         trig_enable();
         small_cap_disable();
         large_cap_enable();
	 mse_high();
         set_dac(0);
         can_bkld_high();
         can_bkld_low();
         can_set(0x9c,0);
         can_shft_rd(&shrt);
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
         if(trig == 1)
         printf("tube=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,tube,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i); 
      }
loop:
    set_tube(0);
    multi_high();
/*    printf("set dac 511\n");  */
    set_dac(511);   
    inject();
    sleep(1);
/*    printf("set dac 1023\n");  */
    set_dac(1023);   
    inject();
    sleep(1);
    goto loop;
}  /* end of main */
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
loop:      stvme();  /* send pulses from the switch box to clear last event */
           tsleep(0x80000005);
	   swtst = latch();  /* wait for the latch bit to set */
/*           printf("swtst=%d\n",swtst);  */
           if(swtst == 0) goto loop; 
           restim();  /* reset the timer for next trigger */
           rlatch();    /* reset the latch bit */
  }
