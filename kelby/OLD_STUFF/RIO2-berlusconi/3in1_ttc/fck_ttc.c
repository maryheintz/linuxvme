#include <stdio.h>
#include <math.h>
#include <time.h>
#include <types.h>
#include <sys/timers.h>
#include <ces/vmelib.h>
struct block
       { unsigned short reg0;        /* base+0 */
         unsigned short reg2;        /* base+2 */
         unsigned short reg4;        /* base+4 */
         unsigned short reg6;        /* base+6 */
         unsigned short reg8;        /* base+8 */
         unsigned short rega;        /* base+a */
       };
struct block *adc1 = (struct block *)0xdeff0000;
struct block *adc2 = (struct block *)0xdeee0000;
static struct pdparam_master param=
      { 1,   /* VME Iack (must be 1 for all normal pages) */
        0,   /* VME Read Prefetch Option */
        0,   /* VME Write Posting Option */
        1,   /* VME Swap Option */
        0,   /* page number return */
        0,   /* reserved */
        0,   /* reserved */
        0 };
unsigned long *swtch, *adr;
int sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8;
int fdata[40],pk,ipk;
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
    int ipeak,timc,timf;
    int zone,sector,tube;
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
/*    printf("input - zone sector tubd & bigain tube number\n");
    fscanf(stdin,"%d %d %d %d",&zone,&sector,&tube,&bigain); */
    l = sw(0); /* initialize switch reading */
    ttcvi_map();
    init_old();
    flex_setup();
    flex_setup();
    zone = 0;
    sector = 0;
    tube = 38;
    set_zone(zone);
    set_sector(sector);
    set_tube(tube);
    set_swln2(1);
    set_swln1(0);

    nevmax=25;
    pmaxh=0.0;
    cmaxh=0.0;
    tmaxh=0.0;
    pmaxl=0.0;
    cmaxl=0.0;
    tmaxl=0.0;
    timc=10;
    timf=0;


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
    capacitor = 5.2;
    large_cap_disable();
    usleep(10);
    small_cap_enable();
    usleep(10);
    charge=11.0;
    fdac = (1023.0*charge) / (2.0 * 4.096 * capacitor);
    dac = (int)fdac;
    charge = 2.0 * 4.096 * capacitor * (double)dac / 1023.0;
    printf("higain charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    set_dac(dac);
    usleep(10);
    flex1_reset();
    flex2_reset();
    inject();
   i=0;
   for (timf=0; timf<240; timf=timf+4)  /* start l loop */
   {    SetTimeDes2(0,timf);  
loop:
     sum=0.0;
     xhsump=0.0;
     xxhsump=0.0;
     xhsumc=0.0;
     xxhsumc=0.0;
     for(nevnt=0;nevnt<nevmax;nevnt++)
      {
re1:
        flex1_reset();
        inject();
/*     go read the fermi module ------------------------------- */
        flex1_read();
	if(fdata[ipk] < 500) goto re1;
/*     display ? */
         sw1 = sw(1);
         if(sw1 == 1) 
          { asum = 0.0;
            axsum = 0.0;
            axxsum = 0.0;
            for(i=10;i<20;i++)
             { aval = (double)fdata[i];
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
              ipk,pk,ped,dped);
            evdisp(32,string,2);}
/* now gets sums for mean calculation */
       sum=sum+1.0;
       yc=(double)fdata[ipk];
       xhsumc=xhsumc+yc;
       xxhsumc=xxhsumc+yc*yc;
       yc=(double)ipk;
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
   if(meanhc>pmaxh)
    { pmaxh=meanhc;
      cmaxh=meanhp;
      tmaxh=timf; }
   dotacc(0,(double)timf,meanhc);
   dotacc(1,(double)timf,meanhp);
   }  /* end of dac loop */
/* --------------------- determine logain timing -------------------*/
    ktime = 2400;
    capacitor = 100.0;
    large_cap_enable();
    usleep(10);
    small_cap_disable();
    usleep(10);
    charge=700.0;
    fdac = (1023.0*charge) / (2.0 * 4.096 * capacitor);
    dac = (int)fdac;
    charge = 2.0 * 4.096 * capacitor * (double)dac / 1023.0;
    printf("higain charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    set_dac(dac);
    usleep(10);
    flex1_reset();
    flex2_reset();
    inject();
   i=0;
   for (timf=0; timf<240; timf=timf+4)  /* start l loop */
   { SetTimeDes2(0,timf);  
     sum=0.0;
     xlsump=0.0;
     xxlsump=0.0;
     xlsumc=0.0;
     xxlsumc=0.0;
     for(nevnt=0;nevnt<nevmax;nevnt++)
      {
re2:
        flex2_reset();
        inject();
/*     go read the fermi module ------------------------------- */
        flex2_read();
	if(fdata[ipk] < 500) goto re2;
/*     display ? */
         sw1 = sw(1);
         if(sw1 == 1) 
          { asum = 0.0;
            axsum = 0.0;
            axxsum = 0.0;
            for(i=10;i<20;i++)
             { aval = (double)fdata[i];
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
              ipk,pk,ped,dped);
            evdisp(32,string,2);}
/* now gets sums for mean calculation */
      if(fdata[ipk]<100) break;
       sum=sum+1.0;
       yc=(double)fdata[ipk];
       xlsumc=xlsumc+yc;
       xxlsumc=xxlsumc+yc*yc;
       yc=(double)ipk;
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
   if(meanlc>pmaxl)
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
   large_cap_disable();
   usleep(10);
   small_cap_enable();
   usleep(10);
   kpeak=(int)(cmaxh+0.5);
   klo=kpeak-2;
   khi=kpeak+2;
   timf=(int)(tmaxh+0.5);
   printf("starting higain  kpeak=%d timf=%d\n",kpeak,timf);
   kpeakh = kpeak;
   timfh = timf;
   SetTimeDes2(0,timf);  
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
	flex1_reset();
        inject();
/*     go read the fermi module ------------------------------- */
        flex1_read();
/*     display ? */
         sw1 = sw(1);
         if(sw1 == 1) 
          { asum = 0.0;
            axsum = 0.0;
            axxsum = 0.0;
            for(i=10;i<20;i++)
             { aval = (double)fdata[i];
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
              ipk,pk,ped,dped);
            evdisp(32,string,2);}
        asum = 0.0;
        axsum = 0.0;
        axxsum = 0.0;
        for(i=10;i<20;i++)
         { aval = (double)fdata[i];
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
        if(fdata[ipk]>1020) goto fithi;
        val = (double)fdata[kpeak] - ped;
	if(val<0.0) break;
        if(ipk>=klo && ipk<=khi && fdata[kpeak]<1020)
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
	      if(icnt > 6500) printf("error icnt=%d\n",icnt); 
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
   large_cap_enable();
   usleep(10);
   small_cap_disable();
   usleep(10);
   kpeak=(int)(cmaxl+0.5);
   klo=kpeak-2;
   khi=kpeak+2;
   timf=(int)(tmaxl+0.5);
   printf("starting logain  kpeak=%d timf=%d\n",kpeak,timf);
   kpeakl = kpeak;
   timfl = timf;
   SetTimeDes2(0,timf);
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
	flex2_reset();
        inject();
/*     go read the fermi module ------------------------------- */
        flex2_read();
/*     display ? */
         sw1 = sw(1);
         if(sw1 == 1) 
          { asum = 0.0;
            axsum = 0.0;
            axxsum = 0.0;
            for(i=10;i<20;i++)
             { aval = (double)fdata[i];
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
              ipk,pk,ped,dped);
            evdisp(32,string,2);}
        asum = 0.0;
        axsum = 0.0;
        axxsum = 0.0;
        for(i=10;i<20;i++)
         { aval = (double)fdata[i];
           asum = asum+1.0;
           axsum = axsum + aval;
           axxsum = axxsum + aval*aval;
         }
        ped = axsum / asum;
        hstacc(0,ped,1.0);
        sig2=axxsum/asum - ped*ped;
        if(sig2>0.0) dped=sqrt(sig2);
        hstacc(2,dped,1.0);
        if(fdata[ipk]>1020) goto lofit;
        val = (double)fdata[kpeak] - ped;
	if(val<0.0) break;
        if(ipk>=klo && ipk<=khi && fdata[kpeak]<1020)
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
           icnt++;
	   if(icnt > 6500) printf("error icnt=%d\n",icnt); 
          }
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
/*     fprintf(fp,"%4d %5d %4d %9.2f %6.2f %4.1f %5.2f %5d %4d %8.2f %4.2f %4.1f %5.2f\n",
          tube,kpeakh,timfh,ah,bh,pedhi,dpedhi,
                 kpeakl,timfl,al,bl,pedlo,dpedlo);  */
        ratio = bh/bl;
    printf("tube=%d bh=%f  bl=%f  ratio=%f\n",tube,bh,bl,ratio);
    sidev(0);
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
       { static int entry=0;
         int c1,c2,f1,f2,swtst,i,i1,i2,i3time;
         if(entry == 0)
	   { /* setup inhibit1 to set tp high */
	     SetBMode(1,0xc);
	     SetInhibitDelay(1,10);
	     SetInhibitDuration(1,10);
	     BPutLong(1,0,1,0x80,0x00);  /* load set tp_high into fifo1 */
	     /* set inhibit2 to drop tp low (timed broadcast mode */
	     SetBMode(2,0xc);
	     SetInhibitDelay(2,2000);
	     SetInhibitDuration(2,10);
	    /* BPutLong(2,0,1,0x80,0x01);  */ /* load set tp_low into fifo2 */
	     BPutShort(2,0xc0);   /* set tp low bcast command  */
	     /* set inhibit3 to set latch                     */
	     /* use latch-bar and inhibit3 to start fermi     */
             /* will use inhibit 3 to set l1a with digitizers */
             i3time=2045;
/*	     printf("input i3time\n");
             fscanf(stdin,"%d",&i3time);  */
	     SetInhibitDelay(3,i3time);
	     SetInhibitDuration(3,10);
	     /* setup the switch box timers */
	     c1=1;
             c2=1;
             f1=5;
             f2=5; 
             setime(c1,f1,c2,f2);
	     restim();
	     rlatch_old();  
	     entry = 1;
	    }
	 /* send pulse out switch box through fanout and
	          into b_go1, b_go2, and b_go3 inputs
            send inhibit1 output into the switch box latch
	    send inhibit3 output into l1a0 or fermi start input */  

           tp_high();
	   usleep(2);
	   stvme();  /* send pulses from the switch box */
loop:
           usleep(1);
	   swtst = latch_old();  /* wait for the latch bit to set */
           if(swtst == 0) goto loop; 
	/*   printf("event found\n");  */
           restim();  /* reset the timer for next trigger */
           rlatch_old();    /* reset the latch bit */
	   trigger_old();
           usleep(1);
      }
flex_setup()
  {     adc1->reg2 = 32;
    adc2->reg2 = 32;
  }
flex1_reset()
  { adc1->reg0 = 0;
  }
flex1_read()
  { int i,k,adr,adc_enable,read_done,adc_done;
    unsigned short kk;
    sw4 = sw(4);
    pk = 0;
    ipk = 0; 
    for(i=0;i<32;i++)
      { kk = adc1->reg8;
       adc1->rega = 0; /* clock the shift register */
        adr = kk & 0xff;
        adc_done = (kk & 0x2000)>>13;
        adc_enable = (kk & 0x4000)>>14;
        read_done = (kk & 0x8000)>>15; 
        kk = adc1->reg4;
        k = kk & 0xfff;
        k = (k>>2);  /* make it 10 bits */
        fdata[i] = k;
        if(k>pk)
          { ipk = i;
            pk = k;}
        if(sw4 == 1) printf(" i=%d  adr = %x  data=%x  en=%d rd=%d  ad=%d\n",
         i,adr,(int)kk,adc_enable,read_done,adc_done);
      }
}
flex2_reset()
  { adc2->reg0 = 0;
  }
flex2_read()
  { int i,k,adr,adc_enable,read_done,adc_done;
    unsigned short kk;
    sw4 = sw(4);
    pk = 0;
    ipk = 0; 
    for(i=0;i<32;i++)
      { kk = adc2->reg8;
       adc2->rega=0; /* clock the shift register */
        adr = kk & 0xff;
        adc_done = (kk & 0x2000)>>13;
        adc_enable = (kk & 0x4000)>>14;
        read_done = (kk & 0x8000)>>15; 
        kk = adc2->reg4;
        k = kk & 0xfff;
        k = (k>>2);  /* make it 10 bits */
        fdata[i] = k;
        if(k>pk)
          { ipk = i;
            pk = k;}
        if(sw4 == 1) printf(" i=%d  adr = %x  data=%x  en=%d rd=%d  ad=%d\n",
         i,adr,k,adc_enable,read_done,adc_done);
      }
}
evdisp(nn,string)
 int nn;
 char *string;
 { int max,mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,k,ii;
   int idel;
   double dely,y,dy;
   char str[5];
   erasm();
   max = pk;
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
    { ii = fdata[i];
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
  {  unsigned short *adr,k;
     int shft,l;
     (unsigned short *)adr = (unsigned short *)0xdeaa000c;
      k = *adr;
        shft=i-1;
        l=(k>>shft)&1;
	return(l); 
  }      
kjasleep(i)
int i;
{int k;
 double d,f=2.0;
 for (k=0;k<i;k++)
   { d = sqrt(f);}
}
  init_old()
  { unsigned long am,vmead,offset,size,len,k;
    int l,shft;
       am=0x39;
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
   latch_old()
     { unsigned long k;
       int l;
       k = *swtch;
       l = k & 1;
       return(l);
     }
   rlatch_old()
     { unsigned long k;
       k = (unsigned long)swtch | 8;
       (unsigned long *)adr = (unsigned long *)k;
       *adr = 0;
     }
   trigger_old()
     { unsigned long k;
       k = (unsigned long)swtch | 4;
       (unsigned long *)adr = (unsigned long *)k;
       *adr = 0;
     }
