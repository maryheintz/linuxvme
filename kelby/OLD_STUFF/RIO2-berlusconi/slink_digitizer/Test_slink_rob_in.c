/* file : test_iom_2.c
   960822  J.Petersen ECP
           test I/O module input thread
   960824  add some locate & remove tests
   960826  add time stamps
   960828  introduce scheduler
   960829  introduce output thread 
   960830  simplify output code 
   960910  add event data 
   960910  go faster  : no longer keyboard driven
   960911  add an (optional) TM thread
   960914  use CORBO to drive the input 
   960918  restructure a bit 
   961009  dummy output thread to simplify
           write event count and vme address into global VME memory.
   961009  add code for TM messages
   961023  msg ACK to avoid VME bug
   961023  poll on space available on input 
   961026  add SRAM configuration & time stamping area
   961029  remove CORBO triggers again - input at full rate  
           slow down a bit ...
   961106  F.Pennerath ECP
           adds S-LINK in the input and output threads
   961113  uses the PCI address field in the event descriptor
           instead of a kind of bricolage to get them.
   961120  Removes the S5933_DMA_Read call. Initializes DMA by itself
   961121  PCI time stamping for input and output threads
   961122  Reorganisation of defines for DEBUG
           Adds dummy input and dummy output features
   970620  Michaela
           Reorganize the code
           Adds the facility to work with SLIDAS
   970820  Michaela
           After SSP_Open tests if SLINK is down and if so reset the SLINK
   970903  Michaela
           Allow to work with the x occurence of the SSP interface. The occurence
           number is an input parameter of the program
   971014  Michaela
           clean up the code
   971016  Michaela
           Start SLIDAS's  data generation in init_input
           Stop SLIDAS's  data generation in IOModOutput by:
           - a rising edge of a pulse
           - reseting SSP interface
  971016   Michaela
           - The starting address of each packet received from SLINK is EvDEsc_in->d_ptr
              No place allocated for the RobHeader
          -  For data simulation with SLIDAS or SLATE, in IOModInput, do initialisation of
             each received packet: evId, evType, dtType

*/


/* ATTENTION, this program works with: - fixed nr. of pages: 100
                                       - fixed nr of classes : 16
                                       - fixed nr. of events in classes : 16 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <smem.h>
#include <time.h>

#include "iomdaq.h"
#include "sram.h"
#include "iomsched.h"

#include <ces/rtpc8067.h>
#include <ces/uiocmd.h>

#include "atdaq_types.h"
#include "pcilib.h"
#include "s5933lib.h"
#include "SLINK.h"

#define SSP_IN_REG imb[3]

/* # messages about the same as # of events in ROB buffer .... */

#ifdef DEBUG_PCI
#define time_stamp(AD,DT) *(sram_time + AD) = DT
#else
#define time_stamp(AD,DT)
#endif

/* local functions */
enum sram_errors SRAM_Init(char** sram_add);
enum sram_errors SRAM_Close(char* sram_add);
void DAQ_Get_Params(void);
void DAQ_Init(void);
void DAQ_Close(void);
void DAQ_Dump(void);

void DAQ_exit_handler(void);

void IOMSched_Init(void);

int getdec(void);
char getfstchar(void);

void push_run (SLINK_device *);

/* IOM threads */
void IOModInput(void *, int);
int init_input(void);
int poll_input(void);
void close_input(void);

void IOModOutput(void *, int);
int init_output(void);
int poll_output(void);
void close_output(void);

/********************************/

int nevents, nevents_in_LC;
int npages,pagesize,nclasses;

int event_no_in;
int event_no_out;

char ich;

time_t tintv;

int MA_openflag = 0;
int LC_openflag = 0;
int IOM_openflag = 0;

/* Input thread variables */

SLINK_parameters ssp_params;
SLINK_device *ssp_device;

enum {
  INPUT_WAITFORPAGE,
  INPUT_WAITFOREVENT
} input_state;

/* Output thread variables */

SLINK_parameters sps_params;
SLINK_device *sps_device;

enum {
  OUTPUT_NOTBUSY,
  OUTPUT_BUSY
} output_state;

/* local SRAM variables */
char* sram_add;
unsigned int* sram_time;
unsigned int sram_data;
long start_time,end_time;
int index;

int main(int argc, char **argv) {

int c_stat;
if(argc <= 1)
   {
    printf("Usage: %s <device number> (e.g Test_slink_rob_in 1)\n",argv[0]);
    exit(0);
   }
sscanf(argv[1],"%d",&index);
if(index <=0)
   {
   printf(" %s got an invalid index number\n",argv[0]);
   exit(0);
   }
index--;

/* exit handler - dumps the structures */
  c_stat = atexit(DAQ_exit_handler);
  printf(" atexit : status = %d\n", c_stat);

/* initialise SRAM mapping */
  SRAM_Init(&sram_add);
  sram_time = (unsigned int*)(sram_add + SRAM_TIMING);
  printf(" LA of SRAM timing space = %x\n",(unsigned int)sram_time);

/* get DAQ parameters */
  DAQ_Get_Params();

  DAQ_Init();

  IOMSched_Init();

  printf(" run start ?");
  ich = getfstchar();

  IOM_scheduler();		/* wait for an IOM_Close ... */

#ifdef DUMP
  DAQ_Dump();
#endif
  
  exit(0);
} /* main */

/*
  --------------------------------------------------------------------------
  INPUT THREAD FUNCTIONS
  --------------------------------------------------------------------------
  */

/* Init part */

int init_input(void)
{
  int code;

  printf("INPUT : init function started\n");

  /* Initalizes the parameter structure for the SSP interface */
  SLINK_InitParameters(&ssp_params);
  ssp_params.position = index;  /* this will indicate the occurence of the SSP */ 
#ifdef SLIDAS
  ssp_params.start_word = 0xB1000000;
  ssp_params.stop_word = 0xE1000000;
#endif

  /* Opens the device */
  if( (code = SSP_Open( &ssp_params, &ssp_device)) != SLINK_OK) {
    SLINK_PrintErrorMessage( code);
    exit(0);
  }
  printf( "INPUT : The S-LINK input was correctly opened.\n");
#ifdef SLIDAS
#ifndef EXT_START
  /* start run on SLIDAS */
  printf("Now starting SLIDAS data generation... \n"); 
  push_run(ssp_device);
#endif
#endif
/*  Initializes the input thread state */
  input_state = INPUT_WAITFORPAGE;
  time(&start_time);
  return IOMSCHED_SUCCESS;
}

/* Close part */

void close_input(void)

{
  printf("INPUT : Close function started\n");

  /* Closes the S-LINK device  REMOVE IT BECAUSE it produces  seg.fault

  printf("INPUT : Removes the SSP device\n");
  if( (code = SSP_Close( ssp_device)) != SLINK_OK) {
    SLINK_PrintErrorMessage( code); 
  }
  ssp_device = NULL;  */
}

/* Polling part */

int poll_input(void)
{

  /* Is any space free  ? */

  if (MA_IsFreePage()) 
      return(DEVICE_PENDING);
  else
      return(DEVICE_IDLE);
}

/* Action part */

void IOModInput(void *dummy1, int dummy2)
{
  static T_EvDesc* EvDesc_in;
  T_EventInputHeader* ev_hdr;
  int c_stat;
  int status;

time_stamp(0x4,0x11001100); /* just enter input thread */
  switch( input_state) {
  case INPUT_WAITFORPAGE :

#if defined(DEBUG_PCI) && defined(DEBUG_LCMA)
  *sram_time = 0x910;
#endif

      /* Gets the page */
      c_stat = MA_GetFreePage(&EvDesc_in);
#if defined(DEBUG_PCI) && defined(DEBUG_LCMA)
  *sram_time = 0x911;
#endif

      if(c_stat) {				/* inconsistency */
	printf("INPUT : after MA_GetFreePage, event # : %d status : %d\n",
	       event_no_in, c_stat);
	exit(0);
      }

      /* Initializes the S-LINK transfer */

#if defined(DEBUG) && defined(DEBUG_INPUT)
      printf("\nINPUT : Now reading transfer on a empty page.\n");
      printf("        PCI address = 0X%8X\n", (dword) EvDesc_in->pci_add);
#endif
      time_stamp(0x4,0x0011); /* now initialise the transfer of one event */
      if((c_stat = SSP_InitRead( ssp_device,
				 EvDesc_in->pci_add,
				 pagesize,
				 SLINK_ENABLE, 0)) != SLINK_OK) {
	SLINK_PrintErrorMessage( c_stat);
	exit(0);
      }
#if defined(DEBUG) && defined(DEBUG_INPUT)
      printf("INPUT : Transfer initialized\n");
#endif

      time_stamp(0x4,0x1111); /* SLINK transfer initialised */
      /* Changes of input state */
      input_state = INPUT_WAITFOREVENT;

#if defined(DEBUG_PCI) && defined(DEBUG_INPUT)
    *sram_time = 0x12;
#endif

  case INPUT_WAITFOREVENT :

    /* Performs the S-LINK transfer */
    time_stamp(0x4,0x2211); /* do transfer */
    if(( c_stat = SSP_ControlAndStatus( ssp_device, &status)) != SLINK_OK) {
      SLINK_PrintErrorMessage(c_stat);
      exit(0);
    }
    /* If it is needed another page to continue data transfer of the current packet */
    if(status == SLINK_NEED_OF_NEW_PAGE)
      {
      printf("\n I am going out because I need another page ..........\n");
      printf(" Until now I read %d bytes \n",SSP_PacketSize( ssp_device));
      exit(0);
      }
    /* If the transfer is finished */
    if( status == SLINK_FINISHED) {
#ifdef DEBUG
	printf("Now transfer is finished, construct an event...\n");
#endif
      /* now construct an event ED + data ... */
      time_stamp(0x4,0x3311); /* transfer of the current package is finished */
      EvDesc_in->gid = event_no_in;

      /* Gets the size and stores it in the descriptor */
      EvDesc_in->size = SSP_PacketSize( ssp_device);
#ifdef DEBUG
	printf("Got the size %d\n",EvDesc_in->size);
#endif

      /* event input header */
      /*ev_hdr = (T_EventInputHeader *)
	((char *)EvDesc_in->d_ptr+sizeof(T_RobHeader)); WRONG !!!!*/
      ev_hdr = (T_EventInputHeader *)((char *)EvDesc_in->d_ptr); /* HERE is the address of the new received packet */
      ev_hdr->evId = event_no_in; /* for the moment because the external device doesn't provide it */
      ev_hdr->evType = 7;			  
      ev_hdr->dtType = 10;        
      /* the ROB header is for later ... */
      time_stamp(0x4,0x4411); /*  the event is built */

      /* Adds the event in the event lists */

#if defined(DEBUG_PCI) && defined(DEBUG_LCMA)
  *sram_time = 0x920;
#endif
      c_stat = LC_Add(EvDesc_in);		/* add event to LC Class */
#if defined(DEBUG_PCI) && defined(DEBUG_LCMA)
  *sram_time = 0x921;
#endif
      if(c_stat) {
	printf("INPUT : after LC_Add, event # & cstat = %d %d\n",
	       event_no_in,c_stat);
	exit(0);				/* may be too hard ... */
      }
     time_stamp(0x4,0x5511); /* event added to the event list */
#if defined(DEBUG) && defined(DEBUG_INPUT)
      printf("INPUT : A new event was received.\n");
      printf("        GID  = %d\n", EvDesc_in->gid);
      printf("        Size = %d\n", EvDesc_in->size);
      dump_packet((long *)ev_hdr, EvDesc_in->size);
#endif

      /* Changes of thread state */
      input_state = INPUT_WAITFORPAGE;

      /* Next event */
      event_no_in++;
    }
time_stamp(0x4,0x11111111); /* mark the output of input thread */
    break;

  default :
    printf( "INPUT : unknown state\n");
    exit(0);
    break;
  }

}

/*
  --------------------------------------------------------------------------
  OUTPUT THREAD FUNCTIONS
  --------------------------------------------------------------------------
  */

/* Init part */

int init_output(void)
{
  printf("OUTPUT : Init function started\n");
  /* Initializes the output thread state */
  output_state = OUTPUT_NOTBUSY;

  return IOMSCHED_SUCCESS;
}

/* Close part */

void close_output(void)
{
  printf("OUTPUT : Close function started\n");
}

/* Polling part */

int poll_output(void)
{
  /* Output thread is busy ? (i.e it's sending a packet)
   If true, does the work */

  if( output_state == OUTPUT_BUSY)
    return( DEVICE_PENDING);

  /* Every events were received ? yes -> output them */
  if( event_no_in >= nevents)
    return( DEVICE_PENDING);

  /* Are there too many events in LC ? Yes -> output them */
  if( event_no_in - event_no_out > nevents_in_LC)
    return( DEVICE_PENDING);

  /* Do nothing */
  return( DEVICE_IDLE);
}

/* Action part */

void IOModOutput(void *dummy1, int dummy2)
{
  static int c_stat;
  static T_EvDesc* EvDesc_out;
  int delta_time;
  float t_per_packet, packet_per_s, mb_per_s;

  time_stamp(0x4,0x22002200); /* just enter output thread */
  switch( output_state) {
  case OUTPUT_NOTBUSY :
    /* We have to send the next event */

#if defined(DEBUG) && defined(DEBUG_OUTPUT)
    printf("OUTPUT : Removes from classes the gid = %d\n", event_no_out);
#endif

#if defined(DEBUG_PCI) && defined(DEBUG_LCMA)
  *sram_time = 0x930;
#endif
    /* Removes the event from the classes */
    c_stat = LC_RemoveByGid( event_no_out, &EvDesc_out);
#if defined(DEBUG_PCI) && defined(DEBUG_LCMA)
  *sram_time = 0x931;
#endif
    if (c_stat != LC_OK) {
      printf( "OUTPUT : gid = %d   LC_Remove status = %d\n",
	      event_no_out,c_stat);
      exit(0);
    }

#if defined(DEBUG) && defined(DEBUG_OUTPUT)
    printf( "OUTPUT : Outputs the event %d\n", EvDesc_out->gid);
    printf( "         PCI address = 0x%8X\n", (dword) EvDesc_out->pci_add);
    printf( "         Size        = %d\n", EvDesc_out->size);
#endif

    /* Go to the BUSY state */
    output_state = OUTPUT_BUSY;

#if defined(DEBUG_PCI) && defined(DEBUG_OUTPUT)
    *sram_time = 0x22;
#endif

    /* !!! There is intentionally no break instruction, in order to
       start the transfer */

  case OUTPUT_BUSY :
    /* We have to continue the S-LINK transfer */

#if defined(DEBUG_PCI) && defined(DEBUG_LCMA)
  *sram_time = 0x940;
#endif
      /* Returns the page to the free pages stack */
      c_stat = MA_ReturnFreePage(EvDesc_out);
#if defined(DEBUG_PCI) && defined(DEBUG_LCMA)
  *sram_time = 0x941;
#endif

#if defined(DEBUG) && defined(DEBUG_OUTPUT)
      printf("OUTPUT : The event %d was output and deleted\n",
	     event_no_out);
#endif
      /* Increments the counter */
      event_no_out++;

      /* Was it the last event to send ? */
      if (event_no_out >= nevents) {
      time(&end_time);
  delta_time = end_time-start_time;
  printf("\n\n");
  printf(" # packets transferred = %d\n",event_no_out);
  printf(" packet size (bytes)   = %d\n", EvDesc_out->size);
  printf(" time elapsed          = %d seconds\n",delta_time);
  if (delta_time > 0)
  {
   t_per_packet = ((float)delta_time*1000000)/event_no_out;
   packet_per_s = (float)1000000/t_per_packet;
   mb_per_s = ((float)(event_no_out)*(EvDesc_out->size))/((float)delta_time*1024*1024);
   printf("\n");
   printf(" time/packet = %7.1f microseconds\n",t_per_packet);
   printf(" packet/s    = %7.1f packet/s \n",packet_per_s);
   printf(" Mbyte/s     = %7.1f\n",mb_per_s);
  }

#ifdef SLIDAS
#ifndef EXT_START
/* stop run on SLIDAS  */
 printf("Now stoping SLIDAS data generation \n"); 
 push_run(ssp_device);
 SSP_Reset(ssp_device);
#endif
#endif
   c_stat = IOM_schedulerClose();
   printf(" IOM_schedulerClose status : %d\n",c_stat);
   return;
   }

      /* Go to the NOTBUSY state */
      output_state = OUTPUT_NOTBUSY;

   time_stamp(0x4,0x22222222); /* end of output thread */ 
   break;

  default :
    printf( "OUTPUT: Unknown state\n");
    exit(0);
    break;
  }
}

enum sram_errors SRAM_Init(char** sram_add)
{
  *sram_add = (char*)smem_create(SRAM_SG ,(char*)SRAM_MEM,
                                SRAM_SIZE,SM_WRITE | SM_READ);
  if (*sram_add == 0) {
    printf("SRAM_Init : cannot create SRAM segment\n");
    exit(0);
  }
}

enum sram_errors SRAM_Close(char* sram_add) {
int s_stat;

  s_stat = (int)smem_create(SRAM_SG,sram_add,SRAM_SIZE,SM_DETACH);
  smem_remove(SRAM_SG);

}

void DAQ_Get_Params(void) 
{

  /* printf(" # pages : ");
  npages = getdec(); */
  npages = 100;

  printf(" page size (bytes) : ");
  pagesize = getdec();

  /* printf(" # local ID classes ( < # pages) : ");
  nclasses = getdec(); */
  nclasses = 16;

  printf(" # events to be produced : ");
  nevents = getdec();

  /* printf(" # events to have in classes : ");
  nevents_in_LC = getdec(); */
  nevents_in_LC = 16;


}

void DAQ_Init(void)
{
  int c_stat;

  /* memory allocation */
  c_stat = MA_Open(npages,pagesize);
  if(c_stat) {
    printf(" after MA_Open : c_stat = %d\n",c_stat);
    exit(0);
  }
  else {
    printf(" MA opened OK \n");
    MA_openflag = 1;
  }

  /* Local ID classes */
  c_stat = LC_Init(nclasses);
  if(c_stat) {
    printf(" after LC_Init : c_stat = %d\n",c_stat);
    exit(0);
  }
  else {
    printf(" LC opened OK\n");
    LC_openflag = 1;
  }
}

void DAQ_Close(void)
{
  int c_stat;

  c_stat = LC_Close();
  LC_openflag = 0;
  if(c_stat) {
    printf(" LC_Close : c_stat = %d\n",c_stat);
  }

  c_stat = MA_Close();
  MA_openflag = 0;
  if(c_stat) {
    printf(" MA_Close : c_stat = %d\n",c_stat);
  }

}

void IOMSched_Init(void) {

/* int up = 7; */              /* user parameter for input function ( dummy) */
int prior = 10;                 /* dummy */
int thread_id[10];
int c_stat;
int j,k;

  c_stat = IOM_schedulerOpen();
  if (c_stat != IOMSCHED_SUCCESS) {
    printf("Status from IOM_schedulerOpen : %d\n", c_stat);
    exit(0);
  }
  else {
    printf(" IOM Scheduler opened OK\n");
    IOM_openflag = 1;
  }

  k = 1;
  j=7;
  c_stat = IOM_schedulerAdd(k, IOModInput, (void *)j, 
                            init_input, poll_input, close_input,
			    prior,"in", &thread_id[k]);
  if (c_stat != IOMSCHED_SUCCESS) {
    printf(" IOM_schedulerAdd status = %d\n",c_stat);
    exit(0);
  }
  else {
    printf("IOM_SchedulerAdd : Entry added at %d\n", thread_id[k]);
  }

  k = 2;
  j=8;
  c_stat = IOM_schedulerAdd(k, IOModOutput, (void *)j, 
                            init_output, poll_output, close_output,
			    prior,"out", &thread_id[k]);
  if (c_stat != IOMSCHED_SUCCESS) {
    printf(" IOM_schedulerAdd status = %d\n",c_stat);
    exit(0);
  }
  else {
    printf("IOM_SchedulerAdd : Entry added at %d\n", thread_id[k]);
  }

  IOM_schedulerDump(0);

}

void DAQ_Dump(void)
{
  if (LC_openflag) {
    LC_DumpClassLists(0, nclasses-1);
  }
  else {
    printf(" LC not open\n");
  }

  MA_DumpPageArray();

}

void DAQ_exit_handler(void)
{
  printf(" DAQ_exit_handler : \n");
  if (MA_openflag) {
    MA_DumpPageArray();
  }
  else {
    printf(" MA not open\n");
  }

  if (LC_openflag) {
    LC_DumpClassLists(0, nclasses-1);
  }
  else {
    printf(" LC not open\n");
  }

  DAQ_Close();
  SRAM_Close(sram_add);

  /* Closes threads */

  close_input();
  close_output();

/* EOR summary */
  printf(" # events in   = %d 0x%x\n",event_no_in,event_no_in);
  printf(" # events out  = %d 0x%x\n",event_no_out,event_no_out);

}

int getdec(void)  /*  get ONE decimal integer */
{
        char sbuf[10];   /* max 9 chars */
        int nfield,nint;

        do {
                fgets(sbuf,10,stdin);
                nfield = sscanf(sbuf,"%d",&nint);
                if (nfield<1) printf(" ??? : ");
           } while (nfield<1);

        return(nint);
}

char getfstchar(void)  /*  get first character */
                   /* simpler with int !! */
{
        char sbuf[10];   /* max 9 chars */

                fgets(sbuf,10,stdin);

        return(sbuf[0]);
}

void dump_packet(long *ptr, int size)
{
long *p;
int w,i;
      w = size >>2;
      p = (long *) ptr;
      printf("\n\n");
      for(i=0;i<w;i++)
      {
        printf("%8x",(unsigned int)*p);
        p++;
        if((i+1)%8==0)
          printf("\n");
      }
}


/****************************************************************/
/*                                                              */
/* This function is equivalent to pushing the RUN/STEP toggle   */
/* switch on the SLIDAS                                         */
/*                                                              */
/* the transition from 0 to 1 is equivalent to pushing the RUN  */
/* button - we don't know whether it will START or STOP !!      */
/*                                                              */
/* NB! operates on globals of SLINK library ......              */
/*                                                              */
/* should be put into a SLIDAS library some day ??              */
/*                                                              */
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

