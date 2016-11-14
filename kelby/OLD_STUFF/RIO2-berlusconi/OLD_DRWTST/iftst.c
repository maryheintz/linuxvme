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
time_t usec = 1;
int fdata[49][50],ipk[49],pk[49]; 
int ttcadr,id;
int digadr[9] = {0,0x100e,0x100f,0x176e,0x10bb,
      0x1011,0x1014,0x1015,0x1017}; 
int dm[9] = {0,0,0,0,0,0,0,0,0};
int ddac1[9]={150,150,150,150,150,150,150,150,150};
int ddac2[9] ={150,150,150,150,150,150,150,150,150};

unsigned int deskew1[9] = {0,0,0,0,0,10,0,0,0};
unsigned int deskew2[9] = {100,100,130,100,118,82,13,56,40};
unsigned int iclksm[9] = {0,0,1,0,1,0,0,0,0};

/*unsigned int deskew1[9] = {112,112,112,112,112,112,112,112,112};
unsigned int deskew2[9] = {66,66,66,66,66,66,66,66,66};
unsigned int iclksm[9] = {0,0,0,0,0,0,0,0,0};  */

unsigned int crca,crcb;
int dbad[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int code;
static int nsamp;
int event=0,nerror=0,ndcrc=0,nbnch=0,ngcrc=0,nctrl=0,nmem,nstr;
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
static int icap;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { int tim,i,l,k,kkk,lll,pipe,drawer,dig,kk,tube;
    int dac,alarm,tm1,tm2,tm3,tm4;
    int good,bad,total,board;
    double charge,dacval,capacitor;
    char xstring[80],ystring[80],string[80],err[80];
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    FILE *fpcfg;

    fpcfg = fopen("/home/user/kelby/DRWTST/cfg.dat","r");
    fscanf(fpcfg,"%x %s",&ttcadr,string);
    printf("ttcadr=%x  ADC=%s\n",ttcadr,string);
    fflush(stdout);

    tube=0;
    kk = 0;
 
   nsamp = 6;
   icap = 0;
   printf("input icap = 0-logain  1-higain\n ");
   fscanf(stdin,"%d",&icap);
   board = 1;
   id = (board<<6) | 1;
   ttcvi_map(ttcadr);
   
   SendLongCommand(ttcadr,0,0x6,0);   
   sleep(1);  /* wait for reset to complete */
   SendLongCommand(ttcadr,0,0x3,0xa9);   /*  set the v3.0 TTCrx control register */
   usleep(1);

   reset_ctrl_can();
   printf("calling can_reset -- wait\n");
   can_reset(ttcadr);  
   printf("calling can_init\n");  
   can_init(board,string);  
   printf("returned from can_init\n");  
   good=0;
   bad=0;
   for(k=1;k<49;k++) 
     {  kkk=cardchk(k);
        if(kkk == 1) 
	 { good++;
           printf(" card %d found\n",k);
	 }
	if(kkk != 1) 
	 { bad++; 
           printf("no card %d\n",k);
	 }
     }
   total=good+bad;
   printf(" good=%d    bad=%d     %d\n",good,bad,total);
    tm1=105;
    tm2=64;
    tm3=62;
    tm4=20;
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
    SetTimeDes2(ttcadr,126);
    CoarseDelay(ttcadr,0,0); 

    multi_low(ttcadr);
    rxw_low(ttcadr);

    for(k=1;k<49;k++)
      { set_tube(ttcadr,k);
        intg_rd_low(ttcadr);
        itr_low(ttcadr);
        set_intg_gain(ttcadr,0);  /* s1=s2=s3=s4=0 */
        trig_enable(ttcadr);
        large_cap_enable(ttcadr);
        small_cap_enable(ttcadr);
        mse_high(ttcadr);         /* mse=1 */
      }
    multi_high(ttcadr);  /* now set_dac calls effect all tubes */
    if(icap == 0)
    { charge = 600.0;
      capacitor = 100.0;
      large_cap_enable(ttcadr);
      small_cap_disable(ttcadr);
    }
    if(icap == 1)
    { 
      charge = 12.0;
      charge = 8.0;
      capacitor = 5.2;
      large_cap_disable(ttcadr);
      small_cap_enable(ttcadr);
    }
    dacval = (charge * 1023.0 ) / (2.0 * 4.096* capacitor);
    if(dacval > 1023.0) dacval = 1023.0;
    dac = (int) dacval;
    charge = ( 2.0 * 4.096 * capacitor *(double)dac ) / 1023.0;
    printf(" charge=%f  dacval=%f  dac=%x(hex)  cap=%f\n",
           charge,dacval,dac,capacitor);
    set_dac(ttcadr,dac);
    
    printf(" calling inject_setup\n");
    inject_setup();

    printf(" calling slink_setup\n");
    slink_setup();
           
    printf(" Setting up Digitizers\n");
    for(dig=0;dig<9;dig++) digitizer_setup(dig);	

/*    printf(" sending reset to interface\n"); */
	 SendLongCommand(0,1,0xf,8);  /* send reset to the interface */
	 if(nsamp < 7) SendLongCommand(0,1,0xf,4);  /* set 16 buffers in interface */

	usleep(0);  
	sidev(1);  /* force plot to screen without asking */
    for(kk=0;kk<5;kk++)
      { /* printf(" sending inject\n"); */
        inject();
        usleep(1);
	/* printf(" sending clear buffer\n"); */
        clear_buffer();  
	/* printf(" returned from clear buffer\n");  */
      }
       alarm=1000;
       if(sw(2) == 1) alarm = 2000;
    printf(" starting event loop\n");
    BroadcastShortCommand(3);       /* BunchCrossing and 
                                       event counter reset */
    usleep(1000);
 loop:
       event++;
       if(sw(8) == 1 && sw(6) == 1 && event>alarm) printf("\a\a\a");
       if((event%50) == 0) 
             printf("event=%d  nerror=%d  ndcrc=%d  nbnch=%d  nctrl=%d\n",
               event,nerror,ndcrc,nbnch,nctrl);
/*       BroadcastShortCommand(3);  */     /* BunchCrossing and 
                                       event counter reset */

/*       clear_buffer();  */
       usleep(1);
       inject();
       usleep(1);
       slink_read();
               if(sw(8) == 1)
	         {usleep(100);
		  if(sw(8) == 0) printf("next event\n");
		  goto loop;}
              if(sw(7) == 0)
   		{ if(icap == 0) sprintf(string,"LOW GAIN TUBES 1-24");
  		  if(icap == 1) sprintf(string,"HIGH GAIN TUBES 1-24");
  		  bigdisp(nsamp+1,string,1);
  		  if(icap == 0) sprintf(string,"LOW GAIN TUBES 25-48");
  		  if(icap == 1) sprintf(string,"HIGH GAIN TUBES 25-48");
 	 	  bigdisp(nsamp+1,string,25);
		}
              if(sw(1) == 1)
	        { if(tube == 0)
		    { printf(" input tube number\n");
		      fscanf(stdin,"%d",&tube);
		    }
                  asum = 0.0;
                  axsum = 0.0;
                  axxsum = 0.0;
                  for(i=1;i<10;i++)
                   { aval = (double)fdata[tube][i];
                     asum = asum+1.0;
                     axsum = axsum + aval;
                     axxsum = axxsum + aval*aval;
                   }
                 ped = axsum / asum;
                 dped = 0.0;
                 sig2=axxsum/asum - ped*ped;
                 if(sig2>0.0) dped=sqrt(sig2);
		  if(icap == 0) sprintf(string,"LOW GAIN TUBES %d ped=%f",
		    tube,ped);
  		  if(icap == 1) sprintf(string,"HIGH GAIN TUBES %d ped=%f",
		    tube,ped);
  		  evdisp(14,string,tube);
                }

          usleep(1);
          goto loop;
   }  /* end of main */
   
evdisp(nn,string,kk)  /* nn=bins to display  kk=card number */
 int nn,kk;
 char *string;
 { int max,mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,k,ii,iii;
   int idel;
   double dely,y,dy;
   char str[5];
   erasm();
   max = 0;
   for(iii=1; iii<=nn; iii++) {
   if (max < fdata[kk][iii]) max = fdata[kk][iii];
   };
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
   for(i=1;i<=nn;i++)
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
    for(i=1;i<nn+2;i=i+idel)
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
             i3time=2049; 
	     /* printf("input i3time\n");
             fscanf(stdin,"%d",&i3time);  */   
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
	 double a;
         
	   a = sin(0.21);  
loop:      stvme();  /* send pulses from the switch box to clear last event */
           swtst = latch();  /* wait for the latch bit to set */
           if(swtst == 0) 
             { a = sin(0.003); 
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
   if(sw(3)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
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
   if(sw(3)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
  if(ok == 2) status=1;
   return(status);
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
 int max,mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,k,ii,iii;
   int idel,kk,ik;
   double dely,y,dy;
   char str[5];
   erasm();
   putchar(27);
   putchar(59);   /* set fontSmall */
   ik=0;
   for(kk=0;kk<24;kk++)
    {
   max = 1024;
   
   mmax=1024;
  
   idx = 250/nn;
   linx = idx*nn;
   ixbas = jx[ik];
   dy = 500.0/(double)mmax;
   iybas = jy[ik];
   mova(ixbas,iybas);
   drwr(linx,0);
   mova(ixbas,iybas);
   iylst = 0;
   for(i=1;i<=nn;i++)
    { ii = fdata[kk+strt][i];
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

slink_setup()
        {
	SLINK_InitParameters(&slink_para);
	slink_para.position = 0;
	slink_para.start_word = 0x51115110;
	slink_para.stop_word = 0xfffffff0;
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
	   { if(word == 0x51115110) data_count++;
	     if(word == 0xfffffff0) return;
	   }	
        }
     }
slink_read()
        {
	unsigned long word,wordlast,crc1,crc2,h1,h2;
	int parity,samples,error,sestr,destr,r_parity,v_parity,
	   mode,gain,bunch,cc,kklast;
	int type_of_word,count,kk,ii,stop,hcnt,crcaa,b1;
	int bnchok,gcrcok,dcrcok,ctrlok,bnch[16];
        slink_formatted_data sd;
	cc = 0;
	count =0;
	kklast = 0;
	hcnt=0;
	bnchok=0;
	gcrcok=0;
	dcrcok=0;
	ctrlok=0;
	nmem=0;
	nstr=0;
	while (code = SSP_ReadOneWord(dev, &word, &type_of_word) == SLINK_OK)
	{if (type_of_word != SLINK_CONTROL_WORD) 
	 	{ kk = count/(nsamp+3);
	          ii = count-(nsamp+3)*kk;
		  count++;
		  if(count>500) return(1);
		  sd.ldata =  word;
		  wordlast = word;
		  if(kklast == 15 && ii == 0) 
		   { if(sw(8) == 0) printf(" dig crc %x\n",word);
		     if(word != 0xffffffff) 
		      { dcrcok++;
		        printf(" dig crc ERROR %x\n",word);
	              }
		   }
	         else if(kklast == 15 && ii == 1) 
		   { if(sw(8) == 0) printf(" final crc %x\n",word);
		     h1 = word & 0xffff;
		     h2 = (word>>16) & 0xffff;
		     if(h1 != h2) printf(" final crc ERROR %x\n",word);
		   }
		 else if(ii == 0) 
		   { parity = (word>>30) & 1;
		     samples = (word>>26) & 0xf;
		     error = (word>>25) & 1;
		     sestr = (word>>24) & 1;
		     destr = (word>>23) & 1;
		     r_parity = (word>>22) & 1;
		     v_parity = (word>>18) & 0xf;
		     mode = (word>>15) & 3;
		     gain = (word>>12) & 0x7;
		     bunch = word & 0xfff;
		     if(error != 0) nmem++;
		     if(sestr != 0) nstr++;
		     if(destr != 0) nstr++;
		     if(count == 1) b1 = bunch;
		     if(count!=1 && bunch!=b1) 
		        { printf("bunch number error  block=%d  count=%d  bunch=%d  b1=%d  error=%d  sestr=%d  destr=%d\n",
		                    hcnt,count,bunch,b1,error,sestr,destr);
		          bnchok++;
					}
		     crc(0,word);
		     hcnt++;
		     bnch[hcnt-1] = bunch;
		     if(hcnt>16)
			{printf("hcnt error count=%d\n",count);
                         slink_setup();
			 return;
			}
		     if(sw(8) == 0) printf("%d header word %x  bunch %d  %d  error=%d  sestr=%d  destr=%d\n",
				hcnt,word,bunch,count,error,sestr,destr);
		   }
		 else if(ii>nsamp+1) 
		   { crc1 = (word>>16);
		     crc2 = word & 0xffff;
		     if(crcaa != crc1 || crcb != crc2) dbad[hcnt-1]++;
		     if(sw(8) == 0)  
		      { printf("      tiledmu crc %4x %4x",crc1,crc2);
		        if(crcaa == crc1 && crcb == crc2) printf("  ok\n");
		        if(crcaa != crc1 || crcb != crc2) printf("  bad\n");
			
		      }
		     crc(1,word);
		   }
	         else
		   { crc(1,word);
		     crcaa = crca & 0x7fff;
                     if(sw(8) == 0) printf(" data word= %4i %4i %4x %4x %4x    %8x  %4x %4x\n",
	                 sd.bits.topbit,sd.bits.parity,
			 sd.bits.data1,sd.bits.data2,
			 sd.bits.data3,word,crcaa,crcb);
		    kklast = kk;
		   } 
		 if(kk<8)
		   { fdata[3*kk+1][ii] = sd.bits.data3;
		     fdata[3*kk+2][ii] = sd.bits.data2;
		     fdata[3*kk+3][ii] = sd.bits.data1;
		   }
		 else
		   { fdata[3*kk+1][ii] = sd.bits.data1;
		     fdata[3*kk+2][ii] = sd.bits.data2;
		     fdata[3*kk+3][ii] = sd.bits.data3;
		   }
				 
		}
	 else 
	       { cc++;
	         if(sw(8) == 0) printf("control word= %x\n", word);
		 if(cc == 1 && word != 0x51115110) 
		   { ctrlok++;
		     printf("control word= %x  cc %d  error %d  event %d\n",
		      word,cc,nerror,event);
		     if(sw(5) == 1) fscanf(stdin,"%d",&stop); 
		   } 
		 if(cc == 2 && word != 0xfffffff0) 
		   { ctrlok++;
		     printf("control word= %x  cc %d  error %d  event %d\n",
		      word,cc,nerror,event);
		     /* sleep(10);  */
		     if(sw(5) == 1) fscanf(stdin,"%d",&stop);
		     return; 
		   }
		 if(word == 0xfffffff0)
		   {crc1 = wordlast & 0xffff;
		    crc2 = (wordlast>>16) & 0xffff;
		    if(crc1 != crc2) 
		       { gcrcok++;
		         printf("crc error wordlast=%x  crc1=%x  crc2=%x\n",
		            wordlast,crc1,crc2);
			 if(sw(5) == 1) fscanf(stdin,"%d",&stop); 
		       }
erxit:
		    if(bnchok != 0 || gcrcok != 0 || dcrcok != 0 || ctrlok != 0) 
		      { nerror++;
			      } 
                    if(bnchok != 0) printf("bnch= %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
			bnch[0],bnch[1],bnch[2],bnch[3],bnch[4],bnch[5],bnch[6],bnch[7],
			bnch[8],bnch[9],bnch[10],bnch[11],bnch[12],bnch[13],bnch[14],bnch[15]);
                    if(dcrcok != 0) printf(" dbad= %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                        dbad[0],dbad[1],dbad[2],dbad[3],dbad[4],dbad[5],dbad[6],dbad[7],
                        dbad[8],dbad[8],dbad[10],dbad[11],dbad[12],dbad[13],dbad[14],dbad[15]);
	            if(bnchok != 0) nbnch++;
	            else if(gcrcok != 0) ngcrc++;
	            else if(dcrcok != 0) ndcrc++;
	            else if(ctrlok != 0) nctrl++;
		    if(nmem!=0 || nstr!=0) printf(" error nmem=%d  nstr=%d\n",nmem,nstr);
		    return;
	           }
               }
       }
	printf("no data   count=%d\n",count);
	return;
       }
	 
send_l1a()
	{
	 static int entry=0;
         int c1,c2,f1,f2,swtst,i,i1,i2,i0time,i3time;
	 double a;
         
	   a = sin(0.21);  
loop:      stvme();  /* send pulses from the switch box to clear last event */
           swtst = latch();  /* wait for the latch bit to set */
           if(swtst == 0) 
             { a = sin(0.003); 
               goto loop;
             }
           restim();  /* reset the timer for next trigger */
           rlatch();    /* reset the latch bit */
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
	usleep(0);  
	SendLongCommand(adrttc,1,0xf,mode);  /* set mode NORMAL=0 */	
	usleep(0);  
	SendLongCommand(adrttc,1,0x8,dac1);  /* set dac1 */	
	usleep(0);  
	SendLongCommand(adrttc,1,0x9,dac2);  /* set dac2 */	
	usleep(0);  
        SendLongCommand(adrttc,1,0xb,0xff);  /* high limit LSB=0xff */    
	usleep(0);  
	SendLongCommand(adrttc,1,0xc,3);     /* high limit MSB=3 */
	usleep(0);  
        SendLongCommand(adrttc,1,0xd,0);     /* low limit LSB=0 */
	usleep(0);  
	SendLongCommand(adrttc,1,0xe,0);     /* low limit MSB=0 */
	usleep(0);  
        SendLongCommand(adrttc,1,1,nsamp); /* number of samples */
	usleep(0);  
	SendLongCommand(adrttc,1,0x2,pipe); /* set pipeline length */
	usleep(1);  
        BroadcastShortCommand(0x40);       
	SendLongCommand(adrttc,1,0x2,pipe); /* set pipeline length */
	usleep(1);  
        BroadcastShortCommand(0x40);       

/* set up digitizer registers again */
/* set up digitizer registers */
        SendLongCommand(adrttc,1,0xa,ick); /* clock invert bits etc */
	usleep(0);  
	SendLongCommand(adrttc,1,0xf,mode);  /* set mode NORMAL=0 */	
	usleep(0);  
	SendLongCommand(adrttc,1,0x8,dac1);  /* set dac1 */	
	usleep(0);  
	SendLongCommand(adrttc,1,0x9,dac2);  /* set dac2 */	
	usleep(0);  
        SendLongCommand(adrttc,1,0xb,0xff);  /* high limit LSB=0xff */    
	usleep(0);  
	SendLongCommand(adrttc,1,0xc,3);     /* high limit MSB=3 */
	usleep(0);  
        SendLongCommand(adrttc,1,0xd,0);     /* low limit LSB=0 */
	usleep(0);  
	SendLongCommand(adrttc,1,0xe,0);     /* low limit MSB=0 */
	usleep(0);  
        SendLongCommand(adrttc,1,1,nsamp); /* number of samples */
	usleep(0);  
	SendLongCommand(adrttc,1,0x2,pipe); /* set pipeline length */
	usleep(1);  
        BroadcastShortCommand(0x40);       
	SendLongCommand(adrttc,1,0x2,pipe); /* set pipeline length */
	usleep(1);  
        BroadcastShortCommand(0x40);       


        SetTimeDes1(adrttc,deskew1[i]);
        SetTimeDes2(adrttc,deskew2[i]);
        CoarseDelay(adrttc,0,0); 
        BroadcastShortCommand(0x40);       
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
sw(int i)
 { /* this routine usually reads some switches on a VME module  
      this is a dummy replacement routine */
   int k;
   if(i=1) k =0;
   if(i=2) k =0;
   if(i=3) k =0;
   if(i=4) k =0;
   if(i=5) k =0;
   if(i=6) k =0;
   if(i=7) k =1;
   if(i=8) k =0;
   return(k);
  }
   
