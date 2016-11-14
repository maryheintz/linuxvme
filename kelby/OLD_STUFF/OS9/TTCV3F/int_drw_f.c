#include <stdio.h>#include <math.h>#include <time.h>char xstring[80],ystring[80],string[80],err[80];int id,ttcadr;main(argc,argv,envs)int argc;char *argv[];char *envs[];{  int board;   int isw[6],i;   int dacset,max[6],del[6],k;   double fmin,volt,ped,expr[6];   double a,b,dac,factor;   int card,l,gain;   unsigned int itst,command;   unsigned short shrt;   long status;   board = 1;   ttcadr = 0x1234;   ttcvi_map(ttcadr);   if(argc != 2)    {for(k=0;k<49;k++)       { if(cardchk(k))          { card = k;            goto found;}       }   printf("no card found\n");   return(0);found:   printf("card found = %d\n",card);   }   if(argc == 2)     { sscanf(argv[1],"%d",&card);       printf("card %d requested\n",card);     }restart:   set_tube(ttcadr,card);   factor = 9.6/4096.0;   /* volts per count  */   multi_low(ttcadr);   rxw_low(ttcadr);   back_low(ttcadr);   mse_low(ttcadr);   max[0]=600;   max[1]=70;   max[2]=60;   max[3]=36;   max[4]=30;   max[5]=20;   del[0]=50;   del[1]=5;   del[2]=5;   del[3]=3;   del[4]=3;   del[5]=2;   expr[0]=1.0;   expr[1]=2.7;   expr[2]=3.7;   expr[3]=7.2;   expr[4]=9.7;   expr[5]=13.2;   isw[0]=0xe;   isw[1]=0xd;   isw[2]=0xf;   isw[3]=0xb;   isw[4]=0x7;   isw[5]=0x3;   dxyzro();   for(i=0;i<6;i++)   { if(i == 0) dxyset(i,(double)max[i],0.0,10.0,0.0,0,0);     if(i != 0) dxyset(i,(double)max[i],0.0,10.0,0.0,0,0);     sprintf(xstring,"isw=%x xaxis=dac value",isw[i]);     sprintf(ystring,"yaxis=volts");     dxylbl(i,xstring,ystring);    }   for(gain=0;gain<6;gain++)    {intg_rd_low(ttcadr);     itr_low(ttcadr);     set_dac(ttcadr,0);  /* dac=0 */     set_intg_gain(ttcadr,isw[gain]);     convert(&shrt);     dacset = max[gain];     for(i=0;i<dacset;i=i+del[gain])              { set_dac(ttcadr,i);         convert(&shrt);         volt = factor * (double)shrt;         if(i == 0) ped = 0.0;           fmin=volt-ped;         if(sw(1) == 0) printf("dac=%d  volt=%f  ped=%f  shrt=%x  %d\n",             i,volt,ped,shrt,shrt);         dxyacc(gain,(double)i,fmin,0.0,0);       }     }   if(sw(8) == 1) goto restart;   set_dac(ttcadr,48);   intg_rd_low(ttcadr);   itr_low(ttcadr);   tsleep(0x80000050);   convert(&shrt);   volt = factor * (double)shrt;   printf("intg_rd=0 itr=0 on volt=%f\n",volt);   intg_rd_high(ttcadr);   itr_low(ttcadr);   sleep(1);   tsleep(0x80000050);   convert(&shrt);   volt = factor * (double)shrt;   printf("intg_rd=1 itr=0 on volt=%f\n",volt);   intg_rd_low(ttcadr);   itr_high(ttcadr);   sleep(1);   tsleep(0x80000050);   convert(&shrt);   volt = factor * (double)shrt;   printf("intg_rd=0 itr=1 on volt=%f\n",volt);   intg_rd_low(ttcadr);   itr_low(ttcadr);   sleep(1);   tsleep(0x80000050);   convert(&shrt);   volt = factor * (double)shrt;   printf("intg_rd=0 itr=0 on volt=%f\n",volt);   printf("card=%d\n",card);   fflush(stdout);   sidev(1);   dxymwr(6,0,5);}cardchk(k)int k;  {int i,status;   unsigned short shrt;   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,ok;   ok=0;   status=0;   multi_low(ttcadr);   back_low(ttcadr);   set_tube(ttcadr,k);   set_intg_gain(ttcadr,5);   /* s1=s3=1 s2=s4=0 */   itr_high(ttcadr);           /* itr=1 */   intg_rd_low(ttcadr);        /* ire=0 */   mse_high(ttcadr);           /* mse=1 */   small_cap_disable(ttcadr);  /* tplo=0 */   large_cap_enable(ttcadr);   /* tphi=1 */   trig_disable(ttcadr);       /* trig=0 */   back_high(ttcadr);   back_low(ttcadr);   status=can_3in1set(id,0xd400);   /* fetch from 3in1 */   status=can_3in1get(id,&shrt);    /* and load can output reg */   i = (int)shrt;   trig = i & 1;   s1 = (i>>1) & 1;   s2 = (i>>2) & 1;   s3 = (i>>3) & 1;   s4 = (i>>4) & 1;   itr = (i>>5) & 1;   ire = (i>>6) & 1;   mse = (i>>7) & 1;   tplo = (i>>8) & 1;   tphi = (i>>9) & 1;   if((s1==1) && (s2==0)  && (s3==1) && (s4==0) && (itr==1) &&      (ire==0) && (mse==1) && (tplo==0) && (tphi==1) &&       (trig==0) ) ok++;   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);   set_intg_gain(ttcadr,0xa);   /* s1=s3=0 s2=s4=1 */   itr_low(ttcadr);           /* itr=0 */   intg_rd_high(ttcadr);        /* ire=1 */   mse_low(ttcadr);           /* mse=0 */   small_cap_enable(ttcadr);  /* tplo=1 */   large_cap_disable(ttcadr);   /* tphi=0 */   trig_enable(ttcadr);       /* trig=1 */   back_high(ttcadr);   back_low(ttcadr);   status=can_3in1set(id,0xd400);   /* fetch from 3in1 */   status=can_3in1get(id,&shrt);    /* and load can output reg */   i = (int)shrt;   trig = i & 1;   s1 = (i>>1) & 1;   s2 = (i>>2) & 1;   s3 = (i>>3) & 1;   s4 = (i>>4) & 1;   itr = (i>>5) & 1;   ire = (i>>6) & 1;   mse = (i>>7) & 1;   tplo = (i>>8) & 1;   tphi = (i>>9) & 1;   if((s1==0) && (s2==1)  && (s3==0) && (s4==1) && (itr==0) &&      (ire==1) && (mse==0) && (tplo==1) && (tphi==0) &&       (trig==1) ) ok++;   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);  if(ok == 2) status=1;   return(status);  }      double dxyfit(l,k,x)      int l,k;      double x;      { double dxyf;        dxyf=0.0;  /* dummy routine */        return(dxyf);      }convert(k)unsigned short *k; { unsigned short kk,*reg;   int stat;/* ----- digitize ----- */   reg=(unsigned short *)0xf0dd0018;   /* set convert low */   *reg=0;   reg=(unsigned short *)0xf0dd000c;   /* set convert high */   *reg=0;   tsleep(0x80000010); /* ----- read out data ----- */   reg=(unsigned short *)0xf0dd000e;   kk = *reg;   *k = kk& 0xfff;   stat=(kk>>15);/*   printf("kk=%x  stat=%x\n",kk,stat);  */ }