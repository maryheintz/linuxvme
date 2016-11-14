
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
int digadr[8] = {0x1011,0x100a,0x1012,0x100f,0x1005,0x1039,0x1009,0x100e};
int dm[8] = {1,1,1,1,1,1,1,1};
int ddac1[8]={175,175,175,175,175,175,175,175};
int ddac2[8] ={175,175,175,175,175,175,175,175};
unsigned int deskew1[8] = {0,10,20,30,30,20,10,0};
unsigned int deskew2[8] = {0,0,0,0,0,0,0,0};
unsigned int crca,crcb;
time_t usec = 1;
int code;
static int nsamp = 5;
int event=0,nerror=0;
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
    int dac,kja;
    double charge,dacval,capacitor;
    char xstring[80],ystring[80],string[80],err[80];
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    tube=0;
    if(sw(5) == 1)
       { printf("input ddac1 values\n");
         fscanf(stdin,"%d %d %d %d %d %d %d %d",
	    &ddac1[0],&ddac1[1],&ddac1[2],&ddac1[3],
	    &ddac1[4],&ddac1[5],&ddac1[6],&ddac1[7]);
         printf("input ddac2 values\n");
         fscanf(stdin,"%d %d %d %d %d %d %d %d", 
	    &ddac2[0],&ddac2[1],&ddac2[2],&ddac2[3],
	    &ddac2[4],&ddac2[5],&ddac2[6],&ddac2[7]);
	 printf("ddac1=%d %d %d %d %d %d %d %d\n",
	    ddac1[0],ddac1[1],ddac1[2],ddac1[3],
	    ddac1[4],ddac1[5],ddac1[6],ddac1[7]);
	 printf("ddac2=%d %d %d %d %d %d %d %d\n",
	    ddac2[0],ddac2[1],ddac2[2],ddac2[3],
	    ddac2[4],ddac2[5],ddac2[6],ddac2[7]);
       }
    kk = 0;
    printf("input 0=600pC  1=10pC\n ");
    fscanf(stdin,"%d",&icap);
    printf("input nsamp\n ");
    fscanf(stdin,"%d",&nsamp);
    id=1;

    usleep(200000);
    ttcadr=0x1234;
    ttcvi_map(ttcadr);
    BroadcastShortCommand(0x10);  /* reset any old version ttcrx chips */

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
    if(icap == 0)
    { charge = 600.0;
      capacitor = 100.0;
      large_cap_enable(ttcadr);
      small_cap_disable(ttcadr);
    }
    if(icap == 1)
    { charge = 7.0;
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
    SetTimeDes2(ttcadr,216);
 
	inject_setup();

	slink_setup();
           
    printf(" Setting up Digitizers\n");
     for(dig=0;dig<8;dig++) digitizer_setup(dig);  
    printf(" Setting up Digitizers\n");
     for(dig=0;dig<8;dig++) digitizer_setup(dig);  

	slink_setup();
           

        BroadcastShortCommand(0x40);       
	usleep(2);  
 loop:
       event++;
       BroadcastShortCommand(3);    /* BunchCrossing and 
                                       event counter reset */

       clear_buffer();  
       usleep(1);
       inject();
       usleep(1);
       for(kja=0;kja<8;kja++)
        { slink_read();
               if(sw(8) == 1)
	         {usleep(1000);
		  if(sw(7) == 0) printf("next event\n");
		  goto loop;}
		  sidev(1);  /* force plot to screen without asking*/
              if(sw(1) == 0)
   		{ if(icap == 0) sprintf(string,"LOW GAIN TUBES 1-24 kja=%d",kja);
  		  if(icap == 1) sprintf(string,"HIGH GAIN TUBES 1-24 kja=%d",kja);
  		  bigdisp(nsamp,string,1);
  		  if(icap == 0) sprintf(string,"LOW GAIN TUBES 25-48 kja=%d",kja);
  		  if(icap == 1) sprintf(string,"HIGH GAIN TUBES 25-48 kja=%d",kja);
 	 	  bigdisp(nsamp,string,25);
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
		  if(icap == 0) sprintf(string,"LOW GAIN TUBES %d ped=%f  kja=%d",
		    tube,ped,kja);
  		  if(icap == 1) sprintf(string,"HIGH GAIN TUBES %d ped=%f kja=%d",
		    tube,ped,kja);
  		  evdisp(14,string,tube);
                }

          usleep(1);
	}
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
             i3time=2040;
	   /*  printf("input i3time\n");
             fscanf(stdin,"%d",&i3time);  */     
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
	unsigned long word,wordlast,crc1,crc2,h1,h2;
	int parity,samples,error,sestr,destr,r_parity,v_parity,
	   mode,gain,bunch,cc,kklast;
	int type_of_word,count,kk,ii,stop,hcnt;
        slink_formatted_data sd;
	cc = 0;
	count =0;
	kklast = 0;
	hcnt=0;
	while (code = SSP_ReadOneWord(dev, &word, &type_of_word) == SLINK_OK)
	{
		
	 if (type_of_word != SLINK_CONTROL_WORD) 
	 	{
				
				kk = count/(nsamp+3);
				ii = count-(nsamp+3)*kk;
				count++;
				sd.ldata =  word;
				wordlast = word;
				if(kklast == 15 && ii == 0) 
				    { if(sw(7) == 0) printf(" CRC_ERROR %x\n",word);
				      if(word != 0xffffffff) printf(" CRC_ERROR trouble %x\n",word);
				    }
				else if(kklast == 15 && ii == 1) 
				    { if(sw(7) == 0) printf(" final CRC %x\n",word);
				      h1 = word & 0xffff;
				      h2 = (word>>16) & 0xffff;
				      if(h1 != h2) printf(" final CRC trouble %x\n",word);
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
				     crc(0,word);
				     hcnt++;
		                     if(sw(7) == 0) printf("%d header word %x   count %d  crc= %x %x\n",
				          hcnt,word,count,crca,crcb);
		                     if(sw(7) == 0) printf(" parity=%d  samples=%d  error=%d\n",
		                        parity,samples,error);
		                     if(sw(7) == 0) printf(" SEstr=%d   r_parity=%d   v_parity=%d\n",
		                        sestr,r_parity,v_parity);
		                     if(sw(7) == 0) printf(" mode=%d   gain=%d   bunch=%d\n",
		                       mode,gain,bunch);
				   }
				else if(ii>nsamp+1) 
				   { if(sw(7) == 0) printf("      tileDMU  crc %x   count %d\n",
				       word,count);
				     crc(1,word);
				     if(sw(7) == 0) printf("after TILEcrc sent  crc= %x %x\n",crca,crcb);
				   }
				else
				  { crc(1,word);
                                    if(sw(7) == 0) printf(" data word= %4i %4i %4x %4x %4x",
	                              sd.bits.topbit,sd.bits.parity,
				      sd.bits.data1,sd.bits.data2,
				      sd.bits.data3);
				    if(sw(7) == 0) printf("   %d  %d  count %d  %x  crc %x %x\n",
				       kk,ii,count,word,crca,crcb);
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
	         if(sw(7) == 0) printf("control word= %x\n", word);
		 if(cc == 1 && word != 0x51115110) 
		   { nerror++;
		     printf("control word= %x  cc %d  error %d  event %d\n",
		      word,cc,nerror,event);
		     if(sw(6) == 1) fscanf(stdin,"%d",&stop); 
		   } 
		 if(cc == 2 && word != 0xfffffff0) 
		   { nerror++;
		     printf("control word= %x  cc %d  error %d  event %d\n",
		      word,cc,nerror,event);
		     /* sleep(10);  */
		     if(sw(6) == 1) fscanf(stdin,"%d",&stop); 
		   }
		 if(word == 0xfffffff0)
		   {crc1 = wordlast & 0xffff;
		    crc2 = (wordlast>>16) & 0xffff;
		    if(crc1 != crc2) 
		       { if(sw(7)==0) printf("crc error wordlast=%x  crc1=%x  crc2=%x\n",
		            wordlast,crc1,crc2);
			 if(sw(6) == 1) fscanf(stdin,"%d",&stop); 
		       }
                    return;
	           }
               }
       }
	if(sw(7) == 0) printf("no data\n");
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
      { int mode, adrttc, dac1, dac2;
        adrttc = digadr[i];
	mode = dm[i];
	dac1 = ddac1[i];
	dac2 = ddac2[i];
/* reset v3.0 ttcrx chips*/
 /*       if(mode == 1) 
	   { SendLongCommand(adrttc,0,0x6,0);  
	     usleep(200000);
	   }  */
        if(mode == 0) SendLongCommand(adrttc,0,0x3,0xb7); /* setup ttcrx chip */
	if(mode == 1) SendLongCommand(adrttc,0,0x3,0xa8); /* setup ttcrx chip */
	usleep(2);

        SendLongCommand(adrttc,1,0xa,0); /* clock invert bits etc */
        usleep(2);

        BroadcastShortCommand(0x40);     /* reset digitizer */
	usleep(2);

        SendLongCommand(adrttc,1,1,nsamp); /* number of samples = 13 */
	usleep(2);	

        SendLongCommand(adrttc,1,0xb,0xfc); /* high limit LSB=0xff */
	usleep(2);
	    
	SendLongCommand(adrttc,1,0xc,3); /* high limit MSB=3 */
        usleep(2);

        SendLongCommand(adrttc,1,0xd,0);  /* low limit LSB=0 */
	usleep(2);
	
	SendLongCommand(adrttc,1,0xe,0);  /* low limit MSB=0 */
	usleep(2);
	
	SendLongCommand(adrttc,1,0x2,21); /* set pipeline length=21 */
	usleep(2);
	
	SendLongCommand(adrttc,1,0xf,8);  /* set mode=0 NORMAL + RESET */	
	usleep(2);
	
	SendLongCommand(adrttc,1,0x8,dac1);  /* set dac=160 */	
	usleep(2);
	
	SendLongCommand(adrttc,1,0x9,dac2);  /* set dac=160 */	
	usleep(2);
	
        SetTimeDes1(adrttc,deskew1[i]);
	usleep(2);

        SetTimeDes2(adrttc,deskew2[i]);
	usleep(2);

        BroadcastShortCommand(0x40);       /* reset digitizer */
	usleep(2);

	SendLongCommand(adrttc,1,0xf,0);  /* set mode=0 */	
	usleep(2);

        BroadcastShortCommand(0x40);       /* reset digitizer */
	usleep(2);

        BroadcastShortCommand(3);    /* BunchCrossing and 
                                       event counter reset */
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
