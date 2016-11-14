#include <stdio.h>
#include <math.h>
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{ 
   int zone,sector,tube,mode,i,phase,dac;
   double asum,axsum,axxsum,ped,dped,sig2,aval;
   char string[80];
   double fdac,charge,capacitor;
   int icap,card,k;
   ttcvi_map();
   zone = 0;
   sector = 0;
/*   printf(" enter zone sector tube\n");
   fscanf(stdin,"%d %d %d",&zone,&sector,&tube);  */
   set_zone(zone);
   set_sector(sector);
   multi_low();
   set_mb_time(1,0);
   set_mb_time(2,0);
   set_mb_time(3,0);
   set_mb_time(4,0);
   if(argc != 2)
    {for(k=0;k<49;k++)
       { if(cardchk(k))
          { card = k;
            goto found;}
       }
   printf("no card found\n");
   return(0);
found:
   printf("card found = %d\n",card);
   set_tube(card);
   }
   if(argc == 2)
     { sscanf(argv[1],"%d",&card);
       printf("card %d requested\n",card);
       set_tube(card);
     }
   printf(" enter capacitor  0=small 1=large\n");
   fscanf(stdin,"%d",&icap);
   set_intg_gain(0);
   tsleep(0x80000002);
   itr_low();
   tsleep(0x80000002);
   intg_rd_low();
   tsleep(0x80000002);
   trig_enable();
   tsleep(0x80000002);
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
   pstatus();
loop:
   inject();
   if(sw(8) != 1) goto loop;
}
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
             rlatch();  /* reset the latch bit */  
     	     entry = 1;
	    }
           tsleep(0x80000005);
loop:      stvme();  /* send pulses from the switch box to clear last event */
           tsleep(0x80000002);
           swtst = latch();  /* wait for the latch bit to set */
           if(swtst == 0) 
             { tsleep(0x80000002);
               goto loop;
             }
           restim();  /* reset the timer for next trigger */
           rlatch();    /* reset the latch bit */
  }
pstatus()
  {unsigned short shrt;
   int i,trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,status;

       back_high();
       back_low();
       load_can();  /* fetch from 3in1 and load can output reg */
       tsleep(0x80000002);
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
       printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
  }