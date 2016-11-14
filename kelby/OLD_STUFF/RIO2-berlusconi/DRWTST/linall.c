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
double crg[6500],vlt[6500];
int ttcadr,id,card;
int sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8;
int digadr[8] = {0x100e,0x100f,0x176e,0x10bb,
      0x1014,0x1011,0x1015,0x1017}; 
int ddac1[8]={150,150,150,150,150,150,150,150};
int ddac2[8] ={150,150,150,150,150,150,150,150};
unsigned int deskew1[8] = {0,0,0,0,10,0,0,0};
unsigned int deskew2[8] = {150,165,115,150,80,50,55,40};
unsigned int iclksm[8] = {0,0,1,0,0,0,0,0};
int dm[8] = {1,1,1,1,1,1,1,1};
unsigned int pipeline[8] = {23,23,24,23,24,24,24,24};
unsigned int crca,crcb;
unsigned int ERRORS[49],find_ERRORS[49];
int ent=0,FAIL,kFAIL,slink_error,nslink_error;
time_t usec = 1;
int code;
static int nsamp;
int event=0,nerror=0,ndcrc=0,nbnch=0,ngcrc=0,nctrl=0;
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
static int icap,igain;
unsigned short *reg,vreg;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { int tim,i,l,k,kkk,lll,pipe,drawer,dig,kk;
    int tm1,tm2,tm3,tm4;
    int dac,alarm,board;
    double charge,dacval,capacitor;
    char xstring[80],ystring[80],string[80],err[80];
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc,mean,sigma;
    unsigned short *adr;
    FILE *fpcfg;

/* ========= check ARBLOGIC is there for trigger logic ========= */
    reg = (unsigned short *)0xdede0000;
    vreg = *reg;
    printf(" serial=%x\n",vreg);

    reg = (unsigned short *)0xdede0008;
    *reg = 0x1234;
    reg = (unsigned short *)0xdede000a;
    vreg = *reg;
    
    printf(" 0x1234 sent %x read back\n",vreg);
    if(vreg != 0x1234) exit(0);

        dotset(0,250.0,0.0,1000.0,500.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"high gain  peak channel amplitude");
        dotlbl(0,xstring,ystring);

        dotset(1,250.0,0.0,10.0,00.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"high gain peak channel number");
        dotlbl(1,xstring,ystring);

        dotset(2,250.0,0.0,1000.0,500.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"low gain  peak channel amplitude");
        dotlbl(2,xstring,ystring);

        dotset(3,250.0,0.0,10.0,0.0,0,0);
        sprintf(xstring,"fine time setting");
        sprintf(ystring,"low gain peak channel number");
        dotlbl(3,xstring,ystring);

        dotset(4,15.0,0.0,1000.0,0.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"high gain  peak");
        dotlbl(4,xstring,ystring);

        dotset(5,800.0,0.0,1000.0,0.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"low gain  peak");
        dotlbl(5,xstring,ystring);

        dotset(6,15.0,0.0,20.0,-20.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"high gain fit yfit-y");
        dotlbl(6,xstring,ystring);  

        dotset(7,800.0,0.0,20.0,-20.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"low gain fit yfit-y");
        dotlbl(7,xstring,ystring);

        dotset(8,15.0,0.0,5.0,-5.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"high gain sigma");
        dotlbl(8,xstring,ystring);

        dotset(9,800.0,0.0,5.0,-5.0,0,0);
        sprintf(xstring,"charge");
        sprintf(ystring,"low gain sigma");
        dotlbl(9,xstring,ystring);

        hstset(0,100.0,0.0,50,1);
        hstlbl(0,"logain pedestal");

        hstset(1,100.0,0.0,50,1);
        hstlbl(1,"higain pedestal");

        hstset(2,5.0,0.0,50,1);
        hstlbl(2,"logain pedestal sigma");

        hstset(3,5.0,0.0,50,1);
        hstlbl(3,"higain pedestal sigma");

        hstset(11,105.0,55.0,50,1);
        hstlbl(11,"higain slope");

        hstset(12,10.0,0.0,50,1);
        hstlbl(12,"higain dymax");

        hstset(13,2.5,0.0,50,1);
        hstlbl(13,"logain slope");

        hstset(14,10.0,0.0,50,1);
        hstlbl(14,"logain dymax");
	
        hstset(15,75.0,55.0,50,1);
        hstlbl(15,"gain ratio");

        hstset(16,250.0,0.0,50,1);
        hstlbl(16,"higain phase");

        hstset(17,250.0,0.0,50,1);
        hstlbl(17,"logain phase");


    card=0;
    kk = 0;
    nsamp = 6;
    icap = 1;
    nslink_error=0;
 
    fpcfg = fopen("/home/user/kelby/DRWTST/cfg.dat","r");
    fscanf(fpcfg,"%x %s",&ttcadr,string);
    printf("ttcadr=%x  ADC=%s\n",ttcadr,string);

    board = 1;
    id = (board<<6) | 1;
    ttcvi_map(ttcadr);
    SendLongCommand(ttcadr,0,0x6,0);   
    sleep(1);  /* wait for reset to complete */
    SendLongCommand(ttcadr,0,0x3,0xa9);   /*  set the v3.0 TTCrx control register */
    reset_ctrl_can();
    printf("calling can_reset -- wait\n");
    can_reset(ttcadr);  
    printf("calling can_init\n");  
    can_init(board,string);  
    printf("returned from can_init\n");  
    k = GetTriggerSelect();
    printf("trigger select=%x\n",k);
    
    tm1=120;
    tm2=64;
    tm3=58;
    tm4=20;
    set_mb_time(ttcadr,1,tm1);
    set_mb_time(ttcadr,1,tm1);
    set_mb_time(ttcadr,2,tm2);
    set_mb_time(ttcadr,2,tm2);
    set_mb_time(ttcadr,3,tm3);
    set_mb_time(ttcadr,3,tm3);
    set_mb_time(ttcadr,4,tm4);
    set_mb_time(ttcadr,4,tm4);
    
    SetTimeDes1(ttcadr,0);
    SetTimeDes2(ttcadr,126);
    CoarseDelay(ttcadr,0,0); 
    

    multi_low(ttcadr);
    rxw_low(ttcadr);

    for(k=0;k<49;k++)
     { set_tube(ttcadr,k);
       intg_rd_low(ttcadr);
       itr_low(ttcadr);
       set_intg_gain(ttcadr,0);  /* s1=s2=s3=s4=0 */
       trig_enable(ttcadr);
       large_cap_enable(ttcadr);
       small_cap_disable(ttcadr);
       set_dac(ttcadr,0);
       mse_high(ttcadr);         /* mse=1 */
       find_ERRORS[k]=0;
     }

    SetTimeDes2(ttcadr,216);
           
    printf(" Setting up Digitizers\n");
    for(dig=0;dig<8;dig++) digitizer_setup(dig);  

/*    printf(" Setting up Digitizers\n");
    for(dig=0;dig<8;dig++) digitizer_setup(dig);  

    printf(" Setting up Digitizers\n");
    for(dig=0;dig<8;dig++) digitizer_setup(dig);  */

    printf(" Setting up slink\n");
    slink_setup();

    printf(" Setting up inject\n");
    inject_setup();
	
    usleep(2);  
    sidev(1);  /* force plot to screen without asking*/

 for(card=1;card<49;card++)   
  { chk_digital();
    chk_fast();
    printf(" card %d FAIL=%x\n",card,FAIL);
    ERRORS[card]=FAIL;
  } /* end of card loop */
  printf(" ========= LINEARITY FAILURE SUMMARY =========\n");
  printf("%4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x\n",
      ERRORS[1],ERRORS[2],ERRORS[3],ERRORS[4],ERRORS[5],
      ERRORS[6],ERRORS[7],ERRORS[8],ERRORS[9],ERRORS[10]);
  printf("%4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x\n",
      ERRORS[11],ERRORS[12],ERRORS[13],ERRORS[14],ERRORS[15],
      ERRORS[16],ERRORS[17],ERRORS[18],ERRORS[19],ERRORS[20]);
  printf("%4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x\n",
      ERRORS[21],ERRORS[22],ERRORS[23],ERRORS[24],ERRORS[25],
      ERRORS[26],ERRORS[27],ERRORS[28],ERRORS[29],ERRORS[30]);
  printf("%4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x\n",
      ERRORS[31],ERRORS[32],ERRORS[33],ERRORS[34],ERRORS[35],
      ERRORS[36],ERRORS[37],ERRORS[38],ERRORS[39],ERRORS[40]);
  printf("%4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x\n",
      ERRORS[41],ERRORS[42],ERRORS[43],ERRORS[44],ERRORS[45],
      ERRORS[46],ERRORS[47],ERRORS[48]);

  printf("\n ========= card find FAILURE SUMMARY =========\n");
  printf("%4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x\n",
      find_ERRORS[1],find_ERRORS[2],find_ERRORS[3],find_ERRORS[4],find_ERRORS[5],
      find_ERRORS[6],find_ERRORS[7],find_ERRORS[8],find_ERRORS[9],find_ERRORS[10]);
  printf("%4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x\n",
      find_ERRORS[11],find_ERRORS[12],find_ERRORS[13],find_ERRORS[14],find_ERRORS[15],
      find_ERRORS[16],find_ERRORS[17],find_ERRORS[18],find_ERRORS[19],find_ERRORS[20]);
  printf("%4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x\n",
      find_ERRORS[21],find_ERRORS[22],find_ERRORS[23],find_ERRORS[24],find_ERRORS[25],
      find_ERRORS[26],find_ERRORS[27],find_ERRORS[28],find_ERRORS[29],find_ERRORS[30]);
  printf("%4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x\n",
      find_ERRORS[31],find_ERRORS[32],find_ERRORS[33],find_ERRORS[34],find_ERRORS[35],
      find_ERRORS[36],find_ERRORS[37],find_ERRORS[38],find_ERRORS[39],find_ERRORS[40]);
  printf("%4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x  %4x\n",
      find_ERRORS[41],find_ERRORS[42],find_ERRORS[43],find_ERRORS[44],find_ERRORS[45],
      find_ERRORS[46],find_ERRORS[47],find_ERRORS[48]);
  printf("\n    nslink_error=%d\n\n",nslink_error);
  sidev(1);
  printf(" ========== plots ready =============\n");
  for(i=0;i<4;i++)
    { hstwrt(i);
      hstmsg(i,&mean,&sigma);
      printf(" plot %d  mean=%f  sigma=%f  3sigma=%f\n",
          i,mean,sigma,3.0*sigma);
    }
  for(i=11;i<18;i++)
    { hstwrt(i);
      hstmsg(i,&mean,&sigma);
      printf(" plot %d  mean=%f  sigma=%f  3sigma=%f\n",
          i,mean,sigma,3.0*sigma);
    }
}

chk_digital()
  {int k,i,kkk,good,bad,total;
   good=0;
   bad=0;
   for(k=1;k<49;k++) 
     {  kkk=cardchk(k);
        if(kkk == 1) 
	 { good++;
           if(sw(4)==1) printf(" card %d found\n",k);
	 }
	if(kkk != 1) 
	 { bad++; 
           if(sw(4)==1) printf("no card %d\n",k);
	   find_ERRORS[k]++;
	 }
     }
   total=good+bad;
   if(sw(4)==1) printf(" good=%d    bad=%d     %d\n",good,bad,total);
  }
cardchk(k)
int k;
  {int i,status;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,ok;
/*   printf("cardchk entered %d\n",k);  */
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
   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);

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
   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
  if(ok == 2) status=1;
   return(status);
  }

chk_fast()
  { double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    double meanhp,meanhc,meanlp,meanlc;
    double sum,xhsump,xxhsump,xhsumc,xxhsumc,xlsump,xxlsump,xlsumc,xxlsumc;
    double sighp,sighc,pmaxh,cmaxh,tmaxh;
    double siglp,siglc,pmaxl,cmaxl,tmaxl;
    int nevnt,nevmax,nstrtev,l,nn,i,k,wait;
    int ipeak,timc,timf,kipk;
    int dac,time1,time2,time3,time4;
    double fdac,charge,val,ratio;
    int bigain,kpeak,klo,khi,icnt,icnt1,icnt2,dstep;
    double s,sx,sy,sxy,sev,sxev,sxxev,sxest,el,er,ep,estimator;
    double sig,sx2,volt,capacitor,a,b,del,x,y,dely;
    double pedhi,dpedhi,pedlo,dpedlo;
    char xstring[80],ystring[80],string[80],err[80];
    unsigned short cadr,tc,tf;
    int kpeakh,timfh,kpeakl,timfl;
    double ah,bh,al,bl;
    double factor,slopeh,dymaxh,noiseh,slopel,dymaxl,noisel,phaseh,phasel;
    double mh,sh,ml,sl;
    unsigned short sval,dval;
    factor = 1.0;

    set_mb_time(ttcadr,1,156);
    set_mb_time(ttcadr,2,104);
    set_mb_time(ttcadr,3,92);
    set_mb_time(ttcadr,4,40);

    set_mb_time(ttcadr,1,156);
    set_mb_time(ttcadr,2,104);
    set_mb_time(ttcadr,3,92);
    set_mb_time(ttcadr,4,40);

    multi_low(ttcadr);
    rxw_low(ttcadr);

    for(k=0;k<49;k++)
     { set_tube(ttcadr,k);
       intg_rd_low(ttcadr);
       itr_low(ttcadr);
       set_intg_gain(ttcadr,0);  /* s1=s2=s3=s4=0 */
       trig_enable(ttcadr);
       large_cap_enable(ttcadr);
       small_cap_disable(ttcadr);
       set_dac(ttcadr,0);
       mse_high(ttcadr);         /* mse=1 */
     }

    multi_high();

    nevmax=10;
    pmaxh=0.0;
    cmaxh=0.0;
    tmaxh=0.0;
    pmaxl=0.0;
    cmaxl=0.0;
    tmaxl=0.0;

/* --------------------- determine higain timing -------------------*/
    capacitor = 5.2;
    large_cap_disable(ttcadr);
    small_cap_enable(ttcadr);
    charge=10.0;
    fdac = (1023.0*charge) / (2.0*4.096*capacitor);
    dac = (int)fdac;
    charge = 2.0*4.096 * capacitor * (double)dac / 1023.0;
    if(sw(5)==1) printf("higain charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    set_dac(ttcadr,dac);
    usleep(2);
    inject();
    slink_read();
    inject();
    slink_read();
   i=0;
   for (timf=0; timf<240; timf=timf+2)  /* start l loop */
   { SetTimeDes2(ttcadr,timf);
     usleep(2);
     SetTimeDes2(ttcadr,timf);
     usleep(2);
     sum=0.0; 
     xhsump=0.0;
     xxhsump=0.0;
     xhsumc=0.0;
     xxhsumc=0.0;
     for(nevnt=0;nevnt<nevmax;nevnt++)
      {inject();
/*     go read digitizer ------------------------------- */
       slink_read();
       if(slink_error!=0)
         { nslink_error++;
	   continue;
	 }
             if(sw(7) == 1)
   		{ igain = icap;
		  if(icap == 0) sprintf(string,"LOW GAIN TUBES 1-24");
  		  if(icap == 1) sprintf(string,"HIGH GAIN TUBES 1-24");
  		  bigdisp(nsamp,string,1);
  		  if(icap == 0) sprintf(string,"LOW GAIN TUBES 25-48");
  		  if(icap == 1) sprintf(string,"HIGH GAIN TUBES 25-48");
 	 	  bigdisp(nsamp,string,25);
		}
/* now gets sums for mean calculation */
       sum=sum+1.0;
       kipk = hipk[card];
       yc=(double)fdhi[card][kipk];
       xhsumc=xhsumc+yc;
       xxhsumc=xxhsumc+yc*yc;
       yc=(double)kipk;
       xhsump=xhsump+yc;
       xxhsump=xxhsump+yc*yc;
     } /* end of nevnt loop */
/*  now get mean  */
   meanhp=xhsump/sum;
   sighp=0.0;
   sig2=xxhsump/sum-meanhp*meanhp;
   if(sig2>0.0) sighp=sqrt(sig2);
   meanhc=xhsumc/sum;
   sighc=0.0;
   sig2=xxhsumc/sum-meanhc*meanhc;
   if(sig2>0.0) sighc=sqrt(sig2);
   sw3 = sw(3);
   if(sw3 == 1)
     { printf("\n timer=%d sum=%f\n",timf,sum); 
       printf("       meanhp=%f sighp=%f\n",meanhp,sighp);
       printf("       meanhc=%f sighc=%f\n",meanhc,sighc);
     }

   if(sw(6)==1) printf("high gain timer=%d channel=%6.1f peak=%5.1f\n",
    timf,meanhp,meanhc);
   if(meanhc>pmaxh && timf<125)
    { pmaxh=meanhc;
      cmaxh=meanhp;
      tmaxh=timf; 
    }
   dotacc(0,(double)timf,meanhc);
   dotacc(1,(double)timf,meanhp);
   }  /* end of dac loop */
   hstacc(16,(double)tmaxh,1.0);
/* --------------------- determine logain timing -------------------*/
    capacitor = 100.0;
    large_cap_enable(ttcadr);
    small_cap_disable(ttcadr);
    charge=600.0;
    fdac = (1023.0*charge) / (2.0*4.096*capacitor);
    dac = (int)fdac;
    charge = 2.0*4.096 * capacitor * (double)dac / 1023.0;
    if(sw(6)==1) printf("higain charge=%f dac=%d capacitor=%f\n",
         charge,dac,capacitor);
    set_dac(ttcadr,dac);
    usleep(2);
    inject();
    slink_read();
    inject();
    slink_read();
   i=0;
   for (timf=0; timf<240; timf=timf+2) 
   { SetTimeDes2(ttcadr,timf);
     usleep(2);
     SetTimeDes2(ttcadr,timf);
     usleep(2);
     sum=0.0;
     xlsump=0.0;
     xxlsump=0.0;
     xlsumc=0.0;
     xxlsumc=0.0;
     for(nevnt=0;nevnt<nevmax;nevnt++)
      {
        inject();
/*     go read the  module ------------------------------- */
        slink_read();
	if(slink_error!=0)
         { nslink_error++;
	   continue;
	 }
/*     display ? */
             if(sw(7) == 1)
   		{ igain = icap;
		  if(icap == 0) sprintf(string,"LOW GAIN TUBES 1-24");
  		  if(icap == 1) sprintf(string,"HIGH GAIN TUBES 1-24");
  		  bigdisp(nsamp,string,1);
  		  if(icap == 0) sprintf(string,"LOW GAIN TUBES 25-48");
  		  if(icap == 1) sprintf(string,"HIGH GAIN TUBES 25-48");
 	 	  bigdisp(nsamp,string,25);
		}
/* now gets sums for mean calculation */
       kipk = lipk[card];
       sum=sum+1.0;
       yc=(double)fdlow[card][kipk];
       xlsumc=xlsumc+yc;
       xxlsumc=xxlsumc+yc*yc;
       yc=(double)kipk;
       xlsump=xlsump+yc;
       xxlsump=xxlsump+yc*yc;
     } /* end of nevnt loop */
/*  now get mean  */
   meanlp=xlsump/sum;
   siglp=0.0;
   sig2=xxlsump/sum-meanlp*meanlp;
   if(sig2>0.0) siglp=sqrt(sig2);
   meanlc=xlsumc/sum;
   siglc=0.0;
   sig2=xxlsumc/sum-meanlc*meanlc;
   if(sig2>0.0) siglc=sqrt(sig2);
   sw3 = sw(3);
   if(sw3 == 1)
     { printf("\n timer=%d sum=%f\n",timf,sum); 
       printf("       meanlp=%f siglp=%f\n",meanlp,siglp);
       printf("       meanlc=%f siglc=%f\n",meanlc,siglc);
     }

   if(sw(6)==1) printf(" low gain timer=%d channel=%6.1f peak=%5.1f\n",
    timf,meanlp,meanlc);
   if(meanlc>pmaxl && timf<125)
    { pmaxl=meanlc;
      cmaxl=meanlp;
      tmaxl=timf;
    }
   dotacc(2,(double)timf,meanlc);
   dotacc(3,(double)timf,meanlp);
   }  /* end of dac loop */
   hstacc(17,(double)tmaxl,1.0);
/*   show the plots */
     if(sw(4)==1) printf("pmaxh=%f cmaxh=%f tmaxh=%f\n",pmaxh,cmaxh,tmaxh);
     if(sw(4)==1) printf("pmaxl=%f cmaxl=%f tmaxl=%f\n",pmaxl,cmaxl,tmaxl);

/* ---------------- high gain calibration ----------------------*/
   kpeak=(int)(cmaxh+0.5);
   klo=kpeak-2;
   khi=kpeak+2;
   timf=(int)(tmaxh+0.5);
   kpeakh = kpeak;
   timfh = timf;
   SetTimeDes2(ttcadr,timf);
   capacitor = 5.2;
   large_cap_disable(ttcadr);
/*   printf(" enable small\n");  */
   small_cap_enable(ttcadr);
   set_dac(ttcadr,0);
   usleep(1);
   inject();
   slink_read();
   inject();
   slink_read();
   icnt=0;
   s=0.0;
   sx=0.0;
   sy=0.0;
   sxy=0.0;
   sx2=0.0;
   dstep = 2;
/*   dstep = 8;  <======================================== */
   for (dac=0; dac<400; dac=dac+dstep)
     {set_dac(ttcadr,dac);
      usleep(0);
      volt = 2.0*4.096 * (double)dac/1023.0;
      charge = volt * capacitor;
      if(charge<0.0) continue;
      sev = 0.0;
      sxev = 0.0;
      sxxev = 0.0;
      sxest = 0.0;
      for(nevnt=0;nevnt<nevmax;nevnt++)
       {
        inject();
/*     go read the  module ------------------------------- */
        slink_read();
	if(slink_error!=0)
         { nslink_error++;
	   continue;
	 }
/*     display ? */
             if(sw(7) == 1)
   		{ igain = icap;
		  if(icap == 0) sprintf(string,"LOW GAIN TUBES 1-24");
  		  if(icap == 1) sprintf(string,"HIGH GAIN TUBES 1-24");
  		  bigdisp(nsamp,string,1);
  		  if(icap == 0) sprintf(string,"LOW GAIN TUBES 25-48");
  		  if(icap == 1) sprintf(string,"HIGH GAIN TUBES 25-48");
 	 	  bigdisp(nsamp,string,25);
		}
        asum = 0.0;
        axsum = 0.0;
        axxsum = 0.0;
        for(i=0;i<2;i++)
         { aval = (double)fdhi[card][i];
           asum = asum+1.0;
           axsum = axsum + aval;
           axxsum = axxsum + aval*aval;
         }
        kipk = hipk[card];
        ped = axsum / asum;
        hstacc(1,ped,1.0);
        sig2=axxsum/asum - ped*ped;
        if(sig2>0.0) dped=sqrt(sig2);
        hstacc(3,dped,1.0);
        if(fdhi[card][kipk]>1020) goto fithi;  
        val = (double)fdhi[card][kpeak] - ped;
        el = (double)fdhi[card][kpeak-1] - ped;
        er = (double)fdhi[card][kpeak+1] - ped;
        ep = val;
        estimator = (el-er)/ep;
        if(kipk>=klo && kipk<=khi && fdhi[card][kpeak]<1020)
         { sev = sev + 1.0;
           sxev = sxev + val;
           sxxev = sxxev + val*val;
           sxest = sxest + estimator;
         }       
      } /* end of nevnt loop */
        val=0.0;
        if(sev>0.0) 
          {val = sxev / sev;
           estimator = sxest / sev;
           if(sw(6)==1) printf(" high gain dac=%d charge=%5.2f fmean=%6.2f\n",
           dac,charge,val);
           dotacc(4,charge,val);
           sig=0.0;
           sig2=sxxev/sev - val*val;
           if(sig2>0.0) sig=sqrt(sig2);
           if(val>0.0) dotacc(8,charge,sig);
          }
        if(val>0.0 && val<4090.0)
         { crg[icnt] = charge;
           vlt[icnt] = val;
           if(charge>0.0)
           { s = s +1.0;
              sx = sx + charge;
              sy = sy + val;
              sxy = sxy + charge*val;
              sx2 = sx2 + charge*charge;
	   }
           icnt++; 
	 }
   }  /* end of dac loop */
   icnt1=icnt;
   if(sw(6)==1) printf(" end of dac loop icnt=%d\n",icnt);
fithi:
   del = s*sx2 - sx*sx;
   a = (sx2*sy - sx*sxy) / del;
   b = (s*sxy - sx*sy) / del;
   if(sw(6)==1) printf(" higain fit results a=%7.2f b=%7.2f y=a+b*charge\n",a,b);
   ah = a;
   bh = b;
   slopeh = bh*factor;
   s = 0.0;
   sx = 0.0;
   for(k=0;k<icnt;k++)
    {x = crg[k];
     y = a + b*x;
     dely = y - vlt[k];
     s = s + 1.0;
     sx = sx + fabs(dely);
     sw3 = sw(3);
     if(sw3 == 1) printf("k=%d x=%f  y=%f  vlt[k]=%f  dely=%f\n",
       k,x,y,vlt[k],dely);
     dotacc(6,x,dely);
    }
   dymaxh = factor * (sx/s);
/* ---------------------- low gain calibration --------------------------*/
   capacitor = 100.0;
   large_cap_enable(ttcadr);
   small_cap_disable(ttcadr);
   kpeak=(int)(cmaxl+0.5);
   klo=kpeak-2;
   khi=kpeak+2;
   timf=(int)(tmaxl+0.5);
   if(sw(4)==1) printf("starting logain  kpeak=%d timf=%d\n",kpeak,timf);
   kpeakl = kpeak;
   timfl = timf;
   SetTimeDes2(ttcadr,timf);
   capacitor = 100.0;
   large_cap_enable(ttcadr);
   small_cap_disable(ttcadr);
   set_dac(ttcadr,0);
   inject();
   slink_read();
   inject();
   slink_read();
   icnt=0;
   s=0.0;
   sx=0.0;
   sy=0.0;
   sxy=0.0;
   sx2=0.0;
   dstep = 2;
/*   dstep = 16;  <======================================== */
   for (dac=0; dac<1024; dac=dac+dstep)
     {set_dac(ttcadr,dac);
      volt = 2.0*4.096 * (double)dac/1023.0;
      charge = volt * capacitor;
      sev = 0.0;
      sxev = 0.0;
      sxxev = 0.0;
      sxest = 0.0;
      for(nevnt=0;nevnt<nevmax;nevnt++)
       {
        inject();
/*     go read the module ------------------------------- */
       slink_read();
       if(slink_error!=0)
         { nslink_error++;
	   continue;
	 }
/*     display ? */
             if(sw(7) == 1)
   		{ igain = icap;
		  if(icap == 0) sprintf(string,"LOW GAIN TUBES 1-24");
  		  if(icap == 1) sprintf(string,"HIGH GAIN TUBES 1-24");
  		  bigdisp(nsamp,string,1);
  		  if(icap == 0) sprintf(string,"LOW GAIN TUBES 25-48");
  		  if(icap == 1) sprintf(string,"HIGH GAIN TUBES 25-48");
 	 	  bigdisp(nsamp,string,25);
		}
        asum = 0.0;
        axsum = 0.0;
        axxsum = 0.0;
        for(i=0;i<2;i++)
         { aval = (double)fdlow[card][i];
           asum = asum+1.0;
           axsum = axsum + aval;
           axxsum = axxsum + aval*aval;
         }
        kipk = lipk[card];
        ped = axsum / asum;
        hstacc(0,ped,1.0);
        sig2=axxsum/asum - ped*ped;
        if(sig2>0.0) dped=sqrt(sig2);
        hstacc(2,dped,1.0);
/*	printf("fdlow[card][kipk]= %d %d %f\n",card,kipk,fdlow[card][kipk]); */
        if(fdlow[card][kipk]>1000) 
	  { /* printf("exit dac loop\n"); */
	    goto lofit;
	  }
        val = (double)fdlow[card][kpeak] - ped;
        el = (double)fdlow[card][kpeak-1] - ped;
        er = (double)fdlow[card][kpeak+1] - ped;
        ep = val;
        estimator = (el-er)/ep;
        if(kipk>=klo && kipk<=khi && fdlow[card][kpeak]<1000)
         { sev = sev + 1.0;
           sxev = sxev + val;
           sxxev = sxxev + val * val;
           sxest = sxest + estimator;
         }
      } /* end of nevnt loop */
        val=0.0;
        if(sev>0.0) 
          {val = sxev / sev;
           estimator = sxest / sev;
           if(sw(6)==1) printf("  low gain dac=%d charge=%6.2f fmean=%6.2f\n",
           dac,charge,val);
           dotacc(5,charge,val);
           sig=0.0;
           sig2=sxxev/sev - val*val;
           if(sig2>0.0) sig=sqrt(sig2);
           if(val>0.0) dotacc(9,charge,sig);
          }
        if(val>0.0 && val<4090.0)
         { crg[icnt] = charge;
           vlt[icnt] = val;
           if(charge>50.0)
            { s = s +1.0;
              sx = sx + charge;
              sy = sy + val;
              sxy = sxy + charge*val;
              sx2 = sx2 + charge*charge;
	    }
           icnt++; 
	 }
   }  /* end of dac loop */
   icnt2=icnt;
   if(sw(6)==1) printf(" end of dac loop icnt=%d\n",icnt);
   if(sw(6)==1) printf(" end of dac loop icnt1=%d  icnt2=%d\n",icnt1,icnt2);
lofit:
   del = s*sx2 - sx*sx;
   a = (sx2*sy - sx*sxy) / del;
   b = (s*sxy - sx*sy) / del;
   al = a;
   bl = b;
   slopel = bl*factor;
   if(sw(6)==1) printf(" logain fit results a=%7.2f b=%7.2f y=a+b*charge\n",a,b);
   s = 0.0;
   sx = 0.0;
   for(k=0;k<icnt;k++)
    {x = crg[k];
     y = a + b*x;
     dely = y - vlt[k];
     s = s + 1.0;
     sx = sx + fabs(dely);
     sw3 = sw(3);
     if(sw3 == 1) printf("k=%d x=%f  y=%f  vlt[k]=%f  dely=%f\n",k,x,y,vlt[k],dely);
     dotacc(7,x,dely);
    }
    dymaxl = factor * (sx/s);

    ratio = bh/bl;

   kFAIL=0;

   hstacc(11,slopeh,1.0);
   if(slopeh<80.0) kFAIL=kFAIL&0x1;
   if(slopeh>93.0) kFAIL=kFAIL&0x2;

   hstacc(12,dymaxh,1.0);    
   if(dymaxh>3.0) kFAIL=kFAIL&0x4;

   hstacc(13,slopel,1.0);
   if(slopel<1.0) kFAIL=kFAIL&0x8;
   if(slopel>1.5) kFAIL=kFAIL&0x10;

   hstacc(14,dymaxl,1.0);
   if(dymaxl>2.5) kFAIL=kFAIL&0x20;
   
   ratio = slopeh/slopel;
   hstacc(15,ratio,1.0);
   if(ratio<62.0 || ratio>66.0) kFAIL=kFAIL&0x40;
   if(kFAIL != 0) FAIL = kFAIL;
   noisel = factor*ml;
   if(sw(6)==1) printf("card=%d bh=%f  bl=%f  ratio=%f   ---------------- ifail=%d\n\n",
      card,bh,bl,ratio,FAIL);
      
   hstmsg(3,&mh,&sh);
   noiseh = mh;
   if(noiseh>5.0) kFAIL=kFAIL&0x80;
   
   hstmsg(2,&mh,&sh);
   noisel = mh;
   if(noisel>2.5) kFAIL=kFAIL&0x100;
   
   hstmsg(16,&mh,&sh);
   phaseh = mh;
   if(phaseh<50.0 || phaseh>120.0) kFAIL=kFAIL&0x200;
   
   hstmsg(17,&mh,&sh);
   phasel = mh;
   if(phasel<50.0 || phasel>120.0) kFAIL=kFAIL&0x200;
   
  }

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
 int max,mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,ii;
   int idel,kk,ik;
   double y,dy;
   char str[5];
   erasm();
   putchar(27);
   putchar(59);   /* set fontSmall */
   ik=0;
   for(kk=0;kk<24;kk++)
    {
   max = 1024;
   
   mmax=1024;
  
   idx = 240/(nn+1);
   linx = idx*(nn+1);
   ixbas = jx[ik];
   dy = 500.0/(double)mmax;
   iybas = jy[ik];
   mova(ixbas,iybas);
   drwr(linx,0);
   mova(ixbas,iybas);
   iylst = 0;
   for(i=0;i<=nn;i++)
    { if(igain == 0) ii = fdlow[kk+strt][i];
      if(igain == 1) ii = fdhi[kk+strt][i];
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
    idel=1;
    if(nn>100) idel=25;
    if(nn>200) idel=50;
    for(i=0;i<=nn+1;i=i+idel)
     { ix=ixbas+idx*i;
       mova(ix,iybas);
       drwr(0,-30);
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
	     SetInhibitDelay(2,1990);
	     SetInhibitDuration(2,2);
	  /*  BPutLong(2,ttcadr,1,0xc0,0x01); */  /* load set tp_low into fifo2 */
	     BPutShort(2,0xc0);   /* set tp low bcast command  */
	     /* set inhibit3 to set latch                     */
	     /* use latch-bar and inhibit3 to start fermi     */
             /* will use inhibit 3 to set l1a with digitizers */
             i3time=2047; 
	     /* printf("input i3time\n");
             fscanf(stdin,"%d",&i3time);  */   
	     SetBMode(3,0xa);
	     SetInhibitDelay(3,i3time);
	     SetInhibitDuration(3,2);
             reset();  
     	
	    }
	     
     inject()
       { static int entry=0,cnt;
         int c1,c2,f1,f2,swtst,i,i1,i2,i0time,i3time;
	 double a,b,c;
           reset();  /* clear last event & ltch bit */
           /* usleep(10);  */
	   cnt=0;
	   a = sin(0.21);  
	   b = sin(0.31);  
	   b = sin(0.41);  
           start();  /* send pulse to start event sequence */
	   a = sin(0.51);  
loop:      latch();  /* wait for the latch bit to set */
           if(vreg == 0) 
             { a = sin(0.003); 
	       cnt++;
               if(cnt<1000000) goto loop;
	       printf(" wait loop trouble\n");
               exit(0);
             }
   }

slink_setup()
        {
	SLINK_InitParameters(&slink_para);
	slink_para.position = 0;
	slink_para.start_word = 0x11111110;
	slink_para.stop_word = 0x3fff3ff0;
	if ((code = SSP_Open(&slink_para, &dev)) != SLINK_OK)
	{
	printf(" slink open error, error code = %d\n",code);
	SLINK_PrintErrorMessage(code);
	exit(0);
	}
	printf("slink opened\n");
	return; 
	}

clear_buffer()
    { unsigned long int word;
      int type_of_word;
      int data_count = 0;
      while (code = SSP_ReadOneWord(dev, &word, &type_of_word) == SLINK_OK)
       { if (type_of_word != SLINK_CONTROL_WORD) data_count++;
	 if (type_of_word == SLINK_CONTROL_WORD) 
	   { if(word == 0x11111110) data_count++;
	     if(word == 0x5fff5ff0) return;
	   }	
        }
     }

dump_buffer()
    { unsigned long int word;
      int type_of_word;
      int data_count = 0;
      while (code = SSP_ReadOneWord(dev, &word, &type_of_word) == SLINK_OK)
       { printf(" dump count=%d  word=%x\n",data_count,word);
         if (type_of_word != SLINK_CONTROL_WORD) data_count++;
	 if (type_of_word == SLINK_CONTROL_WORD) 
	   { if(word == 0x11111110) data_count++;
	     if(word == 0x5fff5ff0) return;
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
	slink_error=0;
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
	if(sw(8) == 1) printf("%d block=%d logain header word %x   bunch %d  samples %d  mode %d  %x %x\n"
	,count,block,word,bunch,samples,mode,crca,crcb);
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
err:    if(sw(8)==1) printf(" error return eflg=%d code=%x   word=%x\n",eflg,code,word);
        slink_error=eflg;
	return;
       }
	 
digitizer_setup(int i)
      { int mode, adrttc, dac1, dac2, ick, pipe;
        adrttc = digadr[i];
	mode = dm[i];
	dac1 = ddac1[i];
	dac2 = ddac2[i];
        ick = iclksm[i];
	pipe=pipeline[i];
	
        SendLongCommand(adrttc,0,0x6,0);  /* reset v3.0 ttcrx chips*/ 
	ksleep(2);
        SendLongCommand(adrttc,0,0x3,0xa9);  /* setup ttcrx chip bnch cntr */
	ksleep(0);  
	
/* set up digitizer registers */
        SendLongCommand(adrttc,1,0xa,ick); /* clock invert bits etc */
	ksleep(2);  
	SendLongCommand(adrttc,1,0xf,mode);  /* set mode NORMAL=0 */	
	ksleep(2);  
	SendLongCommand(adrttc,1,0x8,dac1);  /* set dac1 */	
	ksleep(2);  
	SendLongCommand(adrttc,1,0x9,dac2);  /* set dac2 */	
	ksleep(2);  
        SendLongCommand(adrttc,1,0xb,0xff);  /* high limit LSB=0xff */    
	ksleep(2);  
	SendLongCommand(adrttc,1,0xc,3);     /* high limit MSB=3 */
	ksleep(2);  
        SendLongCommand(adrttc,1,0xd,0);     /* low limit LSB=0 */
	ksleep(2);  
	SendLongCommand(adrttc,1,0xe,0);     /* low limit MSB=0 */
	ksleep(2);  
        SendLongCommand(adrttc,1,1,nsamp); /* number of samples */
	ksleep(2);  
	SendLongCommand(adrttc,1,0x2,pipe); /* set pipeline length */
	ksleep(2);  
        BroadcastShortCommand(0x40);       
	ksleep(2);  
	SendLongCommand(adrttc,1,0x2,pipe); /* set pipeline length */
	ksleep(2);  
        BroadcastShortCommand(0x40);       
	ksleep(2);  

/* set up digitizer registers again */
/* set up digitizer registers */
        SendLongCommand(adrttc,1,0xa,ick); /* clock invert bits etc */
	ksleep(2);  
	SendLongCommand(adrttc,1,0xf,mode);  /* set mode NORMAL=0 */	
	ksleep(2);  
	SendLongCommand(adrttc,1,0x8,dac1);  /* set dac1 */	
	ksleep(2);  
	SendLongCommand(adrttc,1,0x9,dac2);  /* set dac2 */	
	ksleep(2);  
        SendLongCommand(adrttc,1,0xb,0xff);  /* high limit LSB=0xff */    
	ksleep(2);  
	SendLongCommand(adrttc,1,0xc,3);     /* high limit MSB=3 */
	ksleep(2);  
        SendLongCommand(adrttc,1,0xd,0);     /* low limit LSB=0 */
	ksleep(2);  
	SendLongCommand(adrttc,1,0xe,0);     /* low limit MSB=0 */
	ksleep(2);  
        SendLongCommand(adrttc,1,1,nsamp); /* number of samples */
	ksleep(2);  
	SendLongCommand(adrttc,1,0x2,pipe); /* set pipeline length */
	ksleep(2);  
        BroadcastShortCommand(0x40);       
	SendLongCommand(adrttc,1,0x2,pipe); /* set pipeline length */
	ksleep(2);  
        BroadcastShortCommand(0x40);       
	ksleep(2);  

        SetTimeDes1(adrttc,deskew1[i]);
        SetTimeDes2(adrttc,deskew2[i]);
        CoarseDelay(adrttc,0,0);
	 
	return;
	}

     crc(int ent,unsigned long word)
        { static unsigned int D0[16],D1[16],C0[16],C1[16],NewCRC[16];
	  int i;
	  unsigned long inwd0,inwd1;
	  /* ==================================================
	     ent=0 first entry (header word) initialize
	           then do crc calculation
	     ent=1 normal data - just update crc calculation
          ================================================== */
	  

/* unpack the data word back into the serial streams D0 and in1 sent
   by the digitizer  D0(0) and D1(0) were the first bits recieved */
        
/* sort the 32 bit word back into the 2 serial streams */
      if(ent == 0) 
         { for(i=0;i<16;i++)
	    { C0[i] = 0;
	      C1[i] = 0;
	    }
	 }	  
	  D0[0] = (word>>30) & 1;
	  D0[1] = (word>>28) & 1;
	  D0[2] = (word>>26) & 1;
	  D0[3] = (word>>24) & 1;
	  D0[4] = (word>>22) & 1;
	  D0[5] = (word>>20) & 1;
	  D0[6] = (word>>18) & 1;
	  D0[7] = (word>>16) & 1;
	  D0[8] = (word>>14) & 1;
	  D0[9] = (word>>12) & 1;
	  D0[10] = (word>>10) & 1;
	  D0[11] = (word>>8) & 1;
	  D0[12] = (word>>6) & 1;
	  D0[13] = (word>>4) & 1;
	  D0[14] = (word>>2) & 1;
	  D0[15] = (word>>0) & 1;

	  D1[0] = (word>>31) & 1;
	  D1[1] = (word>>29) & 1;
	  D1[2] = (word>>27) & 1;
	  D1[3] = (word>>25) & 1;
	  D1[4] = (word>>23) & 1;
	  D1[5] = (word>>21) & 1;
	  D1[6] = (word>>19) & 1;
	  D1[7] = (word>>17) & 1;
	  D1[8] = (word>>15) & 1;
	  D1[9] = (word>>13) & 1;
	  D1[10] = (word>>11) & 1;
	  D1[11] = (word>>9) & 1;
	  D1[12] = (word>>7) & 1;
	  D1[13] = (word>>5) & 1;
	  D1[14] = (word>>3) & 1;
	  D1[15] = (word>>1) & 1; 
	  
 /*   printf(" C1= %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
       C1[0],C1[1],C1[2],C1[3],C1[4],C1[5],C1[6],C1[7],
       C1[8],C1[9],C1[10],C1[11],C1[12],C1[13],C1[14],C1[15]);

    printf(" D1= %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
       D1[0],D1[1],D1[2],D1[3],D1[4],D1[5],D1[6],D1[7],
       D1[8],D1[9],D1[10],D1[11],D1[12],D1[13],D1[14],D1[15]);  */

/*	  inwd0 = 0;
	  for(i=0;i<16;i++)
	    {inwd0 = inwd0 | (D0[i]<<i);
	    }
	  inwd1 = 0; 
	  for(i=0;i<16;i++)
	    {inwd1 = inwd1 | (D1[i]<<i);
	    }
	  printf(" word=%x  inwd0=%x  inwd1=%x\n",word,inwd0,inwd1); */
	  
    NewCRC[0] = D0[15] ^ D0[13] ^ D0[12] ^ D0[11] ^ D0[10] ^ D0[9] ^ 
                 D0[8] ^ D0[7] ^ D0[6] ^ D0[5] ^ D0[4] ^ D0[3] ^ 
                 D0[2] ^ D0[1] ^ D0[0] ^ C0[0] ^ C0[1] ^ C0[2] ^ 
                 C0[3] ^ C0[4] ^ C0[5] ^ C0[6] ^ C0[7] ^ C0[8] ^ 
                 C0[9] ^ C0[10] ^ C0[11] ^ C0[12] ^ C0[13] ^ C0[15];
    NewCRC[1] = D0[14] ^ D0[13] ^ D0[12] ^ D0[11] ^ D0[10] ^ D0[9] ^ 
                 D0[8] ^ D0[7] ^ D0[6] ^ D0[5] ^ D0[4] ^ D0[3] ^ 
                 D0[2] ^ D0[1] ^ C0[1] ^ C0[2] ^ C0[3] ^ C0[4] ^ 
                 C0[5] ^ C0[6] ^ C0[7] ^ C0[8] ^ C0[9] ^ C0[10] ^ 
                 C0[11] ^ C0[12] ^ C0[13] ^ C0[14];
    NewCRC[2] = D0[14] ^ D0[1] ^ D0[0] ^ C0[0] ^ C0[1] ^ C0[14];
    NewCRC[3] = D0[15] ^ D0[2] ^ D0[1] ^ C0[1] ^ C0[2] ^ C0[15];
    NewCRC[4] = D0[3] ^ D0[2] ^ C0[2] ^ C0[3];
    NewCRC[5] = D0[4] ^ D0[3] ^ C0[3] ^ C0[4];
    NewCRC[6] = D0[5] ^ D0[4] ^ C0[4] ^ C0[5];
    NewCRC[7] = D0[6] ^ D0[5] ^ C0[5] ^ C0[6];
    NewCRC[8] = D0[7] ^ D0[6] ^ C0[6] ^ C0[7];
    NewCRC[9] = D0[8] ^ D0[7] ^ C0[7] ^ C0[8];
    NewCRC[10] = D0[9] ^ D0[8] ^ C0[8] ^ C0[9];
    NewCRC[11] = D0[10] ^ D0[9] ^ C0[9] ^ C0[10];
    NewCRC[12] = D0[11] ^ D0[10] ^ C0[10] ^ C0[11];
    NewCRC[13] = D0[12] ^ D0[11] ^ C0[11] ^ C0[12];
    NewCRC[14] = D0[13] ^ D0[12] ^ C0[12] ^ C0[13];
    NewCRC[15] = D0[15] ^ D0[14] ^ D0[12] ^ D0[11] ^ D0[10] ^ D0[9] ^ 
                  D0[8] ^ D0[7] ^ D0[6] ^ D0[5] ^ D0[4] ^ D0[3] ^ 
                  D0[2] ^ D0[1] ^ D0[0] ^ C0[0] ^ C0[1] ^ C0[2] ^ 
                  C0[3] ^ C0[4] ^ C0[5] ^ C0[6] ^ C0[7] ^ C0[8] ^ 
                  C0[9] ^ C0[10] ^ C0[11] ^ C0[12] ^ C0[14] ^ C0[15];

    crca = 0;
    for(i=0;i<16;i++) 
      { C0[i] = NewCRC[i];
        crca = crca | (NewCRC[i]<<i);
      }
      
    NewCRC[0] = D1[15] ^ D1[13] ^ D1[12] ^ D1[11] ^ D1[10] ^ D1[9] ^ 
                 D1[8] ^ D1[7] ^ D1[6] ^ D1[5] ^ D1[4] ^ D1[3] ^ 
                 D1[2] ^ D1[1] ^ D1[0] ^ C1[0] ^ C1[1] ^ C1[2] ^ 
                 C1[3] ^ C1[4] ^ C1[5] ^ C1[6] ^ C1[7] ^ C1[8] ^ 
                 C1[9] ^ C1[10] ^ C1[11] ^ C1[12] ^ C1[13] ^ C1[15];
    NewCRC[1] = D1[14] ^ D1[13] ^ D1[12] ^ D1[11] ^ D1[10] ^ D1[9] ^ 
                 D1[8] ^ D1[7] ^ D1[6] ^ D1[5] ^ D1[4] ^ D1[3] ^ 
                 D1[2] ^ D1[1] ^ C1[1] ^ C1[2] ^ C1[3] ^ C1[4] ^ 
                 C1[5] ^ C1[6] ^ C1[7] ^ C1[8] ^ C1[9] ^ C1[10] ^ 
                 C1[11] ^ C1[12] ^ C1[13] ^ C1[14];
    NewCRC[2] = D1[14] ^ D1[1] ^ D1[0] ^ C1[0] ^ C1[1] ^ C1[14];
    NewCRC[3] = D1[15] ^ D1[2] ^ D1[1] ^ C1[1] ^ C1[2] ^ C1[15];
    NewCRC[4] = D1[3] ^ D1[2] ^ C1[2] ^ C1[3];
    NewCRC[5] = D1[4] ^ D1[3] ^ C1[3] ^ C1[4];
    NewCRC[6] = D1[5] ^ D1[4] ^ C1[4] ^ C1[5];
    NewCRC[7] = D1[6] ^ D1[5] ^ C1[5] ^ C1[6];
    NewCRC[8] = D1[7] ^ D1[6] ^ C1[6] ^ C1[7];
    NewCRC[9] = D1[8] ^ D1[7] ^ C1[7] ^ C1[8];
    NewCRC[10] = D1[9] ^ D1[8] ^ C1[8] ^ C1[9];
    NewCRC[11] = D1[10] ^ D1[9] ^ C1[9] ^ C1[10];
    NewCRC[12] = D1[11] ^ D1[10] ^ C1[10] ^ C1[11];
    NewCRC[13] = D1[12] ^ D1[11] ^ C1[11] ^ C1[12];
    NewCRC[14] = D1[13] ^ D1[12] ^ C1[12] ^ C1[13];
    NewCRC[15] = D1[15] ^ D1[14] ^ D1[12] ^ D1[11] ^ D1[10] ^ D1[9] ^ 
                  D1[8] ^ D1[7] ^ D1[6] ^ D1[5] ^ D1[4] ^ D1[3] ^ 
                  D1[2] ^ D1[1] ^ D1[0] ^ C1[0] ^ C1[1] ^ C1[2] ^ 
                  C1[3] ^ C1[4] ^ C1[5] ^ C1[6] ^ C1[7] ^ C1[8] ^ 
                  C1[9] ^ C1[10] ^ C1[11] ^ C1[12] ^ C1[14] ^ C1[15];

    crcb = 0;
    for(i=0;i<16;i++) 
      { C1[i] = NewCRC[i];
        crcb = crcb | (NewCRC[i]<<i);
	/* printf("i=%d  NewCRC=%x  crc1=%x\n",i,NewCRC[i],crc1); */
      }
/*    printf("crca=%x  crcb=%x\n",crca,crcb);  */
  }
    unscramble(unsigned long word)
        { int in0[16],in1[16];
	  unsigned long out0,out1;
	  int i;
/*  unscramble the crc word back into the 2 serial streams */
 	  in0[15] = (word>>30) & 1;
	  in0[14] = (word>>28) & 1;
	  in0[13] = (word>>26) & 1;
	  in0[12] = (word>>24) & 1;
	  in0[11] = (word>>22) & 1;
	  in0[10] = (word>>20) & 1;
	  in0[9] = (word>>18) & 1;
	  in0[8] = (word>>16) & 1;
	  in0[7] = (word>>14) & 1;
	  in0[6] = (word>>12) & 1;
	  in0[5] = (word>>10) & 1;
	  in0[4] = (word>>8) & 1;
	  in0[3] = (word>>6) & 1;
	  in0[2] = (word>>4) & 1;
	  in0[1] = (word>>2) & 1;
	  in0[0] = (word>>0) & 1;

	  in1[15] = (word>>31) & 1;
	  in1[14] = (word>>29) & 1;
	  in1[13] = (word>>27) & 1;
	  in1[12] = (word>>25) & 1;
	  in1[11] = (word>>23) & 1;
	  in1[10] = (word>>21) & 1;
	  in1[9] = (word>>19) & 1;
	  in1[8] = (word>>17) & 1;
	  in1[7] = (word>>15) & 1;
	  in1[6] = (word>>13) & 1;
	  in1[5] = (word>>11) & 1;
	  in1[4] = (word>>9) & 1;
	  in1[3] = (word>>7) & 1;
	  in1[2] = (word>>5) & 1;
	  in1[1] = (word>>3) & 1;
	  in1[0] = (word>>1) & 1; 

/* printf("in0 = %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
 in0[0], in0[1], in0[2], in0[3], in0[4], in0[5], in0[6], in0[7],
 in0[8], in0[9],in0[10],in0[11],in0[12],in0[13],in0[14],in0[15]); 
printf("in1 = %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
 in1[0], in1[1], in1[2], in1[3], in1[4], in1[5], in1[6], in1[7],
 in1[8], in1[9],in1[10],in1[11],in1[12],in1[13],in1[14],in1[15]); */
/* now pack the 2 streams back into 16 bit words */

          out0 = 0;
	  out1 = 0;
	  for(i=0;i<16;i++)
	     { out0 = out0 | (in0[i]<<i);
	       out1 = out1 | (in1[i]<<i);
	     }
	  printf("unscrambled TILEdmu crc= %x %x\n",out0,out1);
  }
ksleep(int i)
  { double s,a;
    a = (double)i * 0.123;
    s = sin(a);
    return(s);
  }

    double hstfit(i,l,x)
    int i,l;
    double x;
    {double y;
     y=0.0;
     return(y); 
    } 
   

reset()
  { reg = (unsigned short *)0xdede0006;
    *reg = 0;
  }

start()
  { reg = (unsigned short *)0xdede0004;
    *reg = 0;
  }

latch()
  { reg = (unsigned short *)0xdede0002;
    vreg = *reg;
  }
  
