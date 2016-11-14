#include <stdio.h>
#include <math.h>
int fdata[40],pk,ipk;
int swtst,iloop,iloopmx;
unsigned short *reg0,*reg2,*reg4,*reg6,*reg8,*rega;
unsigned long *swtch, *adr;
unsigned int base;
main()
{  int ipass=0,restart;
   int zone,sector,tube,mode,i,phase,dac,k,ena[5];
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
   double asum,axsum,axxsum,ped,dped,sig2,aval,signal;
   char string[80];
   double fdac,charge,capacitor;
   unsigned short shrt;
   ttcvi_map();
   base = 0xf0ff0000;
   flex_setup();
/*   printf(" enter zone sector tube\n");
   fscanf(stdin,"%d %d %d",&zone,&sector,&tube);  */
   zone = 0;
   sector = 0;
   set_zone(zone);
   tsleep(0x80000005);
   set_sector(sector);
   tsleep(0x80000005);
/*   set_swln2(1);
   set_swln1(0);  */
   set_mb_time(1,0);
   tsleep(0x80000005);
   set_mb_time(2,0);
   tsleep(0x80000005);
   set_mb_time(3,0);
   tsleep(0x80000005);
   set_mb_time(4,0);
   tsleep(0x80000005);
   multi_low();
   tsleep(0x80000005);
   capacitor = 100.0;
    printf("enable enable pattern eg 0 1 0 0 0 for 44 only\n");
    fscanf(stdin,"%d %d %d %d %d",&ena[0],&ena[1],&ena[2],&ena[3],&ena[4]);
    printf("ena = %d %d %d %d %d\n",ena[0],ena[1],ena[2],ena[3],ena[4]);
   for(k=43;k<48;k++)
    { tube = k;
      set_tube(tube);
      tsleep(0x80000005);
      large_cap_enable();
      tsleep(0x80000005);
      small_cap_disable();
      tsleep(0x80000005);
      trig_disable();
      tsleep(0x80000005);
      mse_low();
      tsleep(0x80000005);
      if(ena[k-43] == 1) 
       { trig_enable();
         mse_high();
       }
       can_bkld_high();
       can_bkld_low();
       can_set(0x9c,0);  /* fetch from 3in1 and load can output reg */
       tsleep(0x80000005);
       can_shft_rd(&shrt);
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
       printf("tube=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,tube,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
      tsleep(0x80000010);
    }     
   for(k=43;k<48;k++)
    { tube = k;
      set_tube(tube);
      tsleep(0x80000005);
       can_bkld_high();
       can_bkld_low();
       can_set(0x9c,0);  /* fetch from 3in1 and load can output reg */
       tsleep(0x80000005);
       can_shft_rd(&shrt);
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
       printf("tube=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,tube,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
      tsleep(0x80000010);
    }     
    tube = 0;
    set_tube(tube);
    tsleep(0x80000005);
   tp_low();
/*   phase = 116;  */
   printf(" enter clock phase 0-239\n");
   fscanf(stdin,"%d",&phase);
   printf(" enter iloopmx\n");
   fscanf(stdin,"%d",&iloopmx);
   SetTimeDes2(0,phase);  
   tsleep(0x80000005);

loop:
   multi_high();
   for(charge=10.0;charge<801.0;charge=charge+10.0)
    {
   for(iloop=0;iloop<iloopmx;iloop++)
    {
    fdac = (1023.0*charge) / (2.0 * 4.096 * capacitor);
    dac = (int)fdac;
    set_dac(dac);
    tsleep(0x80000005);
    charge = 2.0 * 4.096 * capacitor * (double)dac / 1023.0;
    printf("logain charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
   inject();
   if(sw(2) == 1) goto loop;
   flex_read();
   ipass++;
/*   if(pk<50.0)
     { printf(" ipass=%d  pk=%d  ipk=%d  swtst=%d\n",
        ipass,pk,ipk,swtst);
       fscanf(stdin,"%d",&restart);
     }  */
   asum = 0.0;
   axsum = 0.0;
   axxsum = 0.0;
   for(i=5;i<15;i++)
    { aval = (double)fdata[i];
      asum = asum+1.0;
      axsum = axsum + aval;
      axxsum = axxsum + aval*aval;
    }
   ped = axsum / asum;
   dped = 0.0;
   signal = (double)fdata[ipk] - ped;
   sig2=axxsum/asum - ped*ped;
   if(sig2>0.0) dped=sqrt(sig2);
/*     display ? */
         if(sw(1) == 1) 
            { sprintf(string,
              "lo gain ipk=%d  pk=%d  ped=%f +/- %f",
              ipk,pk,ped,dped);
            evdisp(32,string);}
   tsleep(0x80000010);
  }
  }
   if(sw(8) != 1) goto loop;
}
     inject()
       { static int entry=0;
         int c1,c2,f1,f2,swtst,i,i1,i2,i0time,i3time;
         if(entry == 0)
	   {/* setup inhibit1 to set tp high */
	     SetBMode(1,0xc);
             tsleep(0x80000005);
	     SetInhibitDelay(1,10);
             tsleep(0x80000005);
	     SetInhibitDuration(1,10);
             tsleep(0x80000005);
	     BPutLong(1,0,1,0x80,0x00);  /* load set tp_high into fifo1 */
             tsleep(0x80000005);
	     /* set inhibit2 to drop tp low (timed broadcast mode */
	     SetBMode(2,0xc);
             tsleep(0x80000005);
	     SetInhibitDelay(2,2000);
             tsleep(0x80000005);
	     SetInhibitDuration(2,10);
             tsleep(0x80000005);
	    /* BPutLong(2,0,1,0x80,0x01);  */ /* load set tp_low into fifo2 */
	     BPutShort(2,0xc0);   /* set tp low bcast command  */
             tsleep(0x80000005);
	     /* set inhibit3 to set latch                     */
	     /* use latch-bar and inhibit3 to start fermi     */
             /* will use inhibit 3 to set l1a with digitizers */
             i3time=2025;
/*	     printf("input i3time\n");
             fscanf(stdin,"%d",&i3time);     */
	     SetInhibitDelay(3,i3time);
             tsleep(0x80000005);
	     SetInhibitDuration(3,10);
             tsleep(0x80000005);
	     SetInhibitDelay(0,1);
             tsleep(0x80000005);
	     SetInhibitDuration(3,10);
             tsleep(0x80000005);
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
      

