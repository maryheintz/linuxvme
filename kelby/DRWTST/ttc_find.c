#include <stdio.h>
#include <math.h>
#include "vme_base.h"
int id,ttcadr;
char string[80];
char adcno[7];
main()
{  FILE *fp;
   char name[7];
   unsigned char wd,*adr;
   int tube,i,k,board,errsum=0,pass=0;
   unsigned int i1,i2,itst;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
   unsigned long status;
   int good,bad,total,kkk;
   status = vme_a24_base();
   status = vme_a16_base(); 
   reset_ctrl_can();
   
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
   printf(" 17 wd=%x\n",wd);
   
/*   printf("enter ttc address\n");
   fscanf(stdin,"%x",&ttcadr);
   ttcadr = 0x3009;  */
   fp=fopen("/home/kelby/DRWTST/cfg.dat","r");
   fscanf(fp,"%x %s",&ttcadr,adcno);
   printf("ttcadr=%x  adcno=%s\n",ttcadr,adcno);
trt0:
   board = 1;
   id = (board<<6) | 1; 
   ttcvi_mapr(ttcadr);
   /* printf("calling can_reset - this takes a few seconds\n"); */
   can_reset(ttcadr);
   can_init(board,adcno);
strt:
   good=0;
   bad=0;
   for(k=1;k<49;k++) 
     {  kkk=cardchk(k);
        if(kkk == 1)
	  { good++;
            printf(" card %d found\n",k);
	  }
 	if(kkk != 1)
	  { bad++;
	    errsum++; 
            printf("no card %d\n",k);
	  }
     }
   total=good+bad;
   if(sw(4)==0) printf(" good=%d    bad=%d     %d\n",good,bad,total);
   kkk=cardchk(0);
   if(kkk == 1) printf("error card 0 found\n");
   kkk=cardchk(49);
   if(kkk == 1) 
     { printf("error card 49 found\n");
       errsum++;
     }
   pass++;
   if(sw(1) == 1) 
     { printf(" pass=%d  errsum=%d \n",pass,errsum);
       sleep(1);
       goto strt;
     }
      
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
   status=can_3in1set(id,0xd400);   /* fetch from 3in1 */
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
   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);

   set_intg_gain(ttcadr,0xa);   /* s1=s3=0 s2=s4=1 */
   itr_low(ttcadr);           /* itr=0 */
   intg_rd_high(ttcadr);        /* ire=1 */
   mse_low(ttcadr);           /* mse=0 */
   small_cap_enable(ttcadr);  /* tplo=1 */
   large_cap_disable(ttcadr);   /* tphi=0 */
   trig_enable(ttcadr);       /* trig=1 */
   back_high(ttcadr);
   back_low(ttcadr);
   status=can_3in1set(id,0xd400);   /* fetch from 3in1 */
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
   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
  if(ok == 2) status=1;
   return(status);
  }









