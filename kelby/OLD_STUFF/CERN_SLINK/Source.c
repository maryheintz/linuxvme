/**************** This version allows to work with two SPS interface as two PCI devices *********************************/
/*********************** the user have to specify the device he wanted to work with : e.g Source 1 *********************/
#include <stdio.h>
#include <mem.h>
#include <ces/uiocmd.h>
#include "atdaq_types.h"
#include "s5933lib.h"
#include "SLINK.h"

#define DMA_CHAIN_LENGTH 200
#define SPS_IN_REG imb[3]

int main(int argc, char **argv)
{
  char *buffer;
  long *p;
  register volatile s5933_regs *s5933_regs;
  long start_time,end_time; 
  float rate;
  int ipackets, npackets,i,w,size;
  char *pci_addr, cc[10];
  int page_size,ret;
  int code, status, print,index;
  uio_mem_t buf_desc;
  SLINK_parameters slink_parameters;
  SLINK_device *dev;
  if(argc <= 1)
      {
      printf("Usage: %s <device number> (e.g Source 1)\n",argv[0]);
      exit(0);
      }
  sscanf(argv[1],"%d",&index);
  if(index <=0)
     {
     printf(" %s got an invalid index number\n",argv[0]);
     exit(0);
     }
  index--;

  /* Initializes The Preference Structure, For Example : */
  SLINK_InitParameters(&slink_parameters); /* call it with a structure not a pointer to it ???*/
  slink_parameters.position = index;  /* this will indicate the occurence of the device */
  printf(" print the contents of slink_parameters:\n");
  printf(" %d %d %d %d 0x%x 0x%x\n",slink_parameters.data_width, slink_parameters.position, slink_parameters.timeout, slink_parameters.byte_swapping, slink_parameters.start_word, slink_parameters.stop_word);
  /* Opens the device */
  if( (code = SPS_Open( &slink_parameters, &dev) != SLINK_OK) ) {
    /* Prints an error message if needed */
    SLINK_PrintErrorMessage( code);
    return(1);
  }
  /* Loads the address of registers */
  s5933_regs = dev->regs;
   /* Checks if the SLINK is LDOWN# */
  if( ((SPS_in_register) ((dword) (s5933_regs->SPS_IN_REG.all))).bit.LDOWN == 0)
     {
      printf(" Now reseting the SLINK......\n");
      code = LSC_Reset(dev);
      if(code != SLINK_OK)
          {
          printf("LSC_Reset : error = %d\n",code);
          SLINK_PrintErrorMessage( code);
          exit(0);
          }
     }
#ifdef DEBUG
  printf("Open returned %d \n",code);
#endif
  if((ret = uio_open()))
  {
  printf("uio_open failed with status =%d",ret);
  exit(0);
  }
  
loop:
  printf( "Number of packets to send ? ");
  scanf( "%d", &npackets);
  printf("\n Size of the packet ? ");
  scanf("%d",&size);
  printf("Print the contents of the packet? (Y/N)\n");
  scanf("%s",cc);
  if((cc[0] == 'Y')||(cc[0] == 'y')) print = 1;
  else print = 0;   

  if((ret=uio_calloc(&buf_desc, size*sizeof(char))))
     {
     printf("uio_calloc failed with status = %d",ret);
     exit(0);
     }
  pci_addr = (char *)(buf_desc.paddr|0x80000000);
  page_size = buf_desc.size;
  buffer = (char *)buf_desc.uaddr; /* virtual address */
#ifdef DEBUG
  printf("PCI address : 0x%08x\n", pci_addr);
  printf("Size (bytes):0x%08x\n",page_size);
  printf("Buf. addr.  : 0x%08x\n", buffer);
#endif
  /* fill in the buffer */
  p= (long *)buffer;
  w= size>>2;
  for(i=0;i<w;i++)
  	*p++ = i;
  p = (long *)buffer;
  if(print)
  	{
	for(i=0;i<w;i++)
            printf("%5d) %8x\n",i,*p++);
	}
  /* Main loop */
  start_time = time(NULL);
  for( ipackets = 1; ipackets <= npackets; ipackets++) {
      /* Initializes the transfer */
      if( (code = SPS_InitWrite( dev, pci_addr, page_size,SLINK_BOTH_CONTROL_WORDS, SLINK_ENABLE) != SLINK_OK) )
           {
            printf("SPS_InitWrite : error = %d \n",code);
            SLINK_PrintErrorMessage( code);
            exit(0);
            }
      do {
	/* Processes the transfer */
	if( (code = SPS_ControlAndStatus( dev, &status) != SLINK_OK) ) {
	  /* Prints an error message if needed */
	  SLINK_PrintErrorMessage( code);
	  return(1);
	}
      } while( status != SLINK_FINISHED);
/*    printf("."); */
    if(ipackets % 100000 ==0)
        {
        end_time = time(NULL);
        rate = (float)(ipackets)/(float)(end_time - start_time);
        printf(" \n packet=%d, rate=%f\n",ipackets,rate);  
        }
    }
    printf("Do you want to continue (Y/N) ?\n");
    scanf("%s",cc);
    if((cc[0] == 'Y')||(cc[0] == 'y')) 
	{
	uio_cfree(&buf_desc);
	goto loop;
	}
  /* Closes the device */
  if( (code = SPS_Close( dev) != SLINK_OK) ) {
    /* Prints an error message if needed */
    SLINK_PrintErrorMessage( code);
    return(1);
  }
  uio_close();
  return(0);
}
