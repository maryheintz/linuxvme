--------------needs much conversion work ------------------------------
#include <stdio.h>
#include <math.h>
#include <time.h>
struct sgtbuf timebuf;
int sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8;
int fdata[3][256],ipk[3],pk[3];
struct{unsigned int trig;
       unsigned int reset;
       unsigned int status;
       unsigned int sample;
       unsigned int delay;
       unsigned int rdall;
       unsigned int rd2;
       unsigned int rd1;
       unsigned int rd0;}fermi;
unsigned short *reg;
unsigned short *reg0,*reg1,*reg2;
unsigned int base=0xf0ee0000;
unsigned int base2=0x80ee0000;
double crg[6500],fvl[6500];
unsigned int delay;
int card,zone,sector;
int year,month,day,hour,min,sec;
char xstring[80],ystring[80],string[80],err[80];
unsigned char gpib_adr = 4;
double nl[6] = {3.0,3.0,5.0,8.0,10.0,10.0};
double nll[6] = {3.0,5.0,5.0,8.0,12.0,20.0};
double sl[6] = {11.7,108.0,117.0,230.0,320.0,430.0};
double sh[6] = {14.3,132.0,143.0,280.0,390.0,510.0};
int ent=0,FAIL,kFAIL;
FILE *fp;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{  double t0,tnow,dt;
   int cardnumber,ii;
    printf("input 3in1 card number\n");
    fscanf(stdin,"%d",&cardnumber);
    sprintf(string,"/h0/kja/3in1_13bit/C%d.pre",cardnumber);
    printf("%s\n",string);
    fp = fopen(string,"w");
    fprintf(fp,"serial number: %d     Bed: 7  Position: 47\n",cardnumber);
    newtime();
    year = year + 1900;
    fprintf(fp," date tested: %2d/%2d/%4d      Test Number: 4\n\n",month,day,year);
    multi_low();
    card = 47;
    zone = 1;
    sector = 1;
    FAIL=0;
    chk_digital();
    chk_fast();
    chk_trig();
    chk_int();
    printf("c%d.pre\n",cardnumber);
    ii=fclose(fp);
    sprintf(string,"list /h0/kja/3in1_13bit/C%d.pre",cardnumber);
    system(string);
    if(FAIL==0)
     { printf(" CARD PASSED\n");
       fprintf(fp,"CARD PASSED\n");
     }
    if(FAIL!=0)
     { printf(" CARD FAILED\n");
       fprintf(fp,"CARD FAILED\n");
     }
    printf("c%d.pre\n",cardnumber);
}

chk_digital()
   {int l,i,ok;
    unsigned short cadr,sval;
    int s1,s2,s3,s4,itr,ire,mse,tpS,tpL,trg;
    ok=0;
    l = (zone<<12) | (sector<<6) | card;
    cadr = (unsigned short)l;
    card_sel(cadr);
    send13_3in1(2,5);  /* s1=s3=1 s2=s4=0 */
    send4_3in1(1,1);   /* itr=1 */
    send4_3in1(0,0);   /* ire=0 */
    send4_3in1(3,1);   /* mse=1 */
    send4_3in1(4,0);   /* tpS=0 */
    send4_3in1(5,1);   /* tpL=1 */
    send4_3in1(7,0);   /* trig=0 */
    read_status(&sval);   /* read back the controller shift register */
    i = (int)sval;
    trg = (i>>3) & 1;
    s1 = (i>>4) & 1;
    s2 = (i>>5) & 1;
    s3 = (i>>6) & 1;
    s4 = (i>>7) & 1;
    itr = (i>>8) & 1;
    ire = (i>>9) & 1;
    mse = (i>>10) & 1;
    tpS = (i>>11) & 1;
    tpL = (i>>12) & 1;
    if( (s1==1) && (s2==0) && (s3==1) && (s4==0) && (itr==1) && (ire==0)
         && (mse==1) && (tpS==0) && (tpL==1) && (trg==0) )ok++;
    if(sw(2)==1)
       {printf("s1..4=1 0 1 0 itr=1 ire=0 mse=1 tpS=0 tpL=1 trg=0 sent  card=%d\n",card);
        printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tpS=%d tpL=%d trg=%d %x\n"
          ,s1,s2,s3,s4,itr,ire,mse,tpS,tpL,trg,i,card);
       }
    send13_3in1(2,0xa);  /* s1=s3=0 s2=s4=1 */
    send4_3in1(1,0);   /* itr=0 */
    send4_3in1(0,1);   /* ire=1 */
    send4_3in1(3,0);   /* mse=0 */
    send4_3in1(4,1);   /* tpS=1 */
    send4_3in1(5,0);   /* tpL=0 */
    send4_3in1(7,1);   /* trig=1 */
    tsleep(0x80000020);
    read_status(&sval);   /* read back the controller shift register */
    i = (int)sval;
    trg = (i>>3) & 1;
    s1 = (i>>4) & 1;
    s2 = (i>>5) & 1;
    s3 = (i>>6) & 1;
    s4 = (i>>7) & 1;
    itr = (i>>8) & 1;
    ire = (i>>9) & 1;
    mse = (i>>10) & 1;
    tpS = (i>>11) & 1;
    tpL = (i>>12) & 1;
    if( (s1==0) && (s2==1) && (s3==0) && (s4==1) && (itr==0) && (ire==1)
     && (mse==0) && (tpS==1) && (tpL==0) && (trg==1) )ok++;
    if(ok!=2) 
      { if(sw(2)==1)
         {printf("s1..4=0 1 0 1 itr=0 ire=1 mse=0 tpS=1 tpL=0 trg=1 sent\n");
          printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tpS=%d tpL=%d trg=%d %x\n"
            ,s1,s2,s3,s4,itr,ire,mse,tpS,tpL,trg,i);}
         }
    if(ok != 2) 
      { printf("card %d not found\n",card);
        fprintf(fp," Read/Write All Registers: Bad\n");
        FAIL=1;
      }
    if(ok == 2) 
      { printf("card %d found\n",card);
        fprintf(fp," Read/Write All Registers: Good\n\n");
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
    set_drawer_timer(zone,sector,1,time1);
    set_drawer_timer(zone,sector,2,time2);
    set_drawer_timer(zone,sector,3,time3);
    set_drawer_timer(zone,sector,4,time4);
    multi_low();
    i = (zone<<12) | (sector<<6) | card;
    cadr = (unsigned short)i;
    card_sel(cadr);
    send13_3in1(2,0);   /* s1=s2=s3=s4=0 */
    send4_3in1(1,0);   /* itr=0 */
    send4_3in1(0,1);   /* ire=0 */
    send4_3in1(3,1);   /* mse=0 */
    send4_3in1(7,1);   /* trig enable */
    tc = (unsigned short)25;
    tim1_set(tc);   /* coarse */
    tf = 0;
    tim2_set(tf);   /* fine */


    nevmax=50;
    pmaxh=0.0;
    cmaxh=0.0;
    tmaxh=0.0;
    pmaxl=0.0;
    cmaxl=0.0;
    tmaxl=0.0;

        dotzro();
        hstrst();

        dotset(0,250.0,0.0,1500.0,500.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"high gain FERMI peak channel amplitude");
        dotlbl(0,xstring,ystring);

        dotset(1,250.0,0.0,30.0,20.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"high gain peak channel number");
        dotlbl(1,xstring,ystring);

        dotset(2,250.0,0.0,2000.0,1000.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"low gain FERMI peak channel amplitude");
        dotlbl(2,xstring,ystring);

        dotset(3,250.0,0.0,30.0,20.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"low gain peak channel number");
        dotlbl(3,xstring,ystring);

        dotset(4,15.0,0.0,2000.0,0.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"high gain fermi peak");
        dotlbl(4,xstring,ystring);

        dotset(5,800.0,0.0,2000.0,0.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"low gain fermi peak");
        dotlbl(5,xstring,ystring);

        dotset(6,15.0,0.0,5.0,-5.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"high gain fit y-yfit");
        dotlbl(6,xstring,ystring);  

        dotset(7,800.0,0.0,5.0,-5.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"low gain fit y-yfit");
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

        hstset(2,2.0,0.0,50,1);
        hstlbl(2,"logain pedestal sigma");

        hstset(3,5.0,0.0,50,1);
        hstlbl(3,"higain pedestal sigma");

/* --------------------- determine higain timing -------------------*/
    delay = 0x80000005;
    capacitor = 5.2;
    send4_3in1(4,1);   /* enable small C */
    send4_3in1(5,0);   /* disable large C */
    charge=8.0;
    fdac = (1023.0*charge) / (8.0*capacitor);
    dac = (int)fdac;
    charge = 8.0 * capacitor * (double)dac / 1023.0;
    printf("higain charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    send13_3in1(6,dac);
    rtime();
    inject();
   i=0;
   for (timf=0; timf<256; timf=timf+4)  /* start l loop */
   { tf = (unsigned short)timf;
     tim2_set(tf);
     sum=0.0;
     xhsump=0.0;
     xxhsump=0.0;
     xhsumc=0.0;
     xxhsumc=0.0;
     for(nevnt=0;nevnt<nevmax;nevnt++)
      {
        fermi_reset();
        rtime();
        tsleep(0x80000002);
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
            evdisp(32,string,1);}
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
   if(meanhc>pmaxh && timf<125)
    { pmaxh=meanhc;
      cmaxh=meanhp;
      tmaxh=timf; }
   dotacc(0,(double)timf,meanhc);
   dotacc(1,(double)timf,meanhp);
   }  /* end of dac loop */
/* --------------------- determine logain timing -------------------*/
    delay = 0x80000006;
    capacitor = 100.0;
    send4_3in1(4,0);   /* disable small C */
    send4_3in1(5,1);   /* dnable large C */
    charge=700.0;
    fdac = (1023.0*charge) / (8.0*capacitor);
    dac = (int)fdac;
    charge = 8.0 * capacitor * (double)dac / 1023.0;
    printf("higain charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    for(i=0;i<dac;i++) send13_3in1(6,i);
    send13_3in1(6,dac);
    rtime();
    inject();
   i=0;
   for (timf=0; timf<256; timf=timf+4)  /* start l loop */
   { tf = (unsigned short)timf;
     tim2_set(tf);
     sum=0.0;
     xlsump=0.0;
     xxlsump=0.0;
     xlsumc=0.0;
     xxlsumc=0.0;
     for(nevnt=0;nevnt<nevmax;nevnt++)
      {
        fermi_reset();
        rtime();
        tsleep(0x80000002);
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
            evdisp(32,string,2);}
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
   if(meanlc>pmaxl && timf<125)
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
    delay = 0x80000005;
    capacitor = 5.2;
    send4_3in1(4,1);   /* enable small C */
    send4_3in1(5,0);   /* disable large C */
   kpeak=(int)(cmaxh+0.5);
   klo=kpeak-2;
   khi=kpeak+2;
   timf=(int)(tmaxh+0.5);
   printf("starting higain  kpeak=%d timf=%d\n",kpeak,timf);
   kpeakh = kpeak;
   timfh = timf;
   tf = (unsigned short)timf;
   tim2_set(tf);
   icnt=0;
   s=0.0;
   sx=0.0;
   sy=0.0;
   sxy=0.0;
   sx2=0.0;
   dstep = 10;
      for (dac=0; dac<400; dac=dac+dstep)
     {send13_3in1(6,dac);
      volt = 8.0 * (double)dac/1023.0;
      charge = volt * capacitor;
      if(charge<0.0) continue;
      sev = 0.0;
      sxev = 0.0;
      sxxev = 0.0;
      sxest = 0.0;
      for(nevnt=0;nevnt<nevmax;nevnt++)
       {
        fermi_reset();
        rtime();
        tsleep(0x80000002);
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
            evdisp(32,string,1);}
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
           printf(" high gain dac=%d charge=%f fmean=%f\n",
           dac,charge,val);
           dotacc(4,charge,val);
           sig=0.0;
           sig2=sxxev/sev - val*val;
           if(sig2>0.0) sig=sqrt(sig2);
           if(val>0.0) dotacc(8,charge,sig);
          }
        if(val>0.0 && val<4090.0)
         { crg[icnt] = charge;
           fvl[icnt] = val;
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
   printf(" higain fit results a=%f b=%f y=a+b*charge\n",a,b);
   ah = a;
   bh = b;
   slopeh = bh*factor;
   s = 0.0;
   sx = 0.0;
   for(k=0;k<icnt;k++)
    {x = crg[k];
     y = a + b*x;
     dely = y - fvl[k];
     s = s + 1.0;
     sx = sx + fabs(dely);
     sw3 = sw(3);
     if(sw3 == 1) printf("k=%d x=%f  y=%f  fvl[k]=%f  dely=%f\n",
       k,x,y,fvl[k],dely);
     dotacc(6,x,dely);
    }
   dymaxh = factor * (sx/s);
/* ---------------------- low gain calibration --------------------------*/
    delay = 0x80000006;
    capacitor = 100.0;
    send4_3in1(4,0);   /* disable small C */
    send4_3in1(5,1);   /* enable large C */
   kpeak=(int)(cmaxl+0.5);
   klo=kpeak-2;
   khi=kpeak+2;
   timf=(int)(tmaxl+0.5);
   printf("starting logain  kpeak=%d timf=%d\n",kpeak,timf);
   kpeakl = kpeak;
   timfl = timf;
   tf = (unsigned short)timf;
   tim2_set(tf);
   icnt=0;
   s=0.0;
   sx=0.0;
   sy=0.0;
   sxy=0.0;
   sx2=0.0;
   dstep = 20;
   for (dac=0; dac<1024; dac=dac+dstep)
     {send13_3in1(6,dac);
      volt = 8.0 * (double)dac/1023.0;
      charge = volt * capacitor;
      sev = 0.0;
      sxev = 0.0;
      sxxev = 0.0;
      sxest = 0.0;
      for(nevnt=0;nevnt<nevmax;nevnt++)
       {
        fermi_reset();
        rtime();
        tsleep(0x80000002);
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
            evdisp(32,string,2);}
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
           printf("  low gain dac=%d charge=%f fmean=%f\n",
           dac,charge,val);
           dotacc(5,charge,val);
           sig=0.0;
           sig2=sxxev/sev - val*val;
           if(sig2>0.0) sig=sqrt(sig2);
           if(val>0.0) dotacc(9,charge,sig);
          }
        if(val>0.0 && val<4090.0)
         { crg[icnt] = charge;
           fvl[icnt] = val;
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
   printf(" logain fit results a=%f b=%f y=a+b*charge\n",a,b);
   s = 0.0;
   sx = 0.0;
   for(k=0;k<icnt;k++)
    {x = crg[k];
     y = a + b*x;
     dely = y - fvl[k];
     s = s + 1.0;
     sx = sx + fabs(dely);
     sw3 = sw(3);
     if(sw3 == 1) printf("k=%d x=%f  y=%f  fvl[k]=%f  dely=%f\n",k,x,y,fvl[k],dely);
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
    send4_3in1(4,0);   /* disable small C */
    send4_3in1(5,1);   /* dnable large C */
    charge=150.0;
    fdac = (1023.0*charge) / (8.0*capacitor);
    dac = (int)fdac;
    charge = 8.0 * capacitor * (double)dac / 1023.0;
    printf("higain charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    send13_3in1(6,dac);
    for(k=0;k<3;k++)
      { if(k==0) send4_3in1(7,1);   /* enable the  output */
        if(k==1) send4_3in1(7,0);   /* disable the trig output */
        if(k==2) send4_3in1(7,1);   /* enable the trig output */
        rtime();
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
        if(sw(8) == 0) evdisp(32,string,0);
      }
   hstkms(3,&mh,&sh);
   kFAIL=0;
   if(slopeh<68.0) kFAIL=1;
   if(slopeh>95.0) kFAIL=1;
   if(noiseh>5.0) kFAIL=1;
   if(dymaxh>30.0) kFAIL=1;
   if(kFAIL != 0) FAIL = kFAIL;
   noiseh = factor*mh;
   printf("high gain noise=%f slope=%f nonlin=%f\n",noiseh,slopeh,dymaxh);   
   if(kFAIL == 0) fprintf(fp,"high gain   noise=%6.2f slope=%6.2f nonlin=%6.2f   OK\n",
           noiseh,slopeh,dymaxh);
   if(kFAIL != 0) fprintf(fp,"high gain   noise=%6.2f slope=%6.2f nonlin=%6.2f  BAD\n",
           noiseh,slopeh,dymaxh);
   hstkms(2,&ml,&sl);
   kFAIL = 0;
   if(slopel<1.0) kFAIL=1;
   if(slopel>1.5) kFAIL=1;
   if(noisel>5.0) kFAIL=1;
   if(dymaxl>30.0) kFAIL=1;
   if(kFAIL != 0) FAIL = kFAIL;
   noisel = factor*ml;
   printf(" low gain  noise=%f slope=%f nonlin=%f\n",noisel,slopel,dymaxl);
   if(kFAIL == 0) fprintf(fp," low gain   noise=%6.2f slope=%6.2f nonlin=%6.2f   OK\n",
           noisel,slopel,dymaxl);
   printf(" low gain  noise=%f slope=%f nonlin=%f\n",noisel,slopel,dymaxl);
   if(kFAIL != 0) fprintf(fp," low gain   noise=%6.2f slope=%6.2f nonlin=%6.2f  BAD\n",
           noisel,slopel,dymaxl);
   printf("card=%d bh=%f  bl=%f  ratio=%f\n\n",card,bh,bl,ratio);
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
    double factor,factor2,slopel,dymaxl,noisel,tmax[3],tok;
    double ml,sl;
    unsigned short sval,dval;
    factor = 2000.0 / 4096.0;  /* convert counts to mv */
    factor2 = 3.8;  /* compensate for adder gain */
    fermi_setup();
    time1 = 0;
    time2 = 0;
    time3 = 0;
    time4 = 0;
    set_drawer_timer(zone,sector,1,time1);
    set_drawer_timer(zone,sector,2,time2);
    set_drawer_timer(zone,sector,3,time3);
    set_drawer_timer(zone,sector,4,time4);
    multi_low();
    i = (zone<<12) | (sector<<6) | card;
    cadr = (unsigned short)i;
    card_sel(cadr);
    send13_3in1(2,0);   /* s1=s2=s3=s4=0 */
    send4_3in1(1,0);   /* itr=0 */
    send4_3in1(0,1);   /* ire=0 */
    send4_3in1(3,1);   /* mse=0 */
    send4_3in1(7,1);   /* trig enable */
    tc = (unsigned short)25;
    tim1_set(tc);   /* coarse */
    tf = 0;
    tim2_set(tf);   /* fine */


    nevmax=50;
    pmaxl=0.0;
    cmaxl=0.0;
    tmaxl=0.0;

        dotzro();
        hstrst();

        dotset(2,250.0,0.0,500.0,250.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"trigger FERMI peak channel amplitude");
        dotlbl(2,xstring,ystring);

        dotset(3,250.0,0.0,30.0,20.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"trigger peak channel number");
        dotlbl(3,xstring,ystring);


        dotset(5,200.0,0.0,2000.0,0.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"trigger fermi peak");
        dotlbl(5,xstring,ystring);


        dotset(7,200.0,0.0,15.0,-15.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"trigger fit y-yfit");
        dotlbl(7,xstring,ystring);


        dotset(9,200.0,0.0,15.0,-15.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"trigger sigma");
        dotlbl(9,xstring,ystring);

        hstset(0,100.0,50.0,50,1);
        hstlbl(0,"trigger pedestal");


        hstset(2,10.0,0.0,50,1);
        hstlbl(2,"trigger pedestal sigma");

/* --------------------- determine trigger output timing -------------------*/
    delay = 0x80000006;
    capacitor = 100.0;
    send4_3in1(4,0);   /* disable small C */
    send4_3in1(5,1);   /* dnable large C */
    charge=150.0;
    fdac = (1023.0*charge) / (8.0*capacitor);
    dac = (int)fdac;
    charge = 8.0 * capacitor * (double)dac / 1023.0;
    printf("TRIGGER charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    for(i=0;i<dac;i++) send13_3in1(6,i);
    send13_3in1(6,dac);
    rtime();
    inject();
   i=0;
   for (timf=0; timf<256; timf=timf+4)  /* start l loop */
   { tf = (unsigned short)timf;
     tim2_set(tf);
     sum=0.0;
     xlsump=0.0;
     xxlsump=0.0;
     xlsumc=0.0;
     xxlsumc=0.0;
     for(nevnt=0;nevnt<nevmax;nevnt++)
      {
        fermi_reset();
        rtime();
        tsleep(0x80000002);
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
              ipk[1],pk[1],ped,dped);
            evdisp(32,string,2);}
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
    delay = 0x80000006;
    capacitor = 100.0;
    send4_3in1(4,0);   /* disable small C */
    send4_3in1(5,1);   /* enable large C */
   kpeak=(int)(cmaxl+0.5);
   klo=kpeak-2;
   khi=kpeak+2;
   timf=(int)(tmaxl+0.5);
   printf("starting trigger  kpeak=%d timf=%d\n",kpeak,timf);
   kpeakl = kpeak;
   timfl = timf;
   tf = (unsigned short)timf;
   tim2_set(tf);
   icnt=0;
   s=0.0;
   sx=0.0;
   sy=0.0;
   sxy=0.0;
   sx2=0.0;
   dstep = 4;
   for (dac=0; dac<256; dac=dac+dstep)
     {send13_3in1(6,dac);
      send13_3in1(6,dac);
      volt = 8.0 * (double)dac/1023.0;
      charge = volt * capacitor;
      sev = 0.0;
      sxev = 0.0;
      sxxev = 0.0;
      sxest = 0.0;
      for(nevnt=0;nevnt<nevmax;nevnt++)
       {
        fermi_reset();
        rtime();
        tsleep(0x80000002);
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
              ipk[1],pk[1],ped,dped);
            evdisp(32,string,2);}
        asum = 0.0;
        axsum = 0.0;
        axxsum = 0.0;
        for(i=10;i<20;i++)
         { aval = (double)fdata[0][i];
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
           printf("  trigger dac=%d charge=%f fmean=%f\n",
           dac,charge,val);
           dotacc(5,charge,val);
           sig=0.0;
           sig2=sxxev/sev - val*val;
           if(sig2>0.0) sig=sqrt(sig2);
           if(val>0.0) dotacc(9,charge,sig);
          }
        if(val>0.0 && val<4090.0)
         { crg[icnt] = charge;
           fvl[icnt] = val;
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
   al = a;
   bl = b;
   printf(" trigger fit results a=%f b=%f y=a+b*charge\n",a,b);
   s = 0.0;
   sx = 0.0;
   for(k=0;k<icnt;k++)
    {x = crg[k];
     y = a + b*x;
     dely = y - fvl[k];
     s = s + 1.0;
     sx = sx + fabs(dely);
     sw3 = sw(3);
     if(sw3 == 1) printf("k=%d x=%f  y=%f  fvl[k]=%f  dely=%f\n",k,x,y,fvl[k],dely);
     dotacc(7,x,dely);
    }
     dymaxl = sx/s;
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
    send4_3in1(4,0);   /* disable small C */
    send4_3in1(5,1);   /* dnable large C */
    charge=150.0;
    fdac = (1023.0*charge) / (8.0*capacitor);
    dac = (int)fdac;
    charge = 8.0 * capacitor * (double)dac / 1023.0;
    printf("trigger charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    send13_3in1(6,dac);
    for(k=0;k<3;k++)
      { if(k==0) send4_3in1(7,1);   /* enable the trig output */
        if(k==1) send4_3in1(7,0);   /* disable the trig output */
        if(k==2) send4_3in1(7,1);   /* enable the trig output */
        rtime();
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
        if(sw(8) == 0) evdisp(32,string,0);
        tmax[k] = 0.0;
        for(i=20;i<30;i++) 
          { aval = (double)fdata[0][i];
            if(aval>tmax[k]) tmax[k] = aval;
          }
      }
   hstkms(2,&ml,&sl);
   slopel = factor*factor2*bl;
   noisel = ml;
   dymaxl = factor*factor2*dymaxl;
   kFAIL=0;
   if(slopel<16.0) kFAIL=1;
   if(slopel>24.0) kFAIL=1;
   if(noisel>5) kFAIL=1;
   if(dymaxl>8) kFAIL=1;
   if(kFAIL != 0) FAIL = kFAIL;

   printf(" TRIGGER   noise=%6.2f slope=%6.2f nonlin=%6.2f  FAIL=%d\n",
     noisel,slopel,dymaxl,FAIL);
   if(kFAIL == 0) fprintf(fp,"TRIGGER   noise=%6.2f slope=%6.2f nonlin=%6.2f   OK\n\n",
     noisel,slopel,dymaxl);
   if(kFAIL != 0) fprintf(fp,"TRIGGER   noise=%6.2f slope=%6.2f nonlin=%6.2f  BAD\n\n",
     noisel,slopel,dymaxl);
   tok=1;
   if(tmax[0]>1500.0 && tmax[1]<150.0 && tmax[2]>1500.0) tok=0;
   if(tok == 0) printf(" trigger switch GOOD\n");
   if(tok == 1) printf(" trigger switch BAD  %f %f %f\n",tmax[0],tmax[1],tmax[2]);
   if(tok == 0) fprintf(fp," trigger switch GOOD\n");
   if(tok == 1) fprintf(fp," trigger switch BAD  %f %f %f\n",tmax[0],tmax[1],tmax[2]);
}  /* end of main */

sw(i)   /* new switch timer module */
int i;
{ int k,l,shift;
  unsigned short *adr;
  adr = (unsigned short *)( 0xf0aa000c);
  k = *adr;
  shift = i-1;
  l = (k>>shift) & 1;
  return(l);
}

fermi_setup()
 { FPermit(base,0x100,3);
   FPermit(base2,0x100,3);
   fermi.trig=base+0x26;
   fermi.reset=base+0x24;
   fermi.status=base+0x22;
   fermi.sample=base+0x20;
   fermi.delay=base+0x1e;
   fermi.rdall=base2+0x18;
   fermi.rd2=base+0x14;
   fermi.rd1=base+0x12;
   fermi.rd0=base+0x10;
/* issue reset */
   reg=(unsigned short *)fermi.reset;
   *reg=0;
/* set to full 255 samples */
   reg=(unsigned short *)fermi.sample;
   *reg=255;
/* set delay */
   reg=(unsigned short *)fermi.delay;
   *reg=2;
   reg0=(unsigned short *) fermi.rd0;
   reg1=(unsigned short *) fermi.rd1;
   reg2=(unsigned short *) fermi.rd2;
 }
fermi_reset()
 { 
/* issue reset */
   reg=(unsigned short *)fermi.reset;
   *reg=0;
 }
fermi_read()
 { int stat,data_ready,busy,lcnt,i,k0,k1,k2,kmax0,kmax1,kmax2;
   int dd0,dd1,dd2;
   unsigned short sd0,sd1,sd2;
   /* wait for a trigger */
loop:
   lcnt=0;
   lcnt++;
   reg = (unsigned short *)fermi.status;
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
   for (i=0;i<255;i++)
    { sd0 = *reg0;
      dd0 = (int)sd0 & 0xfff;
      fdata[0][i] = 0xfff - dd0;
      if(fdata[0][i]>kmax0)
       { kmax0=fdata[0][i];
         k0=i;}
      sd1 = *reg1;
      dd1 = (int)sd1 & 0xfff;
      fdata[1][i] = 0xfff - dd1;
      if(fdata[1][i]>kmax1)
       { kmax1=fdata[1][i];
         k1=i;}
      sd2 = *reg2;
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
 { fermi_reset();
   tp_high();
   tsleep(0x80000002);
   stime();
 }
newtime()
 {  getime(&timebuf);
    year = (int)timebuf.t_year;
    month = (int)timebuf.t_month;
    day = (int)timebuf.t_day;
    hour = (int)timebuf.t_hour;
    min = (int)timebuf.t_minute;
    sec = (int)timebuf.t_second;
/*    printf("newtime  year=%d  month=%d  day=%d  hour=%d  min=%d  sec=%d\n",
          year,month,day,hour,min,sec);  */
 }

chk_int()
 { int isw[6],i1,i2,ll,i,mode,s1,s2,s3,s4,sw8,itr,ire,mse,tpe,lcal;
   int sw1,dacset,max[6],del[6],imax;
   short cadr,sval,dval,tval,t1val,t2val;
   double fmin,ratio,volt,ped,expr[6];
   int l,gain,icnt,sbad;
   double s,sx,sy,sxy,sx2,delta,a,b,y,noise,sqr,slope;
   double charge,crg[30],vlt[30],dymax,dely,factor,dymaxf;
   factor = 1000.0;
   card = 47;
   i = (zone<<12) | (sector<<6) | card;
   cadr = (unsigned short)i;
   multi_low();
   card_sel(cadr);
   tval = (short)100;
   tim1_set(tval);
   tval = (short)50;
   tim2_set(tval);
   rtime();

   max[0]=800;
   max[1]=200;
   max[2]=180;
   max[3]=100;
   max[4]=60;
   max[5]=50;

   del[0]=80;
   del[1]=20;
   del[2]=16;
   del[3]=10;
   del[4]=6;
   del[5]=5;

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

   for(i=0;i<6;i++)
    { if(i==0) dxyset(i,(double)max[i],0.0,5.0,0.0,0,0);
      if(i!=0) dxyset(i,(double)max[i],0.0,10.0,0.0,0,0);
      sprintf(xstring,"isw=%x xaxis=dac value",isw[i]);
      sprintf(ystring,"yaxis=volts");
      dxylbl(i,xstring,ystring);
    }
   for(gain=0;gain<6;gain++)
    { send13_3in1(2,isw[gain]);
      send4_3in1(0,0);  /* intg_rd=0 */
      send4_3in1(1,0);  /* itr=0 */
      s = 0.0;
      sx = 0.0;
      sx2 = 0.0;
      send13_3in1(6,0);
      tsleep(0x80000050);
      go_get(&ped);
      send13_3in1(6,0);
      tsleep(0x80000050);
      go_get(&ped);
      send13_3in1(6,0);
      tsleep(0x80000050);
      go_get(&ped);
      for(i=0;i<10;i++)
       { tsleep(0x80000020);
         go_get(&ped);
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
         send13_3in1(6,i);
         tsleep(0x80000020);
         go_get(&volt);
         fmin=volt-ped;
         if(sw1 == 0) printf("dac=%d  volt=%f  ped=%f\n",i,volt,ped);
         dxyacc(gain,(double)i,fmin,0.0,0);
          
         charge = 4.0 * 100.0 * (double)i / 1023.0;
         s = s +1.0;
         sx = sx + charge;
         sy = sy + volt;
         sxy = sxy + charge*volt;
         sx2 = sx2 + charge*charge;
         crg[icnt] = charge;
         vlt[icnt] = volt;
         icnt++;

       }
       delta = s*sx2 - sx*sx;
       a = (sx2*sy - sx*sxy) / delta;
       b = (s*sxy - sx*sy) / delta;
       printf(" integrator gain=%d fit results a=%f b=%f y=a+b*charge\n",
               gain,a,b);
       dymax = 0.0;
       imax = -1;
       for(i=0;i<icnt;i++)
        { y = a + b * crg[i];
          dely = y - vlt[i];
          if(dely>dymax) 
            { dymax = dely;
              imax = i;
            }
         printf("  i=%d  vlt=%f  y=%f  dely=%f\n",
               i,vlt[i],y,dely);
        }
       dymaxf = factor * dymax;
       slope = factor * b;
       kFAIL=0; 
       if(noise>nl[gain]) kFAIL=1;
       if(dymaxf>nll[gain]) kFAIL=1;
       if(slope<sl[gain]) kFAIL=1;
       if(slope>sh[gain]) kFAIL=1;
       printf("integrator nonlin=%f %f %d  FAIL=%d\n",dymaxf,dymax,imax,kFAIL);
       if(kFAIL != 0) FAIL = kFAIL;
       if(kFAIL == 0) fprintf(fp,"Gain %d  noise=%6.2f  slope=%6.2f   nonlin=%6.2f   OK\n",
           gain,noise,slope,dymaxf);
       if(kFAIL != 0) fprintf(fp,"Gain %d  noise=%6.2f  slope=%6.2f   nonlin=%6.2f  BAD\n",
           gain,noise,slope,dymaxf);
    }
   sbad = 0;
   send13_3in1(6,48);
   send4_3in1(0,0);  /* intg_rd=0 */
   send4_3in1(1,0);  /* itr=0 */
   tsleep(0x80000100);
   go_get(&volt);
   printf("intg_rd=0 itr=0 on volt=%f\n",volt);
   if(volt < 8.5) sbad = 1;

   send4_3in1(0,0);  /* intg_rd=0 */
   send4_3in1(1,1);  /* itr=1 */
   tsleep(0x80000100);
   go_get(&volt);
   printf("intg_rd=0 itr=1 on volt=%f\n",volt);
   if(volt > 2.0) sbad = 1;
   

   send4_3in1(0,1);  /* intg_rd=1 */
   send4_3in1(1,0);  /* itr=0 */
   tsleep(0x80000100);
   go_get(&volt);
   printf("intg_rd=1 itr=0 on volt=%f\n",volt);
   if(volt > 2.0) sbad = 1;
   send4_3in1(0,0);  /* intg_rd=0 */
   send4_3in1(1,0);  /* itr=0 */
   tsleep(0x80000100);
   go_get(&volt);
   printf("intg_rd=0 itr=0 on volt=%f\n",volt);
   if(volt < 8.5) sbad = 1;
   if(sbad == 0) fprintf(fp," Gate Open/Close : Good\n");
   if(sbad != 0) fprintf(fp," Gate Open/Close : Bad\n");

   printf("card=%d\n",card);
   printf("\n\n\n");
   fflush(stdout);
   if(sw(8) == 0)
    { sidev(1);
      dxymwr(6,0,5);
/*    sidev(0);
      dxymwr(6,0,5);  */
    }
}
/* old switch module
sw(i)
  int i;
  {int *adr,k,l,shft;
   adr=(int *)0x80cf0000;
   k=*adr;
   shft=23+i;
   l=(k>>shft)&1; 
   return(l);
  }      */

    go_get(v)
      double *v;
      {double volts;
       if(ent == 0)
         {gpib_reset();
          sendmessage(gpib_adr,"A2 X");
          tsleep(0x80000005);
/*          sendmessage(gpib_adr,"A? X");
          getmessage(gpib_adr, &string[0] );
          printf("A2 back=%s\n",string); */

          sendmessage(gpib_adr,"C X");
          tsleep(0x80000005);
/*          sendmessage(gpib_adr,"C? X");
          getmessage(gpib_adr, &string[0] );
          printf("C back=%s\n",string); */
  
          sendmessage(gpib_adr,"R3 X");
          tsleep(0x80000005);
/*          sendmessage(gpib_adr,"R? X");
          getmessage(gpib_adr, &string[0] );
          printf("R1,3 back=%s\n",string); */

          sendmessage(gpib_adr,"N1 X");
          tsleep(0x80000005);
/*          sendmessage(gpib_adr,"N? X");
          getmessage(gpib_adr, &string[0] );
          printf("N1 back=%s\n",string); */

          sendmessage(gpib_adr,"G0 X");
          tsleep(0x80000005);
/*          sendmessage(gpib_adr,"G? X");
          getmessage(gpib_adr, &string[0] );
          printf("G0 back=%s\n",string); */
  
          sendmessage(gpib_adr,"Q0 X");
          tsleep(0x80000005);
/*          sendmessage(gpib_adr,"Q? X");
          getmessage(gpib_adr, &string[0] );
          printf("Q0 back=%s\n",string); */

          ent=1;
         }

          sendmessage(gpib_adr,"T6 X");
/*          sendmessage(gpib_adr,"T? X");
          getmessage(gpib_adr, &string[0] );
          printf("T6 back=%s\n",string); */

          tsleep(0x80000020);
          getmessage(gpib_adr, &string[0] );
/*          printf("first readback=%s\n",string); */
          sscanf(string,"%le",&volts);
/*          printf("volts=%e\n",volts);  */

          *v=volts*0.95;
          return(0);
}

      double dxyfit(l,k,x)
      int l,k;
      double x;
      { double dxyf;
        dxyf=0.0;  /* dummy routine */
        return(dxyf);
      }


