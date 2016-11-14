
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <ces/vmelib.h>
int ttcadr,id,kk;
time_t usec = 1;
int sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { FILE *fp;
    char serial[7] = {"S10135\0"};
    int tim,i,l,k;
    int card,start,kl,board;
    int dac,icap;
    double charge,dacval,capacitor;
    kk = 0;
   ttcadr = 0x300a;
   board = 1;
   id = (board<<6) | 1;
   ttcvi_map(ttcadr);
   reset_ctrl_can();
   printf("calling can_init\n");
   can_reset(ttcadr);  
   can_init(board,serial);  
   set_mb_time(ttcadr,1,0);
   set_mb_time(ttcadr,2,0);
   set_mb_time(ttcadr,3,0);
   set_mb_time(ttcadr,4,0);
   start=0;
   if(argc == 2)
     { sscanf(argv[1],"%d",&start);
       printf("start at %d requested\n",start);
     }
   for(kl=start;kl<49;kl++) 
       { if(cardchk(kl) == 1) 
         { printf(" card %d found",kl);
            card = kl;
	    goto strt;
	 }
       }
    printf("no card exit\n");
    exit(0);  
strt:
   printf(" enter icap 0=trig 1=small 2=large\n");
   fscanf(stdin,"%x",&icap);  
   printf("icap = %x\n",icap);  
    set_tube(ttcadr,card);
    multi_low(ttcadr);
    rxw_low(ttcadr);
    intg_rd_low(ttcadr);
    itr_low(ttcadr);
    set_intg_gain(ttcadr,0);  /* s1=s2=s3=s4=0 */
    mse_low(ttcadr);         /* mse=0 */
    trig_enable(ttcadr);
    SetTimeDes2(ttcadr,116);
    if(icap == 2 || icap == 0)
      {capacitor = 100.0;
       large_cap_enable(ttcadr);
       small_cap_disable(ttcadr);
       charge = 800.0;
        printf(" enter charge\n");
        fscanf(stdin,"%lf",&charge);
	printf("charge=%f\n",charge);  
      }
    if(icap == 1)
       {capacitor = 5.2;
        large_cap_disable(ttcadr);
        small_cap_enable(ttcadr);
        charge = 10.0;
        printf(" enter charge\n");
        fscanf(stdin,"%lf",&charge);
	printf("charge=%f\n",charge);  
       }
	kk = icap;
    dacval = (charge * 1023.0 ) / (2.0 * 4.096* capacitor);
    if(dacval > 1023.0) dacval = 1023.0;
    dac = (int) dacval;
    set_dac(ttcadr,dac);
    charge = ( 2.0 * 4.096 * capacitor *(double)dac ) / 1023.0;
    printf(" charge=%f  dacval=%f  dac=%x(hex)  cap=%f\n",
           charge,dacval,dac,capacitor);
loop:
       inject();
   }  /* end of main */

     inject()
       { static int entry=0;
         int c1,c2,f1,f2,swtst,i,i1,i2,i0time,i3time;
	 double a;
       /* setup inhibit1 to set tp high */
	     SetBMode(1,0xa);
	     SetInhibitDelay(1,10);
	     SetInhibitDuration(1,10);
	     BPutLong(1,ttcadr,1,0xc0,0x00);   /* load set tp_high into fifo1 */
	  /*   BPutShort(2,0xc1); /*  /* set tp high bcast command  */
	     /* set inhibit2 to drop tp low (timed broadcast mode */
	     SetBMode(2,0xa);
	     SetInhibitDelay(2,2000);
	     SetInhibitDuration(2,10);
	  /* BPutLong(2,ttcadr,1,0xc0,0x01);  */  /* load set tp_low into fifo2 */
	     BPutShort(2,0xc0);   /* set tp low bcast command  */
	     /* set inhibit3 to set latch                     */
	     /* use latch-bar and inhibit3 to start fermi     */
             /* will use inhibit 3 to set l1a with digitizers */
             i3time=2010;
 	  /*   printf("input i3time\n");
             fscanf(stdin,"%d",&i3time);    */
	     SetInhibitDelay(3,i3time);
	     SetInhibitDuration(3,10);
	     SetInhibitDelay(0,1);
	     SetInhibitDuration(3,10);
  }
cardchk(k)
int k;
  {int i,status;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,ok;
   ok=0;
   status=0;
   multi_low(ttcadr);
   back_low(ttcadr);
   set_tube(ttcadr,k);
   set_intg_gain(ttcadr,5);   /* s1=s3=1 s2=s4=0 */
   itr_high(ttcadr);           /* itr=1 */
   intg_rd_low(ttcadr);        /* ire=0 */
   mse_high(ttcadr);           /* mse=1 */
   small_cap_disable(ttcadr);  /* tplo=0 */
   large_cap_enable(ttcadr);   /* tphi=1 */
   trig_disable(ttcadr);       /* trig=0 */
   back_high(ttcadr);
   back_low(ttcadr);
   load_can(ttcadr);   /* fetch from 3in1 */
   usleep(20);
   status=can_3in1get(id,&shrt);    /* and load can output reg */
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
   if((s1==1) && (s2==0)  && (s3==1) && (s4==0) && (itr==1) &&
      (ire==0) && (mse==1) && (tplo==0) && (tphi==1) && 
      (trig==0) ) ok++;
   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x %d\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i,ok);

   set_intg_gain(ttcadr,0xa);   /* s1=s3=0 s2=s4=1 */
   itr_low(ttcadr);           /* itr=0 */
   intg_rd_high(ttcadr);        /* ire=1 */
   mse_low(ttcadr);           /* mse=0 */
   small_cap_enable(ttcadr);  /* tplo=1 */
   large_cap_disable(ttcadr);   /* tphi=0 */
   trig_enable(ttcadr);       /* trig=1 */
   back_high(ttcadr);
   back_low(ttcadr);
   load_can(ttcadr);   /* fetch from 3in1 */
   usleep(20);
   status=can_3in1get(id,&shrt);    /* and load can output reg */
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
   if((s1==0) && (s2==1)  && (s3==0) && (s4==1) && (itr==0) &&
      (ire==1) && (mse==0) && (tplo==1) && (tphi==0) && 
      (trig==1) ) ok++;
   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x %d\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i,ok);
  if(ok == 2) status=1;
   return(status);
  }
