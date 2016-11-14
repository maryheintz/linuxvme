#include <stdio.h>
#include <math.h>
#include "vme_base.h"
#include "cberr.h"
char adcno[7];
double crg[6500],fvl[6500];
int id,ttcadr;
int i3time,ent;
unsigned short *reg;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{  FILE *fp;
   unsigned char wd,*adr;
   int tube,i,k,board,idac,pass,errors,ierr,retry,rsum;
   unsigned int i1,i2,itst;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
   long status;
   int good,bad,total,kkk;
   int phase,dac,card;
   double asum,axsum,axxsum,ped,dped,sig2,aval;
   char string[80];
   double fdac,charge,capacitor;
   int icap;
   status = vme_a24_base();
   status = vme_a16_base();
/*   
   k = vme16base + 0x6081;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 01 wd=%x\n",wd);
   
   k = vme16base + 0x6083;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 03 wd=%x\n",wd);
   
   k = vme16base + 0x6085;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 05 wd=%x\n",wd);
   
   k = vme16base + 0x6087;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 07 wd=%x\n",wd);
   
   k = vme16base + 0x6089;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 09 wd=%x\n",wd);
   
   k = vme16base + 0x608b;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 0b wd=%x\n",wd);
   
   k = vme16base + 0x608d;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 0d wd=%x\n",wd);
   
   k = vme16base + 0x608f;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 0f wd=%x\n",wd);
   
   k = vme16base + 0x6091;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 11 wd=%x\n",wd);
   
   k = vme16base + 0x6093;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 13 wd=%x\n",wd);
   
   k = vme16base + 0x6095;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 15 wd=%x\n",wd);
   
   k = vme16base + 0x6097;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 17 wd=%x\n",wd);  */
   
   
/*   printf("enter ttc address\n");
   fscanf(stdin,"%x",&ttcadr); 
   ttcadr=0x300d; */
   fp=fopen("/home/kelby/DRWTST/cfg.dat","r");
   fscanf(fp,"%x %s",&ttcadr,adcno);
   printf("%x  %s\n",ttcadr,adcno);
   pass=0;
   errors=0;
   retry=0;
   rsum=0;
strt0:
   board = 1;
   id = (board<<6) | 1; 
   ttcvi_mapr(ttcadr);
   reset_ctrl_can();
   printf("calling can_reset - this takes a few seconds\n");
   can_reset(ttcadr);
   can_init(board,adcno);
   if(cberr == 1) 
      { retry++;
        rsum++;
        printf(" error retry=%d\n",retry);
        if(retry>10) exit(0);
	goto strt0;
      }
strt1:
   good=0;
   bad=0;
   ierr=0;
   for(k=1;k<49;k++) 
     {  
        kkk=cardchk(k);
        if(kkk == 1)
	  { good++;
            printf(" card %d found  %x\n",k,k);
	  }
 	if(kkk != 1)
	  { bad++; 
            printf("no card %d  %x\n",k,k);
	  }
     }
   pass++;
   total=good+bad;
   if(good != 48) ierr=1;
   kkk = cardchk(0);
   if(kkk == 1) ierr=1;
   if(kkk == 1) printf("error card 0 found\n");
   kkk = cardchk(49);
   if(kkk == 1) printf("error card 49 found\n");
   if(kkk == 1) ierr=1;
   if(ierr != 0) errors++;
   if(sw(4)==0) printf("pass=%d   good=%d    bad=%d     %d   errors=%d  rsum=%d\n",
         pass,good,bad,total,errors,rsum);
   multi_low(ttcadr);
   set_mb_time(ttcadr,1,0);
   set_mb_time(ttcadr,2,0);
   set_mb_time(ttcadr,3,0);
   set_mb_time(ttcadr,4,0);
/*   printf(" enter capacitor  0=trig 1=small  2=large\n");
   fscanf(stdin,"%d",&icap);  */
   printf(" enter dac value (hex)\n");
   fscanf(stdin,"%x",&idac);
   rxw_low(ttcadr);
   multi_low(ttcadr);
   if(sw(4)==0) printf(" good=%d    bad=%d     %d\n",good,bad,total);
   multi_high(ttcadr);
   capacitor = 100.0;
   charge=800.0;
   if(icap==1)
     { capacitor = 5.1;
       charge = 30.0;
       large_cap_disable(ttcadr);
       small_cap_enable(ttcadr);
     }
/*   printf("enter charge\n");
   fscanf(stdin,"%lf",&charge);  
   fdac = (1023.0*charge) / (2.0 * 4.096 * capacitor);
   dac = (int)fdac;  */
   dac = idac;
   charge = 2.0 * 4.096 * capacitor * (double)dac / 1023.0;
   printf("logain charge=%f dac=%x capacitor=%f\n",
         charge,dac,capacitor);
   for(k=1;k<49;k++)
     { set_tube(ttcadr,k); 
       set_dac(ttcadr,dac);
     }  
/*   usleep(10);
   tp_low(ttcadr);
   phase=116;
   SetTimeDes2(ttcadr,phase);  
loop:
   inject();
   usleep(10);
   if(sw(8) != 1) goto loop; */
}
     inject()
       {if(ent==0)
         {/* now start pulses - take 1 */
            /* setup inhibit1 to set tp high */
	     SetBMode(1,0xa);  /* internal inhibit-bgo connection */
	     SetInhibitDelay(1,10);
	     SetInhibitDuration(1,10);
	/*     BPutLong(1,ttcadr,1,0xc0,0x00); */ /* load set tp_high into fifo1 */
	     BPutShort(1,0xc4);    /* set tp high bcast command  */
	     /* set inhibit2 to drop tp low (timed broadcast mode */
	     SetBMode(2,0xa);
	     SetInhibitDelay(2,2000);
	     SetInhibitDuration(2,10);
	    /* BPutLong(2,0,1,0xc0,0x01);  */ /* load set tp_low into fifo2 */
	     BPutShort(2,0xc0);   /* set tp low bcast command  */
	     /* set inhibit3 to set latch                     */
	     /* use latch-bar and inhibit3 to start fermi     */
             /* will use inhibit 3 to set l1a with digitizers */
             usleep(50);
             i3time=2266;
/*	     printf("input i3time\n");
             fscanf(stdin,"%d",&i3time);   
             printf("i3time=%d\n",i3time);  */ 
	     SetInhibitDelay(3,i3time);
	     SetInhibitDuration(3,10);
	     SetInhibitDelay(0,1);
	     SetInhibitDuration(3,10);
             ent = 1;
             /* printf("inject setup\n"); */
           }
/* take the next event */
   reg=(unsigned short *)(vme24base+0xde0000);
   *reg=0;
   usleep(1000); 
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
   usleep(1);
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
   usleep(1);
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
