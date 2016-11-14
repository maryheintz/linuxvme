
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
int digadr[8] = {0x100f,0x100a,0x1011,0x1012,0x1005,0x1039,0x1009,0x100e};
int dm[8] = {1,1,1,1,1,1,1,1};
int ddac1[8]={175,175,175,175,175,175,175,175};
int ddac2[8] ={175,175,175,175,175,175,175,175};
unsigned int deskew1[8] = {0,0,0,0,0,0,0,0};
unsigned int deskew2[8] = {0,0,0,0,0,0,0,0};
unsigned int crca,crcb;
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
     for(dig=0;dig<8;dig++) digitizer_setup(dig);  
    printf(" Setting up Digitizers\n");
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
       	if(sw(8) == 0) i=getchar();
       if(sw(7) == 0) goto loop;
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
	unsigned int type_of_word,block,i;
	nkja = -1;
	code = SSP_ReadOneWord(dev, &word, &type_of_word);
	if(code != SLINK_OK) goto rerror;
        nkja++;
	printf(" start control word %x\n",word);
        for(block=1;block<17;block++)
	  { code = SSP_ReadOneWord(dev, &word, &type_of_word);
	    if(code != SLINK_OK) goto rerror;
	    nkja++;
	    crc(0,word);
            printf("%d  header=%x     crc=%x %x\n",nkja,word,crca,crcb);
	    for(i=0;i<8;i++)
	      { code = SSP_ReadOneWord(dev, &word, &type_of_word);
	        if(code != SLINK_OK) goto rerror;
		crc(1,word);
		nkja++;
                printf("%d  data gain1 = %x     crc=%x %x\n",
		    nkja,word,crca,crcb);
	      }
            code = SSP_ReadOneWord(dev, &word, &type_of_word);
	    if(code != SLINK_OK) goto rerror;
	    nkja++;
	    crc(1,word);
            printf("%d  header=%x     crc=%x %x\n",nkja,word,crca,crcb);
	    for(i=0;i<8;i++)
	      { code = SSP_ReadOneWord(dev, &word, &type_of_word);
	        if(code != SLINK_OK) goto rerror;
		crc(1,word);
		nkja++;
                printf("%d  data gain2 = %x     crc=%x %x\n",
		    nkja,word,crca,crcb);
	      }
	    code = SSP_ReadOneWord(dev, &word, &type_of_word);
	    if(code != SLINK_OK) goto rerror;
	    nkja++;
            printf("%d  block %d  TILEDMU crc=%x\n",nkja,block,word);
	  }
        code = SSP_ReadOneWord(dev, &word, &type_of_word);
	if(code != SLINK_OK) goto rerror;
	nkja++;
        printf("%d  CRC_ERROR=%x\n",nkja,word);
        code = SSP_ReadOneWord(dev, &word, &type_of_word);
	if(code != SLINK_OK) goto rerror;
	nkja++;
        printf("%d  GLOBAL CRC=%x\n",nkja,word);
        code = SSP_ReadOneWord(dev, &word, &type_of_word);
	if(code != SLINK_OK) goto rerror;
	nkja++;
        printf("%d  end control word=%x\n",nkja,word);
	return(0);
rerror:
        printf(" SLINK read error %d\n",nkja);	
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
