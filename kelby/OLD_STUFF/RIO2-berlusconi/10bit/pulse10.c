#include <stdio.h>
#include <math.h>
#include <time.h>
#include <types.h>
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
int sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8;
int fdata[3][40],pk[3],ipk[3];
time_t usec;
int ktime=2400;
main()
  { FILE *fp;
    int timc,timf,i,l;
    int zone,sector,card;
    double asum,axsum,axxsum,ped,dped,sig2,aval;
    int time1,time2,time3,time4;
    int dac,icap;
    double charge,dacval,capacitor;
    char xstring[80],ystring[80],string[80],err[80];
    unsigned short cadr,tc,tf;
    unsigned short sval,dval;
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
    i = (zone<<12) | (sector<<6) | card;
    cadr = (unsigned short)i;
    card_sel(cadr);
    send12_3in1(2,0,0);   /* s1=s2=s3=s4=0 */
    send4_3in1(1,0);   /* itr=0 */
    send4_3in1(0,0);   /* ire=0 */
    send4_3in1(3,0);   /* mse=0 */
    time1 = 0;
    time2 = 0;
    time3 = 0;
    time4 = 0;
    set_drawer_timer(zone,sector,1,time1);
    set_drawer_timer(zone,sector,2,time2);
    set_drawer_timer(zone,sector,3,time3);
    set_drawer_timer(zone,sector,4,time4);
    timc=10;
    timf=100;
    tc = (unsigned short)timc;
    tim1_set(tc);   /* coarse */
    tf = (unsigned short)timf;
    tim2_set(tf);   /* fine */
/* ---------------- do logain 220pF -------------------------------- */
    printf("which capacitor 0=small 1=large\n");
    fscanf(stdin,"%d",&icap);
    if(icap == 0)
     { send4_3in1(4,1);   /* enable small capacitor */
       send4_3in1(5,0);   /* disable large capacitor */
       capacitor = 5.2;
     }
    if(icap == 1)
     { send4_3in1(4,0);   /* disable small capacitor */
       send4_3in1(5,1);   /* enable large capacitor */
       capacitor = 100.0;
     }
    charge = 800.0;
    printf("input charge\n");
    fscanf(stdin,"%lf",&charge);
    dacval = (charge * 1023.0 ) / (2.0*4.096 * capacitor);
    if(dacval > 1023.0) dacval = 1023.0;
    dac = (int) dacval;
    charge = ( 2.0 * 4.096 * capacitor *(double)dac ) / 1023.0;
    printf(" charge=%f  dacval=%f  dac=%d  cap=%f\n",
           charge,dacval,dac,capacitor);
loop:
        set_dac(dac);
        fermi_reset();
        rtime();
	usec = 2;
        usleep(usec);
        inject();
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
              ipk,pk,ped,dped);
            evdisp(32,string,2);}
/*     display ? */
         sw2 = sw(2);
         if(sw2 == 1) 
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
              ipk,pk,ped,dped);
            evdisp(32,string,1);}
       sw8 = sw(8);
        if(sw8 == 0) goto loop;
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
kjasleep(i)
int i;
{int k;
 double d,f=2.0;
 for (k=0;k<i;k++)
   { d = sqrt(f);}
}
