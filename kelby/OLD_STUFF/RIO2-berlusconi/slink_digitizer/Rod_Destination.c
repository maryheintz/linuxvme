#include <stdio.h>
#include <unistd.h>
#include <mem.h>
#include <time.h>
#include <stdlib.h> 
#include <ces/uiocmd.h>
#include <ces/vmelib.h>

#include "atdaq_types.h"
#include "s5933lib.h"
#include "SLINK.h"
#include "Rod_Destination.h"
#include "ttcvi.h"
#define EVENT_FILE "/home/tilecal/event_file.dat"

/*#define DEBUG*/
void push_run (SLINK_device *dev);




main(int argc, char **argv){
/* Virtual-Physical PCI descriptors*//*CES provided*/
uio_mem_t slink_PCIdesc;
uio_mem_t slvVME_PCIdesc;

/*Virtual-Physical PCI pointers*/
char *slvVME_PCIaddr, *slink_PCIaddr;
char *slvVME_VIRTaddr,*slink_VIRTaddr;
char *vme_virt, *slink_virt;
int slink_PageSize,slvVME_PageSize;
long *dump_address;

/* Slink to PMC registers */
register volatile s5933_regs *regs;

/*SLINK variables*/
SLINK_parameters slink_parameters;
SLINK_device *dev;

/*VME Descriptor Params*/
struct pdparam_slave slvVME_VMEdesc;
word32 vme_addr;

/*ROD variables*/
ReadOutParams ReadOut;
Event *event;
EventResources event_resources;
FILE *data_file;

/*Control variables*/
int ipacket, npackets, ii;
int packet_size;
int code, status,i,j;
long start_time,end_time;
int delta_time;
float t_per_packet, packet_per_s, mb_per_s;

/* Errors statistics variables */
int ndpar_errors=0,ntpar_errors=0,read_errors=0;
int tec_errors=0,db_errors=0,sb_errors=0;
int nbadframes,n_wrong_samples,w,parity,parity_bit;

/* Initializes the SLINK Structure */
 SLINK_InitParameters(&slink_parameters);
 slink_parameters.position = SLOT1;

 /*Initializes Resources Flags to free dynamic Memory */
 event_resources.item.all=ROD_FALSE;

#ifdef SLIDAS
 slink_parameters.start_word = 0xB1000000;
 slink_parameters.stop_word = 0xE1000000;
#endif
 
#ifdef _DIGITIZER_
  slink_parameters.start_word = 0x00000000;
  slink_parameters.stop_word  = 0xFFFFFFF0;
#endif

#ifdef DEBUG
 printf(" slink_parameters structure : \n");
 printf("%d %d %d %d 0x%x 0x%x \n",
	slink_parameters.data_width, slink_parameters.position, 
	slink_parameters.timeout,    slink_parameters.byte_swapping,
	slink_parameters.start_word, slink_parameters.stop_word);
#endif
 
   if (map_vme()) {
      printf("Error mapping TTC in VME.");
   }

  setup_ttcvi_mapping();

  init_digitizer();


 /****************************** PCI Management ***************************/
 if( (status = uio_open()) ){
   printf("uio_open failed with status =%d",status);
   exit(0);
 }

/* Allocate a contiguous buffer which will be used to receive SLINK packets */
 if((status=uio_calloc(&slink_PCIdesc, MAX_BUF_SIZE*sizeof(int)))){
   printf("uio_calloc failed with status = %d",status);
   fflush(stdout);
   uio_close();
   exit(0);
 }
 slink_PCIaddr =  (char*)(slink_PCIdesc.paddr|0x80000000);
 slink_PageSize = slink_PCIdesc.size;
 slink_VIRTaddr = (char*)(slink_PCIdesc.uaddr);
 event_resources.item.field.slink_PCIdesc=ROD_TRUE;
 event_resources.slink_PCIdesc=&slink_PCIdesc;
 printf("\nSLINK PCI-Parametters");
 printf(" PCI address of data buffer = 0x%08x\n",(word32)slink_PCIaddr);
 printf(" Virtual address of buffer  = 0x%08x\n",(word32)slink_VIRTaddr);
 printf(" Buffer size in bytes       = %d\n",slink_PCIdesc.size);


 /* Allocate a contiguous buffer which will be read by the VME MASTER CPU */
 if((status=uio_calloc(&slvVME_PCIdesc, MAX_BUF_SIZE*sizeof(int)))){
   printf("uio_calloc failed with status = %d",status);
   ReleaseResources(&event_resources);
   uio_close();
   fflush(stdout);
   exit(0);
 }
 slvVME_PCIaddr= (char *)(slvVME_PCIdesc.paddr|0x80000000);
 slvVME_PageSize= slvVME_PCIdesc.size;
 slvVME_VIRTaddr = (char *)slvVME_PCIdesc.uaddr;
 event_resources.item.field.slvVME_PCIdesc=ROD_TRUE;
 event_resources.slvVME_PCIdesc=&slvVME_PCIdesc;
 printf("\nSlave VME PCI-Parametters");
 printf(" PCI address of data buffer = 0x%08x\n",(word32)slvVME_PCIaddr);
 printf(" Virtual address of buffer  = 0x%08x\n", (word32)slvVME_VIRTaddr);
 printf(" Buffer size in bytes       = %d\n",slvVME_PageSize);



 /***** Dynamic Window Mapping of the the PCI & VME busses *****/
 slvVME_VMEdesc.rdpref=ROD_SET;
 slvVME_VMEdesc.wrpost=ROD_SET;
 slvVME_VMEdesc.wrprotect=ROD_CLEAR;
 slvVME_VMEdesc.swap=SINGLE_AUTO_SWAP;
 slvVME_VMEdesc.pcispace=PCI_MEM_CES;
 vme_addr= vme_slave_map
      (slvVME_PCIdesc.paddr|0x80000000,slvVME_PCIdesc.size,&slvVME_VMEdesc);
 if(vme_addr==-1){
   printf("FAULT mapping [PCI address %x] on ",(word32)slvVME_PCIaddr);
   fflush(stdout);
   ReleaseResources(&event_resources);
   uio_close();
   exit(0);
   }
 event_resources.item.field.slvVME_VMEmap=ROD_TRUE;
 event_resources.vme_addr=vme_addr; 
 printf("PCI address %x mapped at VME address %x",
	(word32)slvVME_PCIaddr,vme_addr);



 /************************* SLINK initialization **********************/
 /* Opens and  resets SSP ; only if SLINK is LDOWN reset SLINK itself */
 if ((code = SSP_Open( &slink_parameters, &dev)) != SLINK_OK ){
   printf(" SSP_Open failed with error = %d\n",code);
   SLINK_PrintErrorMessage(code);
   ReleaseResources(&event_resources);
   uio_close();
   exit(0);
 }
 event_resources.item.field.slink_dev=ROD_TRUE;
 event_resources.slink_dev=dev;
 
 regs = dev->regs;/* Loads the address of registers */
 npackets=100;

#ifdef SLIDAS
 push_run(dev);/* start run on SLIDAS */
#endif

 
 
 /*********** Configuration of the FrontEnd ReadOut *************/
 ReadOut.NumOfBlocks=NUM_OF_BLOCKs;
 ReadOut.EventSize=EVENT_SIZE; 
 ReadOut.WordsPerBlock=WORDs_PER_BLOCK;
 ReadOut.BitsPerData=BITs_PER_DATA;
 ReadOut.SizeOfBlock=WORDs_PER_BLOCK;
 
 if((event=AllocMemory(&ReadOut,event))==NULL){
   printf("\n\tSomething wrong ALLOCATING EVENT memory");
   fflush( stdout );
   ReleaseResources( &event_resources );
   uio_close();
   exit( 0 );
 }
 event_resources.item.field.event=ROD_TRUE;
 event_resources.event=event;
 event->RaidWin.item.VIRTaddr=(word32*)slvVME_VIRTaddr;
 event->SlinkWin.item.VIRTaddr=(word32*)slink_VIRTaddr;

 /*In this file the VME MASTER CPU will know where is mapped the PMC data. 
   Alfonso Suggest.
   So you need to have mounted your system files in both boards*/
 data_file=fopen(EVENT_FILE,"w");
 fprintf(data_file,"VMEaddress:%x\nEvent_size:%d\n",vme_addr,event->EventSize);
 fclose(data_file);
 
 /*We fill the VME SLAVE Window of dummy data to see that we receive really 
   the PMC data*/
 for(ii=0;ii<event->EventSize;ii++){
   event->RaidWin.item.VIRTaddr[ii]=0xAABBCCDD;
 }
 printf("\nVMESlave Window Buffer filled");

 

 /*************************** SLINK RECEIVER ********************************/ 
 time(&start_time);
 for( ipacket = 1; 1; ipacket++) {
   if( (code = SSP_InitRead
	( dev, slink_PCIaddr, slink_PageSize, DMA_ON, 0) != SLINK_OK) ){
     printf("SSP_InitRead : error %d = \n",code);
     SLINK_PrintErrorMessage( code );
     ReleaseResources( &event_resources );
     exit(0);
   }

#ifdef DEBUG
   printf("DMA/CPU transfer was initialised, with %d retcode\n",code);
#endif

   do{
#ifdef DEBUG
     printf("** reading a packet %d **\n",ipacket);
#endif
     if( (code = SSP_ControlAndStatus( dev, &status) != SLINK_OK) ) {
       printf("SSP_ControlAndStatus : error = %d\n",code);
       SLINK_PrintErrorMessage( code );
       ReleaseResources( &event_resources );
       exit(0);
     }
     /* if(dev->transfer_status == SLINK_WAITING_FOR_SYNC)
	printf("Warning no packets on SLINK !!!!\n"); */
   } while( status != SLINK_FINISHED);

   packet_size = SSP_PacketSize( dev ); 
   /* reset the DMA counter */
   regs->mwtc = 0;
  
   printf("\nSLink Pack [SIZE %d]-Event Size [SIZE %d]",
	  packet_size, ReadOut.EventSize); 
/****** Copying the data on VME **********/
       vme_virt = slvVME_VIRTaddr;
       slink_virt = slink_VIRTaddr;
       for(j=0;j<packet_size;j++)
       {
         (*vme_virt) = (*slink_virt);
         vme_virt++;
         slink_virt++;
       }

/** Printing Out the Packet */
      w = packet_size >>2;
      dump_address = (long *)slvVME_VIRTaddr;
      /*printf(" packet # %d -- all in hexidecimal :\n",ipacket);
      printf("\n");*/
      for(i=0;i<w;i++)
      {
        if ( ((*dump_address & 0x3ff)==0x3ff) ) n_wrong_samples++;
     /* Calculating the parity bit for all the words*/
        parity =0;
        for(j=0;j<32;j++) parity += (((*dump_address) >> j) & 0x1);
        parity_bit = (parity%2);
        if ( parity_bit ) ndpar_errors++;
      /*  printf("%d ",parity_bit);*/

     /* Printing Out the data and decoding headers */
        if ( ((*dump_address & 0x80000000)==0) && (ipacket%100 == 0) )
        {
        printf("%3x ",(*dump_address & 0x3ff));
        printf("%3x ",((*dump_address & 0xffc00 )>>10));
        printf("%3x ",((*dump_address &0x3ff00000)>>20));
        }
/* else {
        printf("%1x ",((*dump_address &0x20000000)>>29));
        if ((*dump_address &0x20000000)>>29) ntpar_errors++;
        printf("%1x ",((*dump_address &0x10000000)>>28));
        if ((*dump_address &0x10000000)>>28) read_errors++;
        printf("%1x ",((*dump_address &0xe000000)>>25));
        if ((*dump_address &0xe000000)>>25) tec_errors++;
        printf("%1x ",((*dump_address &0x1000000)>>24));
        if ((*dump_address &0x1000000)>>24) db_errors++;
        printf("%1x ",((*dump_address &0x800000)>>23));
        if ((*dump_address &0x800000)>>23) sb_errors++;
        printf("%1x ",((*dump_address &0x700000)>>20));
        printf("%1x ",((*dump_address &0xc0000)>>18));
        printf("%1x ",((*dump_address &0x38000)>>15));
        printf("%1x ",((*dump_address &0x4000)>>14));
        printf("%1x ",(*dump_address &0xfff));
        }*/
        dump_address++;
        /*if((i+1)%8==0)*/
      }


   /* ROD CALL */
   /* Processing Overhead */
   /*DataProcessing( event );*/
   
 }
 time(&end_time); 

 /**************************** Timming Statistics **************************/
 delta_time = end_time-start_time;
 printf("\n\n");
 printf(" # packets transferred = %d\n",npackets);
 printf(" packet size (bytes)   = %d\n",packet_size);
 printf(" time elapsed          = %d seconds\n",delta_time);
 if (delta_time > 0){
   t_per_packet = ((float)delta_time*1000000)/npackets;
   packet_per_s = (float)1000000/t_per_packet;
   mb_per_s = ((float)npackets*packet_size)/((float)delta_time*1024*1024);
   printf("\n");
   printf(" time/packet = %7.1f microseconds\n",t_per_packet);
   printf(" packet/s    = %7.1f packet/s \n",packet_per_s);
   printf(" Mbyte/s     = %7.1f\n",mb_per_s);
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

 getchar();
 data_file=fopen(EVENT_FILE,"r");
 remove(EVENT_FILE);
 fclose(data_file);

#ifdef SLIDAS
 push_run( dev );/* stop run on SLIDAS */ 
#endif

 printf("\n Now reset the SSP interface ...\n");
 SSP_Reset( dev ); /* reset the SSP interface */
 ReleaseResources(&event_resources); 
 uio_close();

 

 printf("\nWORK Done....\n"); 
 return ( ROD_FALSE );

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
void push_run (SLINK_device *dev){
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
