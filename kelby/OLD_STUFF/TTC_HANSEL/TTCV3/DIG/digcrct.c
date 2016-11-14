
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
int ipass,nkja,wkja[1000],nerr;

int ttcadr,id;
int digadr[8] = {0x100a,0x100f,0x1011,0x1012,0x1005,0x1039,0x1009,0x100e};
int dm[8] = {1,1,1,1,1,1,1,1};
int ddac1[8]={175,175,175,175,175,175,175,175};
int ddac2[8] ={175,175,175,175,175,175,175,175};
unsigned int deskew1[8] = {0,0,0,0,0,0,0,0};
unsigned int deskew2[8] = {0,0,0,0,0,0,0,0};
time_t usec = 1,tstart,tnow;
int code,event=0;
unsigned long qout;
static int nsamp = 7;
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
static int icap,sw1;
FILE *fp;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { int tim,i,l,k,kkk,pipe,drawer,dig,kk,tube,wait;
    int dac;
    double charge,dacval,capacitor;
    char xstring[80],ystring[80],string[80],err[80];
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    double dtime,tlast;
    int itime,rem;
    unsigned short iadc;
    ttcadr=0x1234;
    ttcvi_map(ttcadr);
    fp = fopen("/zip/test.dat","w");
    sw1 = sw(1);
    printf("sw1=%d\n",sw1);
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
     slink_setup();
     clear_buffer();

    printf(" Setting up Digitizers\n");
    fprintf(fp," Setting up Digitizers\n");
     for(dig=0;dig<8;dig++) digitizer_setup(dig);  
    printf(" Setting up Digitizers\n");
    fprintf(fp," Setting up Digitizers\n");
     for(dig=0;dig<8;dig++) digitizer_setup(dig);  
     SetTriggerSelect(4); /* software triggers */
/* ------------- throw away 1 event to start ---------------- */
     for(dig=0;dig<8;dig++) set_sample(dig);
     BroadcastShortCommand(3);    /* BunchCrossing and 
                                       event counter reset */

     clear_buffer();
     SendL1A();
/* ------------------------------------------------------------ */
    ipass=0;
    nkja=0;
    nerr=0;
    tstart = time(NULL);
loop: 
       event++;          
       for(dig=0;dig<8;dig++) set_sample(dig);
        BroadcastShortCommand(3);    /* BunchCrossing and 
*/
      clear_buffer();
      SendL1A();
       usleep(100000);
       slink_read();
       if(sw(2) == 1) 
          { printf(" enter character for next event\n");
            fscanf(stdin,"%x",&wait);
	  }
       tnow = time(NULL);
       dtime = difftime(tnow,tstart);
       itime = (int)dtime;
       rem = itime % 100;
       if((rem == 0) && (tlast != dtime))
         { printf(" ipass=%d  nerr=%d  dtime=%f\n",ipass,nerr,dtime);
	   fprintf(fp," ipass=%d  nerr=%d  dtime=%f\n",ipass,nerr,dtime);
	   tlast = dtime;
	 }	  
       ipass++;
       nkja=0;
       if(sw(8) == 0) goto loop;
        printf(" ipass=%d  nerr=%d  dtime=%f\n",ipass,nerr,dtime);
        fprintf(fp," ipass=%d  nerr=%d  dtime=%f\n",ipass,nerr,dtime);
   }  /* end of main */
   

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
/*	printf("slink opened\n");  */
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
	int data_read;
	int parity,samples,error,sestr,destr,r_parity,v_parity,
	   mode,gain,bunch,stop;
	   /*  parity - parity (odd)
	       samples - derandomizer length (number of samples)
	       error - parity error detected from memory in last readout
	       sestr - single error strobe recieved from the ttc
	       destr - double error strobe recieved from the ttc
	       r_parity - register parity (parity from the register in the chip)
	       v_parity - parity from the variables in the chip
	       mode - 0=normal  1=calibration 2=test 3=unused
	       gain - 1=high  0=low
	       bunch - bunch crossing number   */
	unsigned int type_of_word,byte1,byte2,byte3,byte4,n,k,whi,wlo;
 
        slink_formatted_data sd;
	data_read = 0;
	while (code = SSP_ReadOneWord(dev, &word, &type_of_word) == SLINK_OK)
	 {data_read = 1;
          if(ipass == 0)
	  { wkja[nkja] = word;
	    if(sw(1) == 1) printf("nkja=%d  word=%x\n",nkja,word);
	    nkja++;
	  }  
         if(ipass != 0)
	  { if(word != wkja[nkja]) 
	     { nerr++;
	       printf("error nkja=%d word=%x wkja=%x  nerr=%d\n",
	          nkja,word,wkja[nkja],nerr);
	       fprintf(fp,"error nkja=%d word=%x wkja=%x  nerr=%d\n",
	          nkja,word,wkja[nkja],nerr);
    slink_setup();
             }
	    if(sw(1) == 1) printf("nkja=%d  word=%x\n",nkja,word);
            nkja++;
	  }
        }
	if(data_read == 0) printf(" error no data read\n");		
   slink_setup();
	return;
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
	
	SendLongCommand(adrttc,1,0xf,2);  /* set mode=2 test */	
	usleep(2);
	
	SendLongCommand(adrttc,1,0x8,dac1);  /* set dac=160 */	
	usleep(2);
	
	SendLongCommand(adrttc,1,0x9,dac2);  /* set dac=160 */	
	usleep(2);
	
        SetTimeDes1(adrttc,deskew1[i]);
	usleep(2);

        SetTimeDes2(adrttc,deskew2[i]);
	usleep(2);

	SendLongCommand(adrttc,1,3,1);  /* test word part 1 LSB */	
	usleep(2);

	SendLongCommand(adrttc,1,4,0);  /* test word part 2 */	
	usleep(2);

	SendLongCommand(adrttc,1,5,0);  /* test word part 3 */	
	usleep(2);

	SendLongCommand(adrttc,1,6,0x4000);  /* test word part 4 MSB */	
	usleep(2);

        BroadcastShortCommand(0x40);       /* reset digitizer */
	usleep(2);

        BroadcastShortCommand(3);    /* BunchCrossing and 
                                       event counter reset */
	return;
	}

set_sample(int i)
      { int mode, adrttc, dac1, dac2;
        adrttc = digadr[i];	 
	
	SendLongCommand(adrttc,1,3,1);  /* test word part 1 LSB */	
	ksleep(20);

	SendLongCommand(adrttc,1,4,0);  /* test word part 2 */	
	ksleep(20);

	SendLongCommand(adrttc,1,5,0);  /* test word part 3 */	
	ksleep(20);

	SendLongCommand(adrttc,1,6,0x4000);  /* test word part 4 MSB */	
	ksleep(20);

        BroadcastShortCommand(0x40);     /* reset digitizer */
        ksleep(20);
      }
ksleep(int k)
    { double a;
      int i;
       for(i=0;i<k;i++)
         { a = sin(0.1);
	 }
    }	 
adc_convert(k)
unsigned short *k;
 { unsigned short kk,*reg;
   int stat,card;
/* ----- digitize ----- */
   reg=(unsigned short *)0xdedd0018;   /* set convert low */
   *reg=0;
   reg=(unsigned short *)0xdedd000c;   /* set convert high */
   *reg=0;
   usleep(10); 
/* ----- read out data ----- */
   reg=(unsigned short *)0xdedd000e;
   kk = *reg;
   *k = kk& 0xfff;
   stat=(kk>>15);
/*   printf("kk=%x  stat=%x\n",kk,stat);  */
   return(stat);
}
