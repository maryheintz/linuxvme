#include <stdio.h>
#include <math.h>
#include <types.h>
#include <time.h>
#include <ces/vmelib.h>
struct block
       { unsigned short reg0;        /* base+0 */
         unsigned short reg2;        /* base+2 */
         unsigned short reg4;        /* base+4 */
         unsigned short reg6;        /* base+6 */
         unsigned short reg8;        /* base+8 */
         unsigned short rega;        /* base+a */
      };
struct block *adc1;
struct block *adc2;
static struct pdparam_master param=
      { 1,   /* VME Iack (must be 1 for all normal pages) */
        0,   /* VME Read Prefetch Option */
        0,   /* VME Write Posting Option */
        1,   /* VME Swap Option */
        0,   /* page number return */
        0,   /* reserved */
        0,   /* reserved */
        0 };
int fdata[40],pk,ipk;
main()
{ 
   int zone,sector,tube,mode,i,phase;
   double asum,axsum,axxsum,ped,dped,sig2,aval;
    char string[80];
   ttcvi_map();
   flex_setup();
/*   printf(" enter zone sector tube\n");
   fscanf(stdin,"%d %d %d",&zone,&sector,&tube);  */
   zone = 0;
   sector = 0;
   tube = 38;
   set_zone(zone);
   set_sector(sector);
   set_tube(tube);
   set_swln2(1);
   set_swln1(0);
   usleep(10);
   large_cap_enable();
   usleep(10);
   small_cap_disable();
   usleep(10);
   set_dac(0x3ff);
   usleep(10);
   tp_low();
   printf(" enter clock phase 0-239\n");
   fscanf(stdin,"%d",&phase);
   SetTimeDes2(0,phase);  
loop:
   flex2_reset();
   inject();
/*     display ? */
         if(sw(2) == 1) 
          { flex2_read();
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
            dped = 0.0;
            sig2=axxsum/asum - ped*ped;
            if(sig2>0.0) dped=sqrt(sig2);
            sprintf(string,
              "lo gain ipk=%d  pk=%d  ped=%f +/- %f",
              ipk,pk,ped,dped);
            evdisp(32,string);}
    usleep(1000);
   if(sw(8) != 1) goto loop;
}
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
/*	     rlatch();  */
	     entry = 1;
	    }
	 /* send pulse out switch box through fanout and
	          into b_go1, b_go2, and b_go3 inputs
            send inhibit1 output into the switch box latch
	    send inhibit3 output into l1a0 or fermi start input */  

           tp_high();
loop:
	   stvme();  /* send pulses from the switch box */
           usleep(500);
	   swtst = latch();  /* wait for the latch bit to set */
      /*     if(swtst == 0) goto loop;  */
	   printf("event found\n");
           restim();  /* reset the timer for next trigger */
      /*     rlatch();   */ /* reset the latch bit */
           usleep(1);
      }
flex_setup()
  { unsigned long am,vmead,offset,size,len;
    am=0x39;
    vmead=0xff0000;
    offset=0;
    size=0;
    len=400;
    adc1 = (struct block *)( find_controller(vmead,len,am,offset,size,&param) );
    if( adc1 == (struct block *)(-1) ) 
      { printf("unable to map vme address\n");
        exit(0);}
    vmead=0xee0000;
    adc2 = (struct block *)( find_controller(vmead,len,am,offset,size,&param) );
    if( adc2 == (struct block *)(-1) ) 
      { printf("unable to map vme address\n");
        exit(0);}
    adc1->reg2 = 32;
    adc2->reg2 = 32;
  }
flex1_reset()
  { adc1->reg0 = 0;
  }
flex1_read()
  { int i,k,adr,adc_enable,read_done,adc_done;
    unsigned short kk;
    pk = 0;
    ipk = 0; 
    for(i=0;i<32;i++)
      { kk = adc1->reg8;
/*	usec = 1;
        usleep(usec);   */
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
        if(sw(4) == 1) printf(" i=%d  adr = %x  data=%x  en=%d rd=%d  ad=%d\n",
         i,adr,(int)kk,adc_enable,read_done,adc_done);
      }
}
flex2_reset()
  { adc2->reg0 = 0;
  }
flex2_read()
  { int i,k,adr,adc_enable,read_done,adc_done;
    unsigned short kk;
    pk = 0;
    ipk = 0; 
    for(i=0;i<32;i++)
      { kk = adc2->reg8;
/* 	usec = 1;
        usleep(usec); */
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
      


