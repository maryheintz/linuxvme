
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <ces/vmelib.h>
struct block
       { unsigned short b0;        /* base+0 */
         unsigned short b2;        /* base+2 */
         unsigned short b4;        /* base+4 */
         unsigned short b6;        /* base+6 */
         unsigned short b8;        /* base+8 */
         unsigned short ba;        /* base+a */
         unsigned short bc;        /* base+c */
         unsigned short be;        /* base+e */
         unsigned short rd0;       /* base+10 */
         unsigned short rd1;       /* base+12 */
         unsigned short rd2;       /* base+14 */
         unsigned short b16;       /* base+16 */
         unsigned short all;       /* base+18 */
         unsigned short b1a;       /* base+1a */
         unsigned short b1c;       /* base+1c */
         unsigned short delay;     /* base+1e */
         unsigned short sample;    /* base+20 */
         unsigned short status;    /* base+22 */
         unsigned short reset;     /* base+24 */
         unsigned short trig;      /* base+26 */
       };
struct block *adc;
static struct pdparam_master param=
      { 1,   /* VME Iack (must be 1 for all normal pages) */
        0,   /* VME Read Prefetch Option */
        0,   /* VME Write Posting Option */
        1,   /* VME Swap Option */
        0,   /* page number return */
        0,   /* reserved */
        0,   /* reserved */
        0 };
int fdata[3][256],ipk[3],pk[3];
unsigned short *reg;
unsigned short *reg0,*reg1,*reg2;
unsigned int base_16bit=0xdfb40000;
unsigned int base_32bit=0xdfb40000;
double crg[6500],fvl[6500];
int ttcadr,id,kk;
time_t usec = 1;
int sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { FILE *fp;
    int tim,i,l,k;
    int tube;
    int dac,icap;
    double charge,dacval,capacitor;
    char xstring[80],ystring[80],string[80],err[80];
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    unsigned short cadr,tc,tf;
    unsigned short sval,dval;
    int time1,time2,time3,time4,board;
    double position,position2,b1,b2,b3,f1,f2,f3;
    double sum,ysum,xsum,fdg=2.1;
   int isw[6],i1,i2,ll,mode,s1,s2,s3,s4,itr,ire,mse,tpe,lcal;
   int sw1,dacset,max[6],del[6],kkk;
   short ctval,t1val,t2val;
   double fmin,ratio,volt,expr[6];
   double a,b,ped0,ped100,factor;
   int gain,pause;
   unsigned int itst;
   unsigned short shrt;
   long status;
    kk = 0;
    ttcadr = 0;  
/*   printf(" enter ttcadr in hex\n");
   fscanf(stdin,"%x",&ttcadr);  
   printf("ttcadr = %x\n",ttcadr);  */
   id=1;
   ttcvi_map(ttcadr);
   can_init(ttcadr,id);
   fermi_setup();
   for(i=1;i<49;i++)
     { tube = i;
       if(cardchk(tube) == 1) goto found; }
   printf("no tube found\n");
   exit(0);
found:    
    printf(" card %d found\n",tube);
    set_tube(ttcadr,tube);
    multi_low(ttcadr);
    rxw_low(ttcadr);
    intg_rd_low(ttcadr);
    itr_low(ttcadr);
    set_intg_gain(ttcadr,0);  /* s1=s2=s3=s4=0 */
    mse_low(ttcadr);         /* mse=0 */
    trig_enable(ttcadr);
    if(tube<13) board = 4;
    if(tube>12 && tube<25) board = 3;
    if(tube>24 && tube<37) board = 2;
    if(tube>36) board = 1;
    printf(" board=%d\n",board);
    SetTimeDes2(ttcadr,172);
    capacitor = 100.0;
    large_cap_enable(ttcadr);
    small_cap_disable(ttcadr);
    charge = 800.0;
    dacval = (charge * 1023.0 ) / (2.0 * 4.096* capacitor);
    if(dacval > 1023.0) dacval = 1023.0;
    dac = (int) dacval;
    set_dac(ttcadr,dac);
    charge = ( 2.0 * 4.096 * capacitor *(double)dac ) / 1023.0;
    printf(" charge=%f  dacval=%f  dac=%x(hex)  cap=%f\n",
           charge,dacval,dac,capacitor);

    dotset(0,250.0,0.0,25.0,15.0,0,0);
    sprintf(xstring,"time setting");
    sprintf(ystring,"position");
    dotlbl(0,xstring,ystring);

    dotset(1,250.0,0.0,25.0,15.0,0,0);
    sprintf(xstring,"time setting");
    sprintf(ystring,"position2");
    dotlbl(1,xstring,ystring);

    for(tim=0;tim<256;tim++)
      { set_mb_time(ttcadr,board,tim);
        sum=0.0;
        ysum=0.0;
        xsum=0.0;
        for(k=0;k<10;k++)
         { inject();
           fermi_read();
/*     display ? */
           sw1 = sw(1);
           if(sw1 == 1) 
            { asum = 0.0;
              axsum = 0.0;
              axxsum = 0.0;
              for(i=10;i<20;i++)
               { aval = (double)fdata[kk][i];
                 asum = asum+1.0;
                 axsum = axsum + aval;
                 axxsum = axxsum + aval*aval;
               }
              ped = axsum / asum;
              dped = 0.0;
              sig2=axxsum/asum - ped*ped;
              if(sig2>0.0) dped=sqrt(sig2);
              sprintf(string,
                "ipk=%d  pk=%d  ped=%f +/- %f",
                ipk[0],pk[0],ped,dped);
              evdisp(32,string,0);
             }

          f1 = (double)fdata[kk][ipk[kk]-1];
          b1 = (double)(ipk[kk]-1);
          f2 = (double)fdata[kk][ipk[kk]];
          b2 = (double)(ipk[kk]);
          f3 = (double)fdata[kk][ipk[kk]+1];
          b3 = (double)(ipk[kk]+1);
          position = (f1*b1 + f2*b2 + f3*b3) / (f1 + f2 + f3);
          position2 = (fdg*f1*b1 + f2*b2 + fdg*f3*b3) / (fdg*f1 + f2 + fdg*f3);
          sum=sum+1.0;
          ysum=ysum+position;
          xsum=xsum+position2;
         }  
       position = ysum/sum;
       position2 = xsum/sum;
       dotacc(0,(double)tim,position);
       dotacc(1,(double)tim,position2);
       if(sw(6) == 1) printf("time=%d   pos=%f  pos2=%f\n",tim,position,position2);
              }
    printf(" card %d  board %d\n",tube,board);
    sidev(1);
   dotwrt(0);
   dotwrt(1);
   set_tube(ttcadr,tube);
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

   factor=1.0;
   for(i=0;i<6;i++)
   { dxyset(i,(double)max[i],0.0,5000.0,0.0,0,0);
     sprintf(xstring,"isw=%x xaxis=dac value",isw[i]);
     sprintf(ystring,"yaxis=volts");
     dxylbl(i,xstring,ystring);
    }
   for(gain=0;gain<6;gain++)
    {set_intg_gain(ttcadr,isw[gain]);
     set_dac(ttcadr,0);  /* dac=0 */
     intg_rd_low(ttcadr);
     itr_low(ttcadr);
     usleep(5);
     status=adc_convert(id,tube,&shrt);
     ped = factor * (double)shrt;
     if(sw(6) == 1) printf("gain setting %d  ped=%f\n",gain,ped);
     dacset = max[gain];
     for(i=0;i<dacset;i=i+del[gain])       
       { set_dac(ttcadr,i);
         usleep(2);
         status=adc_convert(id,tube,&shrt);
         volt = factor * (double)shrt;
         fmin=volt-ped;
         if(sw(6) == 1) printf("dac=%d  volt=%f  ped=%f  shrt=%x\n",
             i,volt,ped,shrt);
         dxyacc(gain,(double)i,fmin,0.0,0);
       }
    }
   set_dac(ttcadr,10);
   sleep(1);
   intg_rd_low(ttcadr);
   itr_low(ttcadr);
   sleep(1);
   status=adc_convert(id,tube,&shrt);
   volt = factor * (double)shrt;
   printf("intg_rd=0 itr=0 on volt=%f  %x  %f\n",volt,shrt,factor);

   intg_rd_low(ttcadr);
   itr_high(ttcadr);
   sleep(1);
   status=adc_convert(id,tube,&shrt);
   volt = factor * (double)shrt;
   printf("intg_rd=0 itr=1 on volt=%f  %x  %f\n",volt,shrt,factor);

llll:
   intg_rd_high(ttcadr);
   itr_low(ttcadr);
   sleep(1);
   status=adc_convert(id,tube,&shrt);
   volt = factor * (double)shrt;
   printf("intg_rd=1 itr=0 on volt=%f  %x  %f\n",volt,shrt,factor);
if(sw(4) == 1) goto llll;
   intg_rd_low(ttcadr);
   itr_low(ttcadr);
   sleep(1);
   status=adc_convert(id,tube,&shrt);
   volt = factor * (double)shrt;
   printf("intg_rd=0 itr=0 on volt=%f  %x  %f\n",volt,shrt,factor);
   printf("\n\n\n");
   fflush(stdout);
   sidev(1);
   dxymwr(6,0,5);
   }  /* end of main */
evdisp(nn,string,kk)
 int nn,kk;
 char *string;
 { int max,mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,k,ii;
   int idel;
   double dely,y,dy;
   char str[5];
   erasm();
   max = pk[kk];
   mmax=5000;
   if(max<4000) mmax=4000;
   if(max<3000) mmax=3000;
   if(max<2000) mmax=2000;
   if(max<1000) mmax=1000;
   if(max<800) mmax=800;
   if(max<600) mmax=600;
   if(max<500) mmax=500;
   if(max<400) mmax=400;
   if(max<200) mmax=200;
   if(max<100) mmax=100;
   if(max<50) mmax=50;
   idx = 1600/nn;
   linx = idx*nn;
   ixbas = 100+(2200-linx)/2;
   dy = 2400.0/(double)mmax;
   iybas = 400;
   mova(ixbas,iybas);
   drwr(linx,0);
   mova(ixbas,iybas);
   iylst = 0;
   for(i=0;i<nn;i++)
    { ii = fdata[kk][i];
      y = (double)ii;
      y = y*dy;
      iy = (int)y;
      idy = iy-iylst;
      drwr(0,idy);
      drwr(idx,0);
      iylst=iy;
    }
      drwr(0,-iylst);

/*  label horizontal axis */
    idel=10;
    if(nn>100) idel=25;
    if(nn>200) idel=50;
    for(i=0;i<nn+1;i=i+idel)
     { ix=ixbas+idx*i;
       mova(ix,iybas);
       drwr(0,-30);
       k=i/2;
       if(k*2 == i)
         { sprintf(str,"%3d",i);
           kx = ix-45;
           if(i<100) kx = ix-55;
           if(i<10) kx=ix-75;
           symb(kx,iybas-100,0.0,str);
         }
     }
/*  label vertical axis  */
    ny=mmax/100;
    if(ny>10) ny=10;
    if(mmax == 200) ny=4;
    if(mmax == 100) ny=4;
    if(mmax == 50) ny=5;
    y=(double)mmax*dy;
    iy = (int)y;
    mova(ixbas,iybas);
    drwr(0,iy);
    for(i=0;i<=ny;i++)
     { ky = i*mmax/ny;
       y = (double)ky * dy;
       iy = (int)y + iybas;
       mova(ixbas,iy);
       drwr(-20,0);
       sprintf(str,"%4d",ky);
       kx = ixbas-150;
       symb(kx,iy-20,0.0,str);
     }
    /*  print label */
    symb(200,3000,0.0,string);
    plotmx();
 }
    double hstfit(i,l,x)
    int i,l;
    double x;
    {double y;
     y=0.0;
     return(y); 
    }  
     inject()
       { static int entry=0;
         int c1,c2,f1,f2,swtst,i,i1,i2,i0time,i3time;
	 double a;
         if(entry == 0)
	   {/* setup inhibit1 to set tp high */
	     SetBMode(1,0xa);
	     SetInhibitDelay(1,10);
	     SetInhibitDuration(1,10);
	     BPutLong(1,ttcadr,1,0xc0,0x00);  /* load set tp_high into fifo1 */
	     /* set inhibit2 to drop tp low (timed broadcast mode */
	     SetBMode(2,0xa);
	     SetInhibitDelay(2,2000);
	     SetInhibitDuration(2,10);
	  /*  BPutLong(2,ttcadr,1,0xc0,0x01); */  /* load set tp_low into fifo2 */
	     BPutShort(2,0xc0);   /* set tp low bcast command  */
	     /* set inhibit3 to set latch                     */
	     /* use latch-bar and inhibit3 to start fermi     */
             /* will use inhibit 3 to set l1a with digitizers */
             i3time=2020;
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
             a = sin(0.11);
     	     entry = 1;
	    }
           fermi_reset();
           a = sin(0.21);
loop:      stvme();  /* send pulses from the switch box to clear last event */
           swtst = latch();  /* wait for the latch bit to set */
           if(swtst == 0) 
             { a = sin(0.003);
               goto loop;
             }
           restim();  /* reset the timer for next trigger */
           rlatch();    /* reset the latch bit */
  }
fermi_setup()
  { unsigned long am,vmead,offset,size,len;
    am=0x39;
    vmead=0xb40000;
    offset=0;
    size=0;
    len=400;
    adc = (struct block *)( find_controller(vmead,len,am,offset,size,&param) );
    if( adc == (struct block *)(-1) ) 
      { printf("unable to map vme address\n");
        exit(0);}
/* issue fermi reset */
   adc->reset=0;
/* set to full 255 samples */
   adc->sample=255;
/* set delay */
   adc->delay=2;
 }
fermi_reset()
 { 
/* issue reset */
   adc->reset=0;
 }
fermi_read()
 { int stat,data_ready,busy,lcnt,i,k0,k1,k2,kmax0,kmax1,kmax2;
   int dd0,dd1,dd2;
   unsigned short sd0,sd1,sd2;
   /* wait for a trigger */
   lcnt=0;
loop:
   lcnt++;
   stat = adc->status;
   data_ready = stat & 4;
   busy = stat & 2;
   if(lcnt > 1000 ) goto error;
   if ( data_ready == 0 ) goto loop;
   /* read the data and find peak channel */
   k0=0;
   kmax0=0;
   k1=0;
   kmax1=0;
   k2=0;
   kmax2=0;
   for (i=0;i<50;i++)
    { sd0 = adc->rd0;
      dd0 = (int)sd0 & 0xfff;
      fdata[0][i] = 0xfff - dd0;
      if(fdata[0][i]>kmax0)
       { kmax0=fdata[0][i];
         k0=i;}
      sd1 = adc->rd1;
      dd1 = (int)sd1 & 0xfff;
      fdata[1][i] = 0xfff - dd1;
      if(fdata[1][i]>kmax1)
       { kmax1=fdata[1][i];
         k1=i;}
      sd2 = adc->rd2;
      dd2 = (int)sd2 & 0xfff;
      fdata[2][i] = 0xfff - dd2;
      if(fdata[2][i]>kmax2)
       { kmax2=fdata[2][i];
         k2=i;}
      sw4 = sw(4);
      if(sw4 == 1) printf("i=%d  dd0=%d  dd1=%d  dd2=$d\n",i,dd0,dd1,dd2);
     }
   ipk[0]=k0;
   pk[0]=kmax0;
   ipk[1]=k1;
   pk[1]=kmax1;
   ipk[2]=k2;
   pk[2]=kmax2;
   return(0);
error:
   ipk[0]=-1;
   ipk[1]=-1;
   ipk[2]=-1;
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
