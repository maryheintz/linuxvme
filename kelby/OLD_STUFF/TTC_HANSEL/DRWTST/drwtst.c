
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <ces/vmelib.h>

#include <sys/types.h>
#include <unistd.h>
#include <mem.h>


#include "atdaq_types.h"
#include "s5933lib.h"
#include "SLINK.h"
#include "ttcvi.h"
int fdlow[49][32],lipk[49],lpk[49];
int fdhi[49][32],hipk[49],hpk[49];
double hphase[48][240],hped[48][240];
int hsamp[48][240],hmax[48];
double lphase[48][240],lped[48][240];
int lsamp[48][240],lmax[48];
double pkhi[48][60],pkhic[48][60],pkhis[48][60],npkhi[48];
double pklo[48][90],pkloc[48][90],pklos[48][90],npklo[48];
double hsum[48],lsum[48],sum5;
double ah[48],bh[48],al[48],bl[48];
int FAIL[48];

int ttcadr,id,kk,board,i3time;
int good,bad,total;
double akja;

time_t usec = 1;
int code;
SLINK_parameters slink_para;
SLINK_device *dev;
typedef union {
	unsigned long ldata;
	struct {
		unsigned int topbit   :  1;   /* bit31 head flag */
		unsigned int parity   :  1;   /* bit30 parity bit */
		unsigned int data1    :  10;  /* bit(29:20) adc value1 */
		unsigned int data2    :  10;  /* bit(19:10) adc value2 */
		unsigned int data3    :  10;  /* bit(9:0) adc value3 */
		} bits;
	} slink_formatted_data;
typedef union {
	unsigned long ldata;
	struct {
		unsigned int board8_dmu2_bits : 2;
		unsigned int board8_dmu1_bits : 2;
		unsigned int board7_dmu2_bits : 2;
		unsigned int board7_dmu1_bits : 2;
		unsigned int board6_dmu2_bits : 2;
		unsigned int board6_dmu1_bits : 2;
		unsigned int board5_dmu2_bits : 2;
		unsigned int board5_dmu1_bits : 2;
		unsigned int board4_dmu2_bits : 2;
		unsigned int board4_dmu1_bits : 2;
		unsigned int board3_dmu2_bits : 2;
		unsigned int board3_dmu1_bits : 2;
		unsigned int board2_dmu2_bits : 2;
		unsigned int board2_dmu1_bits : 2;
		unsigned int board1_dmu2_bits : 2;
		unsigned int board1_dmu1_bits : 2;
		} bits;
	} slink_unformatted_data;

/* ttcvi parameter */
static char *ttcvi_address;
int digadr[9] = {0,0x100e,0x100f,0x176e,0x10bb,
      0x1011,0x1014,0x1015,0x1017}; 
int dm[9] = {1,1,1,1,1,1,1,1,1};
int ddac1[9]={150,150,150,150,150,150,150,150,150};
int ddac2[9] ={150,150,150,150,150,150,150,150,150};
unsigned int deskew1[9] = {0,0,0,0,0,10,0,0,0};
/* unsigned int deskew2[9] = {100,100,130,100,118,82,13,56,40};  */
unsigned int deskew2[9] = {100,100,130,100,118,69,13,56,40};
unsigned int iclksm[9] = {0,0,1,0,1,0,0,0,0};
unsigned int crca,crcb;
int dbad[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int code;
static int nsamp,icap,card;
int event=0,nerror=0,ndcrc=0,nbnch=0,ngcrc=0,nctrl=0;
double fl[12]={0.0,100.0,75.0,158.0,-6.0,0.0,4.7,2.5,-5.0,0.0,6.4,64.0};
double dfl[12]={0.0,60.0,60.0,13.0,17.6,8.8,6.5,0.16,10.0,9.0,5.4,5.0};
FILE *fp;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { 
    char serial[7] = {"S10117\0"}; 
    int tim,i,l,k,kkk,ev,step,n,idell,idelh,iavg,pset[48],sw1;
    int dac,tm1,tm2,tm3,tm4;
    double charge,dacval,capacitor,ratio;
    char xstring[80],ystring[80],string[80],err[80];
    double psum,pksum,s,sx,sy,sxy,sx2,del,x,y,dely;
    double sum,xsum,x2sum,sig2,sig,pk;
    fp = fopen("phase.data","w");
    for(i=0;i<48;i++) FAIL[i] = 0;
    sidev(1);  /* send plots to screen */
    kk = 0;
    ttcadr = 0;
    nsamp=6;
    id=1;
    ttcadr= 0x3009;
    /* printf(" ttcadr = %x\n",ttcadr); */ 
    board = 1;
    id = (board<<6) | 1;
    ttcvi_map(ttcadr);
    reset_ctrl_can();
    /* printf(" calling can_reset\n"); */
    can_reset(ttcadr);  
    /* printf(" calling can_init\n"); */
    can_init(board,serial);  
    /* printf(" returned from can_init\n"); */
    SendLongCommand(ttcadr,0,0x6,0);   
    sleep(1);  /* wait for reset to complete */
    SendLongCommand(ttcadr,0,0x3,0xa9);   /*  set the v3.0 TTCrx control register */
    k = GetTriggerSelect();
    /* printf(" trigger select=%x\n",k); */
    
/*    tm1=110;
    tm2=58;
    tm3=58;
    tm4=20; */
    tm1=105;
    tm2=64;
    tm2=60;
    /* tm3=62; */
    tm3=62;
    tm4=35;
    set_mb_time(ttcadr,1,tm1);
    set_mb_time(ttcadr,1,tm1);
    set_mb_time(ttcadr,2,tm2);
    set_mb_time(ttcadr,2,tm2);
    set_mb_time(ttcadr,3,tm3);
    set_mb_time(ttcadr,3,tm3);
    set_mb_time(ttcadr,4,tm4);
    set_mb_time(ttcadr,4,tm4);
    
    SetTimeDes1(ttcadr,0);
    SetTimeDes2(ttcadr,100);
    CoarseDelay(ttcadr,0,0);
    
    hstset(2,200.0,50.0,100,1);
    hstlbl(2,"higain phase");
    hstset(3,200.0,50.0,100,1);
    hstlbl(3,"logain phase");
    hstset(4,180.0,140.0,100,1);
    hstlbl(4,"higain slope");
    hstset(5,50.0,-50.0,100,1);
    hstlbl(5,"higain intercept");
    hstset(6,20.0,-20.0,100,1);
    hstlbl(6,"higain linearity");
    hstset(7,20.0,-20.0,100,1);
    hstlbl(7,"higain sigma");
    hstset(8,10.0,0.0,100,1);
    hstlbl(8,"logain slope");
    hstset(9,10.0,-10.0,100,1);
    hstlbl(9,"logain intercept");
    hstset(10,20.0,-20.0,100,1);
    hstlbl(10,"logain linearity");
    hstset(11,20.0,-20.0,100,1);
    hstlbl(11,"logain sigma");
    hstset(12,75.0,55.0,100,1);
    hstlbl(12,"gain ratio");
     
    
/* ======================== card found test ========================= */
   kkk=cardchk(0);
   if(kkk == 1) 
     { printf(" card 0 found ERROR <<=============================\n");
       exit(0);
     }
   good=0;
   bad=0;
   for(k=1;k<49;k++) 
     {  kkk=cardchk(k);
        if(kkk == 1)
	  { good++;
            /* printf(" card %d found\n",k); */
	  }
 	if(kkk != 1)
	  { bad++; 
            printf(" bad card %d\n",k);
	    FAIL[k-1] = FAIL[k-1] | 1;
	  }
     }
   total=good+bad;
   printf(" good=%d    bad=%d     %d\n",good,bad,total);

    multi_low(ttcadr);
    rxw_low(ttcadr);

    for(k=1;k<49;k++)
      { set_tube(ttcadr,k);
        intg_rd_low(ttcadr);
        itr_low(ttcadr);
        set_intg_gain(ttcadr,0);  /* s1=s2=s3=s4=0 */
        trig_enable(ttcadr);
        large_cap_enable(ttcadr);
        small_cap_disable(ttcadr);
        mse_high(ttcadr);         /* mse=1 */
      }
    multi_high(ttcadr);  /* now set_dac calls effect all tubes */
/* setup for high gain phase data ============================================== */
    charge = 10.0;
    capacitor = 5.2;
    large_cap_disable(ttcadr);
    small_cap_enable(ttcadr);
    dacval = (charge * 1023.0 ) / (2.0 * 4.096* capacitor);
    if(dacval > 1023.0) dacval = 1023.0;
    dac = (int) dacval;
    set_dac(ttcadr,dac);
    inject_setup();
    slink_setup();
    for(i=1;i<9;i++) digitizer_setup(i);  
    kwait(10);
    inject();
    kwait(10);
    clear_buffer();
    inject();
    kwait(10);
    clear_buffer();
   for(i=0;i<240;i++)
    { printf(" taking high gain data phase=%d     \r",i);
      SetTimeDes2(ttcadr,i);
      usleep(10);
      SetTimeDes2(ttcadr,i);
      usleep(10);
      for(k=0;k<48;k++) 
        { hphase[k][i] = 0.0;
	  hped[k][i] = 0.0;
	}
      for(ev=0;ev<10;ev++)
       { inject();
         kwait(10);
         slink_read();
         for(card=1;card<49;card++)
           { k = card-1;
	     l = hipk[card];
	     hsamp[k][i] = l;
	     hphase[k][i] = hphase[k][i] + (double) fdhi[card][l];
	     hped[k][i] = hped[k][i] + 0.5*(fdhi[card][0] + fdhi[card][1]);
	   }
       }
      for(k=0;k<48;k++) 
       { hphase[k][i] = 0.1 * hphase[k][i];
	 hped[k][i] = 0.1 * hped[k][i];
	 hphase[k][i] = hphase[k][i]-hped[k][i]; /* subtract the pedestal */
       }
     }
    printf("\n");
/* setup for low gain phase data =============================================== */
    charge = 500.0;
    capacitor = 100.0;
    large_cap_enable(ttcadr);
    small_cap_disable(ttcadr);
    dacval = (charge * 1023.0 ) / (2.0 * 4.096* capacitor);
    if(dacval > 1023.0) dacval = 1023.0;
    dac = (int) dacval;
    set_dac(ttcadr,dac);
   for(i=0;i<240;i++)
    { printf(" taking low gain data phase=%d      \r",i);
      SetTimeDes2(ttcadr,i);
      usleep(10);
      SetTimeDes2(ttcadr,i);
      usleep(10);
      for(k=0;k<48;k++) 
        { lphase[k][i] = 0.0;
	  lped[k][i] = 0.0;
	}
      for(ev=0;ev<10;ev++)
       { inject();
         kwait(10);   /* kjakja */
         slink_read();
         for(card=1;card<49;card++)
           { k = card-1;
	     l = lipk[card];
	     lsamp[k][i] = l;
	     lphase[k][i] = lphase[k][i] + (double) fdlow[card][l];
	     lped[k][i] = lped[k][i] + 0.5*(fdlow[card][0] + fdlow[card][1]);
	   }
       }
      for(k=0;k<48;k++) 
       { lphase[k][i] = 0.1 * lphase[k][i];
	 lped[k][i] = 0.1 * lped[k][i];
	 lphase[k][i] = lphase[k][i]-lped[k][i]; /* subtract the pedestal */
       }
     }
    printf("\n");
printf(" starting high gain phase data\n");
/* plot the high gain phase data ============================================= */
    for(card=1;card<49;card++)
      { i = card-1;
        hsum[i]=0.0;
        hmax[i]=0;
        dotzro(); 
        dotset(0,240.0,0.0,1000.0,600.0,0,0);
        dotset(1,240.0,0.0,10.0,0.0,0,0);
	for(k=0;k<240;k++)
	  { /* printf("k=%d  %d  %d\n",k,hphase[i][k],hsamp[i][k]);  */
	    dotacc(0,(double)k,hphase[i][k]);
	    dotacc(1,(double)k,(double)hsamp[i][k]);
	    if(k>2 && k<238) 
	      { sum5 = hphase[i][k-2] + hphase[i][k-1]
                + hphase[i][k] + hphase[i][k+1] + hphase[i][k+2];
		/* printf(" k=%d  sum5=%f\n",k,sum5); */
                if(sum5>hsum[i])
	          { hsum[i] = sum5;
	            hmax[i] = k;
		    /* printf(" new peak k=%d  hsum=%f  hmax=%d\n",
		       k,hsum[i],hmax[i]); */
	          }
	      }
	  }
        sw1=sw(1);
	/* printf("sw1=%d\n",sw1); */
	if(sw1 == 1)
          { printf(" high gain phase data card=%d\n",card);
            sprintf(xstring,"card %d high gain phase plots    max bin %d",
	      card,hmax[i]);
            dotmlbl(xstring); 
            dotmwr(2,0,1);
	  }
	}
printf(" starting low gain phase data\n");
/* plot the low gain phase data ============================================== */
    for(card=1;card<49;card++)
      { i = card-1;
        lsum[i]=0.0;
        lmax[i]=0;
        dotzro(); 
        dotset(0,240.0,0.0,900.0,500.0,0,0);
        dotset(1,240.0,0.0,10.0,0.0,0,0);  
	for(k=2;k<238;k++)
	  { dotacc(0,(double)k,lphase[i][k]);
	    dotacc(1,(double)k,(double)lsamp[i][k]); 
	    if(k>2 && k<238) 
	     { sum5 = lphase[i][k-2] + lphase[i][k-1]
                + lphase[i][k] + lphase[i][k+1] + lphase[i][k+2];
               if(sum5>lsum[i])
	         { lsum[i] = sum5;
	           lmax[i] = k;
	         }
	     }
	   }
	if(sw(1) == 1) 
          { printf(" low gain phase data card=%d\n",card);
            sprintf(xstring,"card %d low gain phase plots    max bin %d",
	     card,lmax[i]);
            dotmlbl(xstring); 
            dotmwr(2,0,1);
          }
      }
   /* plot the phase peaks ================================= */     
	dotzro();
	dotset(0,50.0,0.0,250.0,0.0,0,0);
	dotset(1,50.0,0.0,250.0,0.0,0,0);
	for(i=0;i<48;i++)
	  { dotacc(0,(double)i,(double)hmax[i]);
	    dotacc(1,(double)i,(double)lmax[i]);
	   /* if(hmax[i]<90 || hmax[i]>130) printf("========= error ======== i=%d hmax=%d\n",
	          i,hmax[i]); */
	    if(hmax[i]<fl[1]-dfl[1] || hmax[i]>fl[1]+dfl[1]) 
	     { FAIL[i] = FAIL[i] | 2;
	       printf(" card %d FAILED higain phase %d\n",i+1,hmax[i]);
	     }
	    if(lmax[i]<fl[2]-dfl[2] || lmax[i]>fl[2]+dfl[2])
	     { FAIL[i] = FAIL[i] | 4;
	       printf(" card %d FAILED logain phase %d\n",i+1,lmax[i]);
	     }
	    hstacc(2,(double)hmax[i],1.0);
	    hstacc(3,(double)lmax[i],1.0);
	  }
        sprintf(xstring,"cd %d high (above) and low (below) gain peak of phase bin",card);
        dotmlbl(xstring); 
        // dotwrt(0);
        // dotwrt(1);
       if(sw(2) == 0) dotmwr(2,0,1);
	
	for(card=1;card<49;card=card+6)
	  { i=card-1;
	    iavg = (lmax[i]+lmax[i+1]+lmax[i+2]+lmax[i+3]+lmax[i+4]+lmax[i+5]+
                    hmax[i]+hmax[i+1]+hmax[i+2]+hmax[i+3]+hmax[i+4]+hmax[i+5])/12;
	    pset[i] = iavg;
	    pset[i+1] = iavg;
	    pset[i+2] = iavg;
	    pset[i+3] = iavg;
	    pset[i+4] = iavg;
	    pset[i+5] = iavg;
	  }

  	for(card=1;card<49;card++)
	  { i = card-1;
	    idell = pset[i]-lmax[i];
	    idelh = pset[i]-hmax[i];
            fprintf(fp," card=%d  lgain phase=%d  hgain phase=%d  ldiff=%d  hdiff=%d  set=%d\n",
	     card,lmax[i],hmax[i],idell,idelh,pset[i]); 
          }

        fclose(fp);
        sprintf(xstring,"high gain peak of phase bin");
        dotmlbl(xstring); 
	// dotwrt(0);
        sprintf(xstring,"low gain peak of phase bin");
        dotmlbl(xstring); 
	// dotwrt(1);
   /* ======================= high gain linearity plot ================= */
    capacitor = 5.2;
    large_cap_disable(ttcadr);
    small_cap_enable(ttcadr);
     for(card=1;card<49;card++)
       { set_dac(ttcadr,0);
         usleep(1);
	 inject();
         kwait(10);   /* kjakja */
         slink_read();
         i = card-1;
         printf(" High Gain Linearity Card %d            \r",card);
	 fflush(stdout);
         SetTimeDes2(ttcadr,hmax[i]);  
       /*  SetTimeDes2(ttcadr,175);  */
	 dotzro();
	 dotset(0,10.0,0.0,2000.0,0.0,0,0);
	 dotset(1,10.0,0.0,20.0,-20.0,0,0);
	 dotset(2,10.0,0.0,50.0,0.0,0,0);
	 n = 0;
	 step = 5;
	 for(dac=0;dac<246;dac=dac+step)
	   { set_dac(ttcadr,dac);
	     inject();
	     kwait(10);
	     clear_buffer();
	     sum = 0.0;
	     xsum=0.0;
	     x2sum = 0.0;
	     for(ev=0;ev<10;ev++)
	       { inject();
                 kwait(10);   /* kjakja */
                 slink_read();
		 psum =  (double)fdhi[card][0] + (double)fdhi[card][1];
                 pksum = (double)fdhi[card][2] + (double)fdhi[card][3]
		       + (double)fdhi[card][4] + (double)fdhi[card][5]
		       + (double)fdhi[card][6];
                 pk = pksum-(2.5*psum);
		 sum = sum + 1.0;
		 xsum = xsum + pk;
		 x2sum = x2sum + (pk*pk);
		 /* printf(" pk=%f  hits=%d %d %d %d %d %d %d xsum=%f  sum=%f\n",
		   pk,fdhi[card][0],fdhi[card][1],fdhi[card][2],
		      fdhi[card][3],fdhi[card][4],fdhi[card][5],
		      fdhi[card][6],xsum,sum); */
	       }
	     pk = xsum/sum;
	     sig2 = (x2sum/sum) - (pk*pk);
	     sig = 0.0;
	     if(sig2>0.0) sig = sqrt(sig2);
	     charge = (2.0 * 4.096 * capacitor * (double)dac)/1023.0;
	     pkhi[i][n] = pk;
	     pkhic[i][n] = charge;
	     pkhis[i][n] = sig;
	     npkhi[i] = n;
	     /* printf(" n=%d  pk=%f  crg=%f\n",n,pkhi[i][n],pkhic[i][n]); */
             n++;
	     dotacc(0,charge,pk);
	     dotacc(2,charge,sig);
	   }
	 /* fit the data to a line */
	 s = 0.0;
	 sx = 0.0;
	 sy = 0.0;
	 sxy = 0.0;
	 sx2 = 0.0;
	 for(k=0;k<n;k++)
	   { x = pkhic[i][k];
	     y = pkhi[i][k];
	     s = s + 1.0;
	     sx = sx + x;
	     sy = sy + y;
	     sxy = sxy + x*y;
	     sx2 = sx2 + x*x;
	   }
	 del = s*sx2 - sx*sx;
	 ah[i] = (sx2*sy - sx*sxy) / del;
	 bh[i] = (s*sxy - sx*sy) / del;
	 /* printf(" higain fit  card=%d  slope=%f  intercept=%f\n",i+1,bh[i],ah[i]); */
	 if(bh[i]<fl[3]-dfl[3] || bh[i]>fl[3]+dfl[3])
	  { FAIL[i] = FAIL[i] | 0x8;
	    printf(" card %d FAILED higain slope %f\n",i+1,bh[i]);
	  }
	 if(ah[i]<fl[4]-dfl[4] || ah[i]>fl[4]+dfl[4]) 
	  { FAIL[i] = FAIL[i] | 0x10;
	    printf(" card %d FAILED higain intercept %f\n",i+1,ah[i]);
	  }
	 hstacc(4,bh[i],1.0);
	 hstacc(5,ah[i],1.0);
	 /* plot the difference between the line and the data */
	 for(k=0;k<n;k++)
	   { x = pkhic[i][k];
	     y = ah[i] + bh[i] * x;
	     dely = y - pkhi[i][k];
	     dotacc(1,x,dely);
	     /*printf(" k=%d x=%f  yfit=%f  ydata=%f  dely=%f  sig=%f\n",
	           k,x,y,pkhi[i][k],dely,pkhis[i][k]); */
	     if(abs(dely)>fl[5]+dfl[5]) 
	      { FAIL[i]= FAIL[i] | 0x20;
	        printf(" card %d FAILED higain linearity %f\n",i+1,dely);
	      }
	     if(abs(pkhis[i][k])>fl[6]+dfl[6]) 
	      { FAIL[i] = FAIL[i] | 0x40;
	        printf(" card %d FAILED higain sigma %f\n",i+1,pkhis[i][k]);
	      }
	     hstacc(6,dely,1.0);
	     hstacc(7,pkhis[i][k],1.0);
	   }
         sprintf(xstring,"Charge");
         sprintf(ystring,"flat filter counts");
         /* dotlbl(0,xstring,ystring);  */
         sprintf(xstring,"HIGAIN Linearity Plot card %d  Charge vs Flat Filter Counts",card);
         dotmlbl(xstring); 
	 if(sw(3) == 1) dotmwr(3,0,2);
     }  /* end of card loop */
    printf("\n");
   /* ======================= low gain linearity plot ================= */
    capacitor = 100;
    large_cap_enable(ttcadr);
    small_cap_disable(ttcadr);
     for(card=1;card<49;card++)
       { set_dac(ttcadr,0);
	 inject();
	 kwait(10);
         clear_buffer();
         i = card-1;
         printf(" Low Gain Linearity Card %d            \r",card);
	 fflush(stdout);
         SetTimeDes2(ttcadr,lmax[i]); 
      /*   SetTimeDes2(ttcadr,125); */
	 dotzro();
	 dotset(0,600.0,0.0,1600.0,0.0,0,0);
	 dotset(1,600.0,0.0,20.0,-20.0,0,0);
	 dotset(2,600.0,0.0,50.0,0.0,0,0);
	 n = 0;
	 step = 10;
	 for(dac=0;dac<801;dac=dac+step)
	   { set_dac(ttcadr,dac);
	     sum = 0.0;
	     xsum=0.0;
	     x2sum=0.0;
	     for(ev=0;ev<10;ev++)
	       { inject();
                 kwait(10);   /* kjakja */
                 slink_read();
		 psum = (double)fdlow[card][0] + (double)fdlow[card][1];
                 pksum = (double)fdlow[card][2] + (double)fdlow[card][3]
		               + (double)fdlow[card][4] + (double)fdlow[card][5]
			       + (double)fdlow[card][6];
                 pk = pksum-(2.5*psum);
		 sum = sum + 1.0;
		 xsum = xsum + pk;
		 x2sum = x2sum + (pk*pk);
	       }
	     pk = xsum/sum;
	     sig2 = (x2sum/sum) - (pk*pk);
	     sig = 0.0;
	     if(sig2>0.0) sig = sqrt(sig2);
	     charge = (2.0 * 4.096 * capacitor * (double)dac)/1023.0;
	     pklo[i][n] = pk;
	     pkloc[i][n] = charge;
	     pklos[i][n] = sig;
	     npklo[i] = n;
	     /* printf(" n=%d  pk=%f  crg=%f\n",n,pklo[i][n],pkloc[i][n]); */
             n++;
	     dotacc(0,charge,pk);
	     dotacc(2,charge,sig);
	   }
	 
	 /* fit the data to a line */
	 s = 0.0;
	 sx = 0.0;
	 sy = 0.0;
	 sxy = 0.0;
	 sx2 = 0.0;
	 for(k=0;k<n;k++)
	   { x = pkloc[i][k];
	     y = pklo[i][k];
	     s = s + 1.0;
	     sx = sx + x;
	     sy = sy + y;
	     sxy = sxy + x*y;
	     sx2 = sx2 + x*x;
	   }
	 del = s*sx2 - sx*sx;
	 al[i] = (sx2*sy - sx*sxy) / del;
	 bl[i] = (s*sxy - sx*sy) / del;
	 /* plot the difference between the line and the data */
	 /* printf(" logain fit  card=%d  slope=%f  intercept=%f\n",i+1,bl[i],al[i]); */
	 if(bl[i]<fl[7]-dfl[7] || bl[i]>fl[7]+dfl[7]) 
	  { FAIL[i] = FAIL[i] | 0x80;
	    printf(" card %d FAILED logain slope %f\n",i+1,bl[i]);
	  }
	 if(al[i]<fl[8]-dfl[8] || al[i]>fl[8]+dfl[8]) 
	  { FAIL[i] = FAIL[i] | 0x100;
	    printf(" card %d FAILED higain intercept %f\n",i+1,al[i]);
	  }
	 hstacc(8,bl[i],1.0);
	 hstacc(9,al[i],1.0);
	 for(k=0;k<n;k++)
	   { x = pkloc[i][k];
	     y = al[i] + bl[i] * x;
	     dely = y - pklo[i][k];
	     dotacc(1,x,dely);
	     /* printf(" k=%d x=%f  yfit=%f  ydata=%f  dely=%f\n",k,x,y,pklo[i][k],dely); */
	     if(abs(dely)>fl[9]+dfl[9]) 
	      { FAIL[i] = FAIL[i] | 0x200;
	        printf(" card %d FAILED logain linearity %f\n",i+1,dely);
	        printf(" k=%d x=%f  yfit=%f  ydata=%f  dely=%f\n",k,x,y,pklo[i][k],dely); 
	      }
	     if(abs(pklos[i][k])>fl[10]+dfl[10]) 
	      { FAIL[i] = FAIL[i] | 0x400;
	        printf(" card %d FAILED logain sigma %f\n",i+1,pklos[i][k]);
	        printf(" k=%d x=%f  yfit=%f  ydata=%f  dely=%f\n",k,x,y,pklo[i][k],dely); 
	      }
	     hstacc(10,dely,1.0);
	     hstacc(11,pklos[i][k],1.0);
	   }
         sprintf(xstring,"LOGAIN Linearity Plot card %d  Charge vs Flat Filter Counts",card);
         dotmlbl(xstring); 
	 if(sw(3) == 1) dotmwr(3,0,2);
	 ratio = bh[i]/bl[i];
	 /* printf(" card=%d  ratio=%f\n",i,ratio); */
	 if(ratio<fl[11]-dfl[11] || ratio>fl[11]+dfl[11])
	  { FAIL[i] = FAIL[i] | 0x800;
	    printf(" card %d FAILED gain ratio %f\n",i+1,ratio);
	  }
	 hstacc(12,ratio,1.0);
     }  /* end of card loop */
    printf("\n");
    for(i=0;i<10;i++)  printf(" %3x ",FAIL[i]);
    printf("\n");
    for(i=10;i<20;i++) printf(" %3x ",FAIL[i]);
    printf("\n");
    for(i=20;i<30;i++) printf(" %3x ",FAIL[i]);
    printf("\n");
    for(i=30;i<40;i++) printf(" %3x ",FAIL[i]);
    printf("\n");
    for(i=40;i<48;i++) printf(" %3x ",FAIL[i]);
    printf("\n");
    printf("   0x1 - card find\n");
    printf("   0x2 - higain phase\n");
    printf("   0x4 - logain phase\n");
    printf("   0x8 - higain slope\n");
    printf("  0x10 - higain intercept\n");
    printf("  0x20 - higain linearity\n");
    printf("  0x40 - higain sigma\n");
    printf("  0x80 - logain slope\n");
    printf(" 0x100 - logain intercept\n");
    printf(" 0x200 - logain linearity\n");
    printf(" 0x400 - logain sigma\n");
    printf(" 0x800 - gain ratio\n");
    if(sw(4) == 1) for(i=2;i<13;i++) hstwrt(i);
    if(sw(4) == 1) for(i=2;i<13;i++) hstwrt(i);
   }  /* end of main */
     
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
	     SetInhibitDelay(2,1990);
	     SetInhibitDuration(2,2);
	  /*  BPutLong(2,ttcadr,1,0xc0,0x01); */  /* load set tp_low into fifo2 */
	     BPutShort(2,0xc0);   /* set tp low bcast command  */
	     /* set inhibit3 to set latch                     */
	     /* use latch-bar and inhibit3 to start fermi     */
             /* will use inhibit 3 to set l1a with digitizers */
             i3time=2047; 
	     /* printf("input i3time\n");
             fscanf(stdin,"%d",&i3time); */   
	     SetBMode(3,0xa);
	     SetInhibitDelay(3,i3time);
	     SetInhibitDuration(3,2);
	     /* setup the switch box timers */
	     c1=1;
             c2=1;
             f1=5;
             f2=5; 
             setime(c1,f1,c2,f2);
	     restim();
             rlatch();  
    }
	     
     inject()
       { static int entry=0;
         int c1,c2,f1,f2,swtst,i,i1,i2,i0time,i3time;
         
	   akja = sin(0.21);  
loop:      stvme();  /* send pulses from the switch box to clear last event */
	   akja = sin(0.27);   /* kja */ 
           swtst = latch();  /* wait for the latch bit to set */
           if(swtst == 0) 
             { akja = sin(0.003); 
  	       akja = sin(0.17);   /* kja */ 
               goto loop;
             }
           restim();  /* reset the timer for next trigger */
           rlatch();    /* reset the latch bit */
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
   can_bkld_high(id);
   can_bkld_low(id);
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
   /* if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i); */

   set_intg_gain(ttcadr,0xa);   /* s1=s3=0 s2=s4=1 */
   itr_low(ttcadr);           /* itr=0 */
   intg_rd_high(ttcadr);        /* ire=1 */
   mse_low(ttcadr);           /* mse=0 */
   small_cap_enable(ttcadr);  /* tplo=1 */
   large_cap_disable(ttcadr);   /* tphi=0 */
   trig_enable(ttcadr);       /* trig=1 */
   can_bkld_high(id);
   can_bkld_low(id);
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
   /* if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i); */
  if(ok == 2) status=1;
   return(status);
  }

slink_setup()
        {
	SLINK_InitParameters(&slink_para);
	slink_para.position = 0;
	slink_para.start_word = 0x51115110;
	slink_para.stop_word = 0xfffffff0;
	if ((code = SSP_Open(&slink_para, &dev)) != SLINK_OK)
	  { printf(" slink open error, error code = %d\n",code);
	    SLINK_PrintErrorMessage(code);
	    exit(0);
	  }
	/* printf(" slink opened\n"); */
	return; 
	}

clear_buffer()
    { unsigned long int word;
      int type_of_word;
      int data_count = 0;
      while (code = SSP_ReadOneWord(dev, &word, &type_of_word) == SLINK_OK)
       { if (type_of_word != SLINK_CONTROL_WORD) data_count++;
	 if (type_of_word == SLINK_CONTROL_WORD) 
	   { if(word == 0x51115110) data_count++;
	     if(word == 0xfffffff0) return;
	   }	
        }
     }
slink_read()
        {
	unsigned long word;
	int parity,samples,error,sestr,destr,r_parity,v_parity,
	   mode,gain,bunch;
	int type_of_word,count,kk,ii,block,i,k,eflg;
        slink_formatted_data sd;
/* ====================== read control word ========================== */
        count = 0;
	eflg=1;
        code = SSP_ReadOneWord(dev, &word, &type_of_word);
	if(code != SLINK_OK) goto err;
	if (type_of_word == SLINK_CONTROL_WORD) 
	    if(sw(8) == 1) printf(" control word=%x\n",word);
/* ====================== read out the 16 tileDMU chips ============== */
	for(kk=0;kk<16;kk++) {
	block=kk+1;
	/* ------------- header low gain ---------------------------- */
        eflg=2;
	code = SSP_ReadOneWord(dev, &word, &type_of_word);
	if(code != SLINK_OK) goto err;
	if (type_of_word == SLINK_CONTROL_WORD) goto err;
	parity = (word>>30) & 1;
	samples = (word>>26) & 0xf;
	error = (word>>25) & 1;
	sestr = (word>>24) & 1;
        destr = (word>>23) & 1;
	r_parity = (word>>22) & 1;
	v_parity = (word>>18) & 0xf;
	mode = (word>>15) & 3;
	gain = (word>>12) & 0x7;
	bunch = word & 0xfff;
	if(sw(8) == 1) printf("%d block=%d logain header word %x   bunch %d  %x %x\n"
	,count,block,word,bunch,crca,crcb);
	count++;
        /* ------------- samples low gain ---------------------------- */
	for(ii=0;ii<nsamp+1;ii++)
            {eflg=3;
	     code = SSP_ReadOneWord(dev, &word, &type_of_word);
	     if(code != SLINK_OK) goto err;
	     if (type_of_word == SLINK_CONTROL_WORD) goto err;
	     sd.ldata =  word;
             if(kk<8)
		{ fdlow[3*kk+1][ii] = sd.bits.data3;
		  fdlow[3*kk+2][ii] = sd.bits.data2;
		  fdlow[3*kk+3][ii] = sd.bits.data1;
	        }
		else
		{ fdlow[3*kk+1][ii] = sd.bits.data1;
		  fdlow[3*kk+2][ii] = sd.bits.data2;
		  fdlow[3*kk+3][ii] = sd.bits.data3;
	        }		 
                if(sw(8) == 1) printf("%d  ldata word= %4i %4i %4x %4x %4x",count,
	           sd.bits.topbit,sd.bits.parity,
		   sd.bits.data1,sd.bits.data2,
		   sd.bits.data3);
		if(sw(8) == 1) printf("   %d  %d    %x\n",kk,ii,word);
		count++;
	    }
	/* ------------- header high gain ---------------------------- */
        eflg=4;
	code = SSP_ReadOneWord(dev, &word, &type_of_word);
	if(code != SLINK_OK) goto err;
	if (type_of_word == SLINK_CONTROL_WORD) goto err;
	sd.ldata =  word;
	parity = (word>>30) & 1;
	samples = (word>>26) & 0xf;
	error = (word>>25) & 1;
	sestr = (word>>24) & 1;
        destr = (word>>23) & 1;
	r_parity = (word>>22) & 1;
	v_parity = (word>>18) & 0xf;
	mode = (word>>15) & 3;
	gain = (word>>12) & 0x7;
	bunch = word & 0xfff;
	if(sw(8) == 1) printf("%d  %d higain header word=%x   bunch=%d  %x %x\n",
	   count,block,word,bunch,crca,crcb);
	count++;
        /* ------------- samples low gain ---------------------------- */
	for(ii=0;ii<nsamp+1;ii++)
            {eflg=5;
	     code = SSP_ReadOneWord(dev, &word, &type_of_word);
             if(code != SLINK_OK) goto err;
	     if (type_of_word == SLINK_CONTROL_WORD) goto err;
	     sd.ldata =  word;
             if(kk<8)
		{ fdhi[3*kk+1][ii] = sd.bits.data3;
		  fdhi[3*kk+2][ii] = sd.bits.data2;
		  fdhi[3*kk+3][ii] = sd.bits.data1;
	        }
		else
		{ fdhi[3*kk+1][ii] = sd.bits.data1;
		  fdhi[3*kk+2][ii] = sd.bits.data2;
		  fdhi[3*kk+3][ii] = sd.bits.data3;
	        }		 
                if(sw(8) == 1) printf("%d  hdata word= %4i %4i %4x %4x %4x",count,
	           sd.bits.topbit,sd.bits.parity,
		   sd.bits.data1,sd.bits.data2,
		   sd.bits.data3);
		if(sw(8) == 1) printf("   %d  %d    %x\n",kk,ii,word);
		count++;
            }
            eflg=6;
	    code = SSP_ReadOneWord(dev, &word, &type_of_word);
            if(code != SLINK_OK) goto err;
 	    if (type_of_word == SLINK_CONTROL_WORD) goto err;
            if(sw(8) == 1) printf("%d   crc=%x\n",count,word);
	    count++;
      } /* end kk loop */
            eflg=7;
	    code = SSP_ReadOneWord(dev, &word, &type_of_word);
	    if(code != SLINK_OK) goto err;
	    if (type_of_word == SLINK_CONTROL_WORD) goto err;  
	        if(sw(8) == 1) printf("%d  digcrc word=%x   %x\n",count,word,code);
	    count++;
            eflg=8;
	    code = SSP_ReadOneWord(dev, &word, &type_of_word);
	    if(code != SLINK_OK) goto err;
	    if (type_of_word == SLINK_CONTROL_WORD) goto err;   
	        if(sw(8) == 1) printf("%d   global crc word=%x   %x\n",count,word,code);
            count++;
            eflg=9;
	    code = SSP_ReadOneWord(dev, &word, &type_of_word);
	    if(code != SLINK_OK) goto err;
	    if (type_of_word != SLINK_CONTROL_WORD) goto err;   
	        if(sw(8) == 1) printf(" end control word=%x   %x\n\n\n",word,code);
            count++;
 
 	    for(i=1;i<49;i++)
	      { hpk[i] = 0;
	        hipk[i] = 0;
		lpk[i] = 0;
		lipk[i] = 0;
		for(k=0;k<7;k++)
		  { if(fdlow[i][k]>lpk[i])
		      { lpk[i] = fdlow[i][k];
		        lipk[i] = k;
		      }
		    if(fdhi[i][k]>hpk[i])
		      { hpk[i] = fdhi[i][k];
		        hipk[i] = k;
		      }
		  }
	      }
	return;
err:    printf(" error return eflg=%d code=%x   word=%x\n",eflg,code,word);
       }
	 
digitizer_setup(int i)
      { int mode, adrttc, dac1, dac2, ick, pipe;
        adrttc = digadr[i];
	mode = dm[i];
	dac1 = ddac1[i];
	dac2 = ddac2[i];
        ick = iclksm[i];
	pipe=24;
	
        SendLongCommand(adrttc,0,0x6,0);  /* reset v3.0 ttcrx chips*/ 
	usleep(2000);
        SendLongCommand(adrttc,0,0x3,0xa9);  /* setup ttcrx chip bnch cntr */
	usleep(0);  
	
/* set up digitizer registers */
        SendLongCommand(adrttc,1,0xa,ick); /* clock invert bits etc */
	usleep(10);  
	SendLongCommand(adrttc,1,0xf,mode);  /* set mode NORMAL=0 */	
	usleep(10);  
	SendLongCommand(adrttc,1,0x8,dac1);  /* set dac1 */	
	usleep(10);  
	SendLongCommand(adrttc,1,0x9,dac2);  /* set dac2 */	
	usleep(10);  
        SendLongCommand(adrttc,1,0xb,0xff);  /* high limit LSB=0xff */    
	usleep(10);  
	SendLongCommand(adrttc,1,0xc,3);     /* high limit MSB=3 */
	usleep(10);  
        SendLongCommand(adrttc,1,0xd,0);     /* low limit LSB=0 */
	usleep(10);  
	SendLongCommand(adrttc,1,0xe,0);     /* low limit MSB=0 */
	usleep(10);  
        SendLongCommand(adrttc,1,1,nsamp); /* number of samples */
	usleep(10);  
	SendLongCommand(adrttc,1,0x2,pipe); /* set pipeline length */
	usleep(10);  
        BroadcastShortCommand(0x40);       
	usleep(10);  
	SendLongCommand(adrttc,1,0x2,pipe); /* set pipeline length */
	usleep(10);  
        BroadcastShortCommand(0x40);       
	usleep(10);  

/* set up digitizer registers again */
/* set up digitizer registers */
        SendLongCommand(adrttc,1,0xa,ick); /* clock invert bits etc */
	usleep(10);  
	SendLongCommand(adrttc,1,0xf,mode);  /* set mode NORMAL=0 */	
	usleep(10);  
	SendLongCommand(adrttc,1,0x8,dac1);  /* set dac1 */	
	usleep(10);  
	SendLongCommand(adrttc,1,0x9,dac2);  /* set dac2 */	
	usleep(10);  
        SendLongCommand(adrttc,1,0xb,0xff);  /* high limit LSB=0xff */    
	usleep(10);  
	SendLongCommand(adrttc,1,0xc,3);     /* high limit MSB=3 */
	usleep(10);  
        SendLongCommand(adrttc,1,0xd,0);     /* low limit LSB=0 */
	usleep(10);  
	SendLongCommand(adrttc,1,0xe,0);     /* low limit MSB=0 */
	usleep(10);  
        SendLongCommand(adrttc,1,1,nsamp); /* number of samples */
	usleep(10);  
	SendLongCommand(adrttc,1,0x2,pipe); /* set pipeline length */
	usleep(10);  
        BroadcastShortCommand(0x40);       
	SendLongCommand(adrttc,1,0x2,pipe); /* set pipeline length */
	usleep(10);  
        BroadcastShortCommand(0x40);       
	usleep(10);  

        SetTimeDes1(adrttc,deskew1[i]);
        SetTimeDes2(adrttc,deskew2[i]);
        CoarseDelay(adrttc,0,0);
	 
	return;
	}
    double hstfit(i,l,x)
    int i,l;
    double x;
    {double y;
     y=0.0;
     return(y); 
    }  
    kwait(int i)
      { int k;
        double a,b;
        b=0.0;
	for(k=0;k<i;k++);
	  { a = 0.13*(double)i;
	    b = b+sin(a);
	  }
	return(b);
      }
