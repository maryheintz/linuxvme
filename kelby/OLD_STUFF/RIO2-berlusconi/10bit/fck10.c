#include <stdio.h>
#include <math.h>
#include <time.h>
#include <types.h>
#include <sys/timers.h>
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
        0 
      };
int sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8;
int fdata[3][40],pk[3],ipk[3];
double crg[6500],fvl[6500];
time_t usec = 1;
int ktime;
main()
  { FILE *fp;
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    double meanhp,meanhc,meanlp,meanlc;
    double sum,xhsump,xxhsump,xhsumc,xxhsumc,xlsump,xxlsump,xlsumc,xxlsumc;
    double sighp,sighc,pmaxh,cmaxh,tmaxh;
    double siglp,siglc,pmaxl,cmaxl,tmaxl;
    int nevnt,nevmax,nstrtev,l,nn,i,k,wait;
    int ipeak,timc,timf,kpk;
    int zone,sector,card;
    int dac,time1,time2,time3,time4;
    double fdac,charge,val,ratio;
    int bigain,kpeak,klo,khi,icnt,dstep;
    double s,sx,sy,sxy,sev,sxev,sxxev,el,er,ep;
    double sig,sx2,volt,capacitor,a,b,del,x,y,dely;
    double pedhi,dpedhi,pedlo,dpedlo;
    char xstring[80],ystring[80],string[80],err[80];
    unsigned short cadr,tc,tf;
    int kpeakh,timfh,kpeakl,timfl;
    double ah,bh,al,bl;
    unsigned short sval,dval;
/*    printf("input - zone sector card & bigain card number\n");
    fscanf(stdin,"%d %d %d %d",&zone,&sector,&card,&bigain); */
    l = sw(0); /* initialize switch reading */
    map_3in1();
    fermi_setup();
    zone = 1;
    sector = 1;
    
   for(i=0;i<49;i++)
    { card = i;
      l = (zone<<12) | (sector<<6) | card;
      cadr = (unsigned short)l;
      card_sel(cadr);
      sval = (unsigned short)0x555;
      wrt_shift(12,sval);
      read_shift(&dval);
      if(sval != (dval&0xfff) ) continue;
      sval = (unsigned short)0xaaa;
      wrt_shift(12,sval);
      read_shift(&dval);
      if(sval != (dval&0xfff) ) continue;
      card = i;
      goto found;
    }
    printf("no card found\n");
    return(0);
found:
    printf("card %d found\n",card);
    sprintf(string,"fck%d.ps",card);
    sidev(1);
    pfset(string);

    fp=fopen("fck.dat","w");   

    i = (zone<<12) | (sector<<6) | card;
    cadr = (unsigned short)i;
    time1 = 0;
    time2 = 0;
    time3 = 0;
    time4 = 0;
    set_drawer_timer(zone,sector,1,time1);
    set_drawer_timer(zone,sector,2,time2);
    set_drawer_timer(zone,sector,3,time3);
    set_drawer_timer(zone,sector,4,time4);
    tc = (unsigned short)15;
    card_sel(cadr);
    send12_3in1(2,0,0);   /* s1=s2=s3=s4=0 */
    send4_3in1(1,0);   /* itr=0 */
    send4_3in1(0,0);   /* ire=0 */
    send4_3in1(3,0);   /* mse=0 */

    nevmax=25;
    pmaxh=0.0;
    cmaxh=0.0;
    tmaxh=0.0;
    pmaxl=0.0;
    cmaxl=0.0;
    tmaxl=0.0;
    timc=15;
    timf=0;

    tc = (unsigned short)timc;
    tim1_set(tc);
    tf = (unsigned short)timf;
    tim2_set(tf);

        dotset(0,250.0,0.0,1000.0,500.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"high gain FERMI peak channel amplitude");
        dotlbl(0,xstring,ystring);

        dotset(1,250.0,0.0,30.0,20.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"high gain peak channel number");
        dotlbl(1,xstring,ystring);

        dotset(2,250.0,0.0,1000.0,500.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"low gain FERMI peak channel amplitude");
        dotlbl(2,xstring,ystring);

        dotset(3,250.0,0.0,30.0,20.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"low gain peak channel number");
        dotlbl(3,xstring,ystring);

        dotset(4,20.0,0.0,1000.0,0.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"high gain fermi peak");
        dotlbl(4,xstring,ystring);

        dotset(5,800.0,0.0,1000.0,0.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"low gain fermi peak");
        dotlbl(5,xstring,ystring);

        dotset(6,20.0,0.0,5.0,-5.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"high gain fit y-yfit");
        dotlbl(6,xstring,ystring);  

        dotset(7,800.0,0.0,5.0,-5.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"low gain fit y-yfit");
        dotlbl(7,xstring,ystring);

        dotset(8,20.0,0.0,5.0,-5.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"high gain sigma");
        dotlbl(8,xstring,ystring);

        dotset(9,800.0,0.0,5.0,-5.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"low gain sigma");
        dotlbl(9,xstring,ystring);

        hstset(0,50.0,0.0,50,1);
        hstlbl(0,"logain pedestal");

        hstset(1,50.0,0.0,50,1);
        hstlbl(1,"higain pedestal");

        hstset(2,2.0,0.0,50,1);
        hstlbl(2,"logain pedestal");

        hstset(3,5.0,0.0,50,1);
        hstlbl(3,"higain pedestal");

/* --------------------- determine higain timing -------------------*/
    ktime = 2400;
/*    printf("plug higain into ADC1\n");
    wait = getchar(); */
    capacitor = 5.2;
    send4_3in1(4,1);   /* enable small C */
    send4_3in1(5,0);   /* disable large C */
    charge=11.0;
    fdac = (1023.0*charge) / (2.0 * 4.096 * capacitor);
    dac = (int)fdac;
    charge = 2.0 * 4.096 * capacitor * (double)dac / 1023.0;
    printf("higain charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    set_dac(dac);
    fermi_reset();
    rtime();
    inject();
   i=0;
   for (timf=0; timf<256; timf++)  /* start l loop */
   { tf = (unsigned short)timf;
     tim2_set(tf);
loop:
     sum=0.0;
     xhsump=0.0;
     xxhsump=0.0;
     xhsumc=0.0;
     xxhsumc=0.0;
     for(nevnt=0;nevnt<nevmax;nevnt++)
      {
        fermi_reset();
        rtime();
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
            evdisp(32,string,2);}
/* now gets sums for mean calculation */
       sum=sum+1.0;
       kpk=ipk[2]; 
       yc=(double)fdata[2][kpk];
       xhsumc=xhsumc+yc;
       xxhsumc=xxhsumc+yc*yc;
       yc=(double)ipk[2];
       xhsump=xhsump+yc;
       xxhsump=xxhsump+yc*yc;
     } /* end of nevnt loop */
   sw8 = sw(8);
   if(sw8 == 1) goto loop;
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
    ktime = 2400;
    capacitor = 100.0;
    send4_3in1(4,0);   /* disable small C */
    send4_3in1(5,1);   /* dnable large C */
    charge=700.0;
    fdac = (1023.0*charge) / (2.0 * 4.096 * capacitor);
    dac = (int)fdac;
    charge = 2.0 * 4.096 * capacitor * (double)dac / 1023.0;
    printf("higain charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    set_dac(dac);
    fermi_reset();
    rtime();
    inject();
   i=0;
   for (timf=0; timf<256; timf++)  /* start l loop */
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
            evdisp(32,string,1);}
/* now gets sums for mean calculation */
       sum=sum+1.0;
       kpk=ipk[1];
       yc=(double)fdata[1][kpk];
       xlsumc=xlsumc+yc;
       xxlsumc=xxlsumc+yc*yc;
       yc=(double)ipk[1];
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
   ktime = 2400;
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
   dstep = 4;
      for (dac=0; dac<400; dac=dac+dstep)
     { set_dac(dac);
      volt = 2.0 * 4.096 * (double)dac/1023.0;
      charge = volt * capacitor;
      sev = 0.0;
      sxev = 0.0;
      sxxev = 0.0;
      for(nevnt=0;nevnt<nevmax;nevnt++)
       {
rstrth:
	fermi_reset();
        rtime();
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
              "lo gain ipk=%d  pk=%d  ped=%f +/- %f",
              ipk[2],pk[2],ped,dped);
            evdisp(32,string,2);}
        asum = 0.0;
        axsum = 0.0;
        axxsum = 0.0;
        for(i=10;i<20;i++)
         { aval = (double)fdata[2][i];
           asum = asum+1.0;
           axsum = axsum + aval;
           axxsum = axxsum + aval*aval;
         }
        if(asum < 1.0) printf(" error asum==%f\n",asum);
        ped = axsum / asum;
	if(ped<10.0) printf("error ped=%f axsum=%f asum=%f\n",ped,axsum,asum);
        hstacc(1,ped,1.0);
        sig2=axxsum/asum - ped*ped;
        if(sig2>0.0) dped=sqrt(sig2);
         hstacc(3,dped,1.0);
	 kpk=ipk[2];
        if(fdata[2][kpk]>1020) goto fithi;
        val = (double)fdata[2][kpeak] - ped;
	if(val<0.0) break;
        if(ipk[2]>=klo && ipk[2]<=khi && fdata[2][kpeak]<1020)
         { sev = sev + 1.0;
           sxev = sxev + val;
           sxxev = sxxev + val*val;
	 }
        sw8 = sw(8);
        if(sw8 == 1) goto rstrth;
        
      } /* end of nevnt loop */
        val=0.0;
        if(sev>1.0) 
          {val = sxev / sev;
           printf(" high gain dac=%d charge=%f fmean=%f\n",
           dac,charge,val);
           dotacc(4,charge,val);
           sig=0.0;
           sig2=sxxev/sev - val*val;
           if(sig2>0.0) sig=sqrt(sig2);
           if(val>0.0) dotacc(8,charge,sig);
          }
        if(val>0.0 && val<1000.0)
         { crg[icnt] = charge;
           fvl[icnt] = val;
           if(charge>0.0)
            { s = s +1.0;
              sx = sx + charge;
              sy = sy + val;
              sxy = sxy + charge*val;
              sx2 = sx2 + charge*charge;}
              icnt++; 
	    }
   }  /* end of dac loop */
fithi:
   del = s*sx2 - sx*sx;
   if(del < 1.0) printf("error del=%f\n",del);
   a = (sx2*sy - sx*sxy) / del;
   b = (s*sxy - sx*sy) / del;
   printf(" higain fit results a=%f b=%f y=a+b*charge\n",a,b);
   ah = a;
   bh = b;
   for(k=0;k<icnt;k++)
    {x = crg[k];
     y = a + b*x;
     dely = y - fvl[k];
     sw3 = sw(3);
     if(sw3 == 1) printf("k=%d x=%f  y=%f  fvl[k]=%f  dely=%f\n",
       k,x,y,fvl[k],dely);
     dotacc(6,x,dely);
    }
/* ---------------------- low gain calibration --------------------------*/
   ktime = 2400;
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
   dstep = 12;
   for (dac=0; dac<1024; dac=dac+dstep)
     { set_dac(dac);
      volt = 2.0 * 4.096 * (double)dac/1023.0;
      charge = volt * capacitor;
      sev = 0.0;
      sxev = 0.0;
      sxxev = 0.0;
      for(nevnt=0;nevnt<nevmax;nevnt++)
       {
rstrtl:
	fermi_reset();
        rtime();
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
            evdisp(32,string,1);}
        asum = 0.0;
        axsum = 0.0;
        axxsum = 0.0;
        for(i=10;i<20;i++)
         { aval = (double)fdata[1][i];
           asum = asum+1.0;
           axsum = axsum + aval;
           axxsum = axxsum + aval*aval;
         }
        ped = axsum / asum;
        hstacc(0,ped,1.0);
        sig2=axxsum/asum - ped*ped;
        if(sig2>0.0) dped=sqrt(sig2);
        hstacc(2,dped,1.0);
	kpk=ipk[1];
        if(fdata[1][kpk]>1020) goto lofit;
        val = (double)fdata[1][kpeak] - ped;
	if(val<0.0) break;
        if(ipk[1]>=klo && ipk[1]<=khi && fdata[1][kpeak]<1020)
         { sev = sev + 1.0;
           sxev = sxev + val;
           sxxev = sxxev + val * val;
         }
        sw8 = sw(8);
        if(sw8 == 1) goto rstrtl;
      } /* end of nevnt loop */
        val=0.0;
        if(sev>0.0) 
          {val = sxev / sev;
           printf("  low gain dac=%d charge=%f fmean=%f\n",
           dac,charge,val);
           dotacc(5,charge,val);
           sig=0.0;
           sig2=sxxev/sev - val*val;
           if(sig2>0.0) sig=sqrt(sig2);
           if(val>0.0) dotacc(9,charge,sig);
          }
        if(val>0.0 && val<1000.0)
         { crg[icnt] = charge;
           fvl[icnt] = val;
           if(charge>50.0)
            { s = s +1.0;
              sx = sx + charge;
              sy = sy + val;
              sxy = sxy + charge*val;
              sx2 = sx2 + charge*charge;
	    }
           icnt++; }
   }  /* end of dac loop */
lofit:
   del = s*sx2 - sx*sx;
   a = (sx2*sy - sx*sxy) / del;
   b = (s*sxy - sx*sy) / del;
   al = a;
   bl = b;
   printf(" logain fit results a=%f b=%f y=a+b*charge\n",a,b);
   for(k=0;k<icnt;k++)
    {x = crg[k];
     y = a + b*x;
     dely = y - fvl[k];
     sw3 = sw(3);
     if(sw3 == 1) printf("k=%d x=%f  y=%f  fvl[k]=%f  dely=%f\n",k,x,y,fvl[k],dely);
     dotacc(7,x,dely);
    }
     fprintf(fp,"%4d %5d %4d %9.2f %6.2f %4.1f %5.2f %5d %4d %8.2f %4.2f %4.1f %5.2f\n",
          card,kpeakh,timfh,ah,bh,pedhi,dpedhi,
                 kpeakl,timfl,al,bl,pedlo,dpedlo);
    ratio = bh/bl;
    printf("card=%d bh=%f  bl=%f  ratio=%f\n",card,bh,bl,ratio);
/*    sidev(0);
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
     plot_reset(); 
     printf("second chance to view plots\n");   */
     sidev(1);
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

}  /* end of main */
inject()
  { tp_high();
    kjasleep(ktime);  
    stime();
  }
fermi_setup()
  { unsigned long am,vmead,offset,size,len;
    am=0x39;
    vmead=0x0;
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
      dd0 = (int)sd0 & 0x3ff;
      fdata[0][i] = dd0;
      if(fdata[0][i]>kmax0)
       { kmax0=fdata[0][i];
         k0=i;}
      sd1 = adc->rd1;
      dd1 = (int)sd1 & 0x3ff;
      fdata[1][i] = dd1;
      if(fdata[1][i]>kmax1)
       { kmax1=fdata[1][i];
         k1=i;}
      sd2 = adc->rd2;
      dd2 = (int)sd2 & 0x3ff;
      fdata[2][i] = dd2;
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
evdisp(nn,string,kkk)
 int nn,kkk;
 char *string;
 { int max,mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,k,ii;
   int idel;
   double dely,y,dy;
   char str[5];
   erasm();
   max = pk[kkk];
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
    { ii = fdata[kkk][i];
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
sw(i)
  int i;
  { unsigned long am,vmead,offset,size,len,k;
    int l,shft;
    static unsigned long *swtch;
    if(i==0)
     { am=0x39;
       vmead=0xcf0000;
       offset=0;
       size=0;
       len=400;
       swtch = 
         (unsigned long *)( find_controller(vmead,len,am,offset,size,&param) );
       if( swtch == (unsigned long *)(-1) ) 
        { printf("unable to map vme address\n");
          exit(0);}
       return(0);	  
      }
     if(i>0 && i<9)	  
      { k=*swtch;
        shft=23+i;
        l=(k>>shft)&1;
	return(l); 
      }
  }      
kjasleep(i)
int i;
{int k;
 double d,f=2.0;
 for (k=0;k<i;k++)
   { d = sqrt(f);}
}