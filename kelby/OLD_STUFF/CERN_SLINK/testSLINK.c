
/****************************************************************************/
/*                                                                          */
/* Test program of library :                                                */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/* File Name   : testSLINK.c                                                */
/*                                                                          */
/* Title       : Test program for the library libSLINK                      */
/*                                                                          */
/* Version     : 1.0                                                        */
/*                                                                          */
/* Description :                                                            */
/*               Tests the library by sending or receiving one or several   */
/*               words, with the dma feature or without it.                 */
/*                                                                          */
/* Author      :                                                            */
/*                                                                          */
/* Notes       :                                                            */
/*                                                                          */
/****************************************************************************/

#define DRAM_2_PCI 0x80000000
#define N_DMAARRAY 100

/****************************************************************************/
/*                                                                          */
/*                       Includes header files                              */
/*                                                                          */
/****************************************************************************/

#include "atdaq_types.h"
#include <conf.h>
#include <mem.h>
#include <stdlib.h>
#include <unistd.h>
#include "pcilib.h"
#include "s5933lib.h"
#include "SLINK.h"
#include <stdio.h>
#include "menu.h"

/****************************************************************************/
/*                                                                          */
/*                                Macros                                    */
/*                                                                          */
/****************************************************************************/

#define WaitAKeyIsPressed() \
   term_cntrl( US_tcap);    /* Underscore */ \
   printf( "Press return to continue.\n"); \
   term_cntrl( UE_tcap);      /* End underscore */ \
   while( getchar() == EOF)

/****************************************************************************/
/*                                                                          */
/*                           Global variables                               */
/*                                                                          */
/****************************************************************************/

static menu_t *main_menu, *dev_menu;
static menu_item_t *menu_item, *position_item;

static long *dev_type, *dev_dma, *dev_multiple, *dev_format;
static long *dev_word, *dev_pages, *dev_number, *dev_position;
static long *dev_size, *dev_mode;
static char **dev_buffer;
static struct dmachain **dev_dmaarray;

static SLINK_device *dev;
static SLINK_parameters params;

static long start_time, end_time;

/****************************************************************************/
/*                                                                          */
/*                          Definition of functions                         */
/*                                                                          */
/****************************************************************************/

/* missing system prototypes */

int vmtopm(int pid, struct dmachain *array, char *base, long size);

/****************************************************************************/
/*                                                                          */
/* Name           : maintain_buffer()                                       */
/*                                                                          */
/* Description    : To maintain the right size of the buffer                */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int maintain_buffer( menu_t *mp, menu_item_t *mitp)
{
  char **buffer;
  struct dmachain **dmaarray;
  long *pages;
  long *size;

  buffer = (char **) &( mitp->data_p->optional[0]);
  dmaarray = ( struct dmachain **) &( mitp->data_p->optional[1]);
  pages = (long *) &(mitp->data_p->optional[2]);
  size = (long *) &( mitp->data_p->value);

  /* Frees the previous buffer */

  if( *buffer != NULL) free( *buffer);
  if( *dmaarray != NULL) free( *dmaarray);

  /* Gets memory for the new one */

  if(( *buffer = (char *) malloc( *size)) == NULL) {
    printf( "Not enough memory to create the buffer.\n");
    *size = 0;
    WaitAKeyIsPressed();
    return(1);
  }

  /* Gets memory for the dma array */

  if((*dmaarray = ( struct dmachain *) 
      calloc( N_DMAARRAY, sizeof( struct dmachain))) == NULL) {
    printf( "Not enough memory to create the dma array.\n");
    free( *buffer);
    *buffer = 0;
    *size = 0;
    WaitAKeyIsPressed();
    return(1);
  }

  /* Gets the physical addresses of the buffer */

  if (( *pages = vmtopm(
			(int) getpid(),
			*dmaarray,
			*buffer,
			*size)) < 0) {
    printf( "Unable to get the physical address.\n");
    free( *buffer);
    free( *dmaarray);
    *buffer = 0;
    *dmaarray = 0;
    *size = 0;
    WaitAKeyIsPressed();
    return(1);
  }

  return(1);
}

/****************************************************************************/
/*                                                                          */
/* Name           : maintain_and_fill_buffer()                              */
/*                                                                          */
/* Description    : To maintain the right size of the buffer and to fill it */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int maintain_and_fill_buffer( menu_t *mp, menu_item_t *mitp)
{

  long *p;
  int number_words, i;

  /* Changes the size of the buffer */

  maintain_buffer( mp, mitp);

  /* Gets the parameters for filling the buffer */

  p = (long *)( mitp->data_p->optional[0]);
  number_words =  ( mitp->data_p->value) >> 2;

  /* Fills the buffer */

  for( i=1; i<= number_words; i++)
    *p++ = i;

  return( 1);
}

/****************************************************************************/
/*                                                                          */
/* Name           : set_datawidth()                                         */
/*                                                                          */
/* Description    : To set the data width                                   */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int set_datawidth( menu_t *mp, menu_item_t *mitp)
{
  int choice;

  /* Gets the choice for data width */

  choice = (int) mitp->data_p->value;

  /* Sets the choice in the parameter structure */

  switch( choice) {
  case 0 :
    params.data_width = SLINK_8BITS;
    break;
  case 1 :
    params.data_width = SLINK_16BITS;
    break;
  case 2 :
    params.data_width = SLINK_32BITS;
    break;
  default :
    break;
  }

  return(1);
}

/****************************************************************************/
/*                                                                          */
/* Name           : set_position()                                          */
/*                                                                          */
/* Description    : To set position on PCI bus                              */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int set_position( menu_t *mp, menu_item_t *mitp)
{
  /* Gets the position */

  params.position = (int) *dev_position;

  return(1);
}

/****************************************************************************/
/*                                                                          */
/* Name           : enable_position_selection()                             */
/*                                                                          */
/* Description    : To enable or disable position selection                 */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int enable_position_selection( menu_t *mp, menu_item_t *mitp)
{
  /* If position selection is enabled */

  if( mitp->data_p->value == 1) {

    /* Sets the position in the parameter structure */

    params.position = (int) *dev_position;

    /* Enables the position item */

    position_item->valid = 1;

  }

  /* If it's not the case */

  else {

    /* No position selection */

    params.position = 0;

    /* Disables the position item */

    position_item->valid = 0;

  }

  return(1);
}

/****************************************************************************/
/*                                                                          */
/* Name           : set_timeout()                                           */
/*                                                                          */
/* Description    : To set the timeout parameter                            */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int set_timeout( menu_t *mp, menu_item_t *mitp)
{
  /* Sets the timeout */

  SLINK_Timeout( dev,(int) mitp->data_p->value);

  return(1);
}

/****************************************************************************/
/*                                                                          */
/* Name           : quit_action                                             */
/*                                                                          */
/* Description    : To exit test program                                    */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int quit_action( menu_t *mp, menu_item_t *mitp)
{
  /* Returns 0 to signal we want to exit */

  return( 0);
}

/****************************************************************************/
/*                                                                          */
/* Name           : print_state()                                           */
/*                                                                          */
/* Description    : To display the device state                             */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int print_state( menu_t *mp, menu_item_t *mitp)
{
  int code;

  /* Prints the state */

  code = SLINK_PrintState( dev);

  printf( "PrintState() returned %d.\n", code);
  SLINK_PrintErrorMessage( code);

  /* Waits a key is pressed to go back to the menu */

  WaitAKeyIsPressed();

  return( 1);
}

/****************************************************************************/
/*                                                                          */
/* Name           : reset_device()                                          */
/*                                                                          */
/* Description    : To reset the device                                     */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int reset_device( menu_t *mp, menu_item_t *mitp)
{
  int code;
  SLINK_device_type type_of_device;

  /* Identifies the device */

  type_of_device = SLINK_WhichTypeOfDevice( dev);

  /* Resets the device */

  switch( type_of_device) {
  case SPS :
    code = SPS_Reset( dev);
    break;
  case SSP :
    code = SSP_Reset( dev);
    break;
  default :
    return(1);
  }

  printf( "Reset() returned %d.\n", code);
  SLINK_PrintErrorMessage( code);

  /* Waits a key is pressed to go back to the menu */

  WaitAKeyIsPressed();

  return( 1);
}

/****************************************************************************/
/*                                                                          */
/* Name           : write_one_word()                                        */
/*                                                                          */
/* Description    : To write only a word with specified type and value      */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int write_one_word( menu_t *mp, menu_item_t *mitp)
{
  int code;

  /* A single word to send */

  code = SPS_WriteOneWord( dev, *dev_word, *dev_type);

  /* Results */

  term_cntrl( SO_tcap);      /* Standout */

  printf( "WriteOneWord returned %d.\n", code);
  SLINK_PrintErrorMessage( code);

  term_cntrl( SE_tcap);      /* End standout */

  WaitAKeyIsPressed();

  return( 1);
}

/****************************************************************************/
/*                                                                          */
/* Name           : write_one_pattern()                                     */
/*                                                                          */
/* Description    : To write a data pattern as specified in the menu        */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int write_one_pattern( menu_t *mp, menu_item_t *mitp)
{
  char *address;
  int status, i_packet;
  int code;
  float real_size;

  /* Checks there is a ready buffer */

  if( *dev_size == 0) {
    printf( "No buffers are allocated/\n");
    WaitAKeyIsPressed();
    return( 1);
  }

  /* Calculates the address */

  if( *dev_dma != 0) {

    /* Converts in a PCI address */

    address = (char *) ( (*dev_dmaarray)[0].address | DRAM_2_PCI);
  }
  else {
    address = *dev_buffer;
  }
 
  /* Checks the boundary */

  if( (( long) address) & 0x3 != 0) {
    printf( "The address isn't on a 4 bytes boundary.\n");
    WaitAKeyIsPressed();
    return(1);
  }

  /* Does the job according to the transfer mode */

  switch( *dev_mode) {
  case 2 :
    /* Starts the measure of the spent time */

    start_time = time( NULL);
    break;
  default :
    break;
  }

  /* For each packet to send */

  for( i_packet=1; i_packet<= *dev_number; i_packet++) {

    /* Initalizes the write operation */

    if(( code = SPS_InitWrite(
			      dev,
			      address,
			      *dev_size,
			      *dev_format,
			      *dev_dma)) != SLINK_OK) {
      printf( "InitWrite returned %d.\n", code);
      SLINK_PrintErrorMessage( code);
      WaitAKeyIsPressed();
      return(1);
    }

    /* Do a loop until the transfer is finished or an error occured */

    do {

      /* Gets the current status */

      if (( code = SPS_ControlAndStatus( dev, &status)) != SLINK_OK) {
	printf( "Status() returned %d.\n", code);
	SLINK_PrintErrorMessage( code);
	WaitAKeyIsPressed();
	return(1);
      }

      /* Exits if the transfer is finished or aborted */

    } while( status != SLINK_FINISHED);


    /* Does the job according to the transfer mode */

    switch(  *dev_mode) {

    case 1 :
      /* Control mode */

      printf( "Packet #%d :\n", i_packet);
      printf( "Transfer finished ( status = %d).\n", status);
      WaitAKeyIsPressed();
      break;

    default :
      break;
    }

  }

  /* Does the job according to the transfer mode */

  switch( *dev_mode) {

  case 0 :
    printf( "Transfers finished ( Status = %d).\n", status);
    WaitAKeyIsPressed();
    break;

  case 2 :
    /* Ends the measure of the spent time */

    end_time = time( NULL);

    /* Calculates the real size */

    real_size = (float)((*dev_size / 4)) * 4;

    /* Displays results */

    printf( "Transfers finished ( Status = %d).\n", status);
    printf( "Spent time            = %d s\n",(int)(end_time - start_time));
    printf( "Transmitted bytes     = %f GB\n",
	   real_size * *dev_number * 1.E-9);
    printf( "Average transfer rate = %f MB/s\n", \
	    ((float)( real_size * (*dev_number))
	     / ( end_time - start_time) * 1.E-6));
    WaitAKeyIsPressed();
    break;

  default :
    break;
  }

  return(1);
}

/****************************************************************************/
/*                                                                          */
/* Name           : testSPS()                                               */
/*                                                                          */
/* Description    : To test a SPS interface                                 */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int testSPS( menu_t *mp, menu_item_t *mitp)
{
  int code;

  /* Accesses to the device */

  code = SPS_Open( &params, &dev);
  printf( "Open returned %d.\n", code);
  SLINK_PrintErrorMessage( code);
  WaitAKeyIsPressed();

  if( code != SLINK_OK) return(1);

  /*----------*/
  /* SPS menu */
  /*----------*/

  /* Title */

  dev_menu = menu_define(
			 "Test program for the source part of S-LINK library");

  /* Values for parameters */

  /* Value for the type of word */

  menu_item = menu_add_item(dev_menu, '1', "Type of single words",
			    1, NULL, 1);

  menu_item->data_p->type = toggle_type;         /* toggle only */
  menu_item->data_p->value = 1;                  /* default value */
  strcpy(menu_item->data_p->valtxt[0], "Control");/* text for 1st comtponent */
  strcpy(menu_item->data_p->valtxt[1], "Data");   /* text for 2nd comtponent */

  dev_type = &(menu_item->data_p->value);

  /* Value for the DMA flag */

  menu_item = menu_add_item(dev_menu, '2', "Transfer type",
			    1, NULL, 1);

  menu_item->data_p->type = toggle_type;         /* toggle only */
  menu_item->data_p->value = 1;                  /* default value */
  strcpy(menu_item->data_p->valtxt[0], "No DMA");/* text for 1st comtponent */
  strcpy(menu_item->data_p->valtxt[1], "DMA");   /* text for 2nd comtponent */

  dev_dma = &(menu_item->data_p->value);

  /* Value for the format */

  menu_item = menu_add_item( dev_menu,
			     '3',
			     "Pattern format",
			     1, NULL, 1);

  menu_item->data_p->type = enum_type;           /* enumeration only */
  menu_item->data_p->value = 3;                  /* default value */
  menu_item->data_p->limits[0] = 4;              /* number of components */
  strcpy(menu_item->data_p->valtxt[0], "Without control words");
  strcpy(menu_item->data_p->valtxt[1], "With a start control word");
  strcpy(menu_item->data_p->valtxt[2], "With a stop control word");
  strcpy(menu_item->data_p->valtxt[3], "With both control words");

  dev_format = &(menu_item->data_p->value);

  /* Value for the size of the source buffer */

  menu_item = menu_add_item( dev_menu, '4',
			     "Buffer size",
			     1, &maintain_and_fill_buffer, 1);

  menu_item->data_p->type = integer_type;        /* integer only */
  menu_item->data_p->value = 1000;               /* default value */
  menu_item->data_p->limits[0] = 0;              /* lower limit */
  menu_item->data_p->limits[1] = 100000;         /* upper limit */

  dev_size     = &(menu_item->data_p->value);
  dev_buffer   = (char **) &(menu_item->data_p->optional[0]);
  dev_dmaarray = ( struct dmachain **) &(menu_item->data_p->optional[1]);
  dev_pages    = &(menu_item->data_p->optional[2]);

  *dev_buffer = NULL;
  *dev_dmaarray = NULL;

  /* Allocates buffer */

  maintain_buffer( dev_menu, menu_item);

  /* Value for the transfer mode */

  menu_item=menu_add_item( dev_menu, '5', "Transfer mode", 1, NULL, 1);

  menu_item->data_p->type = enum_type;           /* enumeration only */
  menu_item->data_p->limits[0] = 3;              /* number of comtponents */
  strcpy(menu_item->data_p->valtxt[0], "Nothing");
  strcpy(menu_item->data_p->valtxt[1], "Display");
  strcpy(menu_item->data_p->valtxt[2], "Speed measurement");

  dev_mode = &(menu_item->data_p->value);

  /* Value for the timeout parameter */

  menu_item = menu_add_item( dev_menu, '6',
			     "Timeout",
			     1, &set_timeout, 1);

  menu_item->data_p->type = integer_type;        /* integer only */
  menu_item->data_p->value = 10;                 /* default value */
  menu_item->data_p->limits[0] = 0;              /* lower limit */
  menu_item->data_p->limits[1] = 100000;         /* upper limit */

  /* Actions */

  /* Value for the single word to send */

  menu_item = menu_add_item( dev_menu, 'S',
			     "To send a single word",
			     1, &write_one_word, 1);

  menu_item->data_p->type = unsigned_type;       /* unsigned only */
  menu_item->data_p->value = 0;                  /* default value */
  menu_item->data_p->limits[0] = 0;              /* lower limit */
  menu_item->data_p->limits[1] = 0xFFFFFFFF;     /* upper limit */

  dev_word = &(menu_item->data_p->value);

  /* To write a pattern */

  menu_item = menu_add_item( dev_menu, 'G',
			     "To write complete patterns",
			     1, &write_one_pattern, 1);

  menu_item->data_p->type = integer_type;        /* integer only */
  menu_item->data_p->value = 1;                  /* default value */
  menu_item->data_p->limits[0] = 0;              /* lower limit */
  menu_item->data_p->limits[1] = 10000000;       /* upper limit */

  dev_number = &(menu_item->data_p->value);

  /* To display the device state */

  menu_item = menu_add_item( dev_menu, 'D',
			     "To display device state",
			     1, &print_state, 0);

  /* To reset the device */

  menu_item = menu_add_item( dev_menu, 'R',
			     "To reset the device",
			     1, &reset_device, 0);

  /* To exit this menu */

  menu_item = menu_add_item( dev_menu, 'Q', "To return to the main menu", 1, \
			     &quit_action, 0);

  /* Displays the menu */

  menu_expose( dev_menu);

  /* Closes the device */

  code = SPS_Close( dev);
  printf( "Close returned %d.\n", code);
  SLINK_PrintErrorMessage( code);

  /* Close the main menu so return 0 */

  return( 0);
}

/****************************************************************************/
/*                                                                          */
/* Name           : read_one_word()                                         */
/*                                                                          */
/* Description    : To read only a word                                     */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int read_one_word( menu_t *mp, menu_item_t *mitp)
{
  long word;
  int type_of_word, code;

  /* Gets the parameters */


  /* A single word to read */

  code = SSP_ReadOneWord( dev, &word, &type_of_word);

  /* Results */

  term_cntrl( SO_tcap);      /* Standout */

  printf( "ReadOneWord returned %d.\n", code);
  SLINK_PrintErrorMessage( code);

  term_cntrl( SE_tcap);      /* End standout */


  if( code == SLINK_OK) {

    /* Prints the received word and its type */

    if( type_of_word == SLINK_CONTROL_WORD)
      printf( "Received a control word ");
    else
      printf( "Received a data word ");
    printf( "%8X\n", (unsigned int)word);
  }
  WaitAKeyIsPressed();

  return( 1);
}

/****************************************************************************/
/*                                                                          */
/* Name           : read_one_pattern()                                      */
/*                                                                          */
/* Description    : To read a data pattern as specified in the menu         */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int read_one_pattern( menu_t *mp, menu_item_t *mitp)
{
  char *address;
  long *p;
  int status, i_packet, i_word;
  int code;
  float real_size;

  /* Checks there is a ready buffer */

  if( *dev_size == 0) {
    printf( "No buffers are allocated.\n");
    WaitAKeyIsPressed();
    return( 1);
  }

  /* Calculates the address */

  if( *dev_dma != 0) {

    /* Converts in a PCI address */

    address = (char *) ( (*dev_dmaarray)[0].address | DRAM_2_PCI);
  }
  else {
    address = *dev_buffer;
  }
 
  /* Checks the boundary */

  if( (( long) address) & 0x3 != 0) {
    printf( "The address isn't on a 4 bytes boundary.\n");
    WaitAKeyIsPressed();
    return(1);
  }

  /* Does the job according to the transfer mode */

  switch( *dev_mode) {
  case 2 :

    /* Initializes the amount of transmitted bytes */

    real_size = 0;

    /* Starts the measure of the spent time */

    start_time = time( NULL);
    break;
  default :
    break;
  }

  /* For each packet to send */

  for( i_packet=1; i_packet<= *dev_number; i_packet++) {

    /* Initalizes the read operation */

    if(( code = SSP_InitRead(
			      dev,
			      address,
			      *dev_size,
			      *dev_dma,
			      *dev_multiple)) != SLINK_OK) {
      printf( "InitRead returned %d.\n", code);
      SLINK_PrintErrorMessage( code);
      WaitAKeyIsPressed();
      return(1);
    }

    /* Do a loop until the transfer is finished or an error occured */

    do {

      /* Gets the current status */

      if (( code = SSP_ControlAndStatus( dev, &status)) != SLINK_OK) {
	printf( "Status() returned %d.\n", code);
	SLINK_PrintErrorMessage( code);
	WaitAKeyIsPressed();
	return(1);
      }

      /* Checks if we need an extra page to complete transfer */

      if( status == SLINK_NEED_OF_NEW_PAGE) {

	switch(  *dev_mode) {

	case 1 :
	  /* Display mode */

	  printf( "Packet #%d :\n", i_packet);
	  printf( "Transfer paused. Page full ( status = %d).\n", status);

	  /* Displays the content of the buffer */

	  printf( "Received data :\n\n");
	  p = ( long *) *dev_buffer;
	  for( i_word = 1; i_word<= ( SSP_PacketSize( dev) >>2); i_word++){
	    printf( "%5d) %8X\n", i_word, (unsigned int)*p++);
	  }

	  printf( "Size of packet = %d\n", SSP_PacketSize( dev));
	  WaitAKeyIsPressed();
	  break;

	case 2 :
	  /* Speed measurement mode */

	  /* Actualises the real size */

	  real_size += SSP_PacketSize( dev);
	  break;

	default :
	  break;
	}

	if(( code = SSP_ContinueRead( dev, address, *dev_size)) != SLINK_OK) {
	  printf( "InitContinue returned %d.\n", code);
	  SLINK_PrintErrorMessage( code);
	  WaitAKeyIsPressed();
	  return(1);
	}
      }

    } while( status != SLINK_FINISHED);

    /* Does the job according to the transfer mode */

    switch(  *dev_mode) {

    case 1 :
      /* Display mode */

      printf( "Packet #%d :\n", i_packet);
      printf( "Transfer finished ( status = %d).\n", status);

      /* Displays the content of the buffer */

      printf( "Received packet :\n\n");
      p = ( long *) *dev_buffer;
      for( i_word = 1; i_word<= ( SSP_PacketSize( dev) >>2); i_word++){
	printf( "%5d) %8X\n", i_word, (unsigned int)*p++);
      }

      printf( "Size of packet = %d\n", SSP_PacketSize( dev));
      WaitAKeyIsPressed();
      break;

    case 2 :
      /* Speed measurement mode */

      /* Actualises the real size */

      real_size += SSP_PacketSize( dev);
      break;

    default :
      break;
    }
  }

  /* Does the job according to the transfer mode */

  switch( *dev_mode) {

  case 0 :
    printf( "Transfers finished ( Status = %d).\n", status);
    WaitAKeyIsPressed();
    break;

  case 2 :
    /* Ends the measure of the spent time */

    end_time = time( NULL);

    /* Displays results */

    printf( "Transfers finished ( Status = %d).\n", status);
    printf( "Spent time            = %d s\n", (int)(end_time - start_time));
    printf( "Transmitted bytes     = %f GB\n",
	   real_size * 1.E-9);
    printf( "Average transfer rate = %f MB/s\n", \
	    ((float)( real_size) / ( end_time - start_time) * 1.E-6));
    WaitAKeyIsPressed();
    break;

  default :
    break;
  }

  return(1);
}

/****************************************************************************/
/*                                                                          */
/* Name           : testSSP()                                               */
/*                                                                          */
/* Description    : To test a SSP interface                                 */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int testSSP( menu_t *mp, menu_item_t *mitp)
{
  int code;

  /* Accesses to the device */

  code = SSP_Open( &params, &dev);
  printf( "Open returned %d.\n", code);
  SLINK_PrintErrorMessage( code);
  WaitAKeyIsPressed();

  if( code != SLINK_OK) return(1);

  /*----------*/
  /* SSP menu */
  /*----------*/

  /* Prepares menus and their items */

  /* Title */

  dev_menu = menu_define(
		   "Test program for the destination part of S-LINK library");

  /* Values for parameters */

  /* Value for the DMA flag */

  menu_item = menu_add_item(dev_menu, '1', "Transfer type",
			    1, NULL, 1);

  menu_item->data_p->type = toggle_type;         /* toggle only */
  menu_item->data_p->value = 1;                  /* default value */
  strcpy(menu_item->data_p->valtxt[0], "No DMA");/* text for 1st comtponent */
  strcpy(menu_item->data_p->valtxt[1], "DMA");   /* text for 2nd comtponent */

  dev_dma = &(menu_item->data_p->value);

  /* Value for the multiple buffers transfer */

  menu_item = menu_add_item(dev_menu, '2', "Multiple buffers transfer",
			    1, NULL, 1);

  menu_item->data_p->type = toggle_type;         /* toggle only */
  menu_item->data_p->value = 0;                  /* default value */
  strcpy(menu_item->data_p->valtxt[0], "Disabled");
                                       /* text for 1st comtponent */
  strcpy(menu_item->data_p->valtxt[1], "Enabled");
                                       /* text for 2nd comtponent */

  dev_multiple = &(menu_item->data_p->value);


  /* Value for the size of the receiving buffer */

  menu_item = menu_add_item( dev_menu, '3',
			     "Buffer size",
			     1, &maintain_buffer, 1);

  menu_item->data_p->type = integer_type;        /* integer only */
  menu_item->data_p->value = 1000;               /* default value */
  menu_item->data_p->limits[0] = 0;              /* lower limit */
  menu_item->data_p->limits[1] = 100000;         /* upper limit */

  dev_size     = &(menu_item->data_p->value);
  dev_buffer   = (char **) &(menu_item->data_p->optional[0]);
  dev_dmaarray = ( struct dmachain **) &(menu_item->data_p->optional[1]);
  dev_pages    = &(menu_item->data_p->optional[2]);

  *dev_buffer = NULL;
  *dev_dmaarray = NULL;

  /* Allocates buffer */

  maintain_buffer( dev_menu, menu_item);

  /* Value for the transfer mode */

  menu_item=menu_add_item( dev_menu, '4', "Transfer mode", 1, NULL, 1);

  menu_item->data_p->type = enum_type;           /* enumeration only */
  menu_item->data_p->limits[0] = 3;              /* number of comtponents */
  strcpy(menu_item->data_p->valtxt[0], "Nothing");
  strcpy(menu_item->data_p->valtxt[1], "Display");
  strcpy(menu_item->data_p->valtxt[2], "Speed measurement");

  dev_mode = &(menu_item->data_p->value);

  /* Value for the timeout parameter */

  menu_item = menu_add_item( dev_menu, '5',
			     "Timeout",
			     1, &set_timeout, 1);

  menu_item->data_p->type = integer_type;        /* integer only */
  menu_item->data_p->value = 10;                 /* default value */
  menu_item->data_p->limits[0] = 0;              /* lower limit */
  menu_item->data_p->limits[1] = 100000;         /* upper limit */


  /* Actions */

  /* To read a single word */

  menu_item = menu_add_item( dev_menu, 'S',
			     "To read a single word",
			     1, &read_one_word, 0);

  /* To read a pattern */

  menu_item = menu_add_item( dev_menu, 'G',
			     "To read complete patterns",
			     1, &read_one_pattern, 1);

  menu_item->data_p->type = integer_type;        /* integer only */
  menu_item->data_p->value = 1;                  /* default value */
  menu_item->data_p->limits[0] = 0;              /* lower limit */
  menu_item->data_p->limits[1] = 10000000;       /* upper limit */

  dev_number = &(menu_item->data_p->value);

  /* To display the device state */

  menu_item = menu_add_item( dev_menu, 'D',
			     "To display device state",
			     1, &print_state, 0);

  /* To reset the device */

  menu_item = menu_add_item( dev_menu, 'R',
			     "To reset the device",
			     1, &reset_device, 0);

  /* To quit this menu */

  menu_item = menu_add_item( dev_menu, 'Q', "To return to the main menu", 1, \
			     &quit_action, 0);

  /* Displays the menu */

  menu_expose( dev_menu);

  /* Closes the device */

  code = SSP_Close( dev);
  printf( "Close returned %d.\n", code);
  SLINK_PrintErrorMessage( code);

  /* Close the main menu so return 0 */

  return( 0);
}

/****************************************************************************/
/*                                                                          */
/*                                Main                                      */
/*                                                                          */
/****************************************************************************/

int main(int argc, char **argv)
{
  /* Defines the standard parameters */

  SLINK_InitParameters( &params);

  /* Opens the menu library */

  menu_init();


  /*-----------*/
  /* Main menu */
  /*-----------*/

  main_menu = menu_define( "Test program for the S-LINK library");

  /* Value for the transfer mode */

  menu_item=menu_add_item( main_menu, '1', "Data width", 1, &set_datawidth, 1);

  menu_item->data_p->type = enum_type;           /* enumeration only */
  menu_item->data_p->value = 2;                  /* default value */
  menu_item->data_p->limits[0] = 3;              /* number of comtponents */
  strcpy(menu_item->data_p->valtxt[0], "8 bits");
  strcpy(menu_item->data_p->valtxt[1], "16 bits");
  strcpy(menu_item->data_p->valtxt[2], "32 bits");

  /* Value for the position flag on PCI bus */

  menu_item=menu_add_item( main_menu, '2',
			   "Index selection", 1,
			   &enable_position_selection, 1);

  menu_item->data_p->type = toggle_type;         /* toggle only */
  menu_item->data_p->value = 0;                  /* default value */
  strcpy(menu_item->data_p->valtxt[0], "Disabled");
  strcpy(menu_item->data_p->valtxt[1], "Enabled");


  /* Value for the position number on PCI bus */

  menu_item=menu_add_item( main_menu, 'P', "Index", 0, &set_position, 1);

  menu_item->data_p->type = integer_type;        /* integer only */
  menu_item->data_p->value = 0;                  /* default value */
  menu_item->data_p->limits[0] = 0;              /* lower limit */
  menu_item->data_p->limits[1] = 16;             /* upper limit */

  dev_position = &(menu_item->data_p->value);
  position_item = menu_item;

  /* To test a source device */

  menu_item = menu_add_item( main_menu, '3',
			     "To test a SPS interface ( source)",
			     1, &testSPS, 0);

  /* To test a destination device */

  menu_item = menu_add_item( main_menu, '4',
			     "To test a SSP interface ( destination)",
			     1, &testSSP, 0);

  /* To quit the program */

  menu_item = menu_add_item( main_menu, 'Q', "To quit the program", 1, 
			     &quit_action, 0);


  /* Displays the menu */

  menu_expose( main_menu);

  return( 0);
}
