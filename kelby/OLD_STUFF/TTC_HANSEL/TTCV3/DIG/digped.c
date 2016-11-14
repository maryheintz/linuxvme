
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
int fdata[49][32],ipk[49],pk[49];

int ttcadr,id;
int digadr[8] = {0x1004,0x101f,0x1006,0x1025,0x101c,0x1021,0x1020,0x1015};
int dm[8] = {0,1,0,0,0,0,0,1};
int ddac1[8]={160,160,160,160,160,160,160,160};
int ddac2[8] ={160,160,160,160,160,160,160,160};
/* pdac[k-1] = dac value controlling ped of 3in1 card k */
int pdac[48]={2,2,2,1,1,1,2,2,2,1,1,1,2,2,2,1,1,1,2,2,2,1,1,1,
              1,1,1,2,2,2,1,1,1,2,2,2,1,1,1,2,2,2,1,1,1,2,2,2};
/* pttc[k-1] = ttcadr of digitizer for 3in1 card k  */
int pttc[48]={0,0,0,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,3,3,3,3,3,3,
              4,4,4,4,4,4,5,5,5,5,5,5,6,6,6,6,6,6,7,7,7,7,7,7};
	      
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
static int icap;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { 
 
    int tim,i,l,k,kkk,pipe,drawer,dig,kk;
    int k1,k2,card;
    int dac;
    double charge,dacval,capacitor,pset;
    char xstring[80],ystring[80],string[80],err[80];
    kk = 0;
    id=1;
    ttcadr=0x1234;
    ttcvi_map(ttcadr);

    set_mb_time(ttcadr,1,136);
    set_mb_time(ttcadr,2,94);
    set_mb_time(ttcadr,3,52);
    set_mb_time(ttcadr,4,0);
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
    charge = 600.0;
    capacitor = 100.0;
    large_cap_enable(ttcadr);
    small_cap_disable(ttcadr);
    dacval = (charge * 1023.0 ) / (2.0 * 4.096* capacitor);
    if(dacval > 1023.0) dacval = 1023.0;
    dac = (int) dacval;
    charge = ( 2.0 * 4.096 * capacitor *(double)dac ) / 1023.0;
    printf(" charge=%f  dacval=%f  dac=%x(hex)  cap=%f\n",
           charge,dacval,dac,capacitor);
        set_dac(ttcadr,dac);
    SetTimeDes2(ttcadr,116);
 
   for(card=1;card<49;card++) 
      { inject_setup();

	slink_setup();
           
   /* for now set up the digitizer ttcrx chips using adrttc=0 */

	clear_buffer();
	pset = 20.0;
        for(dig=0;dig<8;dig++) digitizer_setup(dig);  
        clear_buffer();
           sleep(1);
	   pset = 20.0;
           dac=find_ped(card,pset);
           printf(" card=%d  pset=%f  dac=%d\n",card,pset,dac);
           for(k1=0;k1<8;k1++) ddac1[k1]=160;
           for(k1=0;k1<8;k1++) ddac2[k1]=160;
         }
/*	 printf("ddac1=%d %d %d %d %d %d %d %d\n",
	    ddac1[0],ddac1[1],ddac1[2],ddac1[3],
	    ddac1[4],ddac1[5],ddac1[6],ddac1[7]);
	 printf("ddac2=%d %d %d %d %d %d %d %d\n",
	    ddac2[0],ddac2[1],ddac2[2],ddac2[3],
	    ddac2[4],ddac2[5],ddac2[6],ddac2[7]);  */
    } /* end of main */
    find_ped(int card,double pset)	
      { int d,k,i,dsave;
        double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
        char string[80];
        dsave = 0;
	for(d=50;d<256;d++)
	  { if(dsave !=0) continue;
	    set_ped_dac(card,d);  
	    inject();
            usleep(10000);
            slink_read();
	    inject();
            usleep(10000);
            slink_read();
            asum = 0.0;
            axsum = 0.0;
            axxsum = 0.0;
            for(i=1;i<10;i++)
             { aval = (double)fdata[card][i];
               asum = asum+1.0;
               axsum = axsum + aval;
               axxsum = axxsum + aval*aval;
             }
            ped = axsum / asum;
/*	    printf("card=%d  d=%d  ped=%f fdata= %d %d\n",
	     card,d,ped,fdata[card][1],fdata[card][2]);  */
            dped = 0.0;
            sig2=axxsum/asum - ped*ped;
            if(sig2>0.0) dped=sqrt(sig2);
            sprintf(string,
              " pk=%d  ped=%f", pk[card],ped);
	    sidev(1);  /* force plot to screen without asking*/
	    sprintf(string,"TUBE %d ped=%f",
		    card,ped);
  	    if(sw(1) == 1) evdisp(14,string,card);
            if(ped > pset) dsave=d;
	  }
          return(dsave);
   }
   
evdisp(nn,string,kk)
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
        /* setup inhibit1 to set tp high */
	     SetBMode(1,0xa);
	     SetInhibitDelay(1,10);
	     SetInhibitDuration(1,10);
	     BPutLong(1,ttcadr,1,0xc0,0x00);  /* load set tp_high into fifo1 */
	     /* set inhibit2 to drop tp low (timed broadcast mode */
	     SetBMode(2,0xa);
	     SetInhibitDelay(2,1990);
	     SetInhibitDuration(2,10);
	  /*  BPutLong(2,ttcadr,1,0xc0,0x01); */  /* load set tp_low into fifo2 */
	     BPutShort(2,0xc0);   /* set tp low bcast command  */
	     /* set inhibit3 to set latch                     */
	     /* use latch-bar and inhibit3 to start fermi     */
             /* will use inhibit 3 to set l1a with digitizers */
             i3time=2045;
/*	     printf("input i3time\n");
             fscanf(stdin,"%d",&i3time);     */
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
	slink_para.start_word = 0x11111110;
	slink_para.stop_word = 0xfffffff0;
	if ((code = SSP_Open(&slink_para, &dev)) != SLINK_OK)
	{
	printf(" slink open error, error code = %d\n",code);
	SLINK_PrintErrorMessage(code);
	exit(0);
	}
/*	printf("slink opened\n"); */
	return; 
	}

clear_buffer()
	{
	unsigned long int word;
	int type_of_word;
	int data_count = 0;
	while ( (code = SSP_ReadOneWord(dev, &word, &type_of_word) !=
		SLINK_FIFO_EMPTY)) {
		data_count++;
		if ( data_count > 1000)  {
			return;
			}
		}	
	}
	  
slink_read()
        {
	unsigned long word;
	int type_of_word,count,kk,ii;
        slink_formatted_data sd;
	count =0;
	while (code = SSP_ReadOneWord(dev, &word, &type_of_word) == SLINK_OK)
	{
		
	 if (type_of_word != SLINK_CONTROL_WORD) 
	 	{
				
				kk = count/16;
				ii = count-16*kk;
				count++;
				sd.ldata =  word;
				if(sw(4) == 1) printf(" data word= %x  %x  %x\n",
				sd.bits.data1,sd.bits.data2,sd.bits.data3); 
				if(kk<32)
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
	 {
	 if(sw(4) == 1) printf("control word= %x\n", word); 
	 if(sw(4) == 1) printf("no data\n");
	 return;
         }
       }
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
      { int mode, adrttc, dac1, dac2;
        adrttc = digadr[i];
	mode = dm[i];
	dac1 = ddac1[i];
	dac2 = ddac2[i]; 
        if(mode == 0) SendLongCommand(adrttc,0,0x3,0xb7); /* reset ttcrx chip */
	if(mode == 1) SendLongCommand(adrttc,0,0x3,0xa8); /* reset ttcrx chip */
	usleep(200);

        BroadcastShortCommand(0x40);     /* reset digitizer */
	usleep(2000);

/*        if(mode == 0) SendLongCommand(adrttc,0,0x3,0xb7); */ /* reset ttcrx chip */
/*	  if(mode == 1) SendLongCommand(adrttc,0,0x3,0xa8); */ /* reset ttcrx chip */
	usleep(200);

        SendLongCommand(adrttc,1,1,0xd); /* number of samples = 13 */
	usleep(200);	

        SendLongCommand(adrttc,1,0xa,0); /* clock invert bits etc */
        usleep(200);

        SendLongCommand(adrttc,1,0xb,0xfc); /* high limit LSB=0xff */
	usleep(200);
	    
	SendLongCommand(adrttc,1,0xc,3); /* high limit MSB=3 */
        usleep(200);

        SendLongCommand(adrttc,1,0xd,0);  /* low limit LSB=0 */
	usleep(200);
	
	SendLongCommand(adrttc,1,0xe,0);  /* low limit MSB=0 */
	usleep(200);
	
	SendLongCommand(adrttc,1,0x2,21); /* set pipeline length=21 */
	usleep(200);
	
	SendLongCommand(adrttc,1,0xf,0);  /* set mode=0 NORMAL */	
	usleep(200);
	
	SendLongCommand(adrttc,1,0x8,dac1);  /* set dac=160 */	
	usleep(200);
	
	SendLongCommand(adrttc,1,0x9,dac2);  /* set dac=160 */	
	usleep(200);
	
        BroadcastShortCommand(0x40);     /* reset digitizer */
	usleep(2000);
	return;
	}

    set_ped_dac(int card,int d)
      { int adrttc,k,l;
	l = pttc[card-1];
        k = pdac[card-1];
        adrttc=digadr[l]; 
        BroadcastShortCommand(0x40);     /* reset digitizer */
	usleep(2000);
        if(k == 1) SendLongCommand(adrttc,1,0x8,d);  /* set dac1 */	
        if(k == 2) SendLongCommand(adrttc,1,0x9,d);  /* set dac2 */	
	usleep(200);
        BroadcastShortCommand(0x40);     /* reset digitizer */
	usleep(2000);
/*	clear_buffer();  */
/*        printf("card=%d l=%d k=%d adrttc=%x d=%d\n",card,l,k,adrttc,d);
*/
      }
