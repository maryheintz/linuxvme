
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
int digadr[8] = {0x100a,0x100f,0x1011,0x1012,0x1005,0x1039,0x1009,0x100e};
int dm[8] = {1,1,1,1,1,1,1,1};
int ddac1[8]={175,175,175,175,175,175,175,175};
int ddac2[8] ={175,175,175,175,175,175,175,175};
unsigned int deskew1[8] = {0,0,0,0,0,0,0,0};
unsigned int deskew2[8] = {0,0,0,0,0,0,0,0};
time_t usec = 1;
int code;
FILE *fp;
static long start_time,current_time,errors=0;
static int nsamp = 7,event=0;
static int test[32];
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
  { int tim,i,l,k,kkk,pipe,drawer,dig,kk,tube,wait,dtime;
    int dac;
    double charge,dacval,capacitor;
    char xstring[80],ystring[80],string[80],err[80];
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    for(l=0;l<32;l++) test[l] = (0x10101<<l);
    fp = fopen("digtest2.log","w");
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
   /* for now set up the digitizer ttcrx chips using adrttc=0 */
    printf(" Setting up Digitizers\n");
     for(dig=0;dig<8;dig++) digitizer_setup(dig);  
    printf(" Setting up Digitizers\n");
     for(dig=0;dig<8;dig++) digitizer_setup(dig);  
     clear_buffer();
     start_time = time(NULL);
     SetTriggerSelect(4); /* software triggers */
loop:           
       for(dig=0;dig<8;dig++) set_sample(dig);
        BroadcastShortCommand(3);    /* BunchCrossing and 
                                       event counter reset */
       SendL1A();
       /* usleep(100);  */
       slink_read();
       /*   printf(" enter character for next event\n");
          fscanf(stdin,"%x",&wait); */
	  event++;
	  current_time = time(NULL);
	  dtime = (int)(current_time - start_time);
	  if(sw(2) == 1) printf("event=%d   time=%d sec   errors=%d\n",
	    event,dtime,errors);
          if(sw(8) == 0) goto loop;
	  fprintf(fp,"event=%d      time=%d sec     errors=%d\n",
	       event,dtime,errors);;
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
	unsigned long word;
	int type_of_word,k,l,sw1,dtime,iok;

	int parity,samples,error,sestr,destr,r_parity,v_parity,
	   mode,gain,bunch;
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
	 sw1 = sw(1); 
	 k=-1;
	 printf("\n\n\n sw1=%d\n",sw1);     
         code = SSP_ReadOneWord(dev, &word, &type_of_word);
	 if(code != SLINK_OK) goto error_exit;
	 if (type_of_word == SLINK_CONTROL_WORD) 
	 	{ if(word == 0x11111110) 
		      {if(sw1 == 1) 
		         printf("start control word = %x\n",word);}
		  else 
		       iok=1;
		       goto error_exit;
		}      		
/* read the 16 blocks of data */
         for(k=0;k<16;k++)
	   { /* read header */
             code = SSP_ReadOneWord(dev, &word, &type_of_word);
	     if(code != SLINK_OK) 
	       { iok=2;
	         goto error_exit;
	       }
	     if (type_of_word == SLINK_CONTROL_WORD ||
	         (word>>31) != 1) 
		   { iok=3;
		     goto error_exit;
		   }
	     if(sw1 == 1)
	      {parity = (word>>30) & 1;
	       samples = (word>>26) & 0xf;
	       error = (word>>25) & 1;
	       sestr = (word>>24) & 1;
	       destr = (word>>23) & 1;
               r_parity = (word>>22) & 1;
	       gain = (word>>12) & 0x7;
	       bunch = word & 0xfff;
	       printf("header word %x\n",word);
	       printf(" parity=%d  samples=%d  error=%d\n",
		  parity,samples,error);
	       printf(" SEstr=%d   r_parity=%d   v_parity=%d\n",
		  sestr,r_parity,v_parity);
	       printf(" mode=%d   gain=%d   bunch=%d\n",
		  mode,gain,bunch);
	      }
             /* now read nsamp good data words */
	     for(l=0;l<nsamp+1;l++)
	      { code = SSP_ReadOneWord(dev, &word, &type_of_word);
	        if(code != SLINK_OK) 
		  { iok=4;
		    goto error_exit;
		  }
	        if (type_of_word == SLINK_CONTROL_WORD ||
	            (word>>31) == 1) 
		      { iok=5;
		        goto error_exit;
                      }
                if(sw1 == 1) printf("block=%d  sample=%d  word=%x\n",k,l,word);
                if(word != test[l] )
		  { current_time = time(NULL);
	            dtime = (int)(current_time - start_time);
		    errors++;
                    printf("error %x %x\n",word,test[l]);
	            printf("event=%d      time=%d sec     errors=%d\n",
	              event,dtime,errors);
                    fprintf(fp,"error %x %x\n",word,test[l]);
	            fprintf(fp,"event=%d      time=%d sec     errors=%d\n",
	              event,dtime,errors);
		  }
	      }
	     /* now read the crc word  */
	     code = SSP_ReadOneWord(dev, &word, &type_of_word);
	     if(code != SLINK_OK) 
	       { iok=6;
	         goto error_exit;
	       }
	     if (type_of_word == SLINK_CONTROL_WORD)
	         { iok=7;
		   goto error_exit;
		 }
             if(sw1 == 1) printf("crc=%x\n",word);
	     /* now do nsamp junk words */
	     	     for(l=0;l<nsamp+1;l++)
	      { code = SSP_ReadOneWord(dev, &word, &type_of_word);
	     if(code != SLINK_OK) 
	        { iok=8;
		  goto error_exit;
		}
	        if (type_of_word == SLINK_CONTROL_WORD) goto error_exit;
                if(sw1 == 1) printf("block=%d  sample=%d  junk word=%x\n",k,l,word);
	      }
	     /* now read the crc word  */
	     code = SSP_ReadOneWord(dev, &word, &type_of_word);
	     if(code != SLINK_OK) 
	       { iok=9;
	         goto error_exit;
	       }
	     if (type_of_word == SLINK_CONTROL_WORD)
	       { iok=10;
	         goto error_exit;
	       }
             if(sw1 == 1) printf("crc=%x\n",word);
	 }
             /* now get the end control word */
         code = SSP_ReadOneWord(dev, &word, &type_of_word);
	     if(code != SLINK_OK) 
	       { iok=11;
	         goto error_exit;
	       }
	     if (type_of_word == SLINK_CONTROL_WORD) 
	 	{ if(word == 0xfffffff0) 
		    { if(sw1 == 1) 
		         printf("end control word %x\n",word); }
		  else 
		       iok=12;
		       goto error_exit;
		}      		
          return;
error_exit:
	printf(" trouble code=%x  word=%x  iok=%d  k=%d\n",
	  code,word,iok,k);
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

	SendLongCommand(adrttc,1,4,1);  /* test word part 2 */	
	usleep(2);

	SendLongCommand(adrttc,1,5,1);  /* test word part 3 */	
	usleep(2);

	SendLongCommand(adrttc,1,6,0);  /* test word part 4 MSB */	
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
	usleep(2);

	SendLongCommand(adrttc,1,4,1);  /* test word part 2 */	
	usleep(2);

	SendLongCommand(adrttc,1,5,1);  /* test word part 3 */	
	usleep(2);

	SendLongCommand(adrttc,1,6,0);  /* test word part 4 MSB */	
	usleep(2);
	 
        BroadcastShortCommand(0x40);       /* reset digitizer */
	usleep(2);

      }
