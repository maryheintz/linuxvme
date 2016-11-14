#include <errno.h>
#include <rd13liberrs.h>
#include <rd13lib.h>
#include <stdio.h>
/*#include <time.h>*/
#include <stdlib.h>

#define SIZE 0xffff
#define RD_PREFETCH 1
#define WR_POSTING  1
#define EVENT_FILE "/Disk3/LynxOS/usr/people/rios/event_file.dat"

typedef unsigned int word32;

int main(){
unsigned char  *slink_pack;
unsigned int *new_pack,vme_addr;
unsigned int event_size,ii;
int status;
FILE* data_file;
 
/*vme_addr&event_size must be read from file*/
 data_file=fopen(EVENT_FILE,"r");
 fscanf(data_file,"VMEaddress:%x\nEvent_size:%d\n",&vme_addr,&event_size);
 printf("\nVMEaddress:%x\n\Event_size:%d",vme_addr,event_size);
 new_pack=(word32*)malloc(sizeof(word32)*event_size);
 

 /*We open 64Kbytes*/
   status = RD13_VmeMap( vme_addr, 0x09, SIZE, &slink_pack );
  if( status != RD13_SUCCESS )
  {
    printf("Error mapping the ROD at %X address\n",vme_addr);
  }
  else
     printf("ROD sucessfully mapped at %X address\n",vme_addr);

/* slink_pack=(unsigned int*)(MstMapToVme(vme_addr,SIZE,0x09,RD_PREFETCH,WR_POSTING));*/
  
 for(ii=0;ii<event_size;ii++){
   new_pack[ii]=(unsigned int*)slink_pack[ii];
   printf("\n[%x]",(unsigned int*)slink_pack[ii]);
 }
 
  /*MstUnmapFromVme((word32)slink_pack,SIZE);*/
    /* Unmap modules */
  status = RD13_VmeUnmap( slink_pack );
  if (status != RD13_SUCCESS)
     printf("Error unmapping VME for ROD module \n");

 free(new_pack);
 fclose(data_file);
 printf("\nWork Done..\n");
 return (0);

}
