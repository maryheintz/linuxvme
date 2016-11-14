#include <stdio.h>
#include <math.h>
#include <time.h>
int sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8;
int fdata[40],pk,ipk;
unsigned short *reg0,*reg2,*reg4,*reg6,*reg8,*rega;
unsigned int base;
double crg[500],fvl[500];
main()
  { FILE *fp;
    int dstep,dac;
    int nevnt,nevmax,i,k,icnt;
    int ipeak,kpeak,klo,khi,timf,timc;
    int wait;
    char xstring[80],ystring[80],string[80],err[80];
    unsigned short cadr,tf,tc;
    int zone,sector,card;
    double ped,dped,sig,sig2,asum,axsum,axxsum,aval,val;
    double volt,capacitor,charge;
    double ss,ssx,ssy,ssxy,ssx2;
    double sev,sxev,sxxev,sxest;
    double a,b,x,y,dely,del;
    double estimator,el,er,ep;
    fp = fopen("dac10.fit","w");
/* enable switch module */
/*    FPermit(0x80cf0000,0x100,3); */
    zone = 0;
    sector = 0;
    card = 1;
/*    printf("input - zone sector card\n");
    fscanf(stdin,"%d %d %d",&zone,&sector,&card);  */
    i = (zone<<12) | (sector<<6) | card;
    cadr = (unsigned short)i;
    card_sel(cadr);
    send12_3in1(2,0,0);   /* s1=s2=s3=s4=0 */
    send4_3in1(1,0);   /* itr=0 */
    send4_3in1(0,0);   /* ire=0 */
    send4_3in1(3,0);   /* mse=0 */
    timc=18;
    tc = (unsigned short)timc;
    tim1_set(tc);   /* coarse */
    tf = 0;
    tim2_set(tf);   /* fine */
    nevmax=100;
    printf(" setting up plots\n");

        dotset(0,800.0,0.0,1000.0,0.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"logain adc peak");
        dotlbl(0,xstring,ystring);

        dotset(2,800.0,0.0,5.0,-5.0,0,0);
/*        sprintf(xstring,"charge");
        sprintf(ystring,"logain fit y-yfit");
        dotlbl(2,xstring,ystring);  */

        dotset(4,800.0,0.0,5.0,-5.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"logain sigma");
        dotlbl(4,xstring,ystring);

        dotset(6,800.0,0.0,5.0,-5.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"logain extimator");
        dotlbl(6,xstring,ystring);

        dotset(8,800.0,0.0,0.01,-0.01,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"logain fit y-yfit/y");
        dotlbl(8,xstring,ystring);

        dotset(1,20.0,0.0,1000.0,0.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"higain adc peak");
        dotlbl(1,xstring,ystring);

        dotset(3,20.0,0.0,5.0,-5.0,0,0);
/*        sprintf(xstring,"charge");
        sprintf(ystring,"higain fit y-yfit");
        dotlbl(3,xstring,ystring);  */

        dotset(5,20.0,0.0,5.0,-5.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"higain sigma");
        dotlbl(5,xstring,ystring);

        dotset(7,20.0,0.0,5.0,-5.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"higain extimator");
        dotlbl(7,xstring,ystring);

        dotset(9,20.0,0.0,0.01,-0.01,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"higain fit y-yfit/y");
        dotlbl(9,xstring,ystring);

        hstset(0,50.0,0.0,50,1);
        hstlbl(0,"logain pedestal");

        hstset(1,50.0,0.0,50,1);
        hstlbl(1,"higain pedestal");

        hstset(2,2.0,0.0,50,1);
        hstlbl(2,"logain pedestal");

        hstset(3,5.0,0.0,50,1);
        hstlbl(3,"higain pedestal");

/* lo gain large C calibration */
   capacitor=100.0;
   send4_3in1(4,0);
   send4_3in1(5,1);
   kpeak=25;
   klo=24;
   khi=27;
   timf=21;
   tf = (unsigned short)timf;
   tim2_set(tf);
   icnt=0;
   ss=0.0;
   ssx=0.0;
   ssy=0.0;
   ssxy=0.0;
   ssx2=0.0;
   dstep = 4;
   sw7 = sw(7);
   if(sw7 == 1)
   {
   base = 0xf0ff0000;
   printf("plug logain into ADC1\n");
   wait = getchar();
   flex_setup();
      for (dac=0; dac<1024; dac=dac+dstep)
     { set_dac(dac);   /* set dac value */
      volt = 2.0*4.096 * (double)dac/1023.0;
      charge = volt * capacitor;
      sev = 0.0;
      sxev = 0.0;
      sxxev = 0.0;
      sxest = 0.0;
      for(nevnt=0;nevnt<nevmax;nevnt++)
       {
        flex_reset();
        rtime();
        tsleep(0x80000002);
        inject();
/*     go read the fermi module ------------------------------- */
        flex_read();
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
        dped = 0.0;
        sig2=axxsum/asum - ped*ped;
        if(sig2>0.0) dped=sqrt(sig2);
        hstacc(2,dped,1.0);
        val = (double)fdata[kpeak] - ped;
        el = (double)fdata[kpeak-1] - ped;
        er = (double)fdata[kpeak+1] - ped;
        ep = val;
        estimator = (el-er)/ep;
        ipeak=ipk;
        if(fdata[kpeak]>1020)
          {sev=0.0;
           break;
          }
        if(ipeak>=klo && ipeak<=khi)
         { sev = sev + 1.0;
           sxev = sxev + val;
           sxxev = sxxev + val*val;
           sxest = sxest + estimator;}       
      } /* end of nevnt loop */
        val=0.0;
        if(sev>0.0) 
          {val = sxev / sev;
           dotacc(0,charge,val);
           estimator = sxest / sev;
           if(val>10.0) dotacc(6,charge,estimator);
           printf(" dac=%d charge=%f fmean=%f\n",
           dac,charge,val);
           sig=0.0;
           sig2=sxxev/sev - val*val;
           if(sig2>0.0) sig=sqrt(sig2);
           dotacc(4,charge,sig);
          }
        if(val>10.0 && charge<800.0)
         { crg[icnt] = charge;
           fvl[icnt] = val;
           ss = ss +1.0;
           ssx = ssx + charge;
           ssy = ssy + val;
           ssxy = ssxy + charge*val;
           ssx2 = ssx2 + charge*charge;
           icnt++;
           sw3 = sw(3);
           if(sw3 == 1) 
            { printf(" charge=%f  val=%f\n",charge,val);
              printf(" ss=%f ssx=%f ssy=%f ssxy=%f ssx2=%f\n",
                ss,ssx,ssy,ssxy,ssx2);
            }              
         }
   }  /* end of dac loop */
   del = ss*ssx2 - ssx*ssx;
   a = (ssx2*ssy - ssx*ssxy) / del;
   b = (ss*ssxy - ssx*ssy) / del;
   printf(" logain fit results a=%f b=%f y=a+b*charge\n",a,b);
   fprintf(fp,"logain fit results a=%f b=%f y=a+b*charge\n",a,b);
        sprintf(xstring,"charge  y=%8.4f + %8.4f*charge",a,b);
        sprintf(ystring,"fit y-yfit");
        dotlbl(2,xstring,ystring);
   for(k=0;k<icnt;k++)
    {x = crg[k];
     y = a + b*x;
     dely = y - fvl[k];
     sw3 = sw(3);
     if(sw3 == 1) printf("k=%d x=%f  y=%f  fvl[k]=%f  dely=%f\n",
       k,x,y,fvl[k],dely);
     dotacc(2,x,dely);
     dotacc(8,x,dely/y);
    }
    }   /* end of sw7 test */
/* hi gain small C calibration */
   capacitor=5.0;
   send4_3in1(4,1);
   send4_3in1(5,0);
   kpeak=25;
   klo=24;
   khi=27;
   timf=18;
   tf = (unsigned short)timf;
   tim2_set(tf);
   icnt=0;
   ss=0.0;
   ssx=0.0;
   ssy=0.0;
   ssxy=0.0;
   ssx2=0.0;
   dstep = 1;
   sw8 = sw(8);
   if(sw8 == 1)
   {
   base = 0xf0ff0000;  
   printf("plug higain into ADC1\n");
/*   base = 0xf0ee0000;
   printf("plug higain into ADC2\n");  */
   wait = getchar();
   flex_setup();
   for (dac=0; dac<1024; dac=dac+dstep)
     { set_dac(dac);   /* set dac value */
      volt = 2.0*4.096 * (double)dac/1023.0;
      charge = volt * capacitor;
      sev = 0.0;
      sxev = 0.0;
      sxxev = 0.0;
      sxest = 0.0;
      for(nevnt=0;nevnt<nevmax;nevnt++)
       {
        flex_reset();
        rtime();
        tsleep(0x80000002);
        inject();
/*     go read the fermi module ------------------------------- */
        flex_read();
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
        hstacc(1,ped,1.0);
        sig2=axxsum/asum - ped*ped;
        if(sig2>0.0) dped=sqrt(sig2);
        hstacc(3,dped,1.0);
        val = (double)fdata[kpeak] - ped;
        el = (double)fdata[kpeak-1] - ped;
        er = (double)fdata[kpeak+1] - ped;
        ep = val;
        estimator = (el-er)/ep;
        ipeak=ipk;
        if(fdata[kpeak]>1020)
          {sev=0.0;
           break;
          }
        if(ipeak>=klo && ipeak<=khi)
         { sev = sev + 1.0;
           sxev = sxev + val;
           sxxev = sxxev + val*val;
           sxest = sxest + estimator;}       
      } /* end of nevnt loop */
        val=0.0;
        if(sev>0.0) 
          {val = sxev / sev;
           dotacc(1,charge,val);
           estimator = sxest / sev;
           if(val>10.0) dotacc(7,charge,estimator);
           printf(" dac=%d charge=%f fmean=%f\n",
           dac,charge,val);
           sig=0.0;
           sig2=sxxev/sev - val*val;
           if(sig2>0.0) sig=sqrt(sig2);
           dotacc(5,charge,sig);
          }
        if(val>10.0 && charge<80.0)
         { crg[icnt] = charge;
           fvl[icnt] = val;
           ss = ss +1.0;
           ssx = ssx + charge;
           ssy = ssy + val;
           ssxy = ssxy + charge*val;
           ssx2 = ssx2 + charge*charge;
           icnt++;
           sw3 = sw(3);
           if(sw3 == 1) 
            { printf(" charge=%f  val=%f\n",charge,val);
              printf(" ss=%f ssx=%f ssy=%f ssxy=%f ssx2=%f\n",
                ss,ssx,ssy,ssxy,ssx2);
            }              
         }
   }  /* end of dac loop */
   del = ss*ssx2 - ssx*ssx;
   a = (ssx2*ssy - ssx*ssxy) / del;
   b = (ss*ssxy - ssx*ssy) / del;
   printf(" higain fit results a=%f b=%f y=a+b*charge\n",a,b);
   fprintf(fp,"higain fit results a=%f b=%f y=a+b*charge\n",a,b);
        sprintf(xstring,"charge  y=%8.4f + %8.4f*charge",a,b);
        sprintf(ystring,"fit y-yfit");
        dotlbl(3,xstring,ystring);
   for(k=0;k<icnt;k++)
    {x = crg[k];
     y = a + b*x;
     dely = y - fvl[k];
     sw3 = sw(3);
     if(sw3 == 1) printf("k=%d x=%f  y=%f  fvl[k]=%f  dely=%f\n",
       k,x,y,fvl[k],dely);
     dotacc(3,x,dely);
     dotacc(9,x,dely/y);
    }
   } /* end of sw8 test */
   sidev(1);
   dotwrt(0);
   dotwrt(2);
   dotwrt(4);
   dotwrt(6);
   dotwrt(8);
   dotwrt(1);
   dotwrt(3);
   dotwrt(5);
   dotwrt(7);
   dotwrt(9);
   hstwrt(0);
   hstwrt(1);
   hstwrt(2);
   hstwrt(3);
   plot_reset(); 
   printf("printing postscript plots\n"); 
   sprintf(string,"dac64.ps");
   sidev(0);
   pfset(string);
   dotwrt(0);
   dotwrt(2);
   dotwrt(4);
   dotwrt(6);
   dotwrt(8);
   dotwrt(1);
   dotwrt(3);
   dotwrt(5);
   dotwrt(7);
   dotwrt(9);
   hstwrt(0);
   hstwrt(1);
   hstwrt(2);
   hstwrt(3);
   dotdone();
}  /* end of main */
sw(i)
  int i;
  {int *adr,k,l,shft;
   adr=(int *)0x80cf0000;
   k=*adr;
   shft=23+i;
   l=(k>>shft)&1; 
   return(l);
  }      
flex_setup()
  { reg0 = (unsigned short *)base;
    reg2 = (unsigned short *)(base | 0x2);
    reg4 = (unsigned short *)(base | 0x4);
    reg6 = (unsigned short *)(base | 0x5);
    reg8 = (unsigned short *)(base | 0x8);
    rega = (unsigned short *)(base | 0xa);
    *reg2 = 32;
  }
flex_reset()
  { *reg0 = 0;
  }
inject()
  { tp_high();
    tsleep(0x80000005); 
    stime();
    tsleep(0x80000005);
/*    rtime(); */
  }
flex_read()
  { int i,k,adr,adc_enable,read_done,adc_done,kk;
    sw4 = sw(4);
    pk = 0;
    ipk = 0; 
    for(i=0;i<32;i++)
      { kk = *reg8;
        tsleep(0x80000001);
        *rega=0; /* clock the shift register */
        adr = kk & 0xff;
        adc_done = (kk & 0x2000)>>13;
        adc_enable = (kk & 0x4000)>>14;
        read_done = (kk & 0x8000)>>15; 
        kk = *reg4;
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
set_dac(dac)
int dac;
 { int hi,lo;
   lo = dac & 0xff;
   send12_3in1(6,0,lo);   /* load low 8 bits */
   hi = dac>>8;
   send12_3in1(6,1,hi);   /* load high 2 bits */
 }


