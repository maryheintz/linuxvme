
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

int ttcadr,id,kk;
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
FILE *fp;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { 
 
    int tim,i,l,k, kkk, pipe;
    int tube, card_num;
    int dac,icap;
    double charge,dacval,capacitor;
    char xstring[80],ystring[80],string[80],err[80];
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    kk = 0;
    ttcadr = 0;
   
   ttc_setup();
 
    

	pipe = 530;
	digitizer_setup(pipe);
	digitizer_setup(pipe);
	 slink_setup();

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

ttc_setup()
	{ 
	unsigned short *csr1, *longvme1, *longvme2;
	char *ttcvi;
	ttcvi_address = (char *) (0xdeb00000);
	ttcvi = (char *) ttcvi_address;
	csr1 = (unsigned short *) &ttcvi[TTCVI_CSR1_OFFSET];
	*csr1 = 0x402a;

	return;
	}

digitizer_setup(pipline)
	int pipline;
	{ 
	char *ttcvi;
	unsigned short *longvme1, *longvme2, *shortvme;
	ttcvi = (char *) ttcvi_address;
	longvme1 = (unsigned short *) &ttcvi[TTCVI_LONGVME_MSW_OFFSET];
	longvme2 = (unsigned short *) &ttcvi[TTCVI_LONGVME_LSW_OFFSET];
	shortvme = (unsigned short *) &ttcvi[TTCVI_SHORT_VME_CYCLE_OFFSET];
	*longvme1 = 0x8000;
	*longvme2 = 0x3b7;
	usec = 200;
	usleep(usec);
	
	
	*longvme1 = 0xa02a;
	*longvme2 = 0x3a8;
	usec = 200;
	usleep(usec);
	
	*longvme1 = 0xa03e;
	*longvme2 = 0x3a8;
	usec = 200;
	usleep(usec);
	
	*longvme1 = 0x8001;
	 *longvme2 = 0xf08;
	usec = 200;
	usleep(usec);			
	*shortvme = 0x40;
	usec = 200;
	usleep(200);		
	
	
	*shortvme = 0x80;
	usec = 200;
	usleep(usec);
	usleep(usec);
	
	*longvme1 = 0x8000;
	*longvme2 = 0x3a3;
	usec = 200;
	usleep(usec);
	*longvme1 = 0x8001;
	*longvme2 = 0x10d;
	usleep(usec);	
	*longvme1 = 0x8001;
	*longvme2 = 0xa00;
	usec = 200;
	usleep(usec);
	*longvme1 = 0x8001;
	*longvme2 = 0xbff;
	usec = 200;
	usleep(usec);
	*longvme1 = 0x8001;
	*longvme2 = 0xc03;
	usec = 200;
	usleep(usec);
	*longvme1 = 0x8001;
	*longvme2 = 0xd00;
	usec = 200;
	usleep(usec);
	*longvme1 = 0x8001;
	*longvme2 = 0xe00;
	usec = 200;
	usleep(usec);
	*longvme1 = 0x8001;
	*longvme2 = (unsigned short) pipline;
	usec = 200;
	usleep(usec);	
	*longvme1 = 0x8001;
	 *longvme2 = 0xf00;
	usec = 200;
	usleep(usec);	
	*longvme1 = 0x8001;
	*longvme2 = 0x990;
	usec = 200;
	usleep(usec);
	 *shortvme = 0x40;
	usec = 200;
	usleep(usec);	
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
				printf(" data word= %x  %x  %x\n",
				sd.bits.data1,sd.bits.data2,sd.bits.data3); 
				fprintf(fp," data word= %x  %x  %x\n",
				sd.bits.data1,sd.bits.data2,sd.bits.data3);
				fdata[3*kk+1][ii] = sd.bits.data1;
				fdata[3*kk+2][ii] = sd.bits.data2;
				fdata[3*kk+3][ii] = sd.bits.data3;
				
		
		}
	 else 
	 {
	 printf("control word= %x\n", word); 
	 fprintf(fp,"control word= %x\n", word); }}
	printf("no data\n");
	return;
       }
	 


