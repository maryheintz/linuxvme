#include <stdio.h>
#include <math.h>
#include <time.h>
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
int fdata[3][256],ipk[3],pk[3];
unsigned short *reg;
unsigned short *reg0,*reg1,*reg2;
unsigned int basef=0xb30000,base_fermi,base_last;
unsigned char gpib_device=1;
int adder[48]={ 4, 4, 4, 4, 4, 7,  /*  1 -  6 */
                7, 7,10,10,10,10,  /*  7 - 12 */
	       15,15,15,15,15,15,  /* 13 - 18 */
               21,21,21,21,21,21,  /* 19 - 24 */
	       28,28,28,28,28,28,  /* 25 - 30 */
	       34,34,34,34,34,34,  /* 31 - 36 */
               42,42,42,42,42,42,  /* 37 - 42 */
	       45,45,45,45,45,45}; /* 43 - 48 */
int finput[48]={0,0,0,0,0,1,
                1,1,2,2,2,2,
		0,0,0,0,0,0,
                1,1,1,1,1,1,
		2,2,2,2,2,2,
		0,0,0,0,0,0,
                1,1,1,1,1,1, 
		2,2,2,2,2,2};
int boff[48]={     0,     0,     0,     0,     0,     0,
                   0,     0,     0,     0,     0,     0,
              0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,
	      0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,
	      0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,
	      0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,
	      0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,
	      0x2000,0x2000,0x2000,0x2000,0x2000,0x2000};
double crg[6500],fvl[6500];
int ttcadr,id;
/* ------------values for database storage ------------ */
    double aa[48],bb[48],nlin[48],noise[48];
    int gateok[48];
/* ---------------------------------------------------- */
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { FILE *fp;
    int board,kk;
    char serial[7] = {"PS1001\0"};
    long stat;
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    double meanhp,meanhc,meanlp,meanlc;
    double sum,xlsump,xxlsump,xlsumc,xxlsumc;
    double siglp,siglc,pmaxl,cmaxl,tmaxl;
    int nevnt,nevmax,nstrtev,l,nn,i,k,wait;
    int card,okcd[48],nall,ngood,nbad;
    unsigned int kl,lk,start,trouble;
    unsigned short shrt;
    int dac,time1,time2,time3,time4;
    double fdac,charge,val,ratio;
    int bigain,kpeak,klo,khi,icnt,dstep;
    double s,sx,sy,sxy,sev,sxev,sxxev,el,er,ep;
    double sig,sx2,volt,capacitor,a,b,del,x,y,dely;
    double pedhi,dpedhi,pedlo,dpedlo;
    char xstring[80],ystring[80],string[80],err[80];
    unsigned short cadr,tc,tf;
    int kpeakl,timfl,timf;
    unsigned short sval,dval;
    base_fermi=basef;
    base_last=basef;
    fermi_setup();
   ttcadr = 0x2001;
   board = 1;
   id = (board<<6) | 1;
   ttcvi_map(ttcadr);
   reset_ctrl_can();
   printf("calling can_init\n");
   can_reset(ttcadr);  
   can_init(board,serial);  
   set_mb_time(ttcadr,1,0);
   set_mb_time(ttcadr,2,0);
   set_mb_time(ttcadr,3,0);
   set_mb_time(ttcadr,4,0);
   start=0;
   if(argc == 2)
     { sscanf(argv[1],"%d",&start);
       printf("start at %d requested\n",start);
     }
/* ------------------ disable all cards ---------------*/
   for (kl=1;kl<49;kl++)
     {set_tube(ttcadr,kl);
      trig_disable(ttcadr);
      small_cap_disable(ttcadr);
      large_cap_disable(ttcadr);
      set_dac(ttcadr,0);
     }
/* ------------------- start card loop --------------- */
   nall=0;
   ngood=0;
   nbad=0;
   for(kl=start;kl<49;kl++) 
       { if(cardchk(kl) == 1) 
         { card = kl;
	   if(sw(5) == 1) printf("card %d  connect to adder=%d  fermi=%d input=%d\n",
	      card,adder[card],(boff[card]<<12),finput[card]);
	    
           base_fermi = basef + boff[kl-1];
           if(base_fermi != base_last)
            { fermi_setup();
              base_last = base_fermi;
            }
        kk = finput[card-1];
        set_tube(ttcadr,card);
        multi_low(ttcadr);
        rxw_low(ttcadr);
        intg_rd_low(ttcadr);
        itr_low(ttcadr);
        set_intg_gain(ttcadr,0);  /* s1=s2=s3=s4=0 */
        mse_low(ttcadr);         /* mse=0 */
        trig_enable(ttcadr);
        nevmax=25;
        pmaxl=0.0;
        cmaxl=0.0;
        tmaxl=0.0;

        dotzro();
        dotset(2,250.0,0.0,3500.0,2500.0,0,0);
        sprintf(xstring,"time setting");
        sprintf(ystring,"low gain FERMI peak channel amplitude");
        dotlbl(2,xstring,ystring);

        dotset(3,250.0,0.0,30.0,20.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"trig out peak channel number");
        dotlbl(3,xstring,ystring);

        dotset(5,100.0,0.0,2000.0,0.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"trig out fermi peak");
        dotlbl(5,xstring,ystring);

        dotset(7,100.0,0.0,10.0,-10.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"trig out fit y-yfit");
        dotlbl(7,xstring,ystring);

        dotset(9,100.0,0.0,10.0,0.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"trig out sigma");
        dotlbl(9,xstring,ystring);

        hstzro();
        hstset(0,150.0,0.0,50,1);
        hstlbl(0,"trig out pedestal");

        hstset(2,10.0,0.0,50,1);
        hstlbl(2,"trig out sigma(pedestal)");

/* --------------------- determine timing -------------------*/
    capacitor = 100.0;
    small_cap_disable(ttcadr);
    large_cap_enable(ttcadr);
    charge=150.0;
    fdac = (1023.0*charge) / (2.0 * 4.096 * capacitor);
    dac = (int)fdac;
    charge = 2.0 * 4.096 * capacitor * (double)dac / 1023.0;
    if(sw(5) == 1) printf("trig charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    set_dac(ttcadr,dac);
    usleep(2000);
   i=0;
   trouble=0;
   for (timf=0; timf<240; timf=timf+4)  /* start l loop */
   { SetTimeDes2(ttcadr,timf);  
     usleep(2000);
loop2:
     sum=0.0;
     xlsump=0.0;
     xxlsump=0.0;
     xlsumc=0.0;
     xxlsumc=0.0;
     for(nevnt=0;nevnt<nevmax;nevnt++)
      {
re2:
        inject();
/*     go read the fermi module ------------------------------- */
        fermi_read();
/*     display ? */
         if(sw(1) == 1) 
          { asum = 0.0;
            axsum = 0.0;
            axxsum = 0.0;
            for(i=10;i<20;i++)
             { aval = (double)fdata[kk][i];
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
              ipk[kk],pk[kk],ped,dped);
            evdisp(50,string,kk);}
/* now gets sums for mean calculation */  
	if(fdata[kk][ipk[kk]] < 100)
	 {trouble++;
	  if(trouble== 1) printf("card %d error ipk=%d fdata[ipk]=%d\n",
	       card,ipk[kk],fdata[kk][ipk[kk]]);
	  if(trouble>20) goto next;
          goto re2;
	 }
       sum=sum+1.0;
       yc=(double)fdata[kk][ipk[kk]];
       xlsumc=xlsumc+yc;
       xxlsumc=xxlsumc+yc*yc;
       yc=(double)ipk[kk];
       xlsump=xlsump+yc;
       xxlsump=xxlsump+yc*yc;
     } /* end of nevnt loop */
   if(sw(8) == 1) goto loop2;
/*  now get mean  */
   meanlp=xlsump/sum;
   siglp=0.0;
   sig2=xxlsump/sum-meanlp*meanlp;
   if(sig2>0.0) siglp=sqrt(sig2);
   meanlc=xlsumc/sum;
   siglc=0.0;
   sig2=xxlsumc/sum-meanlc*meanlc;
   if(sig2>0.0) siglc=sqrt(sig2);
   if(sw(3) == 1)
     { printf("\n timer=%d sum=%f\n",timf,sum); 
       printf("       meanlp=%f siglp=%f\n",meanlp,siglp);
       printf("       meanlc=%f siglc=%f\n",meanlc,siglc);
     }

   if(meanlc>pmaxl)
    { pmaxl=meanlc;
      cmaxl=meanlp;
      tmaxl=timf; }
    if(sw(5) == 1) printf(" timf=%d  meanlc=%f  meanlp=%f\n",timf,meanlc,meanlp);
   dotacc(2,(double)timf,meanlc);
   dotacc(3,(double)timf,meanlp);
   }  /* end of dac loop */
/*   show the plots */
     if(sw(5) == 1) printf("pmaxl=%f cmaxl=%f tmaxl=%f\n",pmaxl,cmaxl,tmaxl);
     
/* ---------------------- linearity check --------------------------*/
   capacitor = 100.0;
   small_cap_disable();
   large_cap_enable();
   kpeak=(int)(cmaxl+0.5);
   klo=kpeak-2;
   khi=kpeak+2;
   timf=(int)(tmaxl+0.5);
   if(sw(5) == 1) printf("starting trigout  kpeak=%d timf=%d\n",kpeak,timf);
   kpeakl = kpeak;
   timfl = timf;
   SetTimeDes2(ttcadr,timf);
   icnt=0;
   s=0.0;
   sx=0.0;
   sy=0.0;
   sxy=0.0;
   sx2=0.0;
   dstep = 4;
   for (dac=4; dac<188; dac=dac+dstep)
     {set_dac(ttcadr,dac);
      usleep(2000);
      volt = 2.0 * 4.096 * (double)dac/1023.0;
      charge = volt * capacitor;
      sev = 0.0;
      sxev = 0.0;
      sxxev = 0.0;
      for(nevnt=0;nevnt<nevmax;nevnt++)
       {
rstrtl:
        inject();
/*     go read the fermi module ------------------------------- */
        fermi_read();
/*     display ? */
         if(sw(1) == 1) 
          { asum = 0.0;
            axsum = 0.0;
            axxsum = 0.0;
            for(i=10;i<20;i++)
             { aval = (double)fdata[kk][i];
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
              ipk[kk],pk[kk],ped,dped);
            evdisp(50,string,0);}
        asum = 0.0;
        axsum = 0.0;
        axxsum = 0.0;
        for(i=10;i<20;i++)
         { aval = (double)fdata[kk][i];
           asum = asum+1.0;
           axsum = axsum + aval;
           axxsum = axxsum + aval*aval;
         }
        ped = axsum / asum;
        hstacc(0,ped,1.0);
        sig2=axxsum/asum - ped*ped;
        if(sig2>0.0) dped=sqrt(sig2);
	noise[card-1] = dped;
        hstacc(2,dped,1.0);
        if(fdata[kk][ipk[kk]]>1600) goto lofit;
        val = (double)fdata[kk][kpeak] - ped;
	if(val<0.0) break;
        if(ipk[kk]>=klo && ipk[kk]<=khi && fdata[kk][kpeak]<1600)
         { sev = sev + 1.0;
           sxev = sxev + val;
           sxxev = sxxev + val * val;
         }
        if(sw(8) == 1) goto rstrtl;
      } /* end of nevnt loop */
        val=0.0;
        if(sev>0.0) 
          {val = sxev / sev;
           if(sw(5) == 1) printf("  trig dac=%d charge=%f fmean=%f\n",
           dac,charge,val);
           dotacc(5,charge,val);
           sig=0.0;
           sig2=sxxev/sev - val*val;
           if(sig2>0.0) sig=sqrt(sig2);
           if(val>0.0) dotacc(9,charge,sig);
          }
        if(val>0.0 && val<1600.0)
         { crg[icnt] = charge;
           fvl[icnt] = val;
           if(charge>20.0)
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
   aa[card-1] = a;
   bb[card-1] = b;
   if(sw(5) == 1) printf(" card %d fit results a=%f b=%f y=a+b*charge\n",card,a,b);
   s=0.0;
   sx=0.0;
   for(k=0;k<icnt;k++)
    {x = crg[k];
     y = a + b*x;
     dely = y - fvl[k];
     s = s+1.0;
     sx = sx + dely*dely;
     if(sw(3) == 1) printf("k=%d x=%f  y=%f  fvl[k]=%f  dely=%f\n",k,x,y,fvl[k],dely);
     dotacc(7,x,dely);
    }
    nlin[card-1] = sqrt(sx)/s;
/*     fprintf(fp,"%4d %5d %4d %9.2f %6.2f %4.1f %5.2f %5d %4d %8.2f %4.2f %4.1f %5.2f\n",
          card,kpeakh,timfh,ah,bh,pedhi,dpedhi,
                 kpeakl,timfl,al,bl,pedlo,dpedlo);  */
/*        ratio = bh/bl;
    printf("card=%d bh=%f  bl=%f  ratio=%f\n",card,bh,bl,ratio);  */
    if(sw(5) == 1)
     { dotmwr(2,2,3);
       dotwrt(5);
       dotwrt(7);
       dotwrt(9);
       hstwrt(0);
       hstwrt(2);
     }
/* test trig disable  */
        gateok[card-1] = 0;
        inject();
        fermi_read();
        if(fdata[kk][ipk[kk]] < 1000)  gateok[card-1] = 1;
	trig_disable(ttcadr);
	usleep(200000);
	inject();
	fermi_read(); 
        if(fdata[kk][ipk[kk]] > 1000)  gateok[card-1] = 2;
	okcd[card-1] = gateok[card-1];
	if(aa[card-1] > 20.0) okcd[card-1] = 3;
	if(aa[card-1] < -20.0) okcd[card-1] = 3;
	if(bb[card-1] < 18.0) okcd[card-1] =4;
	if(bb[card-1] > 25.0) okcd[card-1] =4;
	if(noise[card-1] > 8.0) okcd[card-1] =5;
	if(nlin[card-1] > 1.0) okcd[card-1] = 6;
        nall++;
	if(okcd[card-1] == 0)
	  { ngood++;
	    printf(" card %d is OK\n",card);
	  }
	if(okcd[card-1] != 0) 
	  { nbad++;
	    printf(" card %d failed %d  gateok=%d  noise=%f  nlin=%f\n",
	      card,okcd[card-1],gateok[card-1],noise[card-1],nlin[card-1]);
            printf("        aa=%f bb=%f y=a+b*charge\n",
            aa[card-1],bb[card-1]);
	  }
	 
/* ------ print card summary - values for database -------------- */
   if(sw(7) == 1)
    { printf(" card %d gateok=%d  noise=%f  nlin=%f\n",
       card,gateok[card-1],noise[card-1],nlin[card-1]);  
      printf("        aa=%f bb=%f y=a+b*charge\n",
         aa[card-1],bb[card-1]);
    }	 
/* --------------------------------------------------------------- */
  }
/*  printf("resetting card %d\n",card);  */
next:
  trig_disable(ttcadr);
  small_cap_disable(ttcadr);
  large_cap_disable(ttcadr);
  set_dac(ttcadr,0);
 }
 printf(" %d cards   %d good   %d bad\n",nall,ngood,nbad);
}  /* end of main */
evdisp(nn,string,kk)
 int nn,kk;
 char *string;
 { int max,mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,k,ii;
   int idel;
   double dely,y,dy;
   char str[5];
   erasm();
   max = pk[kk];
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
    { ii = fdata[kk][i];
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
       { static int entry=0;
         int c1,c2,f1,f2,swtst,i,i1,i2,i0time,i3time;
	 double a;
         if(entry == 0)
	   {/* setup inhibit1 to set tp high */
	     SetBMode(1,0xa);
	     SetInhibitDelay(1,10);
	     SetInhibitDuration(1,10);
	     BPutLong(1,ttcadr,1,0xc0,0x00);  /* load set tp_high into fifo1 */
	     /* set inhibit2 to drop tp low (timed broadcast mode */
	     SetBMode(2,0xa);
	     SetInhibitDelay(2,2000);
	     SetInhibitDuration(2,10);
	  /*  BPutLong(2,ttcadr,1,0xc0,0x01); */  /* load set tp_low into fifo2 */
	     BPutShort(2,0xc0);   /* set tp low bcast command  */
	     /* set inhibit3 to set latch                     */
	     /* use latch-bar and inhibit3 to start fermi     */
             /* will use inhibit 3 to set l1a with digitizers */
             i3time=2010;
 	 /*    printf("input i3time\n");
             fscanf(stdin,"%d",&i3time);    */   
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
             a = sin(0.11);
     	     entry = 1;
	    }
           fermi_reset();
	   a = sin(0.21);  
loop:      stvme();  /* send pulses from the switch box to clear last event */
           a = sin(0.003);
           swtst = latch();  /* wait for the latch bit to set */
           if(swtst == 0) 
             { a = sin(0.003); 
               goto loop;
             }
           restim();  /* reset the timer for next trigger */
           rlatch();    /* reset the latch bit */
  }
fermi_setup()
  { unsigned long am,vmead,offset,size,len;
    am=0x39;
    vmead=base_fermi;
/*    printf(" fermi setup  base=%x\n",base_fermi);  */
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
      dd0 = (int)sd0 & 0xfff;
      fdata[0][i] = 0xfff - dd0;
      if(fdata[0][i]>kmax0)
       { kmax0=fdata[0][i];
         k0=i;}
      sd1 = adc->rd1;
      dd1 = (int)sd1 & 0xfff;
      fdata[1][i] = 0xfff - dd1;
      if(fdata[1][i]>kmax1)
       { kmax1=fdata[1][i];
         k1=i;}
      sd2 = adc->rd2;
      dd2 = (int)sd2 & 0xfff;
      fdata[2][i] = 0xfff - dd2;
      if(fdata[2][i]>kmax2)
       { kmax2=fdata[2][i];
         k2=i;}
      if(sw(4) == 1) printf("i=%d  dd0=%d  dd1=%d  dd2=$d\n",i,dd0,dd1,dd2);
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
cardchk(k)
int k;
  {int i,status;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,ok;
   ok=0;
   status=0;
   multi_low(ttcadr);
   back_low(ttcadr);
   set_tube(ttcadr,k);
   set_intg_gain(ttcadr,5);   /* s1=s3=1 s2=s4=0 */
   itr_high(ttcadr);           /* itr=1 */
   intg_rd_low(ttcadr);        /* ire=0 */
   mse_high(ttcadr);           /* mse=1 */
   small_cap_disable(ttcadr);  /* tplo=0 */
   large_cap_enable(ttcadr);   /* tphi=1 */
   trig_disable(ttcadr);       /* trig=0 */
   back_high(ttcadr);
   back_low(ttcadr);
   status=can_3in1set(id,0xd400);   /* fetch from 3in1 */
   status=can_3in1get(id,&shrt);    /* and load can output reg */
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
   if((s1==1) && (s2==0)  && (s3==1) && (s4==0) && (itr==1) &&
      (ire==0) && (mse==1) && (tplo==0) && (tphi==1) && 
      (trig==0) ) ok++;
   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x %d\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i,ok);

   set_intg_gain(ttcadr,0xa);   /* s1=s3=0 s2=s4=1 */
   itr_low(ttcadr);           /* itr=0 */
   intg_rd_high(ttcadr);        /* ire=1 */
   mse_low(ttcadr);           /* mse=0 */
   small_cap_enable(ttcadr);  /* tplo=1 */
   large_cap_disable(ttcadr);   /* tphi=0 */
   trig_enable(ttcadr);       /* trig=1 */
   back_high(ttcadr);
   back_low(ttcadr);
   status=can_3in1set(id,0xd400);   /* fetch from 3in1 */
   status=can_3in1get(id,&shrt);    /* and load can output reg */
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
   if((s1==0) && (s2==1)  && (s3==0) && (s4==1) && (itr==0) &&
      (ire==1) && (mse==0) && (tplo==1) && (tphi==0) && 
      (trig==1) ) ok++;
   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x %d\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i,ok);
  if(ok == 2) status=1;
   return(status);
  }
