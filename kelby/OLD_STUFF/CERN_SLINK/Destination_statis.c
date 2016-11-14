/****************************************************************/
/*								*/
/* Measure S-link performance at the destination SSP		*/
/* the source could be any SLINK source  including SLIDAS       */
/*								*/
/* The program has no exception/exit handlers ==> uio may not	*/
/* be cleaned correctly ....					*/
/*								*/
/* 9706      M.Niculescu					*/
/*           Version 1.0					*/
/* 970723    JOP :						*/
/*           improve RUN/STEP programming			*/
/*           restructure and clean				*/
/*           add statistics					*/
/* 970820    M.Niculescu:                                       */
/*           Modify in order to allow the work with one of the  */
/*           occurence of the SSP interface found on PCI bus.   */
/*           The input parameters, requested by the program is  */
/*           the occurence number                               */ 
/* 970826    M.Niculescu:                                       */
/*           After SSP interface is open check if SLINK is down */ 
/*           and if it is dowan call LDC_reset                  */
/****************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <mem.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <ces/uiocmd.h>

#include "atdaq_types.h"
#include "s5933lib.h"
#include "SLINK.h"

/* max buffer size in words - SLIDAS generates max. 8k words */
#define MAX_BUF_SIZE 9000
#define SSP_IN_REG imb[3]

void push_run (SLINK_device *dev);
int imode;

/* signal handler for SIGQUIT */
void sigquit_handler(int signum)
{
  printf("sigquit_handler: seen SIGQUIT \n");
  imode=1;
}

int main(int argc, char **argv)
{

uio_mem_t buf_desc;
char *pci_addr, *virt_addr, *s_addr;
register volatile s5933_regs *regs;

int ipacket, npackets;
int page_size;
int packet_size;
int code, status,index;
struct sigaction sa;

int dma;
char cc[10];

long start_time,end_time;
int delta_time;
float t_per_packet, packet_per_s, mb_per_s;

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

  /* install signal handler for SIGQUIT */
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=0;
  sa.sa_handler=sigquit_handler;

  /* dont block any signal in intercept handler */
  if (sigaction(SIGQUIT,&sa,NULL)<0)
  {
    printf("Install sigaction SIGQUIT\n");
    code=errno;
    if(code==EFAULT) printf("EFAULT\n");
    if(code==EINVAL) printf("EINVAL\n");
    exit(1);
  }
  printf(" SIGQUIT handler installed\n");
  printf(" Press ctrl+\\ to output statistics\n");
  imode=0;

/* Initializes the SLINK Structure */
  SLINK_InitParameters(&slink_parameters);
  slink_parameters.position = index;  /* this will indicate the occurence of the device */
#ifdef SLIDAS
  slink_parameters.start_word = 0xB1000000;
  slink_parameters.stop_word = 0xE1000000;
#endif

#ifdef DEBUG
  printf(" slink_parameters structure : \n");
  printf("%d %d %d %d 0x%x 0x%x \n",
         slink_parameters.data_width, slink_parameters.position, 
         slink_parameters.timeout,    slink_parameters.byte_swapping,
         slink_parameters.start_word, slink_parameters.stop_word);
#endif
 
  if((status = uio_open()))
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

  /* Opens and  resets SSP ; if SLINK is LDOWN reset SLINK itself*/
  if ((code = SSP_Open( &slink_parameters, &dev)) != SLINK_OK )
  {
    printf(" SSP_Open failed with error = %d\n",code);
    SLINK_PrintErrorMessage(code);
    exit(0);
  }

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
  
#ifdef SLIDAS
/* start run on SLIDAS */
  push_run(dev);
#endif
  printf(" The program will input an infinite number of packets \n");
  printf(" Enter Ctrl + \\ to show statistics \n");
  /* Main loop */
  time(&start_time);
  npackets=0;
  ipacket = 0;
  for(; ;) {
     
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
  /* Loads the address of registers */
  regs = dev->regs;
  /* reset the DMA counter */
  regs->mwtc = 0;
    ipacket++;
    if(imode==1)
    {
    imode =0;
    if(start_time)
        {
        time(&end_time);
        delta_time = end_time-start_time;
        if(delta_time == 0 ) continue;
        printf("\n\n");
        printf(" # delta packets transferred = %d\n",ipacket);
        printf(" packet size (bytes)   = %d\n",packet_size);
        printf(" time elapsed          = %d seconds\n",delta_time);
        t_per_packet = ((float)delta_time*1000000)/ipacket;
        packet_per_s = (float)1000000/t_per_packet;
        mb_per_s = ((float)ipacket*packet_size)/((float)delta_time*1024*1024);
        printf("\n");
        printf(" time/packet = %7.1f microseconds\n",t_per_packet);
        printf(" packet/s    = %7.1f packet/s \n",packet_per_s);
        printf(" Mbyte/s     = %7.1f\n",mb_per_s);
        } 
    time(&start_time); 
    ipacket = 0;
    }

  } /* end of for ipacket */


  if( (code = SSP_Close( dev) != SLINK_OK) ) {
    printf(" SSP_Close : error = %d\n", code);
    SLINK_PrintErrorMessage( code);
    exit(0);
  }

  uio_close();
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
register volatile s5933_regs *s5933_regs;

  s5933_regs = dev->regs;
  dev->specific.ssp.out.bit.URLs = 0; /* set to 0  - keep the other bits */
  s5933_regs->omb[0].all = dev->specific.ssp.out.all;
  usleep(10);
  dev->specific.ssp.out.bit.URLs = 1; /* a transition 0-->1 on URL0 */
  s5933_regs->omb[0].all = dev->specific.ssp.out.all;
  usleep(10);
  dev->specific.ssp.out.bit.URLs = 0; /* set to 0  - keep the other bits */
  s5933_regs->omb[0].all = dev->specific.ssp.out.all;

}
