#include <stdio.h>#include <math.h>int fdata[3][256],ipk[3],pk[3];struct{unsigned int trig;       unsigned int reset;       unsigned int status;       unsigned int sample;       unsigned int delay;       unsigned int rdall;       unsigned int rd2;       unsigned int rd1;       unsigned int rd0;}fermi;unsigned short *reg;unsigned short *reg0,*reg1,*reg2;unsigned int fbase=0xf0b50000;unsigned int fbase2=0x80b50000;double crg[6500],fvl[6500];int id,ttcadr,board;int i3time,ent;main(argc,argv,envs)int argc;char *argv[];char *envs[];{    int i,phase,dac,card,k,jkja;   double asum,axsum,axxsum,ped,dped,sig2,aval;   char string[80];   double fdac,charge,capacitor,fval;   int icap;   ent=0;   id=0;   ttcadr = 0x3016;/*   printf("enter ttcadr\n");   fscanf(stdin,"%x",&ttcadr); */   board = 1;   id = (board<<6) | 1;   ttcvi_map(ttcadr);   reset_ctrl_can();   printf("calling can_reset - this takes a few seconds\n");   can_reset(ttcadr);     can_init(board,"S10114");     fermi_setup();   multi_low(ttcadr);   set_mb_time(ttcadr,1,158);   set_mb_time(ttcadr,2,115);   set_mb_time(ttcadr,3,83);   set_mb_time(ttcadr,4,36);   hstset(2,2180.0,2080.0,100,1);   hstlbl(2,"peak amplitude");   hstset(1,1850.0,1750.0,100,1);   hstlbl(1,"peak amplitude ped subtracted"); /*  charge=450.0; */   printf("enter charge to put on 5 cards\n");   fscanf(stdin,"%lf",&charge);   for(card=32;card<37;card++)    {  set_tube(ttcadr,card);       jkja=2;       icap = 0;       rxw_low(ttcadr);       mse_low(ttcadr);       set_intg_gain(ttcadr,0);       itr_low(ttcadr);       intg_rd_low(ttcadr);       trig_enable(ttcadr);       capacitor = 100.0;       large_cap_enable(ttcadr);       small_cap_disable(ttcadr);       fdac = (1023.0*charge) / (2.0 * 4.096 * capacitor);       dac = (int)fdac;       charge = 2.0 * 4.096 * capacitor * (double)dac / 1023.0;       printf("logain charge=%f dac=%x capacitor=%f\n",         charge,dac,capacitor);       set_dac(ttcadr,dac);       }   tsleep(0x80000010);   tp_low(ttcadr);   phase=116;/*   printf(" enter clock phase 0-239\n");   fscanf(stdin,"%d",&phase);  */   SetTimeDes2(ttcadr,phase);  loop:   inject();   fermi_read();   asum = 0.0;   axsum = 0.0;   axxsum = 0.0;   for(i=10;i<30;i++)     { aval = (double)fdata[jkja][i];       asum = asum+1.0;       axsum = axsum + aval;       axxsum = axxsum + aval*aval;     }   ped = axsum / asum;   dped = 0.0;   sig2=axxsum/asum - ped*ped;   if(sig2>0.0) dped=sqrt(sig2);   sprintf(string,     "trigger ipk=%d  pk=%d  ped=%f +/- %f",        ipk[jkja],pk[jkja],ped,dped);   if(sw(1) == 1) evdisp(50,string,jkja);   tsleep(0x80000002);   fval = (double)pk[jkja] - ped;   hstacc(1,fval,1.0);   fval = (double)pk[jkja];   hstacc(2,fval,1.0);   if(sw(8) != 1) goto loop;   hstwrt(1);   hstwrt(2);}     inject()       {          fermi_reset();         if(ent==0)         {/* now start pulses - take 1 */            /* setup inhibit1 to set tp high */	     SetBMode(1,0xa);  /* internal inhibit-bgo connection */	     SetInhibitDelay(1,10);	     SetInhibitDuration(1,10);	   /*  BPutLong(1,ttcadr,1,0xc0,0x00); */ /* load set tp_high into fifo1 */	     BPutShort(1,0xc4);   /* set tp high bcast command  */	     /* set inhibit2 to drop tp low (timed broadcast mode */	     SetBMode(2,0xa);	     SetInhibitDelay(2,2000);	     SetInhibitDuration(2,10);	   /*  BPutLong(2,0,1,0xc0,0x01); */   /* load set tp_low into fifo2 */	     BPutShort(2,0xc0);   /* set tp low bcast command  */	     /* set inhibit3 to set latch                     */	     /* use latch-bar and inhibit3 to start fermi     */             /* will use inhibit 3 to set l1a with digitizers */             tsleep(0x80000050);             i3time=2266;	  /*   printf("input i3time\n");             fscanf(stdin,"%d",&i3time);   */             printf("i3time=%d\n",i3time);  	     SetInhibitDelay(3,i3time);	     SetInhibitDuration(3,10);	     SetInhibitDelay(0,1);	     SetInhibitDuration(3,10);             ent = 1;           }/* take the next event */   reg=(unsigned short *)0xf0de0000;   *reg=0;   tsleep(0x80000007);  }cardchk(k)int k;  {int i,status;   unsigned short shrt;   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,ok;   ok=0;   status=0;   multi_low(ttcadr);   back_low(ttcadr);   set_tube(ttcadr,k);   set_intg_gain(ttcadr,5);   /* s1=s3=1 s2=s4=0 */   itr_high(ttcadr);           /* itr=1 */   intg_rd_low(ttcadr);        /* ire=0 */   mse_high(ttcadr);           /* mse=1 */   small_cap_disable(ttcadr);  /* tplo=0 */   large_cap_enable(ttcadr);   /* tphi=1 */   trig_disable(ttcadr);       /* trig=0 */   back_high(ttcadr);   back_low(ttcadr);   load_can(ttcadr);   /* fetch from 3in1 */   tsleep(0x80000010);   status=can_3in1get(id,&shrt);    /* and load can output reg */   i = (int)shrt;   trig = i & 1;   s1 = (i>>1) & 1;   s2 = (i>>2) & 1;   s3 = (i>>3) & 1;   s4 = (i>>4) & 1;   itr = (i>>5) & 1;   ire = (i>>6) & 1;   mse = (i>>7) & 1;   tplo = (i>>8) & 1;   tphi = (i>>9) & 1;   if((s1==1) && (s2==0)  && (s3==1) && (s4==0) && (itr==1) &&      (ire==0) && (mse==1) && (tplo==0) && (tphi==1) &&       (trig==0) ) ok++;   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x %d\n"           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i,ok);   set_intg_gain(ttcadr,0xa);   /* s1=s3=0 s2=s4=1 */   itr_low(ttcadr);           /* itr=0 */   intg_rd_high(ttcadr);        /* ire=1 */   mse_low(ttcadr);           /* mse=0 */   small_cap_enable(ttcadr);  /* tplo=1 */   large_cap_disable(ttcadr);   /* tphi=0 */   trig_enable(ttcadr);       /* trig=1 */   back_high(ttcadr);   back_low(ttcadr);   load_can(ttcadr);   /* fetch from 3in1 */   tsleep(0x80000010);   status=can_3in1get(id,&shrt);    /* and load can output reg */   i = (int)shrt;   trig = i & 1;   s1 = (i>>1) & 1;   s2 = (i>>2) & 1;   s3 = (i>>3) & 1;   s4 = (i>>4) & 1;   itr = (i>>5) & 1;   ire = (i>>6) & 1;   mse = (i>>7) & 1;   tplo = (i>>8) & 1;   tphi = (i>>9) & 1;   if((s1==0) && (s2==1)  && (s3==0) && (s4==1) && (itr==0) &&      (ire==1) && (mse==0) && (tplo==1) && (tphi==0) &&       (trig==1) ) ok++;   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x %d\n"           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i,ok);  if(ok == 2) status=1;   return(status);  }fermi_setup() { /* FPermit(fbase,0x100,3); */   /* FPermit(fbase2,0x100,3); */   fermi.trig=fbase|0x26;   fermi.reset=fbase|0x24;   fermi.status=fbase|0x22;   fermi.sample=fbase|0x20;   fermi.delay=fbase|0x1e;   fermi.rdall=fbase2|0x18;   fermi.rd2=fbase|0x14;   fermi.rd1=fbase|0x12;   fermi.rd0=fbase|0x10;/* issue reset */   reg=(unsigned short *)fermi.reset;   *reg=0;/* set to full 32 samples */   reg=(unsigned short *)fermi.sample;   *reg=255;/* set delay */   reg=(unsigned short *)fermi.delay;   *reg=1;   reg0=(unsigned short *) fermi.rd0;   reg1=(unsigned short *) fermi.rd1;   reg2=(unsigned short *) fermi.rd2; }fermi_reset() { /* issue reset */   reg=(unsigned short *)fermi.reset;   *reg=0; }fermi_read() { int stat,data_ready,busy,lcnt,i,k0,k1,k2,kmax0,kmax1,kmax2;   int dd0,dd1,dd2;   unsigned short sd0,sd1,sd2;   double a;   /* wait for a trigger */   lcnt=0;loop:   lcnt++;   reg = (unsigned short *)fermi.status;   stat = *reg;/*   printf("fermi_status=%x  %x\n",stat,(int)reg);   fflush(stdout); */   data_ready = stat & 4;   busy = stat & 2;   if(lcnt > 1000 ) goto error;   if ( data_ready == 0 ) goto loop;   /* read the data and find peak channel */   a=sin(0.3);  /* little delay after ready */   k0=0;   kmax0=0;   k1=0;   kmax1=0;   k2=0;   kmax2=0;   for (i=0;i<255;i++)    { sd0 = *reg0;      dd0 = (int)sd0 & 0xfff;      fdata[0][i] = 0xfff - dd0;  /*      fdata[0][i] = dd0; */      if(fdata[0][i]>kmax0)       { kmax0=fdata[0][i];         k0=i;}      sd1 = *reg1;      dd1 = (int)sd1 & 0xfff;      fdata[1][i] = 0xfff - dd1;  /*      fdata[1][i] = dd1;  */      if(fdata[1][i]>kmax1)       { kmax1=fdata[1][i];         k1=i;}      sd2 = *reg2;      dd2 = (int)sd2 & 0xfff;      fdata[2][i] = 0xfff - dd2;  /*      fdata[2][i] = dd2;  */      if(fdata[2][i]>kmax2)       { kmax2=fdata[2][i];         k2=i;}      if(sw(4) == 1) printf("i=%d  dd0=%d  dd1=%d  dd2=%d\n",i,dd0,dd1,dd2);       }   ipk[0]=k0;   pk[0]=kmax0;   ipk[1]=k1;   pk[1]=kmax1;   ipk[2]=k2;   pk[2]=kmax2;   return(0);error:   printf("fermi_read error\n");   ipk[0]=-1;   ipk[1]=-1;   ipk[2]=-1;}evdisp(nn,string,l) int nn,l; char *string; { int max,mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,k,ii;   int idel;   double dely,y,dy;   char str[5];   erasm();   max = pk[l];   mmax=5000;   if(max<4000) mmax=4000;   if(max<3000) mmax=3000;   if(max<2000) mmax=2000;   if(max<1000) mmax=1000;   if(max<800) mmax=800;   if(max<600) mmax=600;   if(max<500) mmax=500;   if(max<400) mmax=400;   if(max<200) mmax=200;   if(max<100) mmax=100;   if(max<50) mmax=50;   idx = 1600/nn;   linx = idx*nn;   ixbas = 100+(2200-linx)/2;   dy = 2400.0/(double)mmax;   iybas = 400;   mova(ixbas,iybas);   drwr(linx,0);   mova(ixbas,iybas);   iylst = 0;   for(i=0;i<nn;i++)    { ii = fdata[l][i];      y = (double)ii;      y = y*dy;      iy = (int)y;      idy = iy-iylst;      drwr(0,idy);      drwr(idx,0);      iylst=iy;    }      drwr(0,-iylst);/*  label horizontal axis */    idel=10;    if(nn>100) idel=25;    if(nn>200) idel=50;    for(i=0;i<nn+1;i=i+idel)     { ix=ixbas+idx*i;       mova(ix,iybas);       drwr(0,-30);       k=i/2;       if(k*2 == i)         { sprintf(str,"%3d",i);           kx = ix-45;           if(i<100) kx = ix-55;           if(i<10) kx=ix-75;           symb(kx,iybas-100,0.0,str);         }     }/*  label vertical axis  */    ny=mmax/100;    if(ny>10) ny=10;    if(mmax == 200) ny=4;    if(mmax == 100) ny=4;    if(mmax == 50) ny=5;    y=(double)mmax*dy;    iy = (int)y;    mova(ixbas,iybas);    drwr(0,iy);    for(i=0;i<=ny;i++)     { ky = i*mmax/ny;       y = (double)ky * dy;       iy = (int)y + iybas;       mova(ixbas,iy);       drwr(-20,0);       sprintf(str,"%4d",ky);       kx = ixbas-150;       symb(kx,iy-20,0.0,str);     }    /*  print label */    symb(200,3000,0.0,string);    plotmx(); }double hstfit(i,l,x)    int i,l;    double x;    { double y;     y=0.0;     return(y);     }  