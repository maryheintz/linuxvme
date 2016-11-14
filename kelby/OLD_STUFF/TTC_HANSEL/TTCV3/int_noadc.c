#include <stdio.h>
#include <math.h>
#include <time.h>
char xstring[80],ystring[80],string[80],err[80];
unsigned int ttcadr,id;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{ 
   int isw[6],i1,i2,ll,i,mode,s1,s2,s3,s4,itr,ire,mse,tpe,lcal;
   int sw1,dacset,max[6],del[6],k,kkk;
   short cadr,sval,dval,tval,t1val,t2val;
   double fmin,ratio,volt,ped,expr[6];
   double a,b,ped0,ped100,dac,factor;
   int l,gain,pause;
   unsigned int itst;
   unsigned short shrt;
   unsigned int card;
   long status;
   factor=1.0;
   printf(" enter ttcadr in hex\n");
   fscanf(stdin,"%x",&ttcadr);
   printf("ttcadr = %x\n",ttcadr);  
   id=1;
   ttcvi_map(ttcadr);
   can_init(ttcadr,id);  
   if(argc != 2)
   for(k=0;k<49;k++) 
     { if(cardchk(k) == 1)
          {card = k; 
           goto found; }
       else  
         printf("no card %d\n",k);
     }
   return(0);
found:
   printf("card found = %d\n",card);
   if(argc == 2)
     { sscanf(argv[1],"%d",&card);
       printf("card %d requested\n",card);
     } 
   set_tube(ttcadr,card);
   multi_low(ttcadr);
   rxw_low(ttcadr);

   max[0]=250;
   max[1]=100;
   max[2]=70;
   max[3]=40;
   max[4]=25;
   max[5]=20;

   del[0]=25;
   del[1]=2;
   del[2]=2;
   del[3]=1;
   del[4]=1;
   del[5]=1;

   expr[0]=1.0;
   expr[1]=2.7;
   expr[2]=3.7;
   expr[3]=7.2;
   expr[4]=9.7;
   expr[5]=13.2;

   isw[0]=0xe;
   isw[1]=0xd;
   isw[2]=0xf;
   isw[3]=0xb;
   isw[4]=0x7;
   isw[5]=0x3;

   for(gain=0;gain<6;gain++)
    {set_intg_gain(ttcadr,isw[gain]);
     set_dac(ttcadr,0);  /* dac=0 */
     intg_rd_low(ttcadr);
     itr_low(ttcadr);
     usleep(5);
      dacset = max[gain];
     for(i=0;i<dacset;i=i+del[gain])       
       { sw1 = sw(1);
         set_dac(ttcadr,i);
         printf("gain=%x   dac=%x\n",gain,i);
         usleep(100);
       }
    }
   set_dac(ttcadr,10);
   sleep(1);
   intg_rd_low(ttcadr);
   itr_low(ttcadr);
   sleep(1);

   intg_rd_low(ttcadr);
   itr_high(ttcadr);
   sleep(1);

   intg_rd_high(ttcadr);
   itr_low(ttcadr);
   sleep(1);

   intg_rd_low(ttcadr);
   itr_low(ttcadr);
   sleep(1);
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
      double dxyfit(l,k,x)
      int l,k;
      double x;
      { double dxyf;
        dxyf=0.0;  /* dummy routine */
        return(dxyf);
      }
