
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
int sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8;
int fdata[40],pk,ipk;
double crg[6500],fvl[6500];
time_t usec = 1;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { FILE *fp;
    int timc,timf,i,l,k;
    int zone,sector,card;
    int dac,icap;
    double charge,dacval,capacitor;
    char xstring[80],ystring[80],string[80],err[80];
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    unsigned short cadr,tc,tf;
    unsigned short sval,dval;
    int time1,time2,time3,time4,board;
    double position,position2,b1,b2,b3,f1,f2,f3;
    double sum,ysum,xsum,fdg;
    l = sw(0); /* initialize switch reading */
    map_3in1();   
    flex_setup();
    fdg=2.1;
    zone = 1;
    sector = 1;
    multi_low();
    printf("argc=%d\n",argc);
    if(argc != 2)
     { for(i=0;i<49;i++)
       { card = i;
         l = (zone<<12) | (sector<<6) | card;
         cadr = (unsigned short)l;
         card_sel(cadr);
         sval = (unsigned short)0x555;
         wrt_shift(13,sval);
         read_shift(&dval);
         printf("card=%d  sval=%x  dval=%x\n",card,(int)sval,(int)dval);
         if(sval != (dval&0x1fff) ) continue;
         sval = (unsigned short)0xaaa;
         wrt_shift(13,sval);
         read_shift(&dval);
         if(sval != (dval&0x1fff) ) continue;
         card = i;
         goto found;
       }
       printf("no card found\n");
       return(0);
found:
    printf("card %d found\n",card);
    }
    if(argc == 2)
     { sscanf(argv[1],"%d",&card);
       printf("card %d requested\n",card);}

    if(card<13) board = 4;
    if(card>12 && card<25) board = 3;
    if(card>24 && card<37) board = 2;
    if(card>35) board = 1;
    printf(" board=%d\n",board);
    i = (zone<<12) | (sector<<6) | card;
    cadr = (unsigned short)i;
    card_sel(cadr);
    send13_3in1(2,0);   /* s1=s2=s3=s4=0 */
    send4_3in1(1,0);   /* itr=0 */
    send4_3in1(0,0);   /* ire=0 */
    send4_3in1(3,0);   /* mse=0 */
    timc=2;
    timf=100;
    tc = (unsigned short)timc;
    tim1_set(tc);   /* coarse */
    tf = (unsigned short)timf;
    tim2_set(tf);   /* fine */
    send4_3in1(4,0);   /* disable small capacitor */
    send4_3in1(5,1);   /* enable large capacitor */
    capacitor = 100.0;
    charge = 800.0;
    dacval = (charge * 1023.0 ) / (2.0 * 4.096* capacitor);
    if(dacval > 1023.0) dacval = 1023.0;
    dac = (int) dacval;
    send13_3in1(6,dac);
    charge = ( 2.0 * 4.096 * capacitor *(double)dac ) / 1023.0;
    printf(" charge=%f  dacval=%f  dac=%x(hex)  cap=%f\n",
           charge,dacval,dac,capacitor);

    dotset(0,250.0,0.0,20.0,10.0,0,0);
    sprintf(xstring,"time setting");
    sprintf(ystring,"position");
    dotlbl(0,xstring,ystring);

    dotset(1,250.0,0.0,20.0,10.0,0,0);
    sprintf(xstring,"time setting");
    sprintf(ystring,"position2");
    dotlbl(1,xstring,ystring);

    time1 = 0;
    time2 = 0;
    time3 = 0;
    time4 = 0;
    set_drawer_timer(zone,sector,1,time1);
    set_drawer_timer(zone,sector,2,time2);
    set_drawer_timer(zone,sector,3,time3);
    set_drawer_timer(zone,sector,4,time4);    

    for(i=0;i<256;i++)
      { set_drawer_timer(zone,sector,board,i);
        usleep(5);
        sum=0.0;
        ysum=0.0;
        xsum=0.0;
        for(k=0;k<10;k++)
         { flex_reset1();
	   flex_reset2();
           rtime();
           inject();
           flex_read2();
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
       dotacc(0,(double)i,position);
       dotacc(1,(double)i,position2);
      }
   dotwrt(0);
   dotwrt(1);
}  /* end of main */
inject()
  { tp_high();
    usleep(20);  
    stime();
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
