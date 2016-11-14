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
time_t usec = 1;
int ktime;
int fdata[40],pk,ipk;
char xstring[80],ystring[80],string[80],err[80];
main()
  { FILE *fp;
    int timc,timf,i,timer_set,itest,icap;
    int zone,sector,card;
    int dac,ii;
    int ifl,icl,ifh,ich;
    double fl,cl,fh,ch,charge,dacval;
    double asum,axsum,axxsum,ped,dped,sig2,aval,capacitor;
    unsigned short cadr,tc,tf;

    map_3in1();
    ktime = 2400;
    zone = 1;
    sector = 1;
    card = 48;
    i = (zone<<12) | (sector<<6) | card;
    cadr = (unsigned short)i;
    card_sel(cadr);
    send12_3in1(2,0,0);   /* s1=s2=s3=s4=0 */
    send4_3in1(1,0);   /* itr=0 */
    send4_3in1(0,0);   /* ire=0 */
    send4_3in1(3,0);   /* mse=0 */
    timc=10;
/*    printf("input coarse time\n");
    fscanf(stdin,"%d",&timc);
    printf("timc=%d\n",timc);  */
    timf=100;
    tc = (unsigned short)timc;
    tim1_set(tc);   /* coarse */
    tf = (unsigned short)timf;
    tim2_set(tf);   /* fine */
/* ---------------- do logain 220pF -------------------------------- */
/*    printf("which capacitor 0=small 1=large\n");
    fscanf(stdin,"%d",&icap); */
    icap = 1;
    if(icap == 0)
     { send4_3in1(4,1);   /* enable small capacitor */
       send4_3in1(5,0);   /* disable large capacitor */
       capacitor = 5.0;
     }
    if(icap == 1)
     { send4_3in1(4,0);   /* disable small capacitor */
       send4_3in1(5,1);   /* enable large capacitor */
       capacitor = 100.0;
     }
    charge = 800.0;
/*    printf("input charge\n");
    fscanf(stdin,"%lf",&charge);  */
    dacval = (charge * 1023.0 ) / (8.0 * capacitor);
    if(dacval > 1023.0) dacval = 1023.0;
    dac = (int) dacval;
    charge = ( 8.0 * capacitor *(double)dac ) / 1023.0;
    printf(" charge=%f  dacval=%f  dac=%d  cap=%f\n",
           charge,dacval,dac,capacitor);
        flex_setup();
loop:
       for(ii=0;ii<256;ii++)
        {    timc=ii;
             tc = (unsigned short)timc;
             tim1_set(tc);   /* coarse */

        set_dac(dac);
        rtime();
        flex_reset();
        inject();
        flex_read();
	if(pk != 2) printf("pk=%d  ii=%d\n",pk,ii);
        if(sw(1) == 1)
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
             if(sig2>0.0) dped = sqrt(sig2);
             sprintf(string,
                "logain ipk=%d  pk=%d  ped=%f +/- %f  i=%d",
                  ipk,pk,ped,dped,ii);
             evdisp(32,string);
         }
	 }
    goto loop;
}  /* end of main */
inject()
  { tp_high();
    kjasleep(ktime);  
    stime();
    usec = 5;
    usleep(usec); 
  }
flex_setup()
  { unsigned long am,vmead,offset,size,len;
    am=0x39;
    vmead=0xff0000;
    offset=0;
    size=0;
    len=400;
    adc = (struct block *)( find_controller(vmead,len,am,offset,size,&param) );
    if( adc == (struct block *)(-1) ) 
      { printf("unable to map vme address\n");
        exit(0);}
    adc->reg2 = 32;
  }
flex_reset()
  { adc->reg0 = 0;
  }
flex_read()
  { int i,k,adr,adc_enable,read_done,adc_done;
    unsigned short kk;
    pk = 0;
    ipk = 0; 
    for(i=0;i<32;i++)
      { kk = adc->reg8;
       adc->rega = 0; /* clock the shift register */
        adr = kk & 0xff;
        adc_done = (kk & 0x2000)>>13;
        adc_enable = (kk & 0x4000)>>14;
        read_done = (kk & 0x8000)>>15; 
        kk = adc->reg4;
        k = kk & 0x3ff;
        fdata[i] = k;
        if(k>pk)
          { ipk = i;
            pk = k;}
        if(sw(4) == 1) printf(" i=%d  adr = %x  data=%x  data=%d\n",
         i,adr,(int)k,(int)k);
      }
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
kjasleep(i)
int i;
{int k;
 double d,f=2.0;
 for (k=0;k<i;k++)
   { d = sqrt(f);}
}



