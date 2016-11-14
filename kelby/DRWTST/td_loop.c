#include <stdio.h>
#include <math.h>
#include <time.h>
#include "vme_base.h"
struct tm *ptr;
time_t tm;
char xstring[80],ystring[80],string[80],err[80];
char adcno[7];
char str1[16],str2[16],str3[16],str4[16];
unsigned int id,ttcadr,itype;
double v1[50],v2[50],v3[50],v4[50];
double ped[50][6],sped[50][6],vlt[50][6][100],dc[50][6][100];
double nonlin[50][6],aa[50][6],bb[50][6];
int imx[6],ii[6],dok[50],ok[50];
double slp[6] ={0.0108,0.100,0.111,0.211,0.292,0.392};
double dslp[6] = {0.001,0.02,0.010,0.020,0.035,0.080};
double tintr[4] = {23.28,22.46,22.18,21.57};
/* double dtintr[4] = {0.60,0.60,0.60,0.60}; */
double dtintr[4] = {2.0,2.0,2.0,2.0};
double tslp[4] = {0.0207,0.0211,0.0215,0.0219};
double dtslp[4] = {0.008,0.005,0.005,0.005};
double ddev[4] = {5.0,5.0,5.0,5.0};
double nonlim[6] = {0.02,0.02,0.02,0.02,0.02,0.02};
unsigned int card,ngood,nbad,ntot;
int fdata[3][256],ipk[3],pk[3];
double tdata[4][256];
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
unsigned int fbase=0xb40000;
unsigned int fbase2=0xb40000;
double crg[6500],fvl[6500];
static int i3time,ent;
int aok,numerr[4],pass;
FILE *fp,*fpout;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{  int board,kja,kkk,k,good,bad,total,crd;
   int aoka,aokb,aokc,aokd;
   unsigned long status;
   double xhi,xlo;
   status = vme_a24_base();
   status = vme_a16_base();
   fbase = fbase + vme24base;
   fbase2 = fbase2 + vme24base;
   aok=0;
   numerr[0]=0;
   numerr[1]=0;
   numerr[2]=0;
   numerr[3]=0;
   pass=0;
   board = 1;
   id = (board<<6) | 1;
/*   printf("enter ttc address\n");
   fscanf(stdin,"%x",&ttcadr);  */
   fp=fopen("/home/kelby/DRWTST/cfg.dat","r");
   fpout=fopen("/home/kelby/DRWTST/td_loop.log","w");
   fscanf(fp,"%x %s",&ttcadr,adcno);
   printf("%x  %s\n",ttcadr,adcno);
   fprintf(fpout,"%x  %s\n",ttcadr,adcno);
   fflush(fpout);
   for(kja=0;kja<6;kja++)
     { xhi = slp[kja]+10.0*dslp[kja];
       xlo = slp[kja]-10.0*dslp[kja];
       hstset(kja,xhi,xlo,100,1);
       sprintf(string,"integrator slope %d",kja);
       hstlbl(kja,string);
     }
   for(kja=0;kja<6;kja++)
     { crd = 1;
       xhi = 10.0*nonlim[kja];
       xlo = -10.0*nonlim[kja];
       hstset(kja+6,xhi,xlo,100,1);
       sprintf(string,"integrator nonlinearity %d",kja);
       hstlbl(kja,string);
     }
   for(kja=0;kja<4;kja++)
     { xhi = tintr[kja]+10.0*tintr[kja];
       xlo = tintr[kja]-10.0*tintr[kja];
       hstset(kja+12,xhi,xlo,100,1);
       sprintf(string,"timer intercept %d",kja);
       hstlbl(kja,string);
     }
   for(kja=0;kja<4;kja++)
     { xhi = tslp[kja]+10.0*dslp[kja];
       xlo = slp[kja]-10.0*dslp[kja];
       hstset(kja+16,xhi,xlo,100,1);
       sprintf(string,"timer intercept %d",kja);
       hstlbl(kja,string);
     }
restrt:
   pass++;
   aok=0;
   for(kja=0;kja<5;kja++)
   { board = 1;
     id = (board<<6) | 1; 
     ttcvi_mapr(ttcadr);
     reset_ctrl_can();
     printf("calling can_reset - this takes a few seconds\n");
     fprintf(fpout,"calling can_reset - this takes a few seconds\n");
     fflush(fpout);
     can_reset(ttcadr);
     can_init(board,adcno);
     good=0;
     bad=0;
     for(k=1;k<49;k++) 
      {  
        kkk=cardchk(k);
        if(kkk == 1)
	  { good++;
            printf(" card %d found  %x\n",k,k);
           /*  fprintf(fpout," card %d found  %x\n",k,k);
            fflush(fpout); */
	  }
 	if(kkk != 1)
	  { bad++; 
            printf("no card %d  %x\n",k,k);
            fprintf(fpout,"no card %d  %x\n",k,k);
            fflush(fpout);
	  }
      }
     total=good+bad;
     if(good != 48) aok=1;
     if(sw(4)==0) 
      { printf(" good=%d    bad=%d   total=%d   err<crd=%d pls=%d int=%d tim=%d>  pass=%d\n",
          good,bad,total,numerr[0],numerr[1],numerr[2],numerr[3],pass);
      }
     if(kja==4) 
      { fprintf(fpout," good=%d    bad=%d   total=%d   err<crd=%d pls=%d int=%d tim=%d>  pass=%d\n",
          good,bad,total,numerr[0],numerr[1],numerr[2],numerr[3],pass);
        fflush(fpout);
      }

     kkk = cardchk(0);
     if(kkk == 1) 
        { printf("error card 0 found\n");
          fprintf(fpout,"error card 0 found\n");
          fflush(fpout);
          aok=1;
        }
     kkk = cardchk(49);
     if(kkk == 1) 
        { printf("error card 49 found\n");
          fprintf(fpout,"error card 49 found\n");
          fflush(fpout);
          aok=1;
        }
    }

   if(aok!=0) numerr[0]++;
   aoka=aok;
   aok=0;
   chk_pulse();
   if(aok!=0) numerr[1]++;
   aokb=aok;
   aok=0;
   chk_intg();  
   if(aok!=0) numerr[2]++;
   aokc=aok;
   aok=0;
   chk_timer();
   if(aok!=0) numerr[3]++;
   aokd=aok;
   aok= aoka+aokb+aokc+aokd;
   if(aok == 0) 
    { printf("PASSED ALL TESTS   err<crd=%d pls=%d int=%d tim=%d>  pass=%d\n",
        numerr[0],numerr[1],numerr[2],numerr[3],pass);
      fprintf(fpout,"PASSED ALL TESTS   err<crd=%d pls=%d int=%d tim=%d>  pass=%d\n",
        numerr[0],numerr[1],numerr[2],numerr[3],pass);
      fflush(fpout);
    }

   if(aok != 0) 
    { printf("FAILED A TEST   numerr=%d %d %d %d  pass=%d\n",
      numerr[0],numerr[1],numerr[2],numerr[3],pass);
      fprintf(fpout,"FAILED A TEST   numerr=%d %d %d %d  pass=%d\n",
      numerr[0],numerr[1],numerr[2],numerr[3],pass);
      fflush(fpout);
    }
   if(sw(5)== 1) 
     { for(kja=0;kja<20;kja++) hstwrt(kja); }
   if(sw(8) == 1) goto restrt;
} 

chk_pulse()
 { /* -------------- check that all slots pulse ---------------- */
   int i,dac,k;
   double fdac,charge,capacitor;
   int kkk,phase,kk,ibt,ngd[48];
   unsigned short kja1,kja2,kja3;
   printf("chk_pulse\n");
   fprintf(fpout,"chk_pulse\n");
   fflush(fpout);
   ngood = 0;
   nbad = 0;
   ntot = 0;
   kk=0;
   ent=0;
   fermi_setup();
   multi_low(ttcadr);
   set_mb_time(ttcadr,1,158);
   set_mb_time(ttcadr,2,115);
   set_mb_time(ttcadr,3,83);
   set_mb_time(ttcadr,4,36);
   kkk = cardchk(0);
   if(kkk == 1) 
     { printf(" error card 0 found\n");
       fprintf(fpout," error card 0 found\n");
       fflush(fpout);
       aok++; }
   kkk = cardchk(49);
   if(kkk == 1) 
     { printf(" error card 49 found\n");
       fprintf(fpout," error card 49 found\n");
       fflush(fpout);
       aok++; }
   for(k=1;k<49;k++)
       { set_tube(ttcadr,k);
         rxw_low(ttcadr);
         intg_rd_low(ttcadr);
         itr_low(ttcadr);
         set_intg_gain(ttcadr,0);  /* s1=s2=s3=s4=0 */
         trig_enable(ttcadr);
         mse_high(ttcadr);         /* mse=1 */
       }
   multi_high(ttcadr);
   capacitor = 100.0;
   large_cap_enable(ttcadr);
   small_cap_disable(ttcadr);
   charge=800.0;
   fdac = (1023.0*charge) / (2.0 * 4.096 * capacitor);
   dac = (int)fdac;
   charge = 2.0 * 4.096 * capacitor * (double)dac / 1023.0;
   set_dac(ttcadr,dac);  
   usleep(10);
   tp_low(ttcadr);
   phase=116;
   SetTimeDes2(ttcadr,phase);  
   for (k=1;k<49;k++) ngd[k-1]=0;
   for(i=0;i<10;i++)
     { 
lup:
       compar_reset();
       inject();
       usleep(50);
       compar_read15_0(&kja1);
       for(ibt=0;ibt<16;ibt++) 
         { if( ((kja1>>ibt) & 1) != 0) ngd[ibt]++; } 
       compar_read31_16(&kja2);
       for(ibt=0;ibt<16;ibt++) 
         { if( ((kja2>>ibt) & 1) != 0) ngd[ibt+16]++; }
       compar_read47_32(&kja3);
       for(ibt=0;ibt<16;ibt++) 
         { if( ((kja3>>ibt) & 1) != 0) ngd[ibt+32]++; }
       usleep(50);
     if(sw(3) == 1) 
       { printf(" kja1=%x  kja2=%x   kja3=%x\n",kja1,kja2,kja3);
         goto lup;
       }
     }
   for(i=0;i<48;i++)
     { if(ngd[i] != 10) 
        { printf(" card %d failed ngd=%d\n",i+1,ngd[i]);
          fprintf(fpout," card %d failed ngd=%d\n",i+1,ngd[i]);
          fflush(fpout);
        }
       if(sw(6) == 1) printf(" card %d ngd=%d\n",i+1,ngd[i]);
       if(ngd[i] == 10) ngood++;
       if(ngd[i] != 10) nbad++;
       if(ngd[i] != 10) 
        { printf("bad card %d  ngd=%d\n",i+1,ngd[i]);
          fprintf(fpout,"bad card %d  ngd=%d\n",i+1,ngd[i]);
          fflush(fpout);
        }
       ntot++;
     }
      printf(" chk_pulse  %d cards   %d good   %d bad\n",ntot,ngood,nbad);
      fprintf(fpout," chk_pulse  %d cards   %d good   %d bad\n",ntot,ngood,nbad);
      fflush(fpout);
   if(ngood != 48) aok++;
  }

chk_timer()
  { int i,card,k,phase,tok[4];
   double asum,axsum,axxsum,ped,dped,sig2,aval;
   char string[80];
   int kk,board,tim,kpk,tbest;
   double sum,xsum,ysum;
   double f1,f2,f3,b1,b2,b3,fdg,position,dbest,dist;
   double ft,s,sx,sy,sxy,sx2,a,b,delta,del,yy,dy,an,bn;
   int ipass;
/* -------- timer check - now do linearity of card from each section -------- */
   printf("chk_timer\n");
   fprintf(fpout,"chk_timer\n");
   fflush(fpout);
    ent=0;
    ntot=0;
    ngood=0;
    nbad=0;
    multi_low(ttcadr);
    dotzro();
   for(card=1;card<49;card++)
       { if(cardchk(card) != 1) 
           { printf(" card %d not found\n",card);
             fprintf(fpout," card %d not found\n",card);
             fflush(fpout);
           }
         set_tube(ttcadr,card);
         rxw_low(ttcadr);
         intg_rd_low(ttcadr);
         itr_low(ttcadr);
         set_intg_gain(ttcadr,0);  /* s1=s2=s3=s4=0 */
         trig_enable(ttcadr);
         mse_high(ttcadr);         /* mse=1 */
       }
  for(ipass=0;ipass<4;ipass++)
  { if(ipass==0) 
        { card=12;
          kk=0;
          fbase=vme24base+0xb40000;
          fbase2=vme24base+0xb40000;
          fermi_setup();
        }
    if(ipass==1) 
        { card=24;
          kk=1;
          fbase=vme24base+0xb40000;
          fbase2=vme24base+0xb40000;
          fermi_setup();
        }
    if(ipass==2) 
        { card=36;
          kk=2;
          fbase=vme24base+0xb40000;
          fbase2=vme24base+0xb40000;
          fermi_setup();
        }
    if(ipass==3) 
        { card=48;
          kk=0;
          fbase=vme24base+0xb50000;
          fbase2=vme24base+0xb50000;
          fermi_setup();
        }
    dbest=100.0;
    set_tube(ttcadr,card);
    trig_enable(ttcadr);
    if(card<13) board = 4;
    if(card>12 && card<25) board = 3;
    if(card>24 && card<37) board = 2;
    if(card>36) board = 1;
    printf(" board=%d  card=%d\n",board,card);
    fprintf(fpout," board=%d  card=%d\n",board,card);
    fflush(fpout);
    phase = 116;
    SetTimeDes2(ttcadr,phase);
    large_cap_enable(ttcadr);
    small_cap_disable(ttcadr);

    dotset(ipass,250.0,0.0,30.0,20.0,0,0);
    sprintf(xstring,"time setting  board %d",board);
    sprintf(ystring,"position");
    dotlbl(ipass,xstring,ystring);

    for(tim=0;tim<256;tim++)
      { set_mb_time(ttcadr,board,tim);
        usleep(1);
        set_mb_time(ttcadr,board,tim);
        usleep(1);
        set_tube(ttcadr,card);
        set_dac(ttcadr,1023);
	usleep(10);  /* kja */
        if(tim != 255) 
         printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\btim=%d",tim);
        if(tim == 255) 
         printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
        sum=0.0;
        ysum=0.0;
        xsum=0.0;
        for(k=0;k<4;k++)
         {
retry:
          ntot++;
          inject();
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
                "ipk=%d  pk=%d  ped=%f +/- %f",
                ipk[0],pk[0],ped,dped);
              evdisp(32,string,kk);
             }
          kpk=ipk[kk];
/*        printf("step 6  kpk=%d  kk=%d  data=%d\n",
           kpk,kk,fdata[kk][kpk]);  */
          if(kpk<22 || kpk>29) continue;
          if(fdata[kk][kpk] < 1000) 
           { printf("error kk=%d kpk=%d  fdata=%d\n",
             kk,kpk,fdata[kk][kpk]);
             nbad++;
             goto retry;
           }
          ngood++;
          f1 = (double)fdata[kk][ipk[kk]-1];
          b1 = (double)(ipk[kk]-1);
          f2 = (double)fdata[kk][ipk[kk]];
          b2 = (double)(ipk[kk]);
          f3 = (double)fdata[kk][ipk[kk]+1];
          b3 = (double)(ipk[kk]+1);
          position = (f1*b1 + f2*b2 + f3*b3) / (f1 + f2 + f3);
          sum=sum+1.0;
          ysum=ysum+position;
         }  
       position = ysum/sum;
       dist = position-25.0;
       if(dist<0.0) dist = -dist;
       if(dist<dbest)
         {dbest = dist;
          tbest = tim;
         }

     /*   printf("  ipass=%d  time=%d   pos=%f\n",
        ipass,tim,position);  */
/*	if(ipass==3 && tim==5) goto retry;     kjakjakja */
       dotacc(ipass,(double)tim,position);
       tdata[ipass][tim] = position; 
    }

/*     printf("board %d  tbest=%d  dbest=%f\n",board,tbest,dbest);  */
     fflush(stdout);
     sidev(1);
     if(sw(6) == 1) dotwrt(ipass);
    }  /* end of ipass loop */
    printf("fit the lines\n");
/* fit a line and get the max deviation */
   for(ipass=0;ipass<4;ipass++)
    { s = 0.0;
      sx = 0.0;
      sy = 0.0;
      sxy = 0.0;
      sx2 = 0.0;
      for(tim=10;tim<225;tim++)
       { s = s + 1.0;
         ft = (double)tim;
         sx = sx + ft;
         sy = sy + tdata[ipass][tim];
         sxy = sxy + ft * tdata[ipass][tim];
         sx2 = sx2 + ft * ft;
/*     printf(" tim %d  tdata %f\n",tim,tdata[ipass][tim]); */
       }
       delta = (s*sx2) - (sx*sx);
/*     printf(" delta=%f  s=%f  sx2=%f  sx=%f  sy=%f\n",delta,s,sx2,sx,sy); */
       an = (sx2*sy) - (sx*sxy);
       bn = (s*sxy) - (sx*sy);
/*       printf("an=%f  bn=%f\n",an,bn); */
       a = an / delta;
       b = bn / delta;
       hstacc(ipass+12,a,1.0);
       /* printf(" 12 test called hstacc %d  %f\n",ipass+12,a); */
       hstacc(ipass+16,b,1.0);
       /* printf(" 16 test called hstacc %d  %f\n",ipass+16,b); */
       printf("a=%f  b=%f  y=a+b*time\n",a,b);  
       del = 0.0;
       for(tim=10;tim<225;tim++)
        { yy = a + b * (double)tim;
          dy = yy - tdata[ipass][tim];
/*          printf("tim=%d  yy=%f  dy=%f\n",tim,yy,dy); */
          if(dy<0.0) dy = -dy;
          if(dy>del) del = dy;
        }
       tok[ipass] = 0;
       if(a < tintr[ipass]-dtintr[ipass] || a > tintr[ipass]+dtintr[ipass])
         tok[ipass] = 1;
       if(a < tintr[ipass]-dtintr[ipass] || a > tintr[ipass]+dtintr[ipass])
         { printf("intercept fail ipass=%d  a=%f\n",ipass,a);
           fprintf(fpout,"intercept fail ipass=%d  a=%f\n",ipass,a);
           fflush(fpout);
         }
       if(b < tslp[ipass]-dtslp[ipass] || b > tslp[ipass]+dtslp[ipass])
         tok[ipass] = 2;
       if(b < tslp[ipass]-dtslp[ipass] || b > tslp[ipass]+dtslp[ipass])
         { printf("slope fail ipass=%d  b=%f\n",ipass,b);
           fprintf(fpout,"slope fail ipass=%d  b=%f\n",ipass,b);
           fflush(fpout);
         }
       if(del > 5.0) tok[ipass] = 3;
       if(del > 5.0) printf("deviation fail ipass=%d deviation=%f\n",
           ipass,del);
       printf("board %d  tok %d\n",ipass+1,tok[ipass]);
       fprintf(fpout,"board %d  tok %d\n",ipass+1,tok[ipass]);
       fflush(fpout);
       if(tok[ipass] != 0) aok++;
     }
    if(sw(6) == 1)
     { dotmwr(4,0,3);
     }
   }  /* end of main */

     inject()
       { 
         fermi_reset();
         if(ent==0)
         {/* now start pulses - take 1 */
            /* setup inhibit1 to set tp high */
	     SetBMode(1,0xa);  /* internal inhibit-bgo connection */
	     SetInhibitDelay(1,10);
	     SetInhibitDuration(1,10);
	/*     BPutLong(1,ttcadr,1,0xc0,0x00); */ /* load set tp_high into fifo1 */
	     BPutShort(1,0xc4);    /* set tp high bcast command  */
	     /* set inhibit2 to drop tp low (timed broadcast mode */
	     SetBMode(2,0xa);
	     SetInhibitDelay(2,2000);
	     SetInhibitDuration(2,10);
	    /* BPutLong(2,0,1,0xc0,0x01);  */ /* load set tp_low into fifo2 */
	     BPutShort(2,0xc0);   /* set tp low bcast command  */
	     /* set inhibit3 to set latch                     */
	     /* use latch-bar and inhibit3 to start fermi     */
             /* will use inhibit 3 to set l1a with digitizers */
             usleep(50);
             i3time=2266;
/*	     printf("input i3time\n");
             fscanf(stdin,"%d",&i3time);   
             printf("i3time=%d\n",i3time);  */ 
	     SetInhibitDelay(3,i3time);
	     SetInhibitDuration(3,10);
	     SetInhibitDelay(0,1);
	     SetInhibitDuration(3,10);
             ent = 1;
             /* printf("inject setup\n"); */
           }
/* take the next event */
   reg=(unsigned short *)(vme24base+0xde0000);
   *reg=0;
   usleep(1000); 
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

fermi_setup()
  {fermi.trig=fbase+0x26;
   fermi.reset=fbase+0x24;
   fermi.status=fbase+0x22;
   fermi.sample=fbase+0x20;
   fermi.delay=fbase+0x1e;
   fermi.rdall=fbase2+0x18;
   fermi.rd2=fbase+0x14;
   fermi.rd1=fbase+0x12;
   fermi.rd0=fbase+0x10;
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
   unsigned short sd0,sd1,sd2;
   /* wait for a trigger */
   lcnt=0;
loop:
   lcnt++;
   reg = (unsigned short *)fermi.status;
   stat = *reg;
   data_ready = stat & 4;
   busy = stat & 2;
   if(lcnt > 1000 ) goto error;
   if ( data_ready == 0 ) 
    { printf("fermi_read wait\n");
      fprintf(fpout,"fermi_read wait\n");
      fflush(fpout);
      goto loop;
    }
   /* read the data and find peak channel */
   k0=0;
   kmax0=0;
   k1=0;
   kmax1=0;
   k2=0;
   kmax2=0;
   for (i=0;i<32;i++)
    { sd0 = *reg0;
      dd0 = (int)sd0 & 0xfff;
      fdata[0][i] = 0xfff - dd0;  
/*      fdata[0][i] = dd0; */
      if(fdata[0][i]>kmax0)
       { kmax0=fdata[0][i];
         k0=i;}
      sd1 = *reg1;
      dd1 = (int)sd1 & 0xfff;
      fdata[1][i] = 0xfff - dd1;  
/*      fdata[1][i] = dd1;  */
      if(fdata[1][i]>kmax1)
       { kmax1=fdata[1][i];
         k1=i;}
      sd2 = *reg2;
      dd2 = (int)sd2 & 0xfff;
      fdata[2][i] = 0xfff - dd2;  
/*      fdata[2][i] = dd2;  */
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
   ipk[0]=-1;
   ipk[1]=-1;
   ipk[2]=-1;
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
compar_reset()
 { reg=(unsigned short *)(vme24base + 0xdd0016);
   *reg=0;
 }
compar_read15_0(k)
unsigned short *k;
 { unsigned short kk;
   reg=(unsigned short *)(vme24base + 0xdd0014);
   kk = *reg;
   *k = kk;
/*   printf("kk15_0 = %x\n",kk);  */
 }
compar_read31_16(k)
unsigned short *k;
 { unsigned short kk;
   reg=(unsigned short *)(vme24base + 0xdd0012);
   kk = *reg;
   *k = kk;
/*   printf("kk31_16 = %x\n",kk);  */
 }
compar_read47_32(k)
unsigned short *k;
 { unsigned short kk;
   reg=(unsigned short *)(vme24base + 0xdd0010);
   kk = *reg;
   *k = kk;
/*   printf("kk47_32 = %x\n",kk);  */
 }
chk_intg()
{  int board;
   int isw[6],i,kl;
   int sw1,dacset,max[6],del[6],k,kk,kkk;
   double volt,expr[6];
   double a,b,delta,dac,factor;
   double fdc,fvlt,s,sx,sy,sxy,sx2;
   double mean,sigma,sqr,val;
   int zone,sector,l,gain,pause;
   unsigned int itst,command;
   unsigned short shrt;
   long status;
   printf("chk_intg\n");
   fprintf(fpout,"chk_intg\n");
   fflush(fpout);
/*   printf(" slp= %f %f %f %f %f\n",
         slp[0],slp[1],slp[2],slp[3],slp[4],slp[5]);
   printf(" dslp= %f %f %f %f %f\n",
         dslp[0],dslp[1],dslp[2],dslp[3],dslp[4],dslp[5]);  */
   status=adc_delay(id,250);
   if(status != 0) printf(" adc_delay status=%x\n",status);
   status=adc_get_delay(id,&shrt);
/*   printf(" status=%x  delay back=%d\n",status,(int)shrt);  */
   for(kl=1;kl<49;kl++) 
       { dok[kl]=0;
         if(cardchk(kl) != 1) 
             { printf(" card %d missing\n",kl);
               fprintf(fpout," card %d missing\n",kl);
               fflush(fpout);
	       dok[kl]=1;
	     } 
         card = kl;
         set_tube(ttcadr,card);
         multi_low(ttcadr);
         rxw_low(ttcadr);
         back_low(ttcadr);
         mse_high(ttcadr);
      } 
    multi_high(ttcadr);
    factor = 9.6/4096.0;  /* volts per count  */

    max[0]=300;
    max[1]=60;
    max[2]=60;
    max[3]=35;
    max[4]=20;
    max[5]=18;

    del[0]=15;
    del[1]=5;
    del[2]=5;
    del[3]=3;
    del[4]=2;
    del[5]=1;

    expr[0]=1.0;
    expr[1]=2.7;
    expr[2]=3.7;
    expr[3]=7.2;
    expr[4]=9.7;
    expr[5]=13.2;

    isw[0]=0xe;
    isw[1]=0xd;
    isw[2]=0xf;
    isw[3]=0xb;
    isw[4]=0x7;
    isw[5]=0x3; 
   if(sw(7)== 1) 
     { printf("chk_intg 1\n");
       fprintf(fpout,"chk_intg 1\n");}
   for(gain=0;gain<6;gain++)
    {printf("starting gain=%d\n",gain);
     fprintf(fpout,"starting gain=%d\n",gain);
     fflush(fpout);
     if(gain==0) status=adc_dacset(id,100);
     if(gain==1) status=adc_dacset(id,100);
     if(gain==2) status=adc_dacset(id,100);
     if(gain==3) status=adc_dacset(id,80);
     if(gain==4) status=adc_dacset(id,70);
     if(gain==5) status=adc_dacset(id,50);
     if(status != 0) printf(" adc_dacset status=%x\n",status);
     intg_rd_low(ttcadr);
     itr_low(ttcadr);
     set_dac(ttcadr,0);  /* dac=0 */
     usleep(100);
   if(sw(7)== 100) 
     { printf("chk_intg 2\n");
       fprintf(fpout,"chk_intg 2\n");}
     set_intg_gain(ttcadr,isw[gain]);
     usleep(10000);
   if(sw(7)== 1) 
     { printf("chk_intg 3\n");
       fprintf(fpout,"chk_intg 3\n");}
     for(card=1;card<49;card++)
       { s=0.0;
         sx=0.0;
	 sx2=0.0;
         set_tube(ttcadr,card);
	 for(i=0;i<5;i++)
           { command = 0xc400 | card;
             status=adc_convert(id,command,&shrt);
	     val = factor * (double)shrt;
	     s=s+1.0;;
	     sx = sx + val;
	     sx2 = sx2 + val*val;
	     if(sw(4)==1) printf("i=%d val=%f  shrt=%x\n",i,val,shrt);
	   }
         mean=sx/s;
	 sqr = sx2/s - (mean*mean);
	 sigma=100.0;
	 if(sqr>0.0) sigma = sqrt(sqr);
	 ped[card][gain] = mean;
	 sped[card][gain] = sigma;
         if(sw(4) == 1) printf("card=%d  gain=%d  ped=%f sped=%f  shrt=%x\n",
	         card,gain,mean,sigma,shrt);
       }
     ii[gain]=0;
     dacset = max[gain];
     for(i=0;i<dacset;i=i+del[gain])       
       { set_dac(ttcadr,i);
         usleep(100);
	 card = 1;  /* toss out first reading */
	 command = 0xc400 | card;
         status=adc_convert(id,command,&shrt);
	 for(card=1;card<49;card++)
	   { command = 0xc400 | card;
             status=adc_convert(id,command,&shrt);
             volt = factor * (double)shrt;
	     if(i==0) ped[card][gain] = volt;  /* ----- kja ----- */
             vlt[card][gain][ii[gain]]=volt-ped[card][gain];
	     dc[card][gain][ii[gain]]=i;
             if(sw(4) == 1) 
	       printf("card=%d dac=%d  volt=%f  ped=%f  vlt=%f shrt=%d\n",
                card,i,volt,ped[card][gain],vlt[card][gain][i],shrt);
/*	if(card==20 && gain==3) printf("card=%d dac=%d  volt=%f  ped=%f  vlt=%f shrt=%d\n",
                card,i,volt,ped[card][gain],vlt[card][gain][ii[gain]],shrt);
*/
	   }
        imx[gain] =ii[gain];
        ii[gain]++;
       }
/*   printf("card=%d  gain=%d  dacset=%d  del=%d  imx=%d\n",
      card,gain,dacset,del[gain],imx[gain]);  */
     }
   set_dac(ttcadr,48);
   intg_rd_low(ttcadr);
   itr_low(ttcadr);
   sleep(1);
   for(card=1;card<49;card++)
    { command = 0xc400 | card;
      status=adc_convert(id,command,&shrt);
      volt = factor * (double)shrt;
      v1[card] = volt;
      if(sw(3) == 1) printf("card=%d intg_rd=1 itr=0 on volt=%f\n",
          card,volt);
    }
    
   set_dac(ttcadr,0);
   usleep(50);
   intg_rd_low(ttcadr);
   itr_high(ttcadr);
   set_dac(ttcadr,48);
   sleep(1);
   for(card=1;card<49;card++)
    { command = 0xc400 | card;
      status=adc_convert(id,command,&shrt);
      volt = factor * (double)shrt;
      v2[card] = volt;
      if(sw(3) == 1) printf("card=%d intg_rd=1 itr=0 on volt=%f\n",
          card,volt);
    }
/* =============================== */
   for(card=1;card<49;card++)
    { command = 0xc400 | card;
      status=adc_convert(id,command,&shrt);
      volt = factor * (double)shrt;
      v2[card] = volt;
      if(sw(3) == 1) printf("card=%d intg_rd=1 itr=0 on volt=%f\n",
          card,volt);
    }
/* =============================== */
   set_dac(ttcadr,0);
   usleep(50);
   intg_rd_high(ttcadr);
   itr_low(ttcadr);
   set_dac(ttcadr,48);
   sleep(1);
   for(card=1;card<49;card++)
    { command = 0xc400 | card;
      status=adc_convert(id,command,&shrt);
      volt = factor * (double)shrt;
      v3[card] = volt;
      if(sw(3) == 1) printf("card=%d intg_rd=1 itr=0 on volt=%f\n",
          card,volt);
    }

   intg_rd_low(ttcadr);
   itr_low(ttcadr);
   sleep(1);
   for(card=1;card<49;card++)
    { command = 0xc400 | card;
      status=adc_convert(id,command,&shrt);
      volt = factor * (double)shrt;
      v4[card] = volt;
      if(sw(3) == 1) printf("card=%d intg_rd=1 itr=0 on volt=%f\n",
          card,volt);
    }
   ngood=0;
   nbad=0;
   ntot=0;
   for(card=1;card<49;card++)
    { dxyzro();
      dxyft_set(0,1,5);
      itype=0;
      ok[card]=0;
      for(gain=0;gain<6;gain++)
       { s=0.0;
         sx=0.0;
         sy=0.0;
         sxy=0.0;
         sx2=0.0;
	 if(gain == 0) dxyset(gain,(double)max[gain],0.0,4.0,0.0,0,0);
         if(gain != 0) dxyset(gain,(double)max[gain],0.0,10.0,0.0,0,0);
         sprintf(xstring,"isw=%x xaxis=dac value",isw[gain]);
         sprintf(ystring,"yaxis=volts");
         dxylbl(i,xstring,ystring);
	  if(sw(6)==1) printf("card %d  imx=%d\n",card,imx[gain]);
          for(i=0;i<=imx[gain];i++)
	    { fdc = dc[card][gain][i];
	      fvlt = vlt[card][gain][i];
	      dxyacc(gain,fdc,fvlt,0.0,0);
	      s=s+1.0;
	      sx=sx+fdc;
	      sy=sy+fvlt;
	      sxy=sxy+(fdc*fvlt);
	      sx2=sx2+(fdc*fdc);
              if(sw(6) == 1) printf(" i=%d  gain=%d  dc=%f  vlt=%f\n",
	               i,gain,fdc,fvlt);
	    }	       
      
         delta = (s*sx2) - (sx*sx);
         if(delta < 1.0) 
          { printf("error delta=%f\n",delta);
            fprintf(fpout,"error delta=%f\n",delta);
            fflush(fpout);
          }
         if(sw(6)==1) printf("s=%f  sx=%f  sy=%f\n  sxy=%f  sx2=%f  delta=%f\n",
             s,sx,sy,sxy,sx2,delta);
         a = ((sx2*sy) - (sx*sxy)) / delta;
         b = ((s*sxy) - (sx*sy)) / delta;
	 aa[card][gain]=a;
	 bb[card][gain]=b;
	 hstacc(gain,b,1.0);
         /* printf(" 0 test called hstacc %d  %f\n",gain,b); */
         if(sw(6) == 1) printf(" card %d gain %d fit results a=%f b=%f volts=a+b*dac  ped=%f  sped=%f\n",
            card,gain,a,b,ped[card][gain],sped[card][gain]);
         if(gain<5 && (a<-0.15 || a>0.15)) ok[card]=6+gain;
         if(gain==5 && (a<-0.25 || a>0.25)) ok[card]=6+gain;
         if(b<(slp[gain]-dslp[gain]) || b>(slp[gain]+dslp[gain]) ) 
	         ok[card]=gain;
	 if(v1[card] < 9.0) ok[card] = 12;
	 if(v2[card] > 2.5) ok[card] = 13;
	 if(v3[card] > 2.5) ok[card] = 14;
	 if(v4[card] < 9.0) ok[card] = 15;
	 if(dok[card] !=0 ) ok[card]=16;
       }  /* end of gain loop */
/*   if(ok[card] == 0) printf(" card %d is OK\n",card);    */
if(sw(6) == 1) 
     { sidev(1);
       dxymwr(6,0,5);
     }  
   }  /* end of card loop */
 /* ------------ calculate the non linearities ----------------- */
      for(gain=0;gain<6;gain++)
       { for(card=1;card<49;card++)
           { s=0.0;
	     sx=0.0;       
             for(i=0;i<=imx[gain];i++)
	       { fdc = dc[card][gain][i];
	         fvlt = vlt[card][gain][i];
	         a=aa[card][gain];
	         b=bb[card][gain];
		 volt = a + b*fdc;
		 delta = fvlt-volt;
/*   if(card==20 && gain==3)   printf("gain=%d  card=%d  fdc=%f  fvlt=%f  volt=%f delta=%f\n",
               gain,card,fdc,fvlt,volt,delta);  
*/
		 s=s+1.0;
		 sx = sx+(delta*delta);
	       }
	      mean = sqrt(sx)/s;
	      nonlin[card][gain] = mean;
	      hstacc(gain+6,mean,1.0);
              /* printf(" 6 test called hstacc %d %f\n",gain+6,mean); */
              if(mean > nonlim[gain]) ok[card] = 17;
              if(mean > nonlim[gain]) 
               { printf("nonlin fail gain=%d  card=%d  nonlin=%f limit %f\n",
                  gain,card,mean,nonlim[gain]);
                 fprintf(fpout,"nonlin fail gain=%d  card=%d  nonlin=%f limit %f\n",
                  gain,card,mean,nonlim[gain]);
                 fflush(fpout);
               }
      }  /* end of card loop */
   }  /* end of gain loop */ 
   for(card=0;card<48;card++)
    { ntot++;
      if(ok[card] == 0) ngood++;    
      if(ok[card] != 0) nbad++;
      kkk=ok[card];    
      if(ok[card] != 0 && ok[card]<6 ) 
         { printf(" card %d is BAD  ok=%d  b=%f  expected=%f\n",
               card,ok[card],bb[card][kkk],slp[kkk]);    
           fprintf(fpout," card %d is BAD  ok=%d  b=%f  expected=%f\n",
               card,ok[card],bb[card][kkk],slp[kkk]);    
           fflush(fpout);
         }
      if(ok[card] != 0 && ok[card]<12 && ok[card]>5 ) 
         { printf(" card %d is BAD  ok=%d  a=%f\n",
	      card,ok[card],aa[card][ok[card]-6]);    
           fprintf(fpout," card %d is BAD  ok=%d  a=%f\n",
	      card,ok[card],aa[card][ok[card]-6]);    
           fflush(fpout);
         }
      if(ok[card] != 0 && ok[card] == 12 ) 
         { printf(" card %d is BAD  ok=%d  v1=%f\n",card,ok[card],v1[card]);    
           fprintf(fpout," card %d is BAD  ok=%d  v1=%f\n",card,ok[card],v1[card]);    
           fflush(fpout);
         }
      if(ok[card] != 0 && ok[card] == 13 ) 
         { printf(" card %d is BAD  ok=%d  v2=%f\n",card,ok[card],v2[card]);    
           fprintf(fpout," card %d is BAD  ok=%d  v2=%f\n",card,ok[card],v2[card]);    
           fflush(fpout);
         }
      if(ok[card] != 0 && ok[card] == 14 ) 
         { printf(" card %d is BAD  ok=%d  v3=%f\n",card,ok[card],v3[card]);    
           fprintf(fpout," card %d is BAD  ok=%d  v3=%f\n",card,ok[card],v3[card]);    
           fflush(fpout);
         }
      if(ok[card] != 0 && ok[card] == 15 ) 
         { printf(" card %d is BAD  ok=%d  v4=%f\n",card,ok[card],v4[card]);    
           fprintf(fpout," card %d is BAD  ok=%d  v4=%f\n",card,ok[card],v4[card]);    
           fflush(fpout);
         }
      if(ok[card] != 0 && ok[card] == 16 ) 
         { printf(" card %d is BAD  ok=%d  no card found\n",card,ok[card]);    
           fprintf(fpout," card %d is BAD  ok=%d  no card found\n",card,ok[card]);    
           fflush(fpout);
         }
    }
/* ---------------plot the slopes ----------------------------- */
/*   if(sw(5)==1)
     { dotset(0,50.0,0.0,0.02,0.0,0,0);
       dotset(1,50.0,0.0,0.2,0.0,0,0);
       dotset(2,50.0,0.0,0.2,0.0,0,0);
       dotset(3,50.0,0.0,0.4,0.0,0,0);
       dotset(4,50.0,0.0,0.4,0.0,0,0);
       dotset(5,50.0,0.0,0.6,0.0,0,0);
       for(gain=0;gain<6;gain++)
         { for(card=1;card<49;card++)
	    {  dotacc(gain,(double)card,bb[card][gain]);
	    }
         }
       dotmwr(6,0,6);
     }  */
/* ---------- plot the deviations from a straight line --------------- */
   for(card=1;card<49;card++)
    { dxyzro();
      itype=1;
      for(gain=0;gain<6;gain++)
       { dxyset(gain,(double)max[gain],0.0,0.1,-0.1,0,0);
             for(i=0;i<=imx[gain];i++)
	       { fdc = dc[card][gain][i];
	         fvlt = vlt[card][gain][i];
	         a=aa[card][gain];
	         b=bb[card][gain];
		 volt = a + b*fdc;
		 delta = fvlt-volt;
		 dxyacc(gain,fdc,delta,0.0,8);
	       }
       }
 if(sw(6) == 1) 
     { sidev(1);
       dxymwr(6,0,5);
     }  
    }
printf("chk_intg  %d cards   %d good    %d bad\n",ntot,ngood,nbad);
fprintf(fpout,"chk_intg  %d cards   %d good    %d bad\n",ntot,ngood,nbad);
fflush(fpout);
if(ngood != 48) aok++;
/* --------------------------------------------------------------------
   patch values into data base here
       card=1 to 48  gain=0 to 5
      dok[card] - 0=good   1=bad   digital check
      ok[card] 0=integrator is ok  nonzero(n)=failed test number n
      sped[card][gain] - pedistal noise (volts)
      bb[card][gain] - slope (volts/dac count)
      nonlin[card][gain] - nonlinearity (volts)	 
      v2[card] - feedthrough (volts)
 ----------------------------------------------------------------------*/
 }
      double dxyfit(l,k,x)
      int l,k;
      double x;
      { char text[80];
        double dxyf;
	int dent=0,i,ix,iy;
        dxyf=0.0;  /* dummy routine */
	if(dent == 0 && itype == 0)
	  {  getp(&ix,&iy);
             sprintf(text,"card %d  %5.1f  %5.1f  %5.1f  %5.1f",
	       card,v1[card],v2[card],v3[card],v4[card]);
	     symb(500,3100,0.0,text);
             mova(ix,iy);  
             dent=1;
          }  
	if(dent == 0 && itype == 1)
	  {  getp(&ix,&iy);
             sprintf(text,"card %d  linearity deviations",
	       card);
	     symb(500,3100,0.0,text);
             mova(ix,iy);  
             dent=1;
             dxyf = -1.0;
          }  
        return(dxyf);
      }
    double hstfit(i,l,x)
    int i,l;
    double x;
    {double y;
     y=0.0;
     return(y); 
    }


