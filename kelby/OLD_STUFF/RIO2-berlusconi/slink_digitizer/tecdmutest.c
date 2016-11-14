/*****************************************************************/
/*								 */
/* Send test data to the TEC DMUs on the TileCal digitizer board */
/* the source is a Stockholm digitizer with SLINK readout card.  */
/*								 */
/* The program has no exception/exit handlers ==> uio may not	 */
/* be cleaned correctly ....					 */
/*								 */
/*****************************************************************/

#include <stdio.h>
/* #include <unistd.h> */
#include <mem.h>
#include <time.h>
#include <ces/uiocmd.h>

#include "atdaq_types.h"
#include "s5933lib.h"
#include "SLINK.h"
#include "ttcvi.h"
#include "tecdmutest.h"

/* max buffer size in words */
#define MAX_BUF_SIZE 9000
#define SSP_IN_REG  imb[3]
#define SSP_OUT_REG omb[0]


int main(int argc, char **argv)
{

uio_mem_t buf_desc;
char *pci_addr, *virt_addr, *s_addr;
register volatile s5933_regs *regs;

int ipacket, npackets;
int page_size;
int packet_size;
int code, status,index;

int dma,print,Chk,nerrors,nbadframes;
int tecdmu,ntecdmus,position;
int i, w;
unsigned int wd1, wd2, wd3, one;
long *p, data[100];
unsigned long int dume1;
char cc[10];
digi_datawd dataword;
digi_testwd testword;
long start_time,end_time;
int delta_time;
float t_per_packet, packet_per_s, mb_per_s;

FILE *fframe;

SLINK_parameters slink_parameters;
SLINK_device *dev;
if(argc <= 1)
   {
    printf("Usage: %s <device number> (e.g tecdmutest 1)\n",argv[0]);
    exit(0);
   }
sscanf(argv[1],"%d",&index);
if(index <=0)
   {
   printf(" %s got an invalid index number\n",argv[0]);
   exit(0);
   }
index--;

#ifdef DEBUG
printf("Initializing TTCVI board \n");
#endif

   if (map_vme()) {
      printf("Error mapping VME....");
   }

   ttcrx_base = TTCVI_DIG_BASEADDR;

   setup_ttcvi_mapping();

   init_digitizer();

#ifdef DEBUG
printf("Digitizer board Initialized\n");
#endif

/* Initializes the SLINK Structure */
  SLINK_InitParameters(&slink_parameters);
  slink_parameters.position = index;  /* this will indicate the occurence of the device */

  slink_parameters.start_word = 0x00000000;
  slink_parameters.stop_word  = 0xFFFFFFF0;

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
  printf("Enter the number of iterations: ");
  scanf( "%d", &npackets);

  printf(" use DMA (Y/N)?\n");
  scanf("%s",cc);
  if((cc[0]=='Y') || (cc[0]=='y'))
      dma=1;
  else 
      dma=0;

  if(dma)
     s_addr = pci_addr;
  else
     s_addr = virt_addr;
  
  printf(" print the received packets? (Y/N)\n");
  scanf("%s",cc);
  if((cc[0]=='Y') || (cc[0]=='y'))
    print=1;
  else
    print=0;

  printf("Save the output to a file? (Otherwise, print to screen) \n");
  scanf("%s",cc);
  if((cc[0]=='Y') || (cc[0]=='y')) {
     Chk=1;
     fframe = fopen("tectest_out","w");
     nbadframes = 0;
  }
  else
    Chk=0;  

  /* Main loop */
  time(&start_time);
  one = 1;
  ntecdmus = 6;
 
  for( ipacket = 1; ipacket <= npackets; ipacket++) {

    if(Chk)
      {
        fprintf(fframe,"Packet %8x : \n",ipacket);
      } 

  for( tecdmu = 0; tecdmu < ntecdmus; tecdmu++) { 

    if (Chk) 
     {
      fprintf (fframe,"TEC DMU %2x \n",tecdmu);
     }
    else 
     {
      printf ("TEC DMU %2x  \n",tecdmu);
     }
    
  for( position = 0; position < 20 ; position++) {

    testword.full = 0;
    
/* Select the tecdmu to look at */

    testword.full += (tecdmu << 20);

/* Set the test pattern...running ones */

    testword.full += (one << position);

    wd1 = testword.bits.word1;
    wd2 = testword.bits.word2;
    wd3 = testword.bits.word3;

    ttcvi_test_word(wd1, wd2, wd3);
    ttcvi_test_word(wd1, wd2, wd3);
    ttcvi_test_word(wd1, wd2, wd3);

    reset_digitizer();
    reset_digitizer();

    usleep(1);

/* Send the Lvl1 Accept from the TTCvi */

    ttcvi_send_l1a();

    if(dma){
       s_addr = pci_addr;}
    else{
       s_addr = virt_addr;}
    
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
	exit(0);
      }

    } while( status != SLINK_FINISHED);

    packet_size = SSP_PacketSize( dev); 
/* reset the DMA counter */
    regs->mwtc = 0;
/* Reading Out the Packet */

    p = (long *)virt_addr;
    w = packet_size >>2;
    for (i=0;i<w;i++)
     {
      dume1 = *p;
      dataword.full = dume1;
      if (Chk) 
       {
        fprintf (fframe,"%8x ",dataword.bits.reading);
       }
      else 
       {
        printf ("%8x ",dataword.bits.reading);
       }
      p++;
     }
   
    if(Chk)
     {
       fprintf (fframe,"\n");
     }
    else
     {
       printf ("\n");
     }
     
     
  } /* end of for position */     
   
  } /* end of for tecdmu */     

  } /* end of for ipacket */

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
    printf(" time/iteration  = %7.1f microseconds\n",t_per_packet);
    printf(" iterations/s    = %7.1f packet/s \n",packet_per_s);
    printf(" Mbyte/s         = %7.1f\n",mb_per_s);
  }

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
  }

  return 0;
}
