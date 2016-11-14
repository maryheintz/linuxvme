/****************************************************************/
/*								*/
/* Measure S-link performance at the destination SSP		*/
/* the source could be any SLINK source  including SLIDAS       */
/*								*/
/* The program has no exception/exit handlers ==> uio may not	*/
/* be cleaned correctly ....					*/
/*								*/
/****************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <mem.h>
#include <time.h>
#include <ces/uiocmd.h>

#include "atdaq_types.h"
#include "s5933lib.h"
#include "SLINK.h"
#include "ttcvi.h"

/* max buffer size in words - SLIDAS generates max. 8k words */
#define MAX_BUF_SIZE 9000
#define SSP_IN_REG  imb[3]
#define SSP_OUT_REG omb[0]

void push_run (SLINK_device *dev);

int main(int argc, char **argv)
{

uio_mem_t buf_desc;
char *pci_addr, *virt_addr, *s_addr;
register volatile s5933_regs *regs;

int ipacket, npackets;
int page_size;
int packet_size;
int code, status,index;

int dma,print,Chk,nerrors,nbadframes,n_wrong_samples=0;
int ndpar_errors=0,ntpar_errors=0,read_errors=0;
int tec_errors=0,db_errors=0,sb_errors=0;
int i,j,w,parity,parity_bit;
long *p, data[10000];
char cc[10];

long start_time,end_time;
int delta_time;
float t_per_packet, packet_per_s, mb_per_s;

FILE *fframe;

SLINK_parameters slink_parameters;
SLINK_device *dev;
if(argc <= 1)
   {
    printf("Usage: %s <device number> (e.g Destination 1)\n",argv[0]);
    exit(0);
   }
sscanf(argv[1],"%d",&index);
if(index <=0)
   {
   printf(" %s got an invalid index number\n",argv[0]);
   exit(0);
   }
index--;

#ifdef _DIGITIZER_
/* Initializes TTCvi */

#ifdef DEBUG
printf("Initializing TTCVI board \n");
#endif

   if (map_vme()) {
      printf("Error mapping VME....");
   }

   setup_ttcvi_mapping();

  init_digitizer();

#ifdef DEBUG
printf("Digitizer board Initialized\n");
#endif

#endif

/* Initializes the SLINK Structure */
  SLINK_InitParameters(&slink_parameters);
  slink_parameters.position = index;  /* this will indicate the occurence of the device */

#ifdef _DIGITIZER_
  slink_parameters.start_word = 0x00000000;
  slink_parameters.stop_word  = 0xFFFFFFF0;
#endif
  
#ifdef SLIDAS
  slink_parameters.start_word = 0xB1000000;
  slink_parameters.stop_word =  0xE1000000;
#endif

#ifdef DEBUG
  printf(" slink_parameters structure : \n");
  printf("%d %d %d %d 0x%x 0x%x \n",
         slink_parameters.data_width, slink_parameters.position, 
         slink_parameters.timeout,    slink_parameters.byte_swapping,
         slink_parameters.start_word, slink_parameters.stop_word);
#endif
 
  if( (status = uio_open()) )
  {
    printf("uio_open failed with status =%d",status);
    exit(0);
  }

  /* allocate a contiguous buffer, which will be used to receive all packets */
  if((status=uio_calloc(&buf_desc, MAX_BUF_SIZE*sizeof(int))))
  {
    printf("uio_calloc failed with status = %d",status);
    exit(0);
  }
  pci_addr = (char *)(buf_desc.paddr|0x80000000);
  page_size = buf_desc.size;
  virt_addr = (char *)buf_desc.uaddr; /* virtual address */

  printf(" PCI address of data buffer = 0x%08x\n",pci_addr);
  printf(" Virtual address of buffer  = 0x%08x\n", virt_addr);
  printf(" Buffer size in bytes       = %d\n",buf_desc.size);


  /* Opens and  resets SSP ; only if SLINK is LDOWN reset SLINK itself */
  if ((code = SSP_Open( &slink_parameters, &dev)) != SLINK_OK )
  {
    printf(" SSP_Open failed with error = %d\n",code);
    SLINK_PrintErrorMessage(code);
    exit(0);
  }
  /* Loads the address of registers */
  regs = dev->regs;

  printf("\n"); 
  printf( " Number of packets to read ? ");
  scanf( "%d", &npackets);

  printf(" use DMA (Y/N)?\n");
  scanf("%s",cc);
  if((cc[0]=='Y') || (cc[0]=='y'))
      dma=1;
  else 
      dma=0;

  if(dma)
     s_addr=pci_addr;
  else
     s_addr = virt_addr;
  
  printf(" print the received buffer? (Y/N)\n");
  scanf("%s",cc);
  if((cc[0]=='Y') || (cc[0]=='y'))
    print=1;
  else
    print=0;

  printf(" Do you want to Save the first packet and Compare it with the incoming
  packets?? \n");
  scanf("%s",cc);
  if((cc[0]=='Y') || (cc[0]=='y')) {
     Chk=1;
     fframe = fopen("Reference_frame.dat","w");
     nbadframes = 0;
  }
  else
    Chk=0;  

#ifdef SLIDAS
/* start run on SLIDAS */
  push_run(dev);
#endif

  /* Main loop */
  time(&start_time);

  for( ipacket = 1; ipacket <= npackets; ipacket++) {

#ifdef _DIGITIZER_
/* Sending the Lvl1 Accept from the TTCvi */
    ttcvi_send_l1a();
#endif
    if(dma)
       s_addr = pci_addr;
    else
       s_addr = virt_addr;
    
    if( (code = SSP_InitRead( dev,s_addr, page_size, dma,0) != SLINK_OK) )
    {
    printf("SSP_InitRead : error %d = \n",code);
    SLINK_PrintErrorMessage( code);
    exit(0);
    }
#ifdef DEBUG
    printf("DMA/CPU transfer was initialised, with %d retcode\n",code);
#endif

    do {
#ifdef DEBUG
      printf("** reading a packet %d **\n",ipacket);
#endif
      if( (code = SSP_ControlAndStatus( dev, &status) != SLINK_OK) ) 
      {
        printf("SSP_ControlAndStatus : error = %d\n",code);
        SLINK_PrintErrorMessage( code);
	/*exit(0);*/
      }
      /* if(dev->transfer_status == SLINK_WAITING_FOR_SYNC)
          printf("Warning no packets on SLINK !!!!\n"); */


    } while( status != SLINK_FINISHED);

    packet_size = SSP_PacketSize( dev); 
  /* reset the DMA counter */
     regs->mwtc = 0;
/** Printing Out the Packet */
    if(print)
    {
      w = packet_size >>2;
      p = (long *)virt_addr;
      printf("\n\n");
      printf(" packet # %d -- all in hexidecimal :\n",ipacket);
      printf("\n");
      for(i=0;i<w;i++)
      { 
        if ( ((*p & 0x3ff)==0x3ff) ) n_wrong_samples++;
     /* Calculating the parity bit for all the words*/
        parity =0;
        for(j=0;j<32;j++) parity += (((*p) >> j) & 0x1);
        parity_bit = (parity%2);
        if ( parity_bit ) ndpar_errors++;
        printf("%d ",parity_bit);

     /* Printing Out the data and decoding headers */
        if ( (*p & 0x80000000)==0 )
        {
	printf("%3x ",(*p & 0x3ff));
        printf("%3x ",((*p & 0xffc00 )>>10));
        printf("%3x ",((*p &0x3ff00000)>>20));
        } else {
        printf("%1x ",((*p &0x20000000)>>29));
        if ((*p &0x20000000)>>29) ntpar_errors++;
        printf("%1x ",((*p &0x10000000)>>28));
        if ((*p &0x10000000)>>28) read_errors++;
        printf("%1x ",((*p &0xe000000)>>25));
        if ((*p &0xe000000)>>25) tec_errors++;
        printf("%1x ",((*p &0x1000000)>>24));
        if ((*p &0x1000000)>>24) db_errors++;
        printf("%1x ",((*p &0x800000)>>23));
        if ((*p &0x800000)>>23) sb_errors++;
        printf("%1x ",((*p &0x700000)>>20));
        printf("%1x ",((*p &0xc0000)>>18));
        printf("%1x ",((*p &0x38000)>>15));
        printf("%1x ",((*p &0x4000)>>14));
        printf("%1x ",(*p &0xfff));
        }
        p++;
        /*if((i+1)%8==0)*/
          printf("\n");
      }
    }
/**** Saving the first packet and comparing it with the incoming ones */
   if(Chk)
    {
      w = packet_size >>2;
      if (ipacket == 1) {
        p = (long *)virt_addr;
        for(i=0;i<w;i++)
	 { 
	  fprintf(fframe,"%8x ",*p);
          data[i] = *p;
          p++;
	}
      } 
     else {
        nerrors = 0; 
        p = (long *)virt_addr;
        for(i=0;i<w;i++)
         { 
	  if ( data[i] != *p ) nerrors++;
          p++;
	 }
	if (nerrors){
	 printf(" The number of different words between the packet %d and the
	 first packet is : %d \n", ipacket, nerrors); 
         nbadframes++;
        }
     }
    }
   
        

  } /* end of for ipackets */

  time(&end_time);

  delta_time = end_time-start_time;
  printf("\n\n");
  printf(" # packets transferred = %d\n",npackets);
  printf(" packet size (bytes)   = %d\n",packet_size);
  printf(" time elapsed          = %d seconds\n",delta_time);
  if (delta_time > 0)
  {
    t_per_packet = ((float)delta_time*1000000)/npackets;
    packet_per_s = (float)1000000/t_per_packet;
    mb_per_s = ((float)npackets*packet_size)/((float)delta_time*1024*1024);
    printf("\n");
    printf(" time/packet = %7.1f microseconds\n",t_per_packet);
    printf(" packet/s    = %7.1f packet/s \n",packet_per_s);
    printf(" Mbyte/s     = %7.1f\n",mb_per_s);
  }

#ifdef SLIDAS
/* stop run on SLIDAS */  
  push_run(dev);
#endif
  printf("\n Now reset the SSP interface ...\n");
  SSP_Reset(dev); /* reset the SSP interface */

  if( (code = SSP_Close( dev) != SLINK_OK) ) {
    printf(" SSP_Close : error = %d\n", code);
    SLINK_PrintErrorMessage( code);
    exit(0);
  }

  uio_close();

  if (Chk) {
    fclose(fframe);
    printf("Number of bad frames was: %d\n",nbadframes);
    printf("Number of wrong samples was: %d\n",n_wrong_samples);
    printf("Errors Statistics: \n");
    printf("\t D-Parity Errors: %d \n",ndpar_errors);
    printf("\t T-Parity Errors: %d \n",ntpar_errors);
    printf("\t Read Errors: %d \n",read_errors);
    printf("\t Tec Errors: %d \n",tec_errors);
    printf("\t DB_Errors: %d \n",db_errors);
    printf("\t SB_Errors: %d \n",sb_errors);
  }

  return 0;
}

/****************************************************************/
/*								*/
/* This function is equivalent to pushing the RUN/STEP toggle	*/
/* switch on the SLIDAS						*/
/*								*/
/* the transition from 0 to 1 is equivalent to pushing the RUN	*/
/* button - we don't know whether it will START or STOP !!      */
/*								*/
/* NB! operates on globals of SLINK library ......		*/
/*								*/
/* should be put into a SLIDAS library some day ??		*/
/*								*/
/****************************************************************/
void push_run (SLINK_device *dev)
{
register volatile s5933_regs *regs;

  regs = dev->regs;
  dev->specific.ssp.out.bit.URLs = 0; /* set to 0  - keep the other bits */
  regs->omb[0].all = dev->specific.ssp.out.all;
  usleep(10);
  dev->specific.ssp.out.bit.URLs = 1; /* a transition 0-->1 on URL0 */
  regs->omb[0].all = dev->specific.ssp.out.all;
  usleep(10); 
  dev->specific.ssp.out.bit.URLs = 0;  /* set to 0  - keep the other bits  */
  regs->omb[0].all = dev->specific.ssp.out.all; 

}