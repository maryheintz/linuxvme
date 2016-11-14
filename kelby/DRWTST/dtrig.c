#include <stdio.h>
#include <math.h>
#include <time.h>
#include "vme_base.h"
 
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
int image[50][150];
int ttcadr,id;
unsigned short *reg,vreg;
//static int icap=0,igain=0;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { FILE *fp;
   char adcno[7];
   unsigned char wd,*adr;
    int board,kk;
    long stat;
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    double meanhp,meanhc,meanlp,meanlc;
    double sum,xlsump,xxlsump,xlsumc,xxlsumc;
    double siglp,siglc,pmaxl,cmaxl,tmaxl;
    int nevnt,nevmax,nstrtev,l,nn,i,k,wait,kkk;
    int card,okcd[48],nall,ngood,nbad,ntry;
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
    int tm1,tm2,tm3,tm4;
    unsigned short sval,dval;
    unsigned long status;
    unsigned short sent,back;
    int good,bad,strt;
    int jpk,peak,peakmx;
    ngood=0;
    nbad=0;

   status = vme_a24_base();
   status = vme_a16_base(); 
   reset_ctrl_can();
   
   k = vme16base + 0x6081;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 01 wd=%x\n",wd);
   
   k = vme16base + 0x6083;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 03 wd=%x\n",wd);
   
   k = vme16base + 0x6085;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 05 wd=%x\n",wd);
   
   k = vme16base + 0x6087;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 07 wd=%x\n",wd);
   
   k = vme16base + 0x6089;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 09 wd=%x\n",wd);
   
   k = vme16base + 0x608b;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 0b wd=%x\n",wd);
   
   k = vme16base + 0x608d;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 0d wd=%x\n",wd);
   
   k = vme16base + 0x608f;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 0f wd=%x\n",wd);
   
   k = vme16base + 0x6091;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 11 wd=%x\n",wd);
   
   k = vme16base + 0x6093;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 13 wd=%x\n",wd);
   
   k = vme16base + 0x6095;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 15 wd=%x\n",wd);
   
   k = vme16base + 0x6097;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 17 wd=%x\n",wd);
   
/*   printf("enter ttc address\n");
   fscanf(stdin,"%x",&ttcadr);
   ttcadr = 0x3009;  */
   fp=fopen("/home/kelby/DRWTST/cfg.dat","r");
   fscanf(fp,"%x %s",&ttcadr,adcno);
   printf("ttcadr=%x  adcno=%s\n",ttcadr,adcno);
trt0:
   board = 1;
   id = (board<<6) | 1; 
   ttcvi_mapr(ttcadr);
   /* printf("calling can_reset - this takes a few seconds\n"); */
   can_reset(ttcadr);
   can_init(board,adcno);

/* ------------------ check all cards are there ---------------*/
   sidev(1);   /* force plot to screen without asking*/
   for(kl=1;kl<49;kl++) 
       { kkk=cardchk(kl);
        if(kkk == 1) 
	 { good++;
           printf(" card %d found\n",kl);
	 }
	if(kkk != 1) 
	 { bad++; 
           printf("no card %d\n",kl);
/*	   exit(0); */
	 }
       }  
/* ------------------ disable all cards ---------------*/
    tm1=149;   /* 37-48 */
    tm2=106;   /* 25-36 */
    tm3=107;   /* 13-24 */
    tm4=82;    /* 01-12 */
    set_mb_time(ttcadr,1,tm1);
    set_mb_time(ttcadr,1,tm1);
    set_mb_time(ttcadr,2,tm2);
    set_mb_time(ttcadr,2,tm2);
    set_mb_time(ttcadr,3,tm3);
    set_mb_time(ttcadr,3,tm3);
    set_mb_time(ttcadr,4,tm4);
    set_mb_time(ttcadr,4,tm4);
   for (kl=1;kl<49;kl++)
     {set_tube(ttcadr,kl);
      trig_enable(ttcadr);
      small_cap_disable(ttcadr);
      large_cap_disable(ttcadr);
      set_dac(ttcadr,200);
      multi_low(ttcadr);
      rxw_low(ttcadr);
      intg_rd_low(ttcadr);
      itr_low(ttcadr);
      set_intg_gain(ttcadr,0);  /* s1=s2=s3=s4=0 */
      mse_low(ttcadr);         /* mse=0 */
      trig_disable(ttcadr);
     }
next:
   base_last = 0;
/* ------------------- start card loop --------------- */
    inject_setup();
//   for(card=1;card<49;card++) 
   for(card=1;card<49;card++) 
       { base_fermi = vme24base + basef + boff[card-1];
           if(base_fermi != base_last)
            { fermi_setup();
              base_last = base_fermi;
            }
     peakmx = 0.0;
     for (timf=0; timf<240; timf=timf+5) 
      { SetTimeDes2(ttcadr,timf);
//         printf(" base_fermi=%x\n",base_fermi);
         fermi_reset();
         kk = finput[card-1];
         set_tube(ttcadr,card);
         large_cap_enable(ttcadr);
         trig_enable(ttcadr);
         inject();
         fermi_read();
/*     display ? */
           if(sw(1) == 1) 
            { asum = 0.0;
              axsum = 0.0;
              axxsum = 0.0;
              for(i=5;i<15;i++)
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
                "ipk=%d  pk=%d  ped=%f +/- %f",
                ipk[0],pk[0],ped,dped);
              evdisp(250,string,kk);
            }
            asum = 0.0;
            axsum = 0.0;
            axxsum = 0.0;
            for(i=5;i<15;i++)
             { aval = (double)fdata[kk][i];
               asum = asum+1.0;
               axsum = axsum + aval;
               axxsum = axxsum + aval*aval;
             }
           ped = axsum / asum;
	   jpk=ipk[kk];  
	   peak = fdata[kk][jpk]-ped;
	   if(peak>peakmx) 
	     { peakmx = peak;
	       for(i=0;i<50;i++) 
	        { image[card][i] = fdata[kk][i];
	          if(sw(1) == 1) printf("card %d     kk %d   i %d     fdata %d\n",
	             card,kk,i,fdata[kk][i]);
                }
             }
	 } 
	 if(peak>2500) 
	   { printf(" card %d  peak=%d  GOOD\n",card,peak);
	     ngood++;
	   } 
	 if(peak<=2500) 
	   { printf(" card %d  peak=%d  BAD\n",card,peak);
	     nbad++;
	   } 
           large_cap_disable(ttcadr);
           trig_disable(ttcadr);
 }  /* end of kl loop */
    printf(" %d GOOD    %d BAD\n",ngood,nbad);

    sprintf(string,"TRIG OUTPUT TUBES 1-24");
    if(sw(7)==0) bigdisp(50,string,1);
    sprintf(string,"TRIG OUTPUT TUBES 25-48");
    if(sw(7)==0) bigdisp(50,string,25);
	    
    if(sw(8)==1) goto next;	    
}  /* end of main */

bigdisp(nn,string,strt)
 int nn,strt;
 char *string;
 { int jx[24] = {200,500,800,1100,1400,1700,
                 200,500,800,1100,1400,1700,
                 200,500,800,1100,1400,1700,
                 200,500,800,1100,1400,1700};
   int jy[24] = {2300,2300,2300,2300,2300,2300,
                 1600,1600,1600,1600,1600,1600,
		  900, 900, 900, 900, 900, 900,
		  200, 200, 200, 200, 200, 200};	     
 int mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,k,ii,iii;
   int idel,kk,ik;
   double dely,y,dy;
   char str[5];
   erasm();
   putchar(27);
   putchar(59);   /* set fontSmall */
   ik=0;
   for(kk=0;kk<24;kk++)
    {
   mmax=4000;
  
   idx = 250/nn;
   linx = idx*nn;
   ixbas = jx[ik];
   dy = 500.0/(double)mmax;
   iybas = jy[ik];
   mova(ixbas,iybas);
   drwr(linx,0);
   mova(ixbas,iybas);
   iylst = 0;
   for(i=0;i<=nn;i++)
    { ii = image[kk+strt][i];
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
    for(i=1;i<nn+2;i=i+idel)
     { ix=ixbas+idx*i;
       mova(ix,iybas);
       drwr(0,-30);
/*       k=i/2;
       if(k*2 == i)
         { sprintf(str,"%3d",i);
           kx = ix-45;
           if(i<100) kx = ix-55;
           if(i<10) kx=ix-75;
           symb(kx,iybas-100,0.0,str); 
         }  */
     }
/*  label vertical axis  */
    ny =2;
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
       kx = ixbas-100;
     if(i == 2)  symb(kx,iy-20,0.0,str);  
     }
   ik=ik+1;
   sprintf(str,"%d",kk+strt);
   symb(ixbas+20,iybas+500,0.0,str);
   }
    /*  print label */
   putchar(27);
   putchar(56);   /* set large font */
    symb(200,3000,0.0,string);
    plotmx();
 }

     inject_setup()
       { static int entry=0;
         int c1,c2,f1,f2,swtst,i,i1,i2,i0time,i3time;
	 double a;
  
              SetInhibitDelay(0,1);
              SetInhibitDuration(0,2);
              SetBMode(0,0xa); 
              BPutShort(0,1);   /* reset bunch counter on inh0  */

        /* setup inhibit1 to set tp high */
	     SetBMode(1,0xa);
	     SetInhibitDelay(1,10);
	     SetInhibitDuration(1,2);
	/*     BPutLong(1,ttcadr,1,0xc0,0x00); */ /* load set tp_high into fifo1 */
	     BPutShort(1,0xc4);   /* set tp high bcast command  */
	     /* set inhibit2 to drop tp low (timed broadcast mode */
	     SetBMode(2,0xa);
	     SetInhibitDelay(2,2000);
	     SetInhibitDuration(2,2);
	  /*  BPutLong(2,ttcadr,1,0xc0,0x01); */  /* load set tp_low into fifo2 */
	     BPutShort(2,0xc0);   /* set tp low bcast command  */
	     /* set inhibit3 to set latch                     */
	     /* use latch-bar and inhibit3 to start fermi     */
             /* will use inhibit 3 to set l1a with digitizers */
             i3time=2000; 
//	     printf("input i3time\n");
//             fscanf(stdin,"%d",&i3time);  
	     SetBMode(3,0xa);
	     SetInhibitDelay(3,i3time);
	     SetInhibitDuration(3,2);
             arbreset();     	
	    }
	     
     inject()
       { static int entry=0,cnt;

         int c1,c2,f1,f2,swtst,i,i1,i2,i0time,i3time;
	 double a,b,c;
	   cnt=0;
           arbstart();  /* send pulse to start event sequence */
loop:      
           arblatch();  /* wait for the latch bit to set */
           if(vreg == 0) 
             { cnt++;
               if(cnt<100) goto loop;
	       printf(" no arblatch bit\n");
             }
           arbreset();  /* clear last event ltch bit */
   }

fermi_setup()
 { reg = (unsigned short *)(base_fermi + 0x24);   /* fermi reset */
   *reg = 0;
   reg = (unsigned short *)(base_fermi + 0x20);   /* set to full 255 samples */
   *reg = 255;
   reg = (unsigned short *)(base_fermi + 0x1e);   /* set delay */
   *reg = 2;
 }
fermi_reset()
 { reg = (unsigned short *)(base_fermi + 0x24);   /* fermi reset */
   *reg = 0;
 }
fermi_read()
 { int stat,data_ready,busy,lcnt,i,k0,k1,k2,kmax0,kmax1,kmax2;
   int dd0,dd1,dd2;
   unsigned short sd0,sd1,sd2;
   double a;
   /* wait for a trigger */
   lcnt=0;
loop:
   lcnt++;
   reg = (unsigned short *)(base_fermi + 0x22);   /* fermi status register */
   stat = *reg;
   data_ready = stat & 4;
   busy = stat & 2;
   if(lcnt > 1000 ) goto error;
   if ( data_ready == 0 ) goto loop;
   /* read the data and find peak channel */
   a = sin(0.3);  /* little delay after ready */
   k0=0;
   kmax0=0;
   k1=0;
   kmax1=0;
   k2=0;
   kmax2=0;
   for (i=0;i<250;i++)
    { reg = (unsigned short *)(base_fermi + 0x10);   /* r0 register */
      sd0 = *reg;
      dd0 = (int)sd0 & 0xfff;
      fdata[0][i] = 0xfff - dd0;
      if(fdata[0][i]>kmax0)
       { kmax0=fdata[0][i];
         k0=i;}
      reg = (unsigned short *)(base_fermi + 0x12);   /* r1 register */
      sd1 = *reg;
      dd1 = (int)sd1 & 0xfff;
      fdata[1][i] = 0xfff - dd1;
      if(fdata[1][i]>kmax1)
       { kmax1=fdata[1][i];
         k1=i;}
      reg = (unsigned short *)(base_fermi + 0x14);   /* r2 register */
      sd2 = *reg;
      dd2 = (int)sd2 & 0xfff;
      fdata[2][i] = 0xfff - dd2;
      if(fdata[2][i]>kmax2)
       { kmax2=fdata[2][i];
         k2=i;}
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
   printf(" fermi read error\n");
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
   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);

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
   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
  if(ok == 2) status=1;
   return(status);
  }

arbreset()
  { reg = (unsigned short *)(vme24base+0xde0004);
    *reg = 0;
  }

arbstart()
  { reg = (unsigned short *)(vme24base+0xde0006);
    *reg = 0;
  }

arblatch()
  { reg = (unsigned short *)(vme24base+0xde0002);
    vreg = *reg;
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
