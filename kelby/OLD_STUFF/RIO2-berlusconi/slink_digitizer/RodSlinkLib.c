#include <stdio.h>
#include <stdlib.h>
#include "RodSlinkLib.h"


int DataProcessing(Event *event){
word32 ii,jj,kk;
word8 parity,bit;
ParityType single;
#define  DEBUG_PROCCESS


#ifdef DEBUG_PROCCESS
 printf("\nInit SLINK packet PROCESSING"); 
#endif

 /* Bypass the data to the PCI SLAVE Window */
 for(ii=0;ii<event->EventSize;ii++){
   event->RaidWin.item.VIRTaddr[ii]=event->SlinkWin.item.VIRTaddr[ii];
 }
#ifdef DEBUG_PROCCESS
 printf("\n\tBypass to RIO2 SLAVE Memory done");
#endif



 /***********************ROD Processing on RIO2****************************/
 /**************************Parity Checking********************************/


 /* We can use FEBlock side of the event structure to index Front End blocks
    but so to first we need to save the pointers to each block */
 for(ii=0;ii<event->NumOfBlocks;ii++){
   event->FEBlock[ii].FEWord=
     &(event->SlinkWin.item.VIRTaddr[ii*event->FEBlock[ii].SizeOfBlock]);
 }

#ifdef DEBUG_PROCCESS
 printf("\n\t[Blocks to proccess %d]",event->NumOfBlocks);
 printf("\n\t[Sizeof each block  %d]",event->FEBlock[0].SizeOfBlock);
 printf("\n\t[BitsPerData        %d]",event->BitsPerData);
#endif

 /* For each FrontEnd Block calculate the vertical & horizontal parity */ 
 for(ii=0;ii<event->NumOfBlocks;ii++){
   event->Vparity[ii]=ROD_CLEAR;
   event->Hparity[ii]=ROD_CLEAR;

#ifdef DEBUG_PROCCESS
   printf("\n\n\t[Block %d]",ii);
#endif
   
   /* Cleaning of Vertical Data Buffer */
   for(jj=0;jj<event->BitsPerData;jj++){
     event->FEBlock[ii].Vdata[jj]=ROD_CLEAR;
   }
   
   for(jj=0;jj<(event->FEBlock[ii].SizeOfBlock-1);jj++){
     single.all=event->FEBlock[ii].FEWord[jj];
     parity=ParityCheck(single.ParityAccess.Data,PARITY_SIZE);
     event->Hparity[ii] |= parity << jj ;
#ifdef DEBUG_PROCCESS
     printf("\n\t[%d][data %x-Parity %x]",jj,single.ParityAccess.Data,parity);
#endif
     for(kk=0;kk<event->BitsPerData;kk++){    
       bit=(( (single.all&( 1<<kk )) >> kk ) << jj );
       event->FEBlock[ii].Vdata[kk] |= bit;
     }
   }
#ifdef DEBUG_PROCCESS
   printf("\n\tHorizontal Parity word\t[Parity %x]",event->Hparity[ii]);
   printf("\n\n\tVertical data");
#endif
  
   for(jj=0;jj<(event->BitsPerData);jj++){
     single.all=event->FEBlock[ii].Vdata[jj];
     parity=ParityCheck(single.all,VERTICAL_PARITY);
     event->Vparity[ii] |=  parity << jj ;
#ifdef DEBUG_PROCCESS
     printf("\n\t[%d][data %x-Parity %x]",jj,single.ParityAccess.Data,parity);
#endif
     }
#ifdef DEBUG_PROCCESS
   printf("\n\n\tVertical Parity\t\t[Parity %x]",event->Vparity[ii]);
#endif

 }

#ifdef DEBUG_PROCCESS
 printf("\nSLINK packet has been processed\n");
#endif
 return(ROD_TRUE);

}

 /* An algorithm to calculate the parity of a single word of size "width" */
int ParityCheck(word32 data, int width){
int right_shift,shift,ii,ACC=0,mask=0;

 shift=width-1;
 right_shift=shift-1;
 mask=1<<right_shift; 
 ACC=(data>>shift)^((data&mask)>>right_shift);
 for(ii=1;ii<shift;ii++)
   ACC^=(data&(mask>>ii))>>(right_shift-ii);
 
 return(ACC&0x1);
}



Event* AllocMemory(ReadOutParams *ReadOut, Event *event){
int ii;

 event=(Event*)malloc(sizeof(Event));
 if(event==NULL){
   printf("\n\tSomething WRONG allocating MEMORY to EVENT");
   fflush(stdout);
   return(NULL);
 }

 event->NumOfBlocks=ReadOut->NumOfBlocks;
 event->EventSize=ReadOut->EventSize;
 event->BitsPerData=ReadOut->BitsPerData;

 /* Memory allocation to Vertical & Horizontal parity of each block */
 event->Vparity=(word32*)malloc(event->NumOfBlocks*sizeof(word32));
 event->Hparity=(word32*)malloc(event->NumOfBlocks*sizeof(word32));
 if((event->Vparity==NULL)||(event->Hparity==NULL)){
   printf("\nSomething WRONG allocating MEMORY to data PARITY PATTERNs");
   fflush(stdout);
   return(NULL);
 }
 
 /* Memory Allocation to FrontEnd Blocks */
 event->FEBlock=(Block*)malloc(event->NumOfBlocks*sizeof(Block));
 if(event->FEBlock==NULL){
   printf("\n\tSomething WRONG allocating memory to FRONT END DATA BLOCKs");
   fflush(stdout);
   return(NULL);
 }

 /* We will follow the SLINK pointers on the FEWord field */
 for(ii=0;ii<event->NumOfBlocks;ii++){
   event->FEBlock[ii].Vdata=(word32*)
     malloc(sizeof(word32)*ReadOut->BitsPerData);

   /* This fragment Must be VALIDATED ONLY if we need fill our structures */

/* event->FEBlock[ii].FEWord=(word32*)
   malloc(sizeof(word32)*ReadOut->WordsPerBlock);
   if((event->FEBlock[ii].Vdata==NULL)||(event->FEBlock[ii].FEWord==NULL)){
     printf("\n\tFAULT allocating memory to FrontEnd DATA BLOCKs, item %d",ii);
     fflush(stdout);
     return(NULL);
   }*/

   event->FEBlock[ii].SizeOfBlock=ReadOut->SizeOfBlock;
 } 
 return(event);

}


/*  A very quick method to calculate the nearest 2 exponent of N */
int Base2(int N){
int jj;

  for (jj=0;N;jj++) 
    N>>=1;

  return(jj-1);

}



/******************* Event Release *******************/
int FreeMemory(Event *event){
int ii;
/*#define DEBUG_FREE*/

 free(event->Vparity);
 free(event->Hparity);

#ifdef DEBUG_FREE
 printf("\n\n[Releasing Memory]");
 printf("\n\t[Parity & vertical pointers released]");
#endif

 for(ii=0;ii<event->NumOfBlocks;ii++){
#ifdef DEBUG_FREE
   printf("\n\t[Block %d]",ii);
#endif
   free(event->FEBlock[ii].Vdata);
/* This fragment Must be VALIDATED ONLY if we need fill it our structures */
/* free(event->FEBlock[ii].FEWord);*/
 }
#ifdef DEBUG_FREE
 printf("\n\tBLOCK data pointers released"); 
#endif
 
 free(event->FEBlock);
#ifdef DEBUG_FREE
 printf("\n\tEVENT FEBlock pointer released");
#endif

 free(event);
#ifdef DEBUG_FREE
 printf("\n\tEVENT pointer released\n\n");
#endif

 return(RELEASE_SUCCESS);
}




int ReleaseResources(EventResources *resource){
int status;

 /* VME Slave Unmapping */
 status=RELEASE_FAIL;
 if(resource->item.field.slvVME_VMEmap){
   status=vme_slave_unmap(resource->vme_addr,resource->slvVME_PCIdesc->size);
   if(status==-1){
     printf("\n\tFAIL unmapping PCI address %x from VME map",
	    (unsigned int)resource->slvVME_PCIdesc->paddr|0x80000000);
     status=RELEASE_FAIL;
   }
   else{
     resource->item.field.slvVME_VMEmap=ROD_FALSE;
     printf("\nPCI Window unmmaped from VME");
     status=RELEASE_SUCCESS;
   }
 }
 
 /* PCI Descriptors */
 if(resource->item.field.slvVME_PCIdesc){
   uio_cfree(resource->slvVME_PCIdesc);
   printf("\nPCI Slave VME Memory RELEASED"); 
   resource->item.field.slvVME_PCIdesc=ROD_FALSE;
   status=RELEASE_SUCCESS;
 }

 if(resource->item.field.slink_PCIdesc){
   uio_cfree(resource->slink_PCIdesc);
   printf("\nPCI SLink Memory RELEASED");
   resource->item.field.slink_PCIdesc=ROD_FALSE;
   status=RELEASE_SUCCESS;
 }

 /* SLINK device */
 if(resource->item.field.slink_dev){
   if( (status = SSP_Close( resource->slink_dev ) != SLINK_OK) ) {
     printf(" SSP_Close : error = %d\n", status);
     SLINK_PrintErrorMessage( status );
     status=RELEASE_FAIL;
   }
   else{
     printf("\nSLINK Device CLOSED..");
     resource->item.field.slink_dev=ROD_FALSE;
     status=RELEASE_SUCCESS;
   }
 }
 
 /* ROD&FrontEnd Event */
 if(resource->item.field.event){
   if(FreeMemory(resource->event)!=EVENT_RELEASED){
     printf("\n\tSomething wrong RELEASING memory");
     fflush(stdout);
     status=RELEASE_FAIL;
   }
   else{
     printf("\nEVENT memory RELEASED");
     resource->item.field.event=ROD_FALSE; 
     status=RELEASE_SUCCESS;
   }
 }
 
 return(status);
 
}


int TestPointers(Event *event){
int ii,jj;

 for(ii=0;ii<event->NumOfBlocks;ii++){
   /*Vertical & Horizontal Buffers*/
   event->Vparity[ii]=0xFFFFFFFF;
   event->Hparity[ii]=0xBBBBBBBB;

   /*FrontEnd Blocks*/
   for(jj=0;jj<event->FEBlock[ii].SizeOfBlock;jj++){
     event->FEBlock[ii].FEWord[jj]=0xAABBAABB;
   }
   printf("\n[DataWord BUFFER Block %d Tested]",ii);
   for(jj=0;jj<event->BitsPerData;jj++){
     event->FEBlock[ii].Vdata[jj]=0xAACCAACC;
   }
   printf("\n[VerticalData parity BUFFER Block %d Tested]",ii);
   printf("\n[Block %d Tested]",ii);
 }
 return(ROD_TRUE);
}
