#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "vme_base.h"
unsigned int fbase=0xb60000;
unsigned int fbase2=0xb60000;
int fdata[3][400],ipk[3],pk[3];
double crg[6500],vlt[6500];
char xstring[80],ystring[80],string[80],err[80];
unsigned int sw_base,fermi_base,vme_base,pio_base;
int sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8;
struct{unsigned int trig;
       unsigned int reset;
       unsigned int status;
       unsigned int sample;
       unsigned int delay;
       unsigned int rdall;
       unsigned int rd2;
       unsigned int rd1;
       unsigned int rd0;}fermi;
unsigned short *reg,*adr;
unsigned short *reg0,*reg1,*reg2;
int card;
int year,month,day,hour,min,sec;
double lim_slp[6] ={0.027,0.254,0.281,0.535,0.740,0.992};
double lim_dslp[6] = {0.001,0.02,0.010,0.020,0.035,0.080};
double lim_sgped[6] = {0.020,0.020,0.020,0.020,0.020,0.020};
double lim_dymx[6] = {0.03,0.03,0.03,0.03,0.03,0.03};
double sn0,sn1;
int kdac[300];
int FAIL,kFAIL;
int board;
unsigned int id,ttcadr;
char adcno[7];
FILE *fp,*fplo,*fphi,*fpadc;
static int i3time,ent;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{  double t0,tnow,dt;
   int ii,i,k;
   unsigned short kkk,stat;
   unsigned long status;
   fp = fopen("lin.txt","w");
   status = vme_a24_base();
   status = vme_a16_base(); 
   fermi_base = fbase + vme24base;
/*   fplo = fopen("fermi_lo.txt","w");
   fphi = fopen("fermi_hi.txt","w");  */
   card = 43;
   printf("enter <card> <ttc address>\n");
   fscanf(stdin,"%d %x",&card,&ttcadr);  
   fpadc=fopen("/home/kelby/TTCV3/adc.num","r");
   fscanf(fpadc,"%s",adcno);
   printf("card=%d   ttcadr=%x   adc=%s\n",card,ttcadr,adcno);

strt0:
   board = 1;
   id = (board<<6) | 1; 
   ttcvi_mapr(ttcadr);
   reset_ctrl_can();
   printf("calling can_reset - this takes a few seconds\n");
   can_reset(ttcadr);
   can_init(board,adcno);

   multi_low(ttcadr);
   back_low(ttcadr);
   set_mb_time(ttcadr,1,158);
   set_mb_time(ttcadr,2,115);
   set_mb_time(ttcadr,3,83);
   set_mb_time(ttcadr,4,36);

   SetTimeDes1(ttcadr,0);
   SetTimeDes2(ttcadr,126);
   CoarseDelay(ttcadr,0,0); 
   
    FAIL=0;
    if(argc<2)
      { chk_digital();
        chk_fast();
        chk_trig();
        chk_int();
      }
    if(argc>1)
      { printf("argv[1]=%s  %x  %x\n",argv[1],argv[1],"3");
        chk_digital();
        if(strcmp(argv[1],"1") == 0) chk_fast();
        if(strcmp(argv[1],"2") == 0) chk_trig();
        if(strcmp(argv[1],"3") == 0) chk_int();
      }
    printf("\n\n");
 /*   ii=fclose(fplo);
    ii=fclose(fphi); */
    ii=fclose(fp);
    sprintf(string,"cat lin.txt");
    ii=system(string);
    if(FAIL==0)
     { printf(" CARD PASSED\n");
     }
    if(FAIL!=0)
     { printf(" CARD FAILED\n");
     }
}

chk_digital()
  {int i,status;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,ok;
   ok=0;
   status=0;
   multi_low(ttcadr);
   back_low(ttcadr);
   set_tube(ttcadr,card);
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
   if(sw(2)==1) printf(" 1k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x %d\n"
           ,card,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i,ok);

   multi_low(ttcadr);
   back_low(ttcadr);
   set_tube(ttcadr,card);

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
   if(sw(2)==1) printf(" 2k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x %d\n"
           ,card,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i,ok);
  if(ok == 2) status=1;
      if(ok != 2) 
      { printf("card %d not found\n",card);
        FAIL=1;
      }
    if(ok == 2) 
      { printf("card %d found\n",card);
      }
  }

chk_fast()
  { double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    double meanhp,meanhc,meanlp,meanlc;
    double sum,xhsump,xxhsump,xhsumc,xxhsumc,xlsump,xxlsump,xlsumc,xxlsumc;
    double sighp,sighc,pmaxh,cmaxh,tmaxh;
    double siglp,siglc,pmaxl,cmaxl,tmaxl;
    int nevnt,nevmax,nstrtev,l,nn,i,k,wait;
    int ipeak,timc,timf,kipk;
    int dac,time1,time2,time3,time4;
    double fdac,charge,val,ratio;
    int bigain,kpeak,klo,khi,icnt,icnt1,icnt2,dstep;
    double s,sx,sy,sxy,sev,sxev,sxxev,sxest,el,er,ep,estimator;
    double sig,sx2,volt,capacitor,a,b,del,x,y,dely;
    double pedhi,dpedhi,pedlo,dpedlo;
    char xstring[80],ystring[80],string[80],err[80];
    unsigned short cadr,tc,tf;
    int kpeakh,timfh,kpeakl,timfl;
    double ah,bh,al,bl;
    double factor,slopeh,dymaxh,noiseh,slopel,dymaxl,noisel;
    double mh,sh,ml,sl;
    unsigned short sval,dval;
    factor = 2000.0 / 4096.0;
    fermi_setup();
    time1 = 0;
    time2 = 0;
    time3 = 0;
    time4 = 0;
    set_mb_time(ttcadr,1,time1);
    set_mb_time(ttcadr,2,time2);
    set_mb_time(ttcadr,3,time3);
    set_mb_time(ttcadr,4,time4);
    multi_low(ttcadr);
    set_tube(ttcadr,card);
    set_intg_gain(ttcadr,0);    /* s1=s2=s3=s4=0 */
    itr_low(ttcadr);            /* itr=1 */
    intg_rd_low(ttcadr);        /* ire=0 */
    mse_low(ttcadr);            /* mse=0 */
    trig_disable(ttcadr);       /* trig=0 */

    nevmax=50;
    pmaxh=0.0;
    cmaxh=0.0;
    tmaxh=0.0;
    pmaxl=0.0;
    cmaxl=0.0;
    tmaxl=0.0;

        dotzro();
        hstrst();

        dotset(0,240.0,0.0,1500.0,500.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"high gain FERMI peak channel amplitude");
        dotlbl(0,xstring,ystring);

        dotset(1,240.0,0.0,35.0,25.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"high gain peak channel number");
        dotlbl(1,xstring,ystring);

        dotset(2,240.0,0.0,2000.0,1000.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"low gain FERMI peak channel amplitude");
        dotlbl(2,xstring,ystring);

        dotset(3,240.0,0.0,35.0,25.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"low gain peak channel number");
        dotlbl(3,xstring,ystring);

        dotset(4,15.0,0.0,2500.0,0.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"high gain fermi peak");
        dotlbl(4,xstring,ystring);

        dotset(5,800.0,0.0,2500.0,0.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"low gain fermi peak");
        dotlbl(5,xstring,ystring);

        dotset(6,15.0,0.0,20.0,-20.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"high gain fit yfit-y");
        dotlbl(6,xstring,ystring);  

        dotset(7,800.0,0.0,20.0,-20.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"low gain fit yfit-y");
        dotlbl(7,xstring,ystring);

        dotset(8,15.0,0.0,5.0,-5.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"high gain sigma");
        dotlbl(8,xstring,ystring);

        dotset(9,800.0,0.0,5.0,-5.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"low gain sigma");
        dotlbl(9,xstring,ystring);

        hstset(0,100.0,50.0,50,1);
        hstlbl(0,"logain pedestal");

        hstset(1,100.0,50.0,50,1);
        hstlbl(1,"higain pedestal");

        hstset(2,5.0,0.0,50,1);
        hstlbl(2,"logain pedestal sigma");

        hstset(3,5.0,0.0,50,1);
        hstlbl(3,"higain pedestal sigma");

/* --------------------- determine higain timing -------------------*/
    capacitor = 5.2;
    small_cap_enable(ttcadr);
    large_cap_disable(ttcadr);
    charge=8.0;
    fdac = (1023.0*charge) / (2.0*4.096*capacitor);
    dac = (int)fdac;
    charge = 2.0 * 4.096 * capacitor * (double)dac / 1023.0;
    printf("higain charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    set_dac(ttcadr,dac);
    inject();
   i=0;
   for (timf=0; timf<240; timf=timf+2)  /* start l loop */
   { SetTimeDes2(ttcadr,timf);
     sum=0.0; 
     xhsump=0.0;
     xxhsump=0.0;
     xhsumc=0.0;
     xxhsumc=0.0;
     for(nevnt=0;nevnt<nevmax;nevnt++)
      {
        inject();
/*     go read the fermi module ------------------------------- */
        fermi_read();
/*     display ? */
         sw1 = sw(1);
         if(sw1 == 1) 
          { asum = 0.0;
            axsum = 0.0;
            axxsum = 0.0;
            for(i=10;i<20;i++)
             { aval = (double)fdata[2][i];
               asum = asum+1.0;
               axsum = axsum + aval;
               axxsum = axxsum + aval*aval;
              }
            ped = axsum / asum;
            dped = 0.0;
            sig2=axxsum/asum - ped*ped;
            if(sig2>0.0) dped=sqrt(sig2);
            sprintf(string,
              "hi gain ipk=%d  pk=%d  ped=%f +/- %f",
              ipk[2],pk[2],ped,dped);
            evdisp(50,string,2);}
/* now gets sums for mean calculation */
       sum=sum+1.0;
       kipk = ipk[2];
       yc=(double)fdata[2][kipk];
       xhsumc=xhsumc+yc;
       xxhsumc=xxhsumc+yc*yc;
       yc=(double)kipk;
       xhsump=xhsump+yc;
       xxhsump=xxhsump+yc*yc;
     } /* end of nevnt loop */
/*  now get mean  */
   meanhp=xhsump/sum;
   sighp=0.0;
   sig2=xxhsump/sum-meanhp*meanhp;
   if(sig2>0.0) sighp=sqrt(sig2);
   meanhc=xhsumc/sum;
   sighc=0.0;
   sig2=xxhsumc/sum-meanhc*meanhc;
   if(sig2>0.0) sighc=sqrt(sig2);
   sw3 = sw(3);
   if(sw3 == 1)
     { printf("\n timer=%d sum=%f\n",timf,sum); 
       printf("       meanhp=%f sighp=%f\n",meanhp,sighp);
       printf("       meanhc=%f sighc=%f\n",meanhc,sighc);
     }

   printf("high gain timer=%d channel=%6.1f peak=%5.1f\n",
    timf,meanhp,meanhc);
   if(meanhc>pmaxh && timf<240)
    { pmaxh=meanhc;
      cmaxh=meanhp;
      tmaxh=timf; }
   dotacc(0,(double)timf,meanhc);
   dotacc(1,(double)timf,meanhp);
   }  /* end of dac loop */
/* --------------------- determine logain timing -------------------*/
    capacitor = 100.0;
    large_cap_enable(ttcadr);
    small_cap_disable(ttcadr);
    charge=700.0;
    fdac = (1023.0*charge) / (2.0*4.096*capacitor);
    dac = (int)fdac;
    charge = 2.0 * 4.096 * capacitor * (double)dac / 1023.0;
    printf("higain charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    for(i=0;i<dac;i++) set_dac(ttcadr,i);
    set_dac(ttcadr,dac);  
    inject();
   i=0;
   for (timf=0; timf<240; timf=timf+2)  /* start l loop */
   { SetTimeDes2(ttcadr,timf);
     sum=0.0;
     xlsump=0.0;
     xxlsump=0.0;
     xlsumc=0.0;
     xxlsumc=0.0;
     for(nevnt=0;nevnt<nevmax;nevnt++)
      {
        inject();
/*     go read the fermi module ------------------------------- */
        fermi_read();
/*     display ? */
         sw1 = sw(1);
         if(sw1 == 1) 
          { asum = 0.0;
            axsum = 0.0;
            axxsum = 0.0;
            for(i=10;i<20;i++)
             { aval = (double)fdata[1][i];
               asum = asum+1.0;
               axsum = axsum + aval;
               axxsum = axxsum + aval*aval;
              }
            ped = axsum / asum;
            dped = 0.0;
            sig2=axxsum/asum - ped*ped;
            if(sig2>0.0) dped=sqrt(sig2);
            sprintf(string,
              "lo gain ipk=%d  pk=%d  ped=%f +/- %f",
              ipk[1],pk[1],ped,dped);
            evdisp(50,string,1);}
/* now gets sums for mean calculation */
       kipk = ipk[1];
       sum=sum+1.0;
       yc=(double)fdata[1][kipk];
       xlsumc=xlsumc+yc;
       xxlsumc=xxlsumc+yc*yc;
       yc=(double)kipk;
       xlsump=xlsump+yc;
       xxlsump=xxlsump+yc*yc;
     } /* end of nevnt loop */
/*  now get mean  */
   meanlp=xlsump/sum;
   siglp=0.0;
   sig2=xxlsump/sum-meanlp*meanlp;
   if(sig2>0.0) siglp=sqrt(sig2);
   meanlc=xlsumc/sum;
   siglc=0.0;
   sig2=xxlsumc/sum-meanlc*meanlc;
   if(sig2>0.0) siglc=sqrt(sig2);
   sw3 = sw(3);
   if(sw3 == 1)
     { printf("\n timer=%d sum=%f\n",timf,sum); 
       printf("       meanlp=%f siglp=%f\n",meanlp,siglp);
       printf("       meanlc=%f siglc=%f\n",meanlc,siglc);
     }

   printf(" low gain timer=%d channel=%6.1f peak=%5.1f\n",
    timf,meanlp,meanlc);
   if(meanlc>pmaxl && timf<240)
    { pmaxl=meanlc;
      cmaxl=meanlp;
      tmaxl=timf; }
   dotacc(2,(double)timf,meanlc);
   dotacc(3,(double)timf,meanlp);
   }  /* end of dac loop */
/*   show the plots */
     printf("pmaxh=%f cmaxh=%f tmaxh=%f\n",pmaxh,cmaxh,tmaxh);
     printf("pmaxl=%f cmaxl=%f tmaxl=%f\n",pmaxl,cmaxl,tmaxl);

/* ---------------- high gain calibration ----------------------*/
    capacitor = 5.2;
   small_cap_enable(ttcadr);
   large_cap_disable(ttcadr);
   kpeak=(int)(cmaxh+0.5);
   klo=kpeak-2;
   khi=kpeak+2;
   timf=(int)(tmaxh+0.5);
   printf("starting higain  kpeak=%d timf=%d\n",kpeak,timf);
   kpeakh = kpeak;
   timfh = timf;
   SetTimeDes2(ttcadr,timf);
   icnt=0;
   s=0.0;
   sx=0.0;
   sy=0.0;
   sxy=0.0;
   sx2=0.0;
   dstep = 2;
   for (dac=0; dac<400; dac=dac+dstep)
    { set_dac(ttcadr,dac);  
      volt = 2.0 * 4.096 * (double)dac/1023.0;
      charge = volt * capacitor;
      if(charge<0.0) continue;
      sev = 0.0;
      sxev = 0.0;
      sxxev = 0.0;
      sxest = 0.0;
      for(nevnt=0;nevnt<nevmax;nevnt++)
       {
        inject();
/*     go read the fermi module ------------------------------- */
        fermi_read();
/*     display ? */
         sw1 = sw(1);
         if(sw1 == 1) 
          { asum = 0.0;
            axsum = 0.0;
            axxsum = 0.0;
            for(i=10;i<20;i++)
             { aval = (double)fdata[2][i];
               asum = asum+1.0;
               axsum = axsum + aval;
               axxsum = axxsum + aval*aval;
              }
            ped = axsum / asum;
            dped = 0.0;
            sig2=axxsum/asum - ped*ped;
            if(sig2>0.0) dped=sqrt(sig2);
            sprintf(string,
              "hi gain ipk=%d  pk=%d  ped=%f +/- %f",
              ipk[2],pk[2],ped,dped);
            evdisp(50,string,2);}
        asum = 0.0;
        axsum = 0.0;
        axxsum = 0.0;
        for(i=10;i<20;i++)
         { aval = (double)fdata[2][i];
           asum = asum+1.0;
           axsum = axsum + aval;
           axxsum = axxsum + aval*aval;
          }
        kipk = ipk[2];
        ped = axsum / asum;
        hstacc(1,ped,1.0);
        sig2=axxsum/asum - ped*ped;
        if(sig2>0.0) dped=sqrt(sig2);
        hstacc(3,dped,1.0);
        if(fdata[2][kipk]>4090) goto fithi;
        val = (double)fdata[2][kpeak] - ped;
        el = (double)fdata[2][kpeak-1] - ped;
        er = (double)fdata[2][kpeak+1] - ped;
        ep = val;
        estimator = (el-er)/ep;
        if(kipk>=klo && kipk<=khi && fdata[2][kpeak]<4090)
         { sev = sev + 1.0;
           sxev = sxev + val;
           sxxev = sxxev + val*val;
           sxest = sxest + estimator;
         }       
      } /* end of nevnt loop */
        val=0.0;
        if(sev>0.0) 
          {val = sxev / sev;
           estimator = sxest / sev;
           printf(" high gain dac=%d charge=%5.2f fmean=%6.2f\n",
           dac,charge,val);
           dotacc(4,charge,val);
	/*   fprintf(fphi,"%f %f\n",charge,val);  */
           sig=0.0;
           sig2=sxxev/sev - val*val;
           if(sig2>0.0) sig=sqrt(sig2);
           if(val>0.0) dotacc(8,charge,sig);
          }
        if(val>0.0 && val<4090.0)
         { crg[icnt] = charge;
           vlt[icnt] = val;
           if(charge>0.0)
           { s = s +1.0;
              sx = sx + charge;
              sy = sy + val;
              sxy = sxy + charge*val;
              sx2 = sx2 + charge*charge;}
           icnt++; }
   }  /* end of dac loop */
   icnt1=icnt;
   printf(" end of dac loop icnt=%d\n",icnt);
fithi:
   del = s*sx2 - sx*sx;
   a = (sx2*sy - sx*sxy) / del;
   b = (s*sxy - sx*sy) / del;
   printf(" higain fit results a=%7.2f b=%7.2f y=a+b*charge\n",a,b);
   ah = a;
   bh = b;
   slopeh = bh*factor;
   s = 0.0;
   sx = 0.0;
   for(k=0;k<icnt;k++)
    {x = crg[k];
     y = a + b*x;
     dely = y - vlt[k];
     s = s + 1.0;
     sx = sx + fabs(dely);
     sw3 = sw(3);
     if(sw3 == 1) printf("k=%d x=%f  y=%f  vlt[k]=%f  dely=%f\n",
       k,x,y,vlt[k],dely);
     dotacc(6,x,dely);
    }
   dymaxh = factor * (sx/s);
/* ---------------------- low gain calibration --------------------------*/
    capacitor = 100.0;
   large_cap_enable(ttcadr);
   small_cap_disable(ttcadr);
   kpeak=(int)(cmaxl+0.5);
   klo=kpeak-2;
   khi=kpeak+2;
   timf=(int)(tmaxl+0.5);
   printf("starting logain  kpeak=%d timf=%d\n",kpeak,timf);
   kpeakl = kpeak;
   timfl = timf;
   SetTimeDes2(ttcadr,timf);
   icnt=0;
   s=0.0;
   sx=0.0;
   sy=0.0;
   sxy=0.0;
   sx2=0.0;
   dstep = 5;
   for (dac=0; dac<1024; dac=dac+dstep)
     {set_dac(ttcadr,dac);
      volt = 2.0 * 4.096 * (double)dac/1023.0;
      charge = volt * capacitor;
      sev = 0.0;
      sxev = 0.0;
      sxxev = 0.0;
      sxest = 0.0;
      for(nevnt=0;nevnt<nevmax;nevnt++)
       {
        inject();
/*     go read the fermi module ------------------------------- */
        fermi_read();
/*     display ? */
         sw1 = sw(1);
         if(sw1 == 1) 
          { asum = 0.0;
            axsum = 0.0;
            axxsum = 0.0;
            for(i=10;i<20;i++)
             { aval = (double)fdata[1][i];
               asum = asum+1.0;
               axsum = axsum + aval;
               axxsum = axxsum + aval*aval;
              }
            ped = axsum / asum;
            dped = 0.0;
            sig2=axxsum/asum - ped*ped;
            if(sig2>0.0) dped=sqrt(sig2);
            sprintf(string,
              "lo gain ipk=%d  pk=%d  ped=%f +/- %f",
              ipk[1],pk[1],ped,dped);
            evdisp(50,string,1);}
        asum = 0.0;
        axsum = 0.0;
        axxsum = 0.0;
        for(i=10;i<20;i++)
         { aval = (double)fdata[1][i];
           asum = asum+1.0;
           axsum = axsum + aval;
           axxsum = axxsum + aval*aval;
          }
        kipk = ipk[1];
        ped = axsum / asum;
        hstacc(0,ped,1.0);
        sig2=axxsum/asum - ped*ped;
        if(sig2>0.0) dped=sqrt(sig2);
        hstacc(2,dped,1.0);
        if(fdata[1][kipk]>4090) goto lofit;
        val = (double)fdata[1][kpeak] - ped;
        el = (double)fdata[1][kpeak-1] - ped;
        er = (double)fdata[1][kpeak+1] - ped;
        ep = val;
        estimator = (el-er)/ep;
        if(kipk>=klo && kipk<=khi && fdata[1][kpeak]<4090)
         { sev = sev + 1.0;
           sxev = sxev + val;
           sxxev = sxxev + val * val;
           sxest = sxest + estimator;
         }
      } /* end of nevnt loop */
        val=0.0;
        if(sev>0.0) 
          {val = sxev / sev;
           estimator = sxest / sev;
           printf("  low gain dac=%d charge=%6.2f fmean=%6.2f\n",
           dac,charge,val);
           dotacc(5,charge,val);
	/*   fprintf(fplo,"%f %f\n",charge,val);  */
           sig=0.0;
           sig2=sxxev/sev - val*val;
           if(sig2>0.0) sig=sqrt(sig2);
           if(val>0.0) dotacc(9,charge,sig);
          }
        if(val>0.0 && val<4090.0)
         { crg[icnt] = charge;
           vlt[icnt] = val;
           if(charge>50.0)
            { s = s +1.0;
              sx = sx + charge;
              sy = sy + val;
              sxy = sxy + charge*val;
              sx2 = sx2 + charge*charge;}
           icnt++; }
   }  /* end of dac loop */
   icnt2=icnt;
   printf(" end of dac loop icnt=%d\n",icnt);
   printf(" end of dac loop icnt1=%d  icnt2=%d\n",icnt1,icnt2);
lofit:
   del = s*sx2 - sx*sx;
   a = (sx2*sy - sx*sxy) / del;
   b = (s*sxy - sx*sy) / del;
   al = a;
   bl = b;
   slopel = bl*factor;
   printf(" logain fit results a=%7.2f b=%7.2f y=a+b*charge\n",a,b);
   s = 0.0;
   sx = 0.0;
   for(k=0;k<icnt;k++)
    {x = crg[k];
     y = a + b*x;
     dely = y - vlt[k];
     s = s + 1.0;
     sx = sx + fabs(dely);
     sw3 = sw(3);
     if(sw3 == 1) printf("k=%d x=%f  y=%f  vlt[k]=%f  dely=%f\n",k,x,y,vlt[k],dely);
     dotacc(7,x,dely);
    }
    dymaxl = factor * (sx/s);

    ratio = bh/bl;
    if(sw(8) == 0)
     { sidev(1);
       dotmwr(2,0,1);
       dotmwr(2,2,3);
       dotwrt(4);
       dotwrt(5);
       dotwrt(6);
       dotwrt(7);
       dotwrt(8);
       dotwrt(9);
       hstwrt(0);
       hstwrt(1);
       hstwrt(2);
       hstwrt(3);
     }   
/*------------------ test the trigger output ----------------- */
    capacitor = 100.0;
    large_cap_enable(ttcadr);
    small_cap_disable(ttcadr);
    charge=500.0;
    fdac = (1023.0*charge) / (2.0*4.096*capacitor);
    dac = (int)fdac;
    charge = 2.0*4.096 * capacitor * (double)dac / 1023.0;
    printf("higain charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    set_dac(ttcadr,dac);
    for(k=0;k<3;k++)
      { if(k==0) trig_enable(ttcadr);   /* enable the  output */
        if(k==1) trig_disable(ttcadr);   /* disable the trig output */
        if(k==2) trig_enable(ttcadr);   /* enable the trig output */
        inject();
        fermi_read();
        asum = 0.0;
        axsum = 0.0;
        axxsum = 0.0;
        for(i=10;i<20;i++)
          { aval = (double)fdata[0][i];
            asum = asum+1.0;
            axsum = axsum + aval;
            axxsum = axxsum + aval*aval;
          }
        ped = axsum / asum;
        dped = 0.0;
        sig2=axxsum/asum - ped*ped;
        if(sig2>0.0) dped=sqrt(sig2);
        sprintf(string,
             "hi gain ipk=%d  pk=%d  ped=%f +/- %f",
             ipk[0],pk[0],ped,dped);
        if(sw(8) == 0) evdisp(50,string,0);
      }
   hstmsg(3,&mh,&sh);
   kFAIL=0;
   if(slopeh<68.0) kFAIL=1;
   if(slopeh>95.0) kFAIL=2;
   if(noiseh>5.0) kFAIL=3;
   if(dymaxh>30.0) kFAIL=4;
   if(kFAIL != 0) FAIL = 1;
   noiseh = factor*mh;
   printf("high gain noise=%f slope=%f nonlin=%f\n",noiseh,slopeh,dymaxh);   
   if(kFAIL == 0) fprintf(fp,"high gain   noise=%6.2f slope=%6.2f nonlin=%6.2f   OK\n",
           noiseh,slopeh,dymaxh);
   if(kFAIL != 0) fprintf(fp,"high gain   noise=%6.2f slope=%6.2f nonlin=%6.2f  BAD=%d\n",
           noiseh,slopeh,dymaxh,kFAIL);
   hstmsg(2,&ml,&sl);
   kFAIL = 0;
   if(slopel<1.0) kFAIL=1;
   if(slopel>1.5) kFAIL=2;
   if(noisel>5.0) kFAIL=3;
   if(dymaxl>30.0) kFAIL=4;
   if(kFAIL != 0) FAIL = 1;
   noisel = factor*ml;
   printf(" low gain  noise=%f slope=%f nonlin=%f\n",noisel,slopel,dymaxl);
   if(kFAIL == 0) fprintf(fp," low gain   noise=%6.2f slope=%6.2f nonlin=%6.2f   OK\n",
           noisel,slopel,dymaxl);
   printf(" low gain  noise=%f slope=%f nonlin=%f\n",noisel,slopel,dymaxl);
   if(kFAIL != 0) fprintf(fp," low gain   noise=%6.2f slope=%6.2f nonlin=%6.2f  BAD=%d\n",
           noisel,slopel,dymaxl,kFAIL);
   printf("card=%d bh=%f  bl=%f  ratio=%f   ---------------- ifail=%d\n\n",
      card,bh,bl,ratio,FAIL);
   fprintf(fp,"Gain Ratio=%6.2f\n\n",ratio);
  }

chk_trig()
  { double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    double meanlp,meanlc;
    double sum,xlsump,xxlsump,xlsumc,xxlsumc;
    double siglp,siglc,pmaxl,cmaxl,tmaxl;
    int nevnt,nevmax,nstrtev,l,nn,i,k,wait;
    int ipeak,timc,timf,kipk;
    int dac,time1,time2,time3,time4;
    double fdac,charge,val,ratio;
    int bigain,kpeak,klo,khi,icnt,icnt1,icnt2,dstep;
    double s,sx,sy,sxy,sev,sxev,sxxev,sxest,el,er,ep,estimator;
    double sig,sx2,volt,capacitor,a,b,del,x,y,dely;
    double pedlo,dpedlo;
    char xstring[80],ystring[80],string[80],err[80];
    unsigned short cadr,tc,tf;
    int kpeakh,timfh,kpeakl,timfl;
    double al,bl;
    double factor,factor2,slope,dymax,noise,tmax[3],tok;
    unsigned short sval,dval;
    factor = 2000.0 / 4096.0;  /* convert counts to mv */
    factor2 = 1.0;  /* don't compensate for adder gain */
    fermi_setup();
    time1 = 0;
    time2 = 0;
    time3 = 0;
    time4 = 0;
    set_mb_time(ttcadr,1,time1);
    set_mb_time(ttcadr,2,time2);
    set_mb_time(ttcadr,3,time3);
    set_mb_time(ttcadr,4,time4);
    multi_low();
    cadr = (unsigned short)i;
    set_tube(ttcadr,card);
    set_intg_gain(ttcadr,0);  /* s1=s2=s3=s4=0 */
    itr_low(ttcadr);          /* itr=0 */
    intg_rd_low(ttcadr);      /* ire=0 */
    mse_low(ttcadr);          /* mse=0 */
    trig_enable(ttcadr);      /* trig=1 */

    nevmax=50;
    pmaxl=0.0;
    cmaxl=0.0;
    tmaxl=0.0;

        dotzro();
        hstrst();

        dotset(2,240.0,0.0,1500.0,1000.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"trigger FERMI peak channel amplitude");
        dotlbl(2,xstring,ystring);

        dotset(3,240.0,0.0,35.0,25.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"trigger peak channel number");
        dotlbl(3,xstring,ystring);


        dotset(5,800.0,0.0,2500.0,0.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"trigger fermi peak");
        dotlbl(5,xstring,ystring);


        dotset(7,800.0,0.0,25.0,-25.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"trigger fit y-yfit");
        dotlbl(7,xstring,ystring);


        dotset(9,800.0,0.0,15.0,-15.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"trigger sigma");
        dotlbl(9,xstring,ystring);

        hstset(0,100.0,50.0,100,1);
        hstlbl(0,"trigger pedestal");


        hstset(2,5.0,0.0,50,1);
        hstlbl(2,"trigger pedestal sigma");

/* --------------------- determine trigger output timing -------------------*/
    capacitor = 100.0;
    small_cap_disable(ttcadr);  /* tplo=0 */
    large_cap_enable(ttcadr);   /* tphi=1 */
    charge=500.0;
    fdac = (1023.0*charge) / (2.0*4.096*capacitor);
    dac = (int)fdac;
    charge = 2.0*4.096 * capacitor * (double)dac / 1023.0;
    printf("TRIGGER charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    for(i=0;i<dac;i++) set_dac(ttcadr,i);
    set_dac(ttcadr,dac);
    inject();
   i=0;
   for (timf=0; timf<240; timf=timf+2)  /* start l loop */
   { SetTimeDes2(ttcadr,timf);
     sum=0.0;
     xlsump=0.0;
     xxlsump=0.0;
     xlsumc=0.0;
     xxlsumc=0.0;
     for(nevnt=0;nevnt<nevmax;nevnt++)
      {
        fermi_reset();
        inject();
/*     go read the fermi module ------------------------------- */
        fermi_read();
/*     display ? */
         sw1 = sw(1);
         if(sw1 == 1) 
          { asum = 0.0;
            axsum = 0.0;
            axxsum = 0.0;
            for(i=10;i<20;i++)
             { aval = (double)fdata[0][i];
               asum = asum+1.0;
               axsum = axsum + aval;
               axxsum = axxsum + aval*aval;
              }
            ped = axsum / asum;
            dped = 0.0;
            sig2=axxsum/asum - ped*ped;
            if(sig2>0.0) dped=sqrt(sig2);
            sprintf(string,
              "trigger ipk=%d  pk=%d  ped=%f +/- %f",
              ipk[0],pk[0],ped,dped);
            evdisp(50,string,0);}
/* now gets sums for mean calculation */
       kipk = ipk[0];
       sum=sum+1.0;
       yc=(double)fdata[0][kipk];
       xlsumc=xlsumc+yc;
       xxlsumc=xxlsumc+yc*yc;
       yc=(double)kipk;
       xlsump=xlsump+yc;
       xxlsump=xxlsump+yc*yc;
     } /* end of nevnt loop */
/*  now get mean  */
   meanlp=xlsump/sum;
   siglp=0.0;
   sig2=xxlsump/sum-meanlp*meanlp;
   if(sig2>0.0) siglp=sqrt(sig2);
   meanlc=xlsumc/sum;
   siglc=0.0;
   sig2=xxlsumc/sum-meanlc*meanlc;
   if(sig2>0.0) siglc=sqrt(sig2);
   sw3 = sw(3);
   if(sw3 == 1)
     { printf("\n timer=%d sum=%f\n",timf,sum); 
       printf("       meanlp=%f siglp=%f\n",meanlp,siglp);
       printf("       meanlc=%f siglc=%f\n",meanlc,siglc);
     }

   printf(" trigger timer=%d channel=%6.1f peak=%5.1f\n",
    timf,meanlp,meanlc);
   if(meanlc>pmaxl && timf<125)
    { pmaxl=meanlc;
      cmaxl=meanlp;
      tmaxl=timf; }
   dotacc(2,(double)timf,meanlc);
   dotacc(3,(double)timf,meanlp);
   }  /* end of dac loop */
/*   show the plots */
     printf("pmaxl=%f cmaxl=%f tmaxl=%f\n",pmaxl,cmaxl,tmaxl);

/* ---------------------- trigger linearity calibration --------------------------*/
    capacitor = 100.0;
    small_cap_disable(ttcadr);  /* tplo=0 */
    large_cap_enable(ttcadr);   /* tphi=1 */
   kpeak=(int)(cmaxl+0.5);
   klo=kpeak-2;
   khi=kpeak+2;
   timf=(int)(tmaxl+0.5);
   printf("starting trigger  kpeak=%d timf=%d\n",kpeak,timf);
   kpeakl = kpeak;
   timfl = timf;
   SetTimeDes2(ttcadr,timf);
   icnt=0;
   s=0.0;
   sx=0.0;
   sy=0.0;
   sxy=0.0;
   sx2=0.0;
   dstep = 10;
   set_dac(ttcadr,dac);
   sleep(0);
   for (dac=0; dac<1000; dac=dac+dstep)
     {set_dac(ttcadr,dac);
      usleep(100);
      volt = 2.0*4.096 * (double)dac/1023.0;
      charge = volt * capacitor;
      sev = 0.0;
      sxev = 0.0;
      sxxev = 0.0;
      sxest = 0.0;
      for(nevnt=0;nevnt<nevmax;nevnt++)
       {
        inject();
/*     go read the fermi module ------------------------------- */
        fermi_read();
/*     display ? */
         sw1 = sw(1);
         if(sw1 == 1) 
          { asum = 0.0;
            axsum = 0.0;
            axxsum = 0.0;
            for(i=10;i<20;i++)
             { aval = (double)fdata[0][i];
               asum = asum+1.0;
               axsum = axsum + aval;
               axxsum = axxsum + aval*aval;
              }
            ped = axsum / asum;
            dped = 0.0;
            sig2=axxsum/asum - ped*ped;
            if(sig2>0.0) dped=sqrt(sig2);
            sprintf(string,
              "trigger ipk=%d  pk=%d  ped=%f +/- %f",
              ipk[0],pk[0],ped,dped);
            evdisp(50,string,0);}
        asum = 0.0;
        axsum = 0.0;
        axxsum = 0.0;
        for(i=10;i<20;i++)
         { aval = (double)fdata[0][i];
           asum = asum+1.0;
           axsum = axsum + aval;
           axxsum = axxsum + aval*aval;
          }
        kipk = ipk[0];
        ped = axsum / asum;
        hstacc(0,ped,1.0);
        sig2=axxsum/asum - ped*ped;
        if(sig2>0.0) dped=sqrt(sig2);
        hstacc(2,dped,1.0);
        if(fdata[1][kipk]>4090) goto lofit;
        val = (double)fdata[0][kpeak] - ped;
        el = (double)fdata[0][kpeak-1] - ped;
        er = (double)fdata[0][kpeak+1] - ped;
        ep = val;
        estimator = (el-er)/ep;
        if(kipk>=klo && kipk<=khi && fdata[0][kpeak]<4090)
         { sev = sev + 1.0;
           sxev = sxev + val;
           sxxev = sxxev + val * val;
           sxest = sxest + estimator;
         }
      } /* end of nevnt loop */
        val=0.0;
        if(sev>0.0) 
          {val = sxev / sev;
           estimator = sxest / sev;
           printf("  trigger dac=%d charge=%6.2f fmean=%7.2f\n",
           dac,charge,val);
           dotacc(5,charge,val);
           sig=0.0;
           sig2=sxxev/sev - val*val;
           if(sig2>0.0) sig=sqrt(sig2);
           if(val>0.0) dotacc(9,charge,sig);
          }
        if(val>0.0 && val<4090.0)
         { crg[icnt] = charge;
           vlt[icnt] = val;
           if(charge>10.0)
            { s = s +1.0;
              sx = sx + charge;
              sy = sy + val;
              sxy = sxy + charge*val;
              sx2 = sx2 + charge*charge;}
           icnt++; }
   }  /* end of dac loop */
   icnt2=icnt;
   printf(" end of dac loop icnt=%d\n",icnt);
   printf(" end of dac loop icnt1=%d  icnt2=%d\n",icnt1,icnt2);
lofit:
   del = s*sx2 - sx*sx;
   a = (sx2*sy - sx*sxy) / del;
   b = (s*sxy - sx*sy) / del;
   printf(" trigger fit results a=%7.2f b=%7.2f y=a+b*charge\n",a,b);
   s = 0.0;
   sx = 0.0;
   dymax = 0.0;
   for(k=0;k<icnt;k++)
    {x = crg[k];
     y = a + b*x;
     dely = y - vlt[k];
     s = s + 1.0;
     sx = sx + fabs(dely);
     sw3 = sw(3);
     if(sw3 == 1) printf("k=%d x=%f  y=%f  vlt[k]=%f  dely=%f\n",k,x,y,vlt[k],dely);
     dotacc(7,x,dely);
     if(fabs(dely)>dymax) dymax = fabs(dely);
    }
     noise = sx/s;
     if(sw(8) == 0)
      { dotmwr(2,2,3);
        dotwrt(5);
        dotwrt(7);
        dotwrt(9);
        hstwrt(0);
        hstwrt(2);
      }
/*-------------- test the trigger output switch -------------- */
    capacitor = 100.0;
     small_cap_disable(ttcadr);  /* tplo=0 */
   large_cap_enable(ttcadr);   /* tphi=1 */
    charge=150.0;
    fdac = (1023.0*charge) / (2.0*4.096*capacitor);
    dac = (int)fdac;
    charge = 2.0*4.096 * capacitor * (double)dac / 1023.0;
    printf("trigger charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    set_dac(ttcadr,dac);
    for(k=0;k<3;k++)
      { if(k==0) trig_enable(ttcadr);   /* enable the trig output */
        if(k==1) trig_disable(ttcadr);   /* disable the trig output */
        if(k==2) trig_enable(ttcadr);   /* enable the trig output */
        inject();
        fermi_read();
        asum = 0.0;
        axsum = 0.0;
        axxsum = 0.0;
        for(i=10;i<20;i++)
          { aval = (double)fdata[0][i];
            asum = asum+1.0;
            axsum = axsum + aval;
            axxsum = axxsum + aval*aval;
          }
        ped = axsum / asum;
        dped = 0.0;
        sig2=axxsum/asum - ped*ped;
        if(sig2>0.0) dped=sqrt(sig2);
        sprintf(string,
             "hi gain ipk=%d  pk=%d  ped=%f +/- %f",
             ipk[0],pk[0],ped,dped);
        if(sw(8) == 0) evdisp(50,string,0);
        tmax[k] = 0.0;
        for(i=20;i<30;i++) 
          { aval = (double)fdata[0][i];
            if(aval>tmax[k]) tmax[k] = aval;
          }
      }
   slope = b;
   kFAIL=0;
   if(slope<2.5) kFAIL=1;
   if(slope>2.8) kFAIL=2;
   if(noise>2.0) kFAIL=3;
   if(dymax>9.0) kFAIL=4;
   if(kFAIL != 0) FAIL = 1;

   printf(" TRIGGER   noise=%6.2f slope=%6.2f nonlin=%6.2f  ---------- FAIL=%d\n",
     noise,slope,dymax,FAIL);
   if(kFAIL == 0) fprintf(fp,"TRIGGER   noise=%6.2f slope=%6.2f dymax=%6.2f   OK\n\n",
     noise,slope,dymax);
   if(kFAIL != 0) fprintf(fp,"TRIGGER   noise=%6.2f slope=%6.2f dymax=%6.2f  BAD=%d\n\n",
     noise,slope,dymax,kFAIL);
   tok=1;
   if(tmax[0]>1500.0 && tmax[1]<150.0 && tmax[2]>1500.0) tok=0;
   if(tok == 0) printf(" trigger switch GOOD\n");
   if(tok == 1) printf(" trigger switch BAD  %f %f %f\n",tmax[0],tmax[1],tmax[2]);
   if(tok == 0) fprintf(fp," trigger switch GOOD\n");
   if(tok == 1) fprintf(fp," trigger switch BAD  %f %f %f\n",tmax[0],tmax[1],tmax[2]);
}  /* end of main */

fermi_setup()
 { fermi.trig=fermi_base+0x26;
   fermi.reset=fermi_base+0x24;
   fermi.status=fermi_base+0x22;
   fermi.sample=fermi_base+0x20;
   fermi.delay=fermi_base+0x1e;
   fermi.rdall=fermi_base+0x18;
   fermi.rd2=fermi_base+0x14;
   fermi.rd1=fermi_base+0x12;
   fermi.rd0=fermi_base+0x10;
/* issue reset */
   (unsigned short *)reg=(unsigned short *)fermi.reset;
   *reg=0;
/* set to full 255 samples */
   (unsigned short *)reg=(unsigned short *)fermi.sample;
   *reg=255;
/* set delay */
   (unsigned short *)reg=(unsigned short *)fermi.delay;
   *reg=2;
   (unsigned short *)reg0=(unsigned short *) fermi.rd0;
   (unsigned short *)reg1=(unsigned short *) fermi.rd1;
   (unsigned short *)reg2=(unsigned short *) fermi.rd2;
 }
fermi_reset()
 { 
/* issue reset */
   (unsigned short *)reg=(unsigned short *)fermi.reset;
   *reg=0;
 }
fermi_read()
 { int stat,data_ready,busy,lcnt,k0,k1,k2,kmax0,kmax1,kmax2,i,kkk,kja;
   unsigned short dd0,dd1,dd2;
   unsigned short sd0,sd1,sd2;
   /* wait for a trigger */
loop:
   lcnt=0;
   lcnt++;
   (unsigned short *)reg = (unsigned short *)(fermi_base + 0x22);
   stat = *reg;
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
   kkk=255;
      sw4 = sw(4);
   for (i=0;i<kkk;i++)
    {  sd0 = *reg0;
      dd0 = sd0 & 0xfff;
      fdata[0][i] = 0xfff - dd0;
      if(fdata[0][i]>kmax0)
       { kmax0=fdata[0][i];
         k0=i;}
      sd1 = *reg1;
      dd1 = sd1 & 0xfff;
      fdata[1][i] = 0xfff - dd1;
      if(fdata[1][i]>kmax1)
       { kmax1=fdata[1][i];
         k1=i;}
      sd2 = *reg2;
      dd2 = sd2 & 0xfff;
      fdata[2][i] = 0xfff - dd2;
      if(fdata[2][i]>kmax2)
       { kmax2=fdata[2][i];
         k2=i;}
      if(sw4 == 1) printf("i=%d  d0=%x  d1=%x  d2=%x\n",i,fdata[0][i],fdata[1][i],fdata[2][i]);
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
evdisp(nn,string,l)
 int nn,l;
 char *string;
 { int max,mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,k,ii;
   int idel;
   double dely,y,dy;
   char str[5];
   erasm();
   max = pk[l];
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
    { ii = fdata[l][i];
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
       { 
         fermi_reset();
         if(ent==0)
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
             i3time=2005;
	    /*  printf("input i3time\n");
             fscanf(stdin,"%d",&i3time);   
             printf("i3time=%d\n",i3time); */
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


chk_int()
 { int isw[6],i1,i2,ll,i,mode,s1,s2,s3,s4,sw8,itr,ire,mse,tpe,lcal;
   int sw1,dacset,max[6],del[6],imax;
   short cadr,sval,dval,tval,t1val,t2val;
   double fmin,ratio,volt,ped,expr[6];
   int l,gain,icnt,sbad,kja,ikja;
   double s,sx,sy,sxy,sx2,delta,a,b,y,noise,sqr,slope;
   double charge,dymax,dely,factor,dymaxf;
   double p1,p2,p3,d1,d2,d3;
   long status;
   factor = 9.6/4096.0;
   multi_low(ttcadr);
   back_low(ttcadr);
   set_tube(ttcadr,card);
   set_intg_gain(ttcadr,0);   /* s1=s3=0 s2=s4=0 */
   itr_low(ttcadr);           /* itr=0 */
   intg_rd_low(ttcadr);        /* ire=0 */
   mse_low(ttcadr);           /* mse=0 */

    max[0]=400;
    max[1]=80;
    max[2]=80;
    max[3]=40;
    max[4]=30;
    max[5]=20;

    del[0]=10;
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

   dotzro();
   for(i=0;i<6;i++)
    { if(i==0) dotset(i,(double)max[i],0.0,5.0,0.0,0,0);
      if(i!=0) dotset(i,(double)max[i],0.0,10.0,0.0,0,0);
      sprintf(xstring,"isw=%x xaxis=dac value",isw[i]);
      sprintf(ystring,"yaxis=volts");
      dotlbl(i,xstring,ystring);
      if(i==0) dotset(i+6,(double)max[i],0.0,0.1,-0.1,0,0);
      if(i!=0) dotset(i+6,(double)max[i],0.0,0.1,-0.1,0,0);
      sprintf(xstring,"isw=%x xaxis=dac value",isw[i]);
      sprintf(ystring,"yaxis=delta volts");
      dotlbl(i,xstring,ystring);
    }
   for(gain=0;gain<6;gain++)
    { printf("starting gain=%d\n",gain);
      if(gain==0) status=adc_dacset(id,100);
      if(gain==1) status=adc_dacset(id,100);
      if(gain==2) status=adc_dacset(id,100);
      if(gain==3) status=adc_dacset(id,80);
      if(gain==4) status=adc_dacset(id,70);
      if(gain==5) status=adc_dacset(id,50);
      if(status != 0) printf(" adc_dacset status=%x\n",status);
      set_intg_gain(ttcadr,isw[gain]);
      intg_rd_low(ttcadr);  /* intg_rd=0 */
      itr_low(ttcadr);  /* itr=0 */
      s = 0.0;
      sx = 0.0;
      sx2 = 0.0;
      set_dac(ttcadr,0);
      sleep(1);
      for(kja=0;kja<5;kja++)
        { go_get(&ped);
        } 
      for(i=0;i<5;i++)
       { go_get(&p1);
         go_get(&p2);
         go_get(&p3);
         d1 = fabs(p1-p2);
         d2 = fabs(p2-p3);
         d3 = fabs(p1-p3);
         ikja=1;
         if(d1<d1 && d1<d3) ikja=1;
         if(d2<d1 && d2<d3) ikja=2;
         if(d3<d1 && d3<d2) ikja=3;
         if(ikja == 1) ped = 0.5*(p1+p2);
         if(ikja == 2) ped = 0.5*(p2+p3);
         if(ikja == 3) ped = 0.5*(p1+p3);
         s = s + 1.0;
         sx = sx + ped;
         sx2 = sx2 + ped*ped;
       }
      ped = sx/s;
      sqr = sx2/s - ped*ped;
      noise = 0.0;
      if(sqr>0.0) noise = sqrt(sqr)*factor;
      printf("gain setting %d  ped=%f\n",gain,ped);
      dacset = max[gain];
      icnt = 0;
      s = 0.0;
      sx = 0.0;
      sy = 0.0;
      sxy = 0.0;
      sx2 = 0.0;
      for(i=0;i<dacset;i=i+del[gain])       
       { sw1 = sw(1);
         set_dac(ttcadr,i);
	 usleep(100);
         go_get(&p1);
         go_get(&p2);
         go_get(&p3);
         d1 = fabs(p1-p2);
         d2 = fabs(p2-p3);
         d3 = fabs(p1-p3);
         ikja=1;
         if(d1<d2 && d1<d3) ikja=1;
         if(d2<d1 && d2<d3) ikja=2;
         if(d3<d1 && d3<d2) ikja=3;
         if(ikja == 1) volt = 0.5*(p1+p2);
         if(ikja == 2) volt = 0.5*(p2+p3);
         if(ikja == 3) volt = 0.5*(p1+p3);
         fmin=volt-ped;
         if(sw1 == 0) printf("dac=%d  volt=%5.2f  ped=%8.4f    %8.4f %8.4f %8.4f\n",
               i,volt,ped,p1,p2,p3);
         dotacc(gain,(double)i,fmin);
          
         charge = 4.0 * 100.0 * (double)i / 1023.0;
         s = s +1.0;
         sx = sx + charge;
         sy = sy + volt;
         sxy = sxy + charge*volt;
         sx2 = sx2 + charge*charge;
         crg[icnt] = charge;
         vlt[icnt] = volt;
         kdac[icnt] = i;
         icnt++;

       }
       delta = s*sx2 - sx*sx;
       a = (sx2*sy - sx*sxy) / delta;
       b = (s*sxy - sx*sy) / delta;
       printf(" integrator gain=%d fit results a=%f b=%f y=a+b*charge\n",
               gain,a,b);
       dymax = 0.0;
       imax = -1;
       s = 0.0;
       sx = 0.0;
       for(i=0;i<icnt;i++)
        { y = a + b * crg[i];
          dely = vlt[i]-y;
          if(fabs(dely)>dymax) 
            { dymax = fabs(dely);
              imax = i;
            }
         printf("  i=%d  vlt=%5.2f  y=%5.2f  dely=%7.5f\n",
               i,vlt[i],y,dely);
         dotacc(gain+6,(double)kdac[i],dely);
         s = s + 1.0;
         sx = sx + fabs(dely);
        }
       dymaxf = dymax;
       slope = b;
       noise = sx/s;
       kFAIL=0; 
       if(noise>lim_sgped[gain]) kFAIL=1;
       if(kFAIL==1) printf("%d ----- noise=%f  limit=%f ---------- FAIL=1\n",
             gain,noise,lim_sgped[gain]);
       if(dymaxf>lim_dymx[gain]) kFAIL=2;
       if(kFAIL==2) printf("%d ----- dymaxf=%f  limit=%f --------- FAIL=2\n",
             gain,dymaxf,lim_dymx[gain]);
       if(slope<lim_slp[gain]-lim_dslp[gain]) kFAIL=3;
       if(kFAIL==3) printf("%d ----- slopelo=%f   limit=%f --------- FAIL=3\n",
             gain,slope,lim_slp[gain]-lim_dslp[gain]);
       if(slope>lim_slp[gain]+lim_dslp[gain]) kFAIL=4;
       if(kFAIL==4) printf("%d ----- slopehi=%f   limit=%f --------- FAIL=3\n",
             gain,slope,lim_slp[gain]+lim_dslp[gain]);
       printf("%d ----- integrator nonlin=%f %f %d  --------------- FAIL=%d\n",
          gain,dymaxf,lim_dymx[gain],imax,kFAIL);
       if(kFAIL != 0) FAIL = kFAIL;
       if(kFAIL == 0) fprintf(fp,"Gain %d  ped=%6.4f  noise=%6.4f  slope=%6.4f   nonlin=%6.4f   OK\n",
           gain,ped,noise,slope,dymaxf);
       if(kFAIL != 0) fprintf(fp,"Gain %d  ped=%6.4f  noise=%6.4f  slope=%6.4f   nonlin=%6.4f  BAD\n",
           gain,ped,noise,slope,dymaxf);
    }

   sbad = 0;
   set_dac(ttcadr,48);
   intg_rd_low(ttcadr);  /* intg_rd=0 */
   itr_low(ttcadr);  /* itr=0 */
   sleep(1);
   go_get(&volt);
   if(volt < 8.0) sbad = 1;
   printf("intg_rd=0 itr=0 on volt=%f  %d\n",volt,sbad);

   intg_rd_low(ttcadr);  /* intg_rd=0 */
   itr_high(ttcadr);  /* itr=1 */
   sleep(1);
   go_get(&volt);
   if(volt > 2.0) sbad = 1;
   printf("intg_rd=0 itr=1 on volt=%f  %d\n",volt,sbad);
   
   intg_rd_high(ttcadr);  /* intg_rd=1 */
   itr_low(ttcadr);  /* itr=0 */
   sleep(1);
   go_get(&volt);
   if(volt > 2.0) sbad = 1;
   printf("intg_rd=1 itr=0 on volt=%f  %d\n",volt,sbad);

   intg_rd_low(ttcadr);  /* intg_rd=0 */
   itr_low(ttcadr);  /* itr=0 */
   sleep(1);
   go_get(&volt);
   if(volt < 8.0) sbad = 1;
   printf("intg_rd=0 itr=0 on volt=%f  %d\n",volt,sbad);

   if(sbad == 0) fprintf(fp," Gate Open/Close : Good\n");
   if(sbad != 0) fprintf(fp," Gate Open/Close : Bad\n");

   printf("card=%d\n",card);
   printf("\n\n\n");
   fflush(stdout);
   if(sw(7) == 0)
    { sidev(1);
      dotmwr(6,0,5);
      dotmwr(6,6,11);
    }
}

  go_get(double* v)
   { unsigned int command;
     unsigned short shrt;
     long status;
     double val,factor;
     factor=9.6/4096.0;
     usleep(10000);  //kjakjakja
     command = 0xc400 | card;
     status=adc_convert(id,command,&shrt);
     val = factor * (double)shrt;
     *v = val;
   }

      waitkja(knt)
       int knt;
       { int i;
         sn0=0.1234;
	 for(i=0;i<knt;i++)
	   { sn1=sin(sn0);
	     sn0=sin(sn1);
	   }  
       }

      double dxyfit(l,k,x)
      int l,k;
      double x;
      { double dxyf;
        dxyf=0.0;  /* dummy routine */
        return(dxyf);
      }

 fpsw(int i)
  { int ent=0,l;
    unsigned long *pio,kk;
    if(ent==0) 
      { pio_base = vme24base + 0x570000;
        pio = (unsigned long *)pio_base;
        ent=1;}
    kk = *pio;
    l = (kk>>i) & 1;
/*    printf(" fpsw i=%d  val=%d\n",i,l);  */
    return(l);
  }

