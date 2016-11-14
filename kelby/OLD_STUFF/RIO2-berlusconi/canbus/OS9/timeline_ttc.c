
#include <stdio.h>
int sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8;
unsigned int base_hi = 0xf0ff0000;
unsigned int base_lo = 0xf0ee0000;
unsigned int base;
unsigned short *reg0,*reg2,*reg4,*reg6,*reg8,*rega;
int fdata[40],pk,ipk;
double crg[6500],fvl[6500];
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { FILE *fp;
    int tim,i,l,k;
    int zone,sector,tube;
    int dac,icap;
    double charge,dacval,capacitor;
    char xstring[80],ystring[80],string[80],err[80];
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    unsigned short cadr,tc,tf;
    unsigned short sval,dval;
    int time1,time2,time3,time4,board;
    double position,position2,b1,b2,b3,f1,f2,f3;
    double sum,ysum,xsum,fdg=2.1;
    ttcvi_map();
    zone = 0;
    sector = 0;
    tube = 45;
    set_zone(zone);
    set_sector(sector);
    set_tube(tube);
    set_swln2(1);
    set_swln1(0);

    multi_low();
    if(tube<13) board = 4;
    if(tube>12 && tube<25) board = 3;
    if(tube>24 && tube<37) board = 2;
    if(tube>35) board = 1;
    printf(" board=%d\n",board);
    set_intg_gain(0);  /* s1=s2=s3=s4=0 */
    itr_low();         /* itr=0 */
    intg_rd_low();     /* ire=0 */
    mse_low();         /* mse=0 */
    SetTimeDes2(0,172);
    capacitor = 100.0;
    large_cap_enable();
    tsleep(0x80000005);
    small_cap_disable();
    tsleep(0x80000005);
    charge = 800.0;
    dacval = (charge * 1023.0 ) / (2.0 * 4.096* capacitor);
    if(dacval > 1023.0) dacval = 1023.0;
    dac = (int) dacval;
    set_dac(dac);
    charge = ( 2.0 * 4.096 * capacitor *(double)dac ) / 1023.0;
    printf(" charge=%f  dacval=%f  dac=%x(hex)  cap=%f\n",
           charge,dacval,dac,capacitor);

    dotset(0,250.0,0.0,30.0,20.0,0,0);
    sprintf(xstring,"time setting");
    sprintf(ystring,"position");
    dotlbl(0,xstring,ystring);

    dotset(1,250.0,0.0,30.0,20.0,0,0);
    sprintf(xstring,"time setting");
    sprintf(ystring,"position2");
    dotlbl(1,xstring,ystring);

    set_mb_time(1,0);
    set_mb_time(2,0);
    set_mb_time(3,0);
    set_mb_time(4,0);

    base=base_lo;
    flex_setup();
    for(tim=0;tim<256;tim++)
      { set_mb_time(board,tim);
        tsleep(0x80000005);
        sum=0.0;
        ysum=0.0;
        xsum=0.0;
        for(k=0;k<10;k++)
         { inject();
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
                "ipk=%d  pk=%d  ped=%f +/- %f",
                ipk,pk,ped,dped);
              evdisp(32,string,2);
             }

          f1 = (double)fdata[ipk-1];
          b1 = (double)(ipk-1);
          f2 = (double)fdata[ipk];
          b2 = (double)(ipk);
          f3 = (double)fdata[ipk+1];
          b3 = (double)(ipk+1);
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
       printf("time=%d   pos=%f  pos2=%f\n",tim,position,position2);
              }
   dotwrt(0);
   dotwrt(1);
   }  /* end of main */
     inject()
       { static int entry=0;
         int c1,c2,f1,f2,swtst,i,i1,i2,i0time,i3time;
         if(entry == 0)
	   {/* setup inhibit1 to set tp high */
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
             i3time=2025;
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
     	     entry = 1;
	    }
           flex_reset();
           tsleep(0x80000005);
loop:      stvme();  /* send pulses from the switch box to clear last event */
           tsleep(0x80000002);
	   swtst = latch();  /* wait for the latch bit to set */
           if(swtst == 0) 
             { tsleep(0x80000002);
               goto loop;
             }
           restim();  /* reset the timer for next trigger */
           rlatch();    /* reset the latch bit */
  }
flex_setup()
  { reg0 = (unsigned short *)base;
    reg2 = (unsigned short *)(base | 0x2);
    reg4 = (unsigned short *)(base | 0x4);
    reg6 = (unsigned short *)(base | 0x6);
    reg8 = (unsigned short *)(base | 0x8);
    rega = (unsigned short *)(base | 0xa);
    *reg2 = 32;
  }
flex_reset()
  { *reg0 = 0;
  }
flex_read()
  { int i,k,adr,adc_enable,read_done,adc_done,kk;
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
        if(sw(4) == 1) printf(" i=%d  adr = %x  data=%x  en=%d rd=%d  ad=%d\n",
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

