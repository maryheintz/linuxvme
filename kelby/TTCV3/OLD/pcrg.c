#include <stdio.h>
#include <math.h>
#include "vmebase.h"
struct{unsigned int trig;
       unsigned int reset;
       unsigned int status;
       unsigned int sample;
       unsigned int delay;
       unsigned int rdall;
       unsigned int rd2;
       unsigned int rd1;
       unsigned int rd0;}fermi;
unsigned short *reg;
unsigned short *reg0,*reg1,*reg2;
unsigned int fbase=0xb50000;
unsigned int fbase2=0xb50000;
int fdata[3][256],ipk[3],pk[3];
double crg[6500],fvl[6500];
int id,ttcadr,board;
int i3time,ent;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { FILE *fp;
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    double meanhp,meanhc,meanlp,meanlc;
    double sum,xhsump,xxhsump,xhsumc,xxhsumc,xlsump,xxlsump,xlsumc,xxlsumc;
    double sighp,sighc,pmaxh,cmaxh,tmaxh;
    double siglp,siglc,pmaxl,cmaxl,tmaxl;
    int nevnt,nevmax,nstrtev,l,nn,i,k,wait;
    int ipeak,timc,timf;
    int zone,sector,tube,card;
    unsigned int i1,i2,itst;
    unsigned short shrt;
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
    int good,bad,total,kkk,nnn;
    int pittdac;
    double sigma,dsigma;
    unsigned long status;
    status = vme_a24_base();
   fp = fopen("pcrg.dat","w");
   kkk=0;
   ttcadr=0;
   board = 1;
   id = (board<<6) | 1; 
   ttcvi_mapr(ttcadr);
   reset_ctrl_can();
   printf("calling can_reset - this takes a few seconds\n");
   can_reset(ttcadr);
   can_init(board,"S10114");  
   good=0;
   bad=0;
   for(k=1;k<49;k++) 
     {  
        kkk=cardchk(k);
        if(kkk == 1)
	  { good++;
            printf(" card %d found  %x\n",k,k);
	  }
 	if(kkk != 1)
	  { bad++; 
            printf("no card %d  %x\n",k,k);
	  }
     }
   total=good+bad;
   if(sw(4)==0) printf(" good=%d    bad=%d     %d\n",good,bad,total);



   fermi_setup();
   multi_low(ttcadr);
   set_mb_time(ttcadr,1,127);
   set_mb_time(ttcadr,2,76);
   set_mb_time(ttcadr,3,68);
   set_mb_time(ttcadr,4,25);
   for(k=1;k<49;k++)
       { set_tube(ttcadr,k);
         large_cap_disable(ttcadr);
         small_cap_disable(ttcadr);
         trig_disable(ttcadr);       /* trig=0 */
       }
    nevmax=25;
    pmaxh=0.0;
    cmaxh=0.0;
    tmaxh=0.0;
    pmaxl=0.0;
    cmaxl=0.0;
    tmaxl=0.0;
    timc=10;
    timf=0;


        dotset(0,250.0,0.0,2000.0,1000.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"trigger FERMI peak channel amplitude");
        dotlbl(0,xstring,ystring);

        dotset(1,250.0,0.0,40.0,20.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"trigger peak channel number");
        dotlbl(1,xstring,ystring);

        dotset(4,100.0,0.0,4000.0,0.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"trigger fermi peak");
        dotlbl(4,xstring,ystring);

        dotset(6,100.0,0.0,25.0,-25.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"trigger fit y-yfit");
        dotlbl(6,xstring,ystring);  

        dotset(8,100.0,0.0,25.0,0.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"trigger sigma");
        dotlbl(8,xstring,ystring);


        hstset(1,200.0,0.0,50,1);
        hstlbl(1,"trigger pedestal");

        hstset(3,25.0,0.0,50,1);
        hstlbl(3,"trigger pedestal sigma");

/* --------------------- determine trigger timing -------------------*/
    card = 35;
    set_tube(ttcadr,card);
    capacitor = 100.0;
    large_cap_enable(ttcadr);
    small_cap_disable(ttcadr);
    trig_enable(ttcadr);       /* trig=0 */
    charge=140.0;
/*    charge=800.0; */
/*   printf("enter charge\n");
   fscanf(stdin,"%lf",&charge); */
    fdac = (1023.0*charge) / (2.0 * 4.096 * capacitor);
    dac = (int)fdac;
    charge = 2.0 * 4.096 * capacitor * (double)dac / 1023.0;
/*    printf(" trigger charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);  */
    set_dac(ttcadr,dac);
/*    usleep(5); */
    inject_setup();  
   i=0;
   tmaxh = 72.0;
   cmaxh = 33.0;
   if(sw(7) == 1) goto skip;
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
        inject();
/*     go read the fermi module ------------------------------- */
        fermi_read();
/*     display ? */
         if(sw(1) == 1 || fdata[2][ipk[2]] < 100) 
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
            evdisp(50,string,2);
          }
/* now gets sums for mean calculation */
       sum=sum+1.0;
       yc=(double)fdata[2][ipk[2]];
       xhsumc=xhsumc+yc;
       xxhsumc=xxhsumc+yc*yc;
       yc=(double)ipk[2];
       xhsump=xhsump+yc;
       xxhsump=xxhsump+yc*yc;
     } /* end of nevnt loop */
   if(sw(8) == 1) goto loop;
/*  now get mean  */
   meanhp=xhsump/sum;
   sighp=0.0;
   sig2=xxhsump/sum-meanhp*meanhp;
   if(sig2>0.0) sighp=sqrt(sig2);
   meanhc=xhsumc/sum;
   sighc=0.0;
   sig2=xxhsumc/sum-meanhc*meanhc;
   if(sig2>0.0) sighc=sqrt(sig2);
/*   if(sw(3) == 1)
     { printf("\n timer=%d sum=%f\n",timf,sum); 
       printf("       meanhp=%f sighp=%f\n",meanhp,sighp);
       printf("       meanhc=%f sighc=%f\n",meanhc,sighc);
     } */

   printf("trigger timer=%d channel=%6.1f peak=%5.1f\n",
    timf,meanhp,meanhc);
   if(meanhc>pmaxh)
    { pmaxh=meanhc;
      cmaxh=meanhp;
      tmaxh=timf;
    }
   dotacc(0,(double)timf,meanhc);
   dotacc(1,(double)timf,meanhp);
   }  /* end of time loop */
/* ---------------- now do plots at 100Pc ----------------------*/
skip:
   for(charge=10.0;charge<255.0;charge=charge+10.0)
   { fdac = (1023.0*charge) / (2.0 * 4.096 * capacitor);
   if(sw(8) == 1) continue;
     dac = (int)fdac;
     charge = 2.0 * 4.096 * capacitor * (double)dac / 1023.0;
    set_dac(ttcadr,dac);
/*    printf(" trigger charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);  */
   kpeak=(int)(cmaxh+0.5);
   klo=kpeak-2;
   khi=kpeak+2;
   timf=(int)(tmaxh+0.5);
/*   printf("starting trig  kpeak=%d timf=%d\n",kpeak,timf);  */
   kpeakh = kpeak;
   timfh = timf;
   SetTimeDes2(0,timf);  
   icnt=0;
   s=0.0;
   sx=0.0;
   sy=0.0;
   sxy=0.0;
   sx2=0.0;
   nevmax=2000;
      sev = 0.0;
      sxev = 0.0;
      sxxev = 0.0;
      for(nevnt=0;nevnt<nevmax;nevnt++)
       {nnn=nevnt/100;
        if(nnn*100 == nevnt) 
          { printf("\b\b\b\b\b\b\b\b%d",nevnt);
            fflush(stdout); }
rstrth:
        inject();
/*     go read the fermi module ------------------------------- */
        fermi_read();
/*     display ? */
         if(sw(1) == 1) 
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
            evdisp(50,string,2);}
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
        val = (double)fdata[2][kpeak] - ped;
	if(val<0.0) 
            { printf(" fdata=%d  ped=%f   negative subtraction\n",
                fdata[2][kpeak],ped);
              break;
            }
/*        if( (ipk[2] >= klo) && (ipk[2] <= khi) && (fdata[2][kpeak]<4000) ) */
        if( (ipk[2] == kpeak) && (fdata[2][kpeak]<4000) )
         { sev = sev + 1.0;
           sxev = sxev + val;
           sxxev = sxxev + val*val;
/*           printf(" sev=%f  val=%f  ipk=%d\n",sev,val,ipk[2]);  */
	 }
        if(sw(8) == 1) goto rstrth;
        
      } /* end of nevnt loop */
        printf("\n");
        val=0.0;
        if(sev>1.0) 
          {val = sxev / sev;
/*           printf(" trigger dac=%d charge=%f fmean=%f\n",
           dac,charge,val);  */
           dotacc(4,charge,val);
       /*    printf(" mean peak counts = %f\n",val); */
           sig=0.0;
           sig2=sxxev/sev - val*val;
           if(sig2>0.0) sig=sqrt(sig2);
           if(val>0.0) dotacc(8,charge,sig);
      /*     printf(" sigma peak counts = %f\n",sig);  */
          }
        if(val>0.0 && val<4000.0)
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
     sidev(1);
     if(sw(7) == 0) 
      { hstwrt(1);
        hstwrt(3);
      }
/*    hststat(1,&sigma,&dsigma);
   fprintf(fp,"charge=%f  peak=%f  peak_sigma=%f  mean_ped=%f  dmean_ped=%f\n",
      charge,val,sig,sigma,dsigma);
   printf("charge=%f  peak=%f  peak_sigma=%f  mean_ped=%f  dmean_ped=%f\n",
      charge,val,sig,sigma,dsigma);  */
    } /* end of pittdac loop */
}  /* end of main */

double hstfit(i,l,x)
    int i,l;
    double x;
    { double y;
     y=0.0;
     return(y); 
    }  
     
inject_setup()
  {/* now start pulses - take 1 */
   /* setup inhibit1 to set tp high */
    SetBMode(1,0xa);  /* internal inhibit-bgo connection */
    SetInhibitDelay(1,10);
    SetInhibitDuration(1,10);
   /*  BPutLong(1,ttcadr,1,0xc0,0x00); */  /* load set tp_high into fifo1 */
    BPutShort(1,0xc4);   /* set tp high bcast command  */
   /* set inhibit2 to drop tp low (timed broadcast mode */
    SetBMode(2,0xa);
    SetInhibitDelay(2,2000);
    SetInhibitDuration(2,10);
   /* BPutLong(2,0,1,0x80,0x01);  */ /* load set tp_low into fifo2 */
    BPutShort(2,0xc0);   /* set tp low bcast command  */
   /* set inhibit3 to set latch                     */
   /* use latch-bar and inhibit3 to start fermi     */
   /* will use inhibit 3 to set l1a with digitizers */
/*    i3time=2260; */
    i3time=2280; 
   printf("input i3time\n");
    fscanf(stdin,"%d",&i3time);   
    printf("i3time=%d\n",i3time);  
    SetInhibitDelay(3,i3time);
    SetInhibitDuration(3,10);
    SetInhibitDelay(0,1);
    SetInhibitDuration(3,10);
  }

inject()
  { fermi_reset();
    /*  pulse arblogic to let next event stop to fermi through */
    reg=(unsigned short *)(vmebase+0xde0000);
    *reg=0;
  }

fermi_setup()
 { fermi.trig=vmebase+fbase+0x26;
   fermi.reset=vmebase+fbase+0x24;
   fermi.status=vmebase+fbase+0x22;
   fermi.sample=vmebase+fbase+0x20;
   fermi.delay=vmebase+fbase+0x1e;
   fermi.rdall=vmebase+fbase2+0x18;
   fermi.rd2=vmebase+fbase+0x14;
   fermi.rd1=vmebase+fbase+0x12;
   fermi.rd0=vmebase+fbase+0x10;
/* issue reset */
   reg=(unsigned short *)fermi.reset;
   *reg=0;
/* set to full 255 samples */
   reg=(unsigned short *)fermi.sample;
   *reg=255;
/* set delay */
   reg=(unsigned short *)fermi.delay;
   *reg=1;
   reg0=(unsigned short *) fermi.rd0;
   reg1=(unsigned short *) fermi.rd1;
   reg2=(unsigned short *) fermi.rd2;
 }
fermi_reset()
 { 
/* issue reset */
   reg=(unsigned short *)fermi.reset;
   *reg=0;
 }
fermi_read()
 { int stat,data_ready,busy,lcnt,i,k0,k1,k2,kmax0,kmax1,kmax2;
   int dd0,dd1,dd2;
   int decode0=0,decode1=0,decode2=1;
   unsigned short sd0,sd1,sd2;
   /* wait for a trigger */
   lcnt=0;
loop:
   lcnt++;
   reg = (unsigned short *)fermi.status;
   stat = *reg;
   data_ready = stat & 4;
   busy = stat & 2;
   if(lcnt > 4000 ) goto error;
   if ( data_ready == 0 ) goto loop;
   /* read the data and find peak channel */
   k0=0;
   kmax0=0;
   k1=0;
   kmax1=0;
   k2=0;
   kmax2=0;
   for (i=0;i<50;i++)
    { if(decode0 == 1) 
       { sd0 = *reg0;
         dd0 = (int)sd0 & 0xfff;
         fdata[0][i] = 0xfff - dd0;  
         if(fdata[0][i]>kmax0)
          { kmax0=fdata[0][i];
            k0=i;}
       }
      if(decode1 == 1)
       { sd1 = *reg1;
         dd1 = (int)sd1 & 0xfff;
         fdata[1][i] = 0xfff - dd1;  
         if(fdata[1][i]>kmax1)
          { kmax1=fdata[1][i];
            k1=i;}
       }
      if(decode2 == 1)
       { sd2 = *reg2;
         dd2 = (int)sd2 & 0xfff;
         fdata[2][i] = 0xfff - dd2;  
         if(fdata[2][i]>kmax2)
          { kmax2=fdata[2][i];
            k2=i;}
       }
      if(sw(4) == 1) printf("i=%d  dd0=%d  dd1=%d  dd2=%d\n",i,dd0,dd1,dd2);
       }
   ipk[0]=k0;
   pk[0]=kmax0;
   ipk[1]=k1;
   pk[1]=kmax1;
   ipk[2]=k2;
   pk[2]=kmax2;
   return(0);
error:
   printf("fermi_read error\n");
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
   load_can(ttcadr);   /* fetch from 3in1 */
   usleep(1);
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
   load_can(ttcadr);   /* fetch from 3in1 */
   usleep(1);
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

evdisp(nn,string,l)
 int nn,l;
 char *string;
 { int max,mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,k,ii;
   int idel;
   double dely,y,dy;
   char str[5];
   erasm();
   max = pk[l];
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
    { ii = fdata[l][i];
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


