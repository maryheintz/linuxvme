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
int digadr[8] = {0x100e,0x100f,0x1010,0x1011,
        0x1012,0x1014,0x1015,0x1017};
int dm[8] = {1,1,1,1,1,1,1,1};
int ddac1[8]={175,175,175,175,175,175,175,175};
int ddac2[8] ={175,175,175,175,175,175,175,175};
/* unsigned int deskew1[8] = {0,10,20,30,30,20,10,0}; */
unsigned int deskew1[8] = {0,0,0,0,0,0,0,0};
unsigned int deskew2[8] = {0,0,0,0,0,0,0,0}; 
unsigned int iclksm[8] = {0,0,0,0,0,0,0,0};
unsigned int crca,crcb;
time_t usec = 1;
int code;
static int nsamp;
int event=0,nerror=0,ndcrc=0,nbnch=0,ngcrc=0,nctrl=0;
int dbad[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
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
    int dac,alarm,rate;
    double charge,dacval,capacitor;
    char xstring[80],ystring[80],string[80],err[80];
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    tube=0;
    kk = 0;
/*    printf("input 0=600pC  1=7pC\n ");
    fscanf(stdin,"%d",&icap);  */
    nsamp = 6;
/*    printf("input nsamp\n ");
    fscanf(stdin,"%d",&nsamp);  */
    icap = 0;
    id=1;

    usleep(200000);
    ttcadr=0x300a;
    ttcvi_map(ttcadr);

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
           
    printf(" Setting up Digitizers\n");
     for(dig=0;dig<8;dig++) digitizer_setup(dig);  
	slink_setup();
	
	SendLongCommand(0,1,0xf,8); /* send reset to the interface */
	if(nsamp < 7) SendLongCommand(0,1,0xf,4); /* set 16 buffers in interface */

    SetBMode(0,0xa); 
    BPutShort(0,1);   /* reset bunch counter on orbit  */

     rate = 7;
/*     printf(" select random trigger rate\n");
     printf("     8 = source\n");
     printf("     7 = 100KHz\n");
     printf("     6 =  50KHz\n");
     printf("     5 =  25KHz\n");
     printf("     4 =  10KHz\n");
     printf("     3 =   5KHz\n");
     printf("     2 =   1KHz\n");
     printf("     1 = 100Hz\n");
     printf("     0 =   1Hz\n");
     fscanf(stdin,"%d",&rate);  */
       if(rate == 8) Setcsr1(0x7028);
       if(rate == 7) Setcsr1(0x702d);
       if(rate == 6) Setcsr1(0x602d);
       if(rate == 5) Setcsr1(0x502d);
       if(rate == 4) Setcsr1(0x402d);
       if(rate == 3) Setcsr1(0x302d);
       if(rate == 2) Setcsr1(0x202d);
       if(rate == 1) Setcsr1(0x102d);
       if(rate == 0) Setcsr1(0x002d);
/* toss 2 events */
       usleep(5000);
       clear_buffer();
       clear_buffer();
	usleep(2);  
	sidev(1);  /* force plot to screen without asking*/
       slink_read();
restrt:
	event=0;
	nerror=0;
	ndcrc=0;
	nbnch=0;
	ngcrc=0;
	nctrl=0;
 loop:
       event++;
       if(event>1000000 && sw(2) != 0)  printf("\a\a\a");
       if(sw(6) != 0 && (event%1000000) == 0) 
             printf("event=%d  nerror=%d  ndcrc=%d  nbnch=%d  nctrl=%d  ngcrc=%d\n",
               event,nerror,ndcrc,nbnch,nctrl,ngcrc);
       if(sw(6) == 0 && (event%5000) == 0) 
             printf("event=%d  nerror=%d  ndcrc=%d  nbnch=%d  nctrl=%d  ngcrc=%d\n",
               event,nerror,ndcrc,nbnch,nctrl,ngcrc);
       slink_read();
       if(sw(8) == 0) goto restrt;
       if(sw(1) == 1) 
	 { Setcsr1(0x0028); /* disable triggers */
	   usleep(100000);
           clear_buffer();
	   printf("event=%d  nerror=%d  ndcrc=%d  nbnch=%d  nctrl=%d  ngcrc=%d\n",
                      event,nerror,ndcrc,nbnch,nctrl,ngcrc);
           exit(0);
         }
          goto loop;
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
	   mode,gain,bunch,cc,kklast,bnch[16];
	int type_of_word,count,kk,ii,stop,hcnt,crcaa,b1;
	int bnchok,gcrcok,dcrcok,ctrlok;
        slink_formatted_data sd;
	cc = 0;
	count =0;
	kklast = 0;
	hcnt=0;
	bnchok=0;
	gcrcok=0;
	dcrcok=0;
	ctrlok=0;
	while (code = SSP_ReadOneWord(dev, &word, &type_of_word) == SLINK_OK)
	{if (type_of_word != SLINK_CONTROL_WORD) 
	 	{ kk = count/(nsamp+3);
	          ii = count-(nsamp+3)*kk;
		  count++;
		  if(count>500) return(1);
		  sd.ldata =  word;
		  wordlast = word;
		  if(kklast == 15 && ii == 0) 
		   { if(sw(7) == 0) printf(" dig crc %x\n",word);
		     if(word != 0x3fff3fff) 
		      { dcrcok++;
		        printf(" dig crc ERROR %x\n",word);
	              }
		   }
	         else if(kklast == 15 && ii == 1) 
		   { if(sw(7) == 0) printf(" final crc %x\n",word);
		     h1 = word & 0xffff;
		     h2 = (word>>16) & 0xffff;
		     if(h1 != h2) printf(" final crc ERROR %x\n",word);
		   }
		 else if(ii == 0) 
		   {
		     bunch = word & 0xfff;
		     bnch[hcnt] = bunch;
		     if(count == 1) b1 = bunch;
		     if(count!=1 && bunch!=b1) 
		     hcnt++;
		   }
		 else if(ii>nsamp+1) 
		   {/* crc1 = (word>>16);
		     crc2 = word & 0xffff;
		     if(crcaa != crc1 || crcb != crc2) dbad[hcnt-1]++;
                       if(sw(7) == 0) 
		         { printf("      tiledmu crc %4x %4x",crc1,crc2);
		           if(crcaa == crc1 && crcb == crc2) printf("  ok\n");
		           if(crcaa != crc1 || crcb != crc2) printf("  bad\n");			
		         }
		     crc(1,word); */
		   }
	         else
		   { kklast = kk;
		   }
				 
		}
	 else 
	       { cc++;
	       if(sw(7) == 0)
	         { if(cc == 1 && word == 0x51115110) printf("\n\n");
	           printf("control word= %x\n", word);
		 }
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
		        if(sw(2) != 0) 
			  { printf("\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a");
		            printf("\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a");
		            printf("\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a");
			  }
                            printf("bnch= %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
			bnch[0],bnch[1],bnch[2],bnch[3],bnch[4],bnch[5],bnch[6],bnch[7],
			bnch[8],bnch[9],bnch[10],bnch[11],bnch[12],bnch[13],bnch[14],bnch[15]);
          if(dcrcok != 0) printf(" dbad= %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
          dbad[0],dbad[1],dbad[2],dbad[3],dbad[4],dbad[5],dbad[6],dbad[7],
          dbad[8],dbad[8],dbad[10],dbad[11],dbad[12],dbad[13],dbad[14],dbad[15]);
		      } 
	            if(dcrcok != 0) ndcrc++;
	            else if(bnchok != 0) nbnch++;
	            else if(gcrcok != 0) ngcrc++;
	            else if(ctrlok != 0) nctrl++;
		    return;
	           }
               }
       }
	printf("no data\n");
	return;
       }
	 
digitizer_setup(int i)
      { int mode, adrttc, dac1, dac2, ick;
        adrttc = digadr[i];
	mode = dm[i];
	dac1 = ddac1[i];
	dac2 = ddac2[i];
        ick = iclksm[i];
	
        SendLongCommand(adrttc,0,0x6,0);  /* reset v3.0 ttcrx chips*/ 
	usleep(200);
        SendLongCommand(adrttc,0,0x3,0xa9);  /* setup ttcrx chip bnch cntr */
	
/* set up digitizer registers */
        SendLongCommand(adrttc,1,0xa,ick); /* clock invert bits etc */
        BroadcastShortCommand(0x40);         /* reset digitizer */
        SendLongCommand(adrttc,1,1,nsamp); /* number of samples = 13 */
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0x2,21); /* set pipeline length=21 */
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0xf,0);  /* set mode=0 NORMAL */	
        BroadcastShortCommand(0x40);         /* reset digitizer */

	SendLongCommand(adrttc,1,0x8,dac1);  /* set dac1 */	
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0x9,dac2);  /* set dac2 */	
        BroadcastShortCommand(0x40);         /* reset digitizer */
        SendLongCommand(adrttc,1,0xb,0xff);  /* high limit LSB=0xff */    
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0xc,3);     /* high limit MSB=3 */
        BroadcastShortCommand(0x40);         /* reset digitizer */
        SendLongCommand(adrttc,1,0xd,0);     /* low limit LSB=0 */
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0xe,0);     /* low limit MSB=0 */
        BroadcastShortCommand(0x40);         /* reset digitizer */

/* set up digitizer registers again */
        SendLongCommand(adrttc,1,0xa,ick); /* clock invert bits etc */
        BroadcastShortCommand(0x40);         /* reset digitizer */
        SendLongCommand(adrttc,1,1,nsamp); /* number of samples = 13 */
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0x2,21); /* set pipeline length=21 */
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0xf,0);  /* set mode=0 NORMAL */	
        BroadcastShortCommand(0x40);         /* reset digitizer */

	SendLongCommand(adrttc,1,0x8,dac1);  /* set dac1 */	
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0x9,dac2);  /* set dac2 */	
        BroadcastShortCommand(0x40);         /* reset digitizer */
        SendLongCommand(adrttc,1,0xb,0xff);  /* high limit LSB=0xff */    
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0xc,3);     /* high limit MSB=3 */
        BroadcastShortCommand(0x40);         /* reset digitizer */
        SendLongCommand(adrttc,1,0xd,0);     /* low limit LSB=0 */
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0xe,0);     /* low limit MSB=0 */

        BroadcastShortCommand(0x40);         /* reset digitizer */

        SetTimeDes1(adrttc,deskew1[i]);
        SetTimeDes2(adrttc,deskew2[i]);
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
