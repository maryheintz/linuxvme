/****************************************************************************/
/*                                                                          */
/* File Name   : libSLINK.c                                                 */
/*                                                                          */
/* Title       : Library for the SPS and SSP interfaces                     */
/*                                                                          */
/* Version     : 1.2                                                        */
/*                                                                          */
/* Description :                                                            */
/*               Gives some functions to use only the SPS and SSP interfaces*/
/*               for S-LINK, the set of functions being designed for the    */
/*               prototype -1 of the ATLAS experiment.                      */
/*                                                                          */
/* Author      : F.Pennerath                                                */
/*                                                                          */
/* Notes       :                                                            */
/*									    */
/* 970721   M.Niculescu							    */
/*          changes to SSP_InitRead and SSP_ControlAndStatus		    */	
/*									    */
/* 970722   JOP								    */
/*          modify SSP_Reset according to the SSP user's guide		    */
/*          add LDC_Reset to reset the link  				    */
/*          NB! : SLINK_sleep relies  on a Lynx timer call that doesn't     */
/*                work correctly (usleep) 				    */
/* 970820   M.Niculescu                                                     */
/*          take out LDC_Reset from SSP_Reset                               */
/* 970829   M.Niculescu                                                     */
/*          split SPS_reset into two functions: LSC_Reset which does the    */
/*          reset of the SLINK and SPS_Reset which does the reset of SPS    */
/* 971120   M.Niculescu                                                     */
/*          In SSP_Open, if SLINK is LDOWN call LDC_Reset                   */
/* 971127   M.Niculescu                                                     */
/*          Add the facility to input, by dma, multi page events            */ 
/* 971203   M.Niculescu                                                     */
/*          Add the possibility to sincronize with the SLINK source, even   */
/*          the control word for the beginning of the first packet is lost; */
/*          This packet will be discarded and the next one will be received */
/*          in the allocated buffer                                         */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*                       Includes header files                              */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <conf.h>
#include "atdaq_types.h"
#include "pcilib.h"
#include "s5933lib.h"
#include "SLINK.h"

/****************************************************************************/
/*                                                                          */
/*                      Static options for compilation                      */
/*                                                                          */
/****************************************************************************/

#define FIFO_WAIT

/****************************************************************************/
/*                                                                          */
/*                   Constant to identify the S-LINK structure              */
/*                                                                          */
/****************************************************************************/

#define SPS_STRUCTURE_ID 0x15482660
#define SSP_STRUCTURE_ID 0x83347100

/****************************************************************************/
/*                                                                          */
/*                     PCI VID and DID words to identify                    */
/*                        the SPS and SSP interfaces                        */
/*                                                                          */
/****************************************************************************/

#define SLINK_SPS_ID 0x001010DC
#define SLINK_SSP_ID 0x001110DC

/****************************************************************************/
/*                                                                          */
/*               Constant to initialize the PCI command word                */
/*                                                                          */
/****************************************************************************/

#define SLINK_PCICMD 0x00000146 /* Enable Memory Access, Bus Master ,
				    Parity Error and System Error */

/****************************************************************************/
/*                                                                          */
/*                     PCI VID and DID words to identify                    */
/*                        the SPS and SSP interfaces                        */
/*                                                                          */
/****************************************************************************/

#define SLINK_SPS_ID 0x001010DC
#define SLINK_SSP_ID 0x001110DC

/****************************************************************************/
/*                                                                          */
/*             Maximum timeout for reset operation in milliseconds          */
/*                                                                          */
/****************************************************************************/

#define SLINK_RESET_TIMEOUT 4000

/****************************************************************************/
/*                                                                          */
/*          Macro waiting one micro : be careful : system dependent         */
/*                                                                          */
/****************************************************************************/

#define SLINK_Sleep( t) usleep((t))

/****************************************************************************/
/*                                                                          */
/*            Definition of the input and output registers for              */
/*                        the SPS and SSP interfaces                        */
/*                                                                          */
/****************************************************************************/

#define SPS_OUT_REG omb[0]
#define SPS_IN_REG  imb[3]
#define SSP_OUT_REG omb[0]
#define SSP_IN_REG  imb[3]

/****************************************************************************/
/*                                                                          */
/*                         Declaration of macros                            */
/*                                                                          */
/****************************************************************************/

#define SPS_bad_structure( device) (( device)->struct_id != SPS_STRUCTURE_ID)
#define SSP_bad_structure( device) (( device)->struct_id != SSP_STRUCTURE_ID)

/****************************************************************************/
/*                                                                          */
/*                           Global variables                               */
/*                                                                          */
/****************************************************************************/

/* Disable/enable strings */

static char * SLINK_enable_messages[] = {
  "disabled",
  "enabled"
};

/* Packet format strings */

static char * SLINK_format_messages[] = {
  "No control words",
  "Start control words",
  "Stop control words",
  "Both start and stop control words"
};

/* Error description strings */

static char * SLINK_error_messages[] = {
  "No error.",
  "Open didn't succeed.",
  "Requested device was not found.",
  "Cannot access to the device.",
  "Not enough memory to allocate device ressources.",
  "The device pointer doesn't really point on a device structure.",
  "The data width provided in parameters has a wrong value.",
  "A parameter provided to the function has a wrong value.",
  "The device seems to be down ( LDOWN# = 0).",
  "The fifo is full.",
  "The fifo is empty.",
  "Unable to remove the device mapping ressources.",
  "A transfer is currently not finished.",
  "There is no transfer currently running.",
  "The reset failed. The link cannot be reseted.",
  "The program counter arrived in an unexpected place in the algorithm.",
  "A new page was asked by SSP_Control&Status but was not provided.",
  "The transfer status has a unknown or not appropriate value."
};

/* Status description strings */

static char * SLINK_status_messages[] = {
  "Free. No request of transfer.",
  "Transfer initiated. Waiting for a packet.",
  "Transfer aborted. Dump of the last defective packet.",
  "Transfer processed. Receiving a packet.",
  "Transfer processed. Sending a packet.",
  "Transfer paused. Waiting for a new buffer.",
  "Transfer finished."
};

/****************************************************************************/
/*                                                                          */
/*                          Definition of functions                         */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/* Name           : bswap                                                   */
/*                                                                          */
/* Description    : Performs byte swapping on 32 bits words                 */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* word                | dword              | The word to byte swap         */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : The byte swapped word                                   */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

static dword bswap (dword word)
{
  register union {
    dword ii;
    char ch[4];
  } w1,w2;

  w1.ii = word;
  w2.ch[0] = w1.ch[3];
  w2.ch[1] = w1.ch[2];
  w2.ch[2] = w1.ch[1];
  w2.ch[3] = w1.ch[0];

  return w2.ii;
}

/****************************************************************************/
/*                                                                          */
/* Name           : SLINK_GetEnablesMessage                                 */
/*                                                                          */
/* Description    : Returns a string of character describing                */
/*                  a enable/disable flag                                   */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* enable_flag         | int                | Enable flag to describe       */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A pointer on characters, pointing on the enable flag    */
/*                  description.                                            */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

char * SLINK_GetEnableMessage( int enable_flag)
{
  if(( enable_flag < SLINK_DISABLE) || ( enable_flag > SLINK_ENABLE)) { 
    return( "Unknown enable flag value.");
  }

  return( SLINK_enable_messages[ enable_flag]);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SLINK_GetFormatMessage                                  */
/*                                                                          */
/* Description    : Returns a string of character describing a packet format*/
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* format_code         | int                | Contains a integer code       */
/*                     |                    | describing a packet format.   */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A pointer on characters, pointing on the format         */
/*                  description.                                            */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

char * SLINK_GetFormatMessage( int format_code)
{
  if(( format_code < SLINK_NO_CONTROL_WORD) ||
     ( format_code > SLINK_BOTH_CONTROL_WORDS)) {
    return( "Unknown packet format.");
  }

  return( SLINK_format_messages[ format_code]);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SLINK_GetErrorMessage                                   */
/*                                                                          */
/* Description    : Returns a string of character describing an error       */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* error_code          | int                | Contains a integer code       */
/*                     |                    | describing a S-LINK error.    */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A pointer on characters, pointing on the error          */
/*                  description.                                            */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

char * SLINK_GetErrorMessage( int error_code)
{
  if(( error_code < 0) || ( error_code >= 
			    SLINK_INSERT_NEW_ERROR_CODES_BEFORE_THIS_ONE)) {
    return( "Unknown error code.");
  }

  return( SLINK_error_messages[ error_code]);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SLINK_GetStatusMessage                                  */
/*                                                                          */
/* Description    : Returns a string of character describing a status       */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* status_code         | int                | Contains a integer code       */
/*                     |                    | describing a S-LINK status.   */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A pointer on characters, pointing on the status         */
/*                  description.                                            */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

char * SLINK_GetStatusMessage( int status_code)
{
  if(( status_code < 0) || ( status_code >= 
			    SLINK_INSERT_NEW_STATUS_CODES_BEFORE_THIS_ONE)) {
    return( "Unknown status code.");
  }

  return( SLINK_status_messages[ status_code]);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SLINK_PrintErrorMessage                                 */
/*                                                                          */
/* Description    : Displays a string of character describing an error      */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* error_code          | int                | Contains a integer code       */
/*                     |                    | describing a S-LINK error.    */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A error message                                         */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int SLINK_PrintErrorMessage( int error_code)
{
  printf( "S-LINK library reports : %s\n", SLINK_GetErrorMessage( error_code));
  return( SLINK_OK);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SLINK_PrintState                                        */
/*                                                                          */
/* Description    : Displays the current state of the device                */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* device              | SLINK_device *     | Pointer to the chip structure */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A error message                                         */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int SLINK_PrintState( SLINK_device *device)
{
  char device_name[25];

  /* Identification of the device */

  switch( device->struct_id) {
  case SPS_STRUCTURE_ID :
    strcpy( device_name, "SPS ( PCI to S-LINK)");
    break;
  case SSP_STRUCTURE_ID :
    strcpy( device_name,"SSP ( S-LINK to PCI)");
    break;
  default :
    return( SLINK_BAD_POINTER);
    break;
  }

  printf( "S-LINK library reports :\n");

  /* Identification part */

  printf( "  Device identification :\n");
  printf( "    Type     : %s\n", device_name);
  printf( "    PCI slot = %d\n", device->pci_slot);

  /* Transfer parameter part */

  printf( "  Device transfer parameters :\n");
  printf( "    Current status : %s\n",
	  SLINK_GetStatusMessage( device->transfer_status));
  printf( "    DMA %s.\n", SLINK_enable_messages[ device->dma]);
  printf( "    Byte swapping %s.\n",
	SLINK_GetEnableMessage( device->params.byte_swapping));
  printf( "    Start control word = 0X%8X\n", (unsigned int)device->params.start_word);
  printf( "    Stop control word  = 0X%8X\n", (unsigned int)device->params.stop_word);
  printf( "    Timeout            = %d\n", device->params.timeout);
  printf( "    Data width         = ");
  switch( device->params.data_width) {
  case SLINK_8BITS :
    printf( "8 bits\n");
    break;
  case SLINK_16BITS :
    printf( "16 bits\n");
    break;
  case SLINK_32BITS :
    printf( "32 bits\n");
    break;
  default :
    printf( "Unknown\n");
    break;
  }

  /* Specific device part */

  printf( "  Device specific fields :\n");

  switch( device->struct_id) {
  case SPS_STRUCTURE_ID :
    printf( "    Packet format      = %s.\n",
	    SLINK_GetFormatMessage( device->specific.sps.packet_format));
    break;
  case SSP_STRUCTURE_ID :
    printf( "    Multiple pages transfer %s.\n",
	SLINK_GetEnableMessage(device->specific.ssp.multiple_pages_transfer));
    printf( "    Keep bad packets %s.\n",
	  SLINK_GetEnableMessage( device->params.keep_bad_packets));
    break;
  default :
    break;
  }

  /* Statistics part */

  printf( "  Device statistics :\n");

  switch( device->struct_id) {
  case SPS_STRUCTURE_ID :
    printf( "    %d correctly sent packets.\n",
	    device->stats.good_packets);
    break;
  case SSP_STRUCTURE_ID :
    printf( "    %d correct received packets.\n",
	    device->stats.good_packets);
    printf( "    %d transmission errors.\n",
	    device->stats.transmission_errors);
    printf( "    %d losts of start control word.\n",
	    device->stats.lost_starts);
    printf( "    %d losts of end control word.\n",
	    device->stats.lost_ends);
    printf( "    %d unknown received control words.\n",
	    device->stats.unknown_control_words);
    printf( "    %d overflown buffers.\n",
	    device->stats.overflown_buffers);
    printf( "    %d asks for a new page.\n",
	    device->stats.asks_for_new_page);
    break;
  default :
    break;
  }

  return( SLINK_OK);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SLINK_WhichTypeOfDevice                                 */
/*                                                                          */
/* Description    : Returns the type of the device                          */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* device              | SLINK_device *     | Pointer to the chip structure */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A S-LINK device type  ( SLINK_device_type)              */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

SLINK_device_type SLINK_WhichTypeOfDevice( SLINK_device *device)
{
  /* Resets the device */

  switch( device->struct_id) {
  case SPS_STRUCTURE_ID :
    return( SPS);
    break;
  case SSP_STRUCTURE_ID :
    return( SSP);
    break;
  default :
    return( UNKNOWN);
    break;
  }
}

/****************************************************************************/
/*                                                                          */
/* Name           : LSC_Reset                                               */
/*                                                                          */
/* Description    : Resets the LSC                                          */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* device              | SLINK_device *     | Pointer to the chip structure */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A error message                                         */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/* 970828         Michaela                                                  */
/*                reset the LSC						    */
/****************************************************************************/

int LSC_Reset( SLINK_device *device)
{
  static volatile s5933_regs *s5933_regs;
  int timeout;

  /* Checks if the device pointer is right */

  if( SPS_bad_structure( device) == SLINK_TRUE){
    return( SLINK_BAD_POINTER);
  }
  /* Gets the address of the registers */

  s5933_regs = device->regs;

  /* Resets the link */

  device->specific.sps.out.bit.URESET = 0;

  s5933_regs->SPS_OUT_REG.all = device->specific.sps.out.all;

  /* Waits few microseconds */

  SLINK_Sleep( 10);

  /* Waits LDOWN# is removed unless the timeout expired */

  timeout = 0;

  do {

    /* Reads the ingoing mailbox */
    device->specific.sps.in.all = s5933_regs->SPS_IN_REG.all;

    /* Waits a milli second */

    SLINK_Sleep( 1000);

    /* Increases timeout */

    timeout++;

    /* Returns an error if the timeout is expired */

    if( timeout > SLINK_RESET_TIMEOUT) {
      return( SLINK_DOWN_LINK);
    }

  } while( device->specific.sps.in.bit.LDOWN == 0);

  /* Removes the reset of the link */

  device->specific.sps.out.bit.URESET = 1;

  s5933_regs->SPS_OUT_REG.all = device->specific.sps.out.all;

  /* Does the software reset */

  device->transfer_status = SLINK_FREE;

  return( SLINK_OK);
}


/****************************************************************************/
/*                                                                          */
/* Name           : SPS_Reset                                               */
/*                                                                          */
/* Description    : Resets a SPS interface                                  */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* device              | SLINK_device *     | Pointer to the chip structure */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A error message                                         */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/* 970828         Michaela                                                  */
/*                reset only the SPS interface                              */
/****************************************************************************/

int SPS_Reset( SLINK_device *device)
{
  static volatile s5933_regs *s5933_regs;

  /* Checks if the device pointer is right */

  if( SPS_bad_structure( device) == SLINK_TRUE){
    return( SLINK_BAD_POINTER);
  }

  /* Gets the address of the registers */

  s5933_regs = device->regs;

  /* Resets the S593X's fifo */

  s5933_regs->mcsr.bit.p2afr = 1;
  
  /* Resets the add-on card */

  s5933_regs->mcsr.bit.aor = 1;

  /* Waits few microseconds */

  SLINK_Sleep( 100);

  /* Removes the reset signal on the add-on card */

  s5933_regs->mcsr.bit.aor = 0;

  /* Does the software reset */

  device->transfer_status = SLINK_FREE;

  return( SLINK_OK);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SPS_Open                                                */
/*                                                                          */
/* Description    : Gives access to a SPS interface and initializes it.     */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* params              | SLINK_parameters*  | Includes the initializing     */
/*                     |                    | parameters.                   */
/* ------------------------------------------------------------------------ */
/* device              | SLINK_device **    | Contains the address of a     */
/*                     |                    | structure defining the chip   */
/*                     |                    | when the function returned    */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A error message                                         */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int SPS_Open( SLINK_parameters *params, SLINK_device **device)
{
  /* Temporary variables */

  int slot = 0;
  int code;
  pci_csh *addr_csh;
  unsigned int pci_cmd_word;
  volatile s5933_regs *regs;

  /* Local procedure to close if an error occured */

  void Close_on_error( int what) {
    switch( what){
    case 10 :
      free( ( void *) *device);
    case 9 :
#ifdef LIBS5933
      S5933_Close( (s5933_regs *) regs);
#endif
    case 8 :
    case 7 :
    case 6 :
    case 5 :
    case 4 :
      PCI_Detach_Device( slot);
    case 3 :
    case 2 :
      PCI_Close();
    case 1 :
    default :
    }
  }

  /* Access to the PCI library */

  if( PCI_Open() != PCI_SUCCESSFUL) {
    Close_on_error( 1);
    return( SLINK_BAD_OPEN);
  }

  /* Tries to get an access to the PCI bus */

  if( PCI_Find_Device( SLINK_SPS_ID, params->position, &slot) != PCI_SUCCESSFUL){
    Close_on_error( 2);
    return( SLINK_DEVICE_NOT_FOUND);
  }

  if( PCI_Attach_Device( slot, SLINK_SPS_ID, &addr_csh) != PCI_SUCCESSFUL){
    Close_on_error( 3);
    return( SLINK_FORBIDDEN_ACCESS);
  }

  /* Tries to get an access to the S5933 registers */

  if( PCI_Alloc_Space( slot, 0, (void **) &regs) != PCI_SUCCESSFUL){
    Close_on_error( 4);
    return( SLINK_FORBIDDEN_ACCESS);
  }

  /* Initializes the PCICMD word of the PCI configuration space header */

  if( PCI_Read_CSH( slot, pci_csh_cmd, &pci_cmd_word) != PCI_SUCCESSFUL){
    Close_on_error( 5);
    return( SLINK_FORBIDDEN_ACCESS);
  }

  pci_cmd_word |= SLINK_PCICMD;

  if( PCI_Write_CSH( slot, pci_csh_cmd, pci_cmd_word) != PCI_SUCCESSFUL){
    Close_on_error( 6);
    return( SLINK_FORBIDDEN_ACCESS);
  }

#ifdef DEBUG

  /* Displays the CSH */

  if( PCI_Dump_CSH( slot) != PCI_SUCCESSFUL){
    Close_on_error( 7);
    return( SLINK_FORBIDDEN_ACCESS);
  }

#endif

#ifdef LIBS5933
  /* Tries to initialize the S5933 registers */

  if( S5933_Open( ( s5933_regs *) regs) != S5933_SUCCESSFUL){
    Close_on_error( 8);
    return( SLINK_FORBIDDEN_ACCESS);
  }
#endif

  /* Allocates memory for the device structure */

  if( ( *device = ( SLINK_device *) malloc( sizeof( SLINK_device))) == NULL){
    Close_on_error( 9);
    return( SLINK_NOT_ENOUGH_MEMORY);
  }

  /* Initializes the device structure */

  ( *device)->struct_id = SPS_STRUCTURE_ID;

  bcopy( ( char *) params, ( char *) &(( *device)->params),
	 sizeof( SLINK_parameters));

  bzero((char *) &(( *device)->stats), sizeof( SLINK_statistics));

  /* Be sure reserved bits in specified control words are 0 */

  ( *device)->params.start_word &= ~0xF;

  ( *device)->params.stop_word &= ~0xF;

  /* Initializes address parameters */

  ( *device)->pci_slot = slot;

  ( *device)->regs = ( s5933_regs *) regs;

  /* Initializes transfer parameters */

  ( *device)->transfer_status = SLINK_FREE;

  ( *device)->error_messages = SLINK_NO_ERROR;

  ( *device)->initial_size = 0;

  ( *device)->dma = SLINK_ENABLE;

  /* Initializes the output S-LINK register */

  (*device)->specific.sps.out.all = 0;

  /* Sets the data width */

  (*device)->specific.sps.out.bit.UDW = (*device)->params.data_width;

  /* Sets the type of word */

  (*device)->specific.sps.out.bit.UCTRL = SLINK_CONTROL_WORD;

  /* Sets the URESET# and UTEST# signals */

  (*device)->specific.sps.out.bit.URESET = 1;
  (*device)->specific.sps.out.bit.UTEST = 1;

  /* Initialises the MCSR register */

  (*device)->mcsr.all = 0;

  /* Resets the DMA counter */

  regs->mrtc = 0;

  /* Enables the DMA */

  (*device)->mcsr.bit.rte = 1;

#ifdef REFRAINED_REQUEST
  /* DMA asks for bus mastering only if 4 or more free rooms in S5933 fifo */

  (*device)->mcsr.bit.rms = 1;
#endif

  /* Initialises the INTCSR register */

  (*device)->intcsr.all = 0;

  /* Enables or disables the byte swapping as requested */

  if( (*device)->params.byte_swapping == SLINK_ENABLE){
    (*device)->intcsr.bit.endian.ec = 2;
  }
  else {
    (*device)->intcsr.bit.endian.ec = 0;
  }

  /* Writes on the output register */

  regs->SPS_OUT_REG.all = (*device)->specific.sps.out.all;

  /* Writes on the MCSR */

  regs->mcsr.all = (*device)->mcsr.all;

  /* Writes on the INTCSR */

  regs->intcsr.all = (*device)->intcsr.all;

  /* Resets the SPS interface */

  if(( code = SPS_Reset( *device)) != SLINK_OK) {
    Close_on_error( 10);
    return( code);
  }

#ifdef DEBUG

#ifdef LIBS5933
  /* Displays the content of S593X's registers */

  if( S5933_Dump( regs) != S5933_SUCCESSFUL){
    Close_on_error( 10);
    return( SLINK_FORBIDDEN_ACCESS);
  }
#endif

#endif

  return( SLINK_OK);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SPS_Close                                               */
/*                                                                          */
/* Description    : Removes the access to a SPS interface and frees the     */
/*                  ressources.                                             */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* device              | SLINK_device *     | Pointer to the chip structure */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A error message                                         */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int SPS_Close( SLINK_device *device)
{
  int error_code = SLINK_OK;

  /* Checks it's a right pointer */

  if( device == NULL) {
    return( SLINK_BAD_POINTER);
  }

  /* Checks if the device pointer is right */

  if( SPS_bad_structure( device) == SLINK_TRUE){
    return( SLINK_BAD_POINTER);
  }

#ifdef LIBS5933
  /* Removes the shared memory segment mapped on the AMCC's registers */

  if( S5933_Close( device->regs) != S5933_SUCCESSFUL){
    error_code = SLINK_DETACH_FAILED;
  }
#endif

  /* Removes the access to the PCI slot */

  if( PCI_Detach_Device( device->pci_slot) != PCI_SUCCESSFUL){
    error_code = SLINK_DETACH_FAILED;
  }

  /* Frees the memory occupied by the device structure */

  free(( void *) device);

  return( error_code);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SPS_WriteOneWord                                        */
/*                                                                          */
/* Description    : Writes one control or data word on the specified S-LINK */
/*                  device.                                                 */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* device              | SLINK_device *     | Pointer to the chip structure */
/* ------------------------------------------------------------------------ */
/* word                | long               | The word to send.             */
/* ------------------------------------------------------------------------ */
/* type_of_word        | int                | To tell if it's a control or  */
/*                     |                    | a data word.                  */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A error message                                         */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int SPS_WriteOneWord( SLINK_device *device, long word, int type_of_word)
{
  register volatile s5933_regs *s5933_regs;

  /* Only for POINTER_CHECK : Checks if the device pointer is right */

#ifdef POINTER_CHECK
  if( SPS_bad_structure( device) == SLINK_TRUE){
    return( SLINK_BAD_POINTER);
  }
#endif

  /* Loads the address of registers */

  s5933_regs = device->regs;

#ifdef PARAMETER_CHECK

  /* Checks the type of word */

  if( ( type_of_word < 0) || ( type_of_word > 1)) {
    return( SLINK_BAD_PARAMETERS);
  }

#endif

  /* Only for LDOWN_CHECK : Checks if the device is down, if so
     exits */

#ifdef LDOWN_CHECK

  /* Checks LDOWN# */

  if( ((SPS_in_register) ((dword)(s5933_regs->SPS_IN_REG.all))).bit.LDOWN == 0) {
    return( SLINK_DEVICE_DOWN);
  }

#endif

  if( device->transfer_status != SLINK_FREE) {
    return( SLINK_TRANSFER_NOT_FINISHED);
  }

  /* Checks for the type of word of the previous sent word
  and changes this type if requested */

  if ( type_of_word != device->specific.sps.out.bit.UCTRL) {

    /* We need to change the type of word so the fifo has to be empty */

    if( s5933_regs->mcsr.bit.p2afe == 1){
      device->specific.sps.out.bit.UCTRL = type_of_word;
      s5933_regs->SPS_OUT_REG.all = device->specific.sps.out.all;
    }
    else {

      /* Returns if we can't write into the fifo */

      return( SLINK_FIFO_FULL);
    }
  }
  else {
    /* Checks that the FIFO is not full */

    if( s5933_regs->mcsr.bit.p2aff == 1){

      /* Returns if we can't write into the fifo */

      return( SLINK_FIFO_FULL);
    }
  }

  /* Sends the word */

  s5933_regs->fifo = word;

  return( SLINK_OK);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SPS_InitWrite                                           */
/*                                                                          */
/* Description    : Initializes a device read master DMA operation          */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* device              | SLINK_device *     | Pointer to the chip structure */
/* ------------------------------------------------------------------------ */
/* address             | char *             | The PCI physical address      */
/*                     |                    | where the DMA starts.         */
/*                     |                    | Or the virtual address where  */
/*                     |                    | the CPU has to read           */
/* ------------------------------------------------------------------------ */
/* size                | int                | The number of bytes to write  */
/* ------------------------------------------------------------------------ */
/* packet_format       | SLINK_packet_format| To tell if we have to write   */
/*                     |                    | data words between two        */
/*                     |                    | control words or not.         */
/* ------------------------------------------------------------------------ */
/* dma                 | int                | To tell if we have to use the */
/*                     |                    | DMA feature or just the CPU   */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A error message                                         */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int SPS_InitWrite( SLINK_device *device, char *address, int size,
		  SLINK_packet_format packet_format, int dma)
{
  register volatile s5933_regs *s5933_regs;

  /* Only for POINTER_CHECK : Checks if the device pointer is right */

#ifdef POINTER_CHECK
  if( SPS_bad_structure( device) == SLINK_TRUE){
    return( SLINK_BAD_POINTER);
  }
#endif

  /* Loads the address of registers */

  s5933_regs = device->regs;

#ifdef PARAMETER_CHECK

  /* Checks the type of word */
/* packet_format is unsigned, SLINK_NO_CONTROL_WORD is 0, => first */
/* comparison is redundant. Remove it to shut up the compiler. TW. */
/*  if( ( packet_format < SLINK_NO_CONTROL_WORD) */
/*      || ( packet_format > SLINK_BOTH_CONTROL_WORDS)){ */
  if(  packet_format > SLINK_BOTH_CONTROL_WORDS ){
    return( SLINK_BAD_PARAMETERS);
  }

#endif

  /* Only for LDOWN_CHECK : Checks if the device is down, if so
     exits */

#ifdef LDOWN_CHECK

  /* Checks LDOWN# */

  if( ((SPS_in_register) ((dword)(s5933_regs->SPS_IN_REG.all))).bit.LDOWN == 0) {
    return( SLINK_DEVICE_DOWN);
  }

#endif

  /* Analyses current transfer status */

  switch( device->transfer_status) {
  case SLINK_FREE :
    device->error_messages = SLINK_NO_ERROR;
    device->transfer_status = SLINK_SENDING;
    break;
  default :
    return( SLINK_TRANSFER_NOT_FINISHED);
    break;
  }

  /* Sets the int address, init size, the current size, the dma flag,
     in the device structure */
  
  device->dma = dma;
  device->initial_address = address;
  device->initial_size = size;
  device->specific.sps.packet_format = packet_format;

  /* If a control word at beginning of packets has to be sent */

  if( (packet_format & SLINK_MASK_START_WORD) != 0) {

    device->specific.sps.where_in_packet = 1;
    device->specific.sps.phase = 0;
  }

  /* If the start control word flag isn't set */

  else {
    device->specific.sps.where_in_packet = 2;
    device->specific.sps.phase = 0;
  }

  /* If the CPU is used, initializes additional parameters */

  if( dma == SLINK_DISABLE) {
    device->number_words = size >> 2;
    device->current_address =( long *) address;
  }

  /* No errors */

  return( SLINK_OK);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SPS_ControlAndStatus                                    */
/*                                                                          */
/* Description    : Gives the current status of the writing operation       */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* device              | SLINK_device *     | Pointer to the chip structure */
/* ------------------------------------------------------------------------ */
/* status              | int *              | The current status            */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A error message                                         */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int SPS_ControlAndStatus( SLINK_device *device, int *status)
{
  register volatile s5933_regs *s5933_regs;
  register int dma, packet_format, ask_for, where_in_packet, phase;
  register int timeout, free_room, remaining_words, words_to_write, i;
  register long *address;

  /* Only for POINTER_CHECK : Checks if the device pointer is right */

#ifdef POINTER_CHECK
  if( SPS_bad_structure( device) == SLINK_TRUE){
    return( SLINK_BAD_POINTER);
  }
#endif

  /* Loads the address of registers */

  s5933_regs = device->regs;

  /* Only for LDOWN_CHECK : Checks if the device is down, if so
     sets the status and exits */

#ifdef LDOWN_CHECK

  /* Checks LDOWN# */

  if( ((SPS_in_register) ((dword)(s5933_regs->SPS_IN_REG.all))).bit.LDOWN == 0) {
    return( SLINK_DEVICE_DOWN);
  }

#endif

  /* Analyses current transfer status */

  switch( device->transfer_status) {
  case SLINK_SENDING :
    break;
  case SLINK_FREE :
    return( SLINK_TRANSFER_NOT_STARTED); 
    break;
  default :
    return( SLINK_BAD_STATUS);
    break;
  }

  /* Gets the relevant parameters */

  dma = device->dma;
  packet_format = device->specific.sps.packet_format;
  timeout = device->params.timeout;

  /* phase is a sub phase of where_in_packet */

  phase = device->specific.sps.phase;

  /* where_in_packet says where we are in processing a data packet */

  where_in_packet = device->specific.sps.where_in_packet;

  /* Do a loop on the timeout */

  do {

    /* Decreases the timeout */

    timeout--;

    /* If we have to proceed initial phase */

    if( phase == 0) {

      /* Looks what type of words we have to ask by using the variable
	 ask_for */

      switch( where_in_packet) {

      case 1 :
	ask_for = SLINK_CONTROL_WORD;
	break;

      case 2 :
	ask_for = SLINK_DATA_WORD;
	break;

      case 3 :
	ask_for = SLINK_CONTROL_WORD;
	break;

      default :

	/* There is a bug somewhere */

	return( SLINK_LOST_IN_FUNCTION);

      } /* End of switch( where_in_packet) */

      /* If the UCTRL# line has to be changed */

      if( device->specific.sps.out.bit.UCTRL != ask_for) {

	/* If the fifo is empty */

	if( s5933_regs->mcsr.bit.p2afe == 1) {

	  /* We can change the UCTRL# line */

	  device->specific.sps.out.bit.UCTRL = ask_for;

	  s5933_regs->SPS_OUT_REG.all = device->specific.sps.out.all;

	  /* We did the initial phase */

	  phase = 1;

	} /* End of if fifo is empty */
      } /* End of if we have to change UCTRL# */

      else {

	/* No need to change UCTRL# */

	phase = 1;
      }

    } /* End of initial phase */

    /* If phase >= 1 */

    if( phase >= 1) {

      /* Looks where the packet is being processed */

      switch( where_in_packet) {

      case 1 :

	/* If the first control word isn't sent, does it */

	s5933_regs->fifo = device->params.start_word;

	/* And goes to the next step */

	where_in_packet = 2;
	phase = 0;

	break;

      case 2 :

	/* If we are sending data words */

	if( dma) {

	  /* If we use dma, looks where we are */

	  switch( phase) {

	  case 2 :

	    /* Looks if the DMA operation is finished */

	    if( s5933_regs->mcsr.bit.p2atc == 1) {

	      /* If the transfer is finished, go to the next step */

	      where_in_packet = 3;
	      phase = 0;

	    } /* End of if DMA is finished */

	    else {

	      /* Decides to go out from the loop if the transfer
		 is not finished */

	      timeout = 0;
	    }

	    break;

	  case 1 :
	    /* We didn't have initialize DMA, so we do it */

	    s5933_regs->mrar = bswap(( long) device->initial_address);
	    s5933_regs->mrtc = bswap( device->initial_size);

	    /* and we go to the next step */

	    phase = 2;

	    /* Decides to go out from the loop */

	    timeout = 0;

	    break;

	  default :
	    /* There is a bug somewhere */

	    return( SLINK_LOST_IN_FUNCTION);

	    break;

	  } /* End of switch( phase) */
	} /* End of if dma */

	else {

	  /* If we use CPU to write words on S-LINK */

	  /* Gets the number of words to write */

	  remaining_words = device->number_words;

	  /* Gets the address where to read words */

	  address = device->current_address;

	  /* Does a loop on the timeout */

	  while( timeout-- > 0){

	    /* Reads the MCSR register */

	    device->mcsr.all = s5933_regs->mcsr.all;

	    /* Checks the state of the AMCC fifo */

	    if( device->mcsr.bit.p2afe == 1){
	      free_room = 8;
	    }
	    else {
	      if( device->mcsr.bit.p2af4 == 1){
		free_room = 4;
	      }
	      else {
		if( device->mcsr.bit.p2aff == 0){
		  free_room = 1;
		}
		else {
		  free_room = 0;
		}
	      }
	    }

	    /* By regarding the free places in the fifo,
	       and the remaining words to write, writes a given amount
	       of words */

	   words_to_write= ( free_room > remaining_words) ?
	      remaining_words : free_room;

	    /* Writes the words in the fifo until we lost
	       the right to do it */

	    for( i=0; (i< words_to_write); i++) {
	      s5933_regs->fifo = *( address++);
	    }

	    /* Calculates the remaining size */

	    remaining_words += -( words_to_write);

	    /* Decrements the timeout with the amount of written words */

	    timeout += -( words_to_write);

	    /* If the transfer is finished, go to the next step */

	    if( remaining_words <= 0){

	      where_in_packet = 3;

	      /* Go directly to phase 1 if you don't need to send
		 the end control word */

	      if( (packet_format & SLINK_MASK_STOP_WORD) != 0)
		phase = 0;
	      else
		phase = 1;

	      /* And go out from the while( timeout) */

	      break;
	    }

	  } /* End of while timeout */

	  device->number_words = remaining_words;
	  device->current_address = address;

	} /* End of CPU reading */

	break;

      case 3 :

	/* Only if we have to send control words */

	if( (packet_format & SLINK_MASK_STOP_WORD) != 0) {
	  s5933_regs->fifo = device->params.stop_word;
	}

	/* Increments statistics */

	device->stats.good_packets++;

	/* And ends the transfer */

	device->transfer_status = SLINK_FREE;

	/* Informs the user that the transfer is finished */

	*status = SLINK_FINISHED;

	/* No errors */

	return( SLINK_OK);

	break;

      default :

	/* There is a bug somewhere */

	return( SLINK_LOST_IN_FUNCTION);

	break;

      } /* End of switch( where_in_packet) */

    } /* End of if it'sn't the initial phase */

  } while( timeout > 0); /* End of while */

  /* Saves the variables */

  device->specific.sps.phase = phase;
  device->specific.sps.where_in_packet = where_in_packet;

  /* Returns the status */

  *status = device->transfer_status;

  /* No errors */

  return( SLINK_OK);
}

/****************************************************************************/
/*                                                                          */
/* Name           : LDC_Reset                                               */
/*                                                                          */
/* Description    : Resets the LDC                                          */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* device              | SLINK_device *     | Pointer to the chip structure */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Return value : An error message                                          */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/* 970722 JOP								    */
/*        implement according to new SSP user's guide			    */
/*                                                                          */
/****************************************************************************/

int LDC_Reset( SLINK_device *device)
{
  static volatile s5933_regs *s5933_regs;
  int timeout;

  /* Checks if the device pointer is right */

  if( SSP_bad_structure( device) == SLINK_TRUE){
    return( SLINK_BAD_POINTER);
  }

  /* Gets the address of the registers */

  s5933_regs = device->regs;

  device->specific.ssp.out.bit.URESET = 0;

  s5933_regs->SSP_OUT_REG.all = device->specific.ssp.out.all;

  /* Waits few microseconds */
  SLINK_Sleep( 10);

  /* Waits until LDOWN# is removed unless the timeout expired */

  timeout = 0;

  do {

    /* Reads the ingoing mailbox */

    device->specific.ssp.in.all = s5933_regs->SSP_IN_REG.all;

    /* Waits a milli second */

    SLINK_Sleep( 1000);

    /* Increases timeout */

    timeout++;

    /* Returns an error if the timeout is expired */

    if( timeout > SLINK_RESET_TIMEOUT) {
      return( SLINK_DOWN_LINK);
    }

  } while( device->specific.ssp.in.bit.LDOWN == 0);

  device->specific.ssp.out.bit.URESET = 1;

  s5933_regs->SSP_OUT_REG.all = device->specific.ssp.out.all;

  
  return( SLINK_OK);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SSP_Reset                                               */
/*                                                                          */
/* Description    : Resets a SSP interface                                  */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* device              | SLINK_device *     | Pointer to the chip structure */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A error message                                         */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/* 970722 JOP								    */
/*        change reset sequence a la SSP user's guide			    */
/*        call LDC_Reset to reset the link				    */
/* 970820 Michaela                                                          */
/*        SSP_Reset does the reset of the SSP interface and not the reset   */
/*        of the SLINK                                                      */
/****************************************************************************/

int SSP_Reset( SLINK_device *device)
{
  static volatile s5933_regs *s5933_regs;

  /* Checks if the device pointer is right */

  if( SSP_bad_structure( device) == SLINK_TRUE){
    return( SLINK_BAD_POINTER);
  }

  /* Gets the address of the registers */

  s5933_regs = device->regs;

  /* Resets the add-on fifo */
  device->specific.ssp.out.bit.FRESET = 0;

  s5933_regs->SSP_OUT_REG.all = device->specific.ssp.out.all;

  /* Waits few microseconds */
  SLINK_Sleep( 10);

  /* Resets the add-on card */
  s5933_regs->mcsr.bit.aor = 1;

  SLINK_Sleep( 10);

  /* Resets the S593X's fifo */

  s5933_regs->mcsr.bit.a2pfr = 1;

  SLINK_Sleep( 10);

  /* Removes the reset signal on the add-on card */

  s5933_regs->mcsr.bit.aor = 0;

  SLINK_Sleep( 10);

  /* Removes the reset of the add-on fifo */

  device->specific.ssp.out.bit.FRESET = 1;

  s5933_regs->SSP_OUT_REG.all = device->specific.ssp.out.all;

  /* Does the software reset */

  device->transfer_status = SLINK_FREE;
  
  return( SLINK_OK);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SSP_Open                                                */
/*                                                                          */
/* Description    : Gives access to a SSP interface and initializes it.     */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* params              | SLINK_parameters*  | Includes the initializing     */
/*                     |                    | parameters.                   */
/* ------------------------------------------------------------------------ */
/* device              | SLINK_device **    | Contains the address of a     */
/*                     |                    | structure defining the chip   */
/*                     |                    |when the function returned     */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A error message                                         */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int SSP_Open( SLINK_parameters *params, SLINK_device ** device)
{
  /* Temporary variables */

  int slot = 0;
  int code;
  pci_csh *addr_csh;
  unsigned int pci_cmd_word;
  volatile s5933_regs *regs;

  /* Local procedure to close if an error occured */

  void Close_on_error( int what) {
    switch( what){
    case 10 :
      free( ( void *) *device);
    case 9 :
#ifdef LIBS5933
      S5933_Close( ( s5933_regs *) regs);
#endif
    case 8 :
    case 7 :
    case 6 :
    case 5 :
    case 4 :
      PCI_Detach_Device( slot);
    case 3 :
    case 2 :
      PCI_Close();
    case 1 :
    default :
    }
  }

  /* Access to the PCI library */

  if( PCI_Open() != PCI_SUCCESSFUL) {
    Close_on_error( 1);
    return( SLINK_BAD_OPEN);
  }

  /* Tries to get an access to the PCI bus */

  if( PCI_Find_Device( SLINK_SSP_ID, params->position, &slot) != PCI_SUCCESSFUL){
    Close_on_error( 2);
    return( SLINK_DEVICE_NOT_FOUND);
  }

  if( PCI_Attach_Device( slot, SLINK_SSP_ID, &addr_csh) != PCI_SUCCESSFUL){
    Close_on_error( 3);
    return( SLINK_FORBIDDEN_ACCESS);
  }

  /* Tries to get an access to the S5933 registers */

  if( PCI_Alloc_Space( slot, 0, (void **) &regs) != PCI_SUCCESSFUL){
    Close_on_error( 4);
    return( SLINK_FORBIDDEN_ACCESS);
  }

  /* Initializes the PCICMD word of the PCI configuration space header */

  if( PCI_Read_CSH( slot, pci_csh_cmd, &pci_cmd_word) != PCI_SUCCESSFUL){
    Close_on_error( 5);
    return( SLINK_FORBIDDEN_ACCESS);
  }

  pci_cmd_word |= SLINK_PCICMD;

  if( PCI_Write_CSH( slot, pci_csh_cmd, pci_cmd_word) != PCI_SUCCESSFUL){
    Close_on_error( 6);
    return( SLINK_FORBIDDEN_ACCESS);
  }

#ifdef DEBUG

  /* Displays the CSH */

  if( PCI_Dump_CSH( slot) != PCI_SUCCESSFUL){
    Close_on_error( 7);
    return( SLINK_FORBIDDEN_ACCESS);
  }

#endif

#ifdef LIBS5933
  /* Tries to initialize the S5933 registers */

  if( S5933_Open( ( s5933_regs *) regs) != S5933_SUCCESSFUL){
    Close_on_error( 8);
    return( SLINK_FORBIDDEN_ACCESS);
  }
#endif

  /* Allocates memory for the device structure */

  if( ( *device = ( SLINK_device *) malloc( sizeof( SLINK_device))) == NULL){
    Close_on_error( 9);
    return( SLINK_NOT_ENOUGH_MEMORY);
  }

  /* Initializes the device structure */

  ( *device)->struct_id = SSP_STRUCTURE_ID;

  bcopy( ( char *) params, ( char *) &(( *device)->params), sizeof( SLINK_parameters));

  bzero((char *) &(( *device)->stats), sizeof( SLINK_statistics));

  ( *device)->pci_slot = slot;

  ( *device)->regs = ( s5933_regs *) regs;

  ( *device)->transfer_status = SLINK_FREE;

  ( *device)->error_messages = SLINK_NO_ERROR;

  ( *device)->initial_size = 0;

  ( *device)->packet_size = 0;

  ( *device)->dma = SLINK_ENABLE;

  /* Rejects overflown buffers by default */

  ( *device)->specific.ssp.multiple_pages_transfer = SLINK_DISABLE;

  /* Initialises the output S-LINK register */

  (*device)->specific.ssp.out.all = 0;

  /* Sets the data width */

  (*device)->specific.ssp.out.bit.UDW = (*device)->params.data_width;

  /* Expects control words */

  (*device)->specific.ssp.out.bit.EXPLCTRL = SLINK_CONTROL_WORD;

  /* Expects no errors */

  (*device)->specific.ssp.out.bit.EXPLDERR = SLINK_RIGHT_WORD;

  /* Sets the URESET#, UTDO#, FRESET# and URLs signals */

  (*device)->specific.ssp.out.bit.URESET = 1;
  (*device)->specific.ssp.out.bit.UTDO = 1;
  (*device)->specific.ssp.out.bit.FRESET = 1;
  (*device)->specific.ssp.out.bit.URLs = 0;

  /* Initialises the MCSR register */

  (*device)->mcsr.all = 0;

#ifdef REFRAINED_REQUEST
  /* DMA asks for bus mastering only if 4 or more words in S5933 fifo */

  (*device)->mcsr.bit.wms = 1;
#endif


  /* Resets the DMA counter */

  regs->mwtc = 0;

  /* Initialises the INTCSR register */

  (*device)->intcsr.all = 0;

  /* Enables or disables the byte swapping as requested */

  if( (*device)->params.byte_swapping == SLINK_ENABLE){
    (*device)->intcsr.bit.endian.ec = 2;
  }
  else {
    (*device)->intcsr.bit.endian.ec = 0;
  }

  /* Writes on the out-going mailbox */

  regs->SSP_OUT_REG.all = (*device)->specific.ssp.out.all;

  /* Writes on the MCSr */

  regs->mcsr.all = (*device)->mcsr.all;

  /* Writes on the INTCSR */

  regs->intcsr.all = (*device)->intcsr.all;

  /* Resets the SSP interface */

  if(( code = SSP_Reset( *device)) != SLINK_OK) {
    Close_on_error( 10);
    return( code);
  }
 /* Checks  if SLINK is LDOWN, and if so reset SLINK */
  if(((SSP_in_register) ((dword)(regs->SSP_IN_REG.all))).bit.LDOWN ==0)
      {
      printf("Now reseting the SLINK ....\n");
      code = LDC_Reset(*device);
      if(code !=SLINK_OK)
          {
           printf("LDC_Reset : error =%d\n", code);
           exit(0);
          }
      }       
  /*  Saves the value of the in-going mailbox */

  (*device)->specific.ssp.previous_in.all = regs->SSP_IN_REG.all;

  /* Sets the expected values for the transfer lines LCTRL# and LDERR#
     to their correct values */

  (*device)->specific.ssp.out.transfer_bits.EXPLINES = \
    (*device)->specific.ssp.previous_in.transfer_bits.LINES;

  regs->SSP_OUT_REG.all = (*device)->specific.ssp.out.all;

  /* Enables the DMA if we expects data words */

  if( (*device)->specific.ssp.previous_in.bit.LCTRL == SLINK_CONTROL_WORD) {
    (*device)->mcsr.bit.wte = 0;
  }
  else {
    (*device)->mcsr.bit.wte = 1;
  }

  /* Writes on the MCSR */

  regs->mcsr.all = (*device)->mcsr.all;

#ifdef DEBUG

#ifdef LIBS5933
  /* Displays the content of S593X's registers */

  if( S5933_Dump( regs) != S5933_SUCCESSFUL){
    Close_on_error( 10);
    return( SLINK_FORBIDDEN_ACCESS);
  }
#endif

#endif

  return( SLINK_OK);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SSP_Close                                               */
/*                                                                          */
/* Description    : Removes the access to a SSP interface and frees the     */
/*                  ressources.                                             */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* device              | SLINK_device *     | Pointer to the chip structure */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A error message                                         */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int SSP_Close( SLINK_device *device)
{
  int error_code = SLINK_OK;

  /* Checks it's a right pointer */

  if( device == NULL) {
    return( SLINK_BAD_POINTER);
  }

  /* Checks if the device pointer is right */

  if( SSP_bad_structure( device) == SLINK_TRUE){
    return( SLINK_BAD_POINTER);
  }

#ifdef LIBS5933
  /* Removes the shared memory segment mapped on the AMCC's registers */

  if( S5933_Close( device->regs) != S5933_SUCCESSFUL){
    error_code = SLINK_DETACH_FAILED;
  }
#endif

  /* Removes the access to the PCI slot */

  if( PCI_Detach_Device( device->pci_slot) != PCI_SUCCESSFUL){
    error_code = SLINK_DETACH_FAILED;
  }

  /* Frees the memory occupied by the device structure */

  free(( void *) device);

  return( error_code);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SSP_ReadOneWord                                         */
/*                                                                          */
/* Description    : Reads one control or data word on the specified SLINK   */
/*                  device.                                                 */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* device              | SLINK_device *     | Pointer to the chip structure */
/* ------------------------------------------------------------------------ */
/* word                | long *             | A variable to store the read  */
/*                     |                    | word.                         */
/* ------------------------------------------------------------------------ */
/* type_of_word        | int *              | To tell if it's a control or  */
/*                     |                    | a data word.                  */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A error message                                         */
/*                                                                          */
/* Notes          :                                                         */
/*                                                                          */
/****************************************************************************/

int SSP_ReadOneWord( SLINK_device *device, long *word, int *type_of_word)
{
  register volatile s5933_regs *s5933_regs;

#ifdef FIFO_WAIT
  volatile int t;
#endif

  /* Only for POINTER_CHECK : Checks if the device pointer is right */

#ifdef POINTER_CHECK
  if( SSP_bad_structure( device) == SLINK_TRUE){
    return( SLINK_BAD_POINTER);
  }
#endif

  /* Loads the address of registers */

  s5933_regs = device->regs;

  /* Reads the in-going register */

  device->specific.ssp.in.all = s5933_regs->SSP_IN_REG.all;

  /* Only for LDOWN_CHECK : Checks if the device is down, if so
     exits */

#ifdef LDOWN_CHECK

  /* Checks LDOWN# */

  if( device->specific.ssp.in.bit.LDOWN == 0) {
    return( SLINK_DEVICE_DOWN);
  }

#endif

  if( device->transfer_status != SLINK_FREE) {
    return( SLINK_TRANSFER_NOT_FINISHED);
  }

  /* Checks that the FIFO is not empty */

  if( s5933_regs->mcsr.bit.a2pfe == 1){

    /* Compares the values of LCTRL# and LDERR# to their previous values */

    if( device->specific.ssp.previous_in.transfer_bits.LINES == \
	device->specific.ssp.in.transfer_bits.LINES) {

      /* If they didn't changed the fifo is really empty */

      return( SLINK_FIFO_EMPTY);

    }
    else {

      /* Checks the fifo a second time */

      if( s5933_regs->mcsr.bit.a2pfe == 1) {

	/* If the fifo is still empty, the expected transfer lines really need
	   to be changed */

	device->specific.ssp.out.transfer_bits.EXPLINES = \
	  device->specific.ssp.in.transfer_bits.LINES;

	s5933_regs->SSP_OUT_REG.all = device->specific.ssp.out.all;

	/* Saves the new ingoing-mailbox in the old copy */

	device->specific.ssp.previous_in.all = \
	  device->specific.ssp.in.all;

	/* Now there are two different philosophy : If FIFO_WAIT is
	   chosen, we have to care about PCI retry ( if we don't want
	   to crash the board, so we wait the fifo is no more empty
	   before we read it.If FIFO_CHECK is chosen without FIFO_WAIT, then
	   we signal an error if the fifo is empty */

#ifdef FIFO_WAIT
	/* If the PCI bus can't support the retry, waits the fifo is no
	   more empty */

        while( s5933_regs->mcsr.bit.a2pfe == 1) {
	  SLINK_WaitOneMicro(t);
	}
#else
#ifdef FIFO_CHECK

	/* Checks that the FIFO is not empty as expected.
	   If the hardware works correctly, this test isn't needed.
	   It's why this test is only done with the FIFO_CHECK option. */

	if( s5933_regs->mcsr.bit.a2pfe == 1){
	  return( SLINK_FIFO_EMPTY);
	}
#endif
#endif

      }
    }
  }

  /* Reads the word */

  *word = s5933_regs->fifo;

  /* Gets the type of word */

  *type_of_word = device->specific.ssp.previous_in.bit.LCTRL;

  return( SLINK_OK);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SSP_InitRead                                            */
/*                                                                          */
/* Description    : Initializes a device write master DMA operation         */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* device              | SLINK_device *     | Pointer to the chip structure */
/* ------------------------------------------------------------------------ */
/* address             | char *             | The PCI physical address      */
/*                     |                    | where the DMA starts.         */
/*                     |                    | Or the virtual address where  */
/*                     |                    | the CPU has to read           */
/* ------------------------------------------------------------------------ */
/* size                | int                | The number of bytes to read   */
/* ------------------------------------------------------------------------ */
/* dma                 | int                | To tell if we have to use the */
/*                     |                    | DMA feature or just the CPU   */
/* ------------------------------------------------------------------------ */
/* multiple_pages      | int                | Allows a one packet transfer  */
/*                     |                    | on several pages              */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : A error message                                         */
/*                                                                          */
/* Notes          :                                                         */
/* Modified       : Jun-1997 by M.Niculescu                                 */
/* - Here prepare only the device structure and don't write into  dma       */ 
/*   registers of S5933                                                     */
/*                : Nov-1997 by M.Niculescu                                 */
/* - allows the state SLINK_NEED_OF_NEW_PAGE                                */
/****************************************************************************/

int SSP_InitRead( SLINK_device *device, char *address, int size, int dma, int multiple_pages)
{
  register volatile s5933_regs *s5933_regs;

  /* Only for POINTER_CHECK : Checks if the device pointer is right */

#ifdef POINTER_CHECK
  if( SSP_bad_structure( device) == SLINK_TRUE){
    return( SLINK_BAD_POINTER);
  }
#endif

  /* Loads the address of registers */

  s5933_regs = device->regs;

  /* Only for LDOWN_CHECK : Checks if the device is down, if so
     exits */

#ifdef LDOWN_CHECK

  /* Checks LDOWN# */

  if( ((SSP_in_register) ((dword)
			  (s5933_regs->SSP_IN_REG.all))).bit.LDOWN == 0) {
    return( SLINK_DEVICE_DOWN);
  }

#endif

  /* Analyses current transfer status */

  switch( device->transfer_status) {

  case SLINK_FREE :                /* Transfer not yet started */

    /* Sets dma and multiple flags */
    device->dma = dma;
    device->specific.ssp.multiple_pages_transfer = multiple_pages;

    /* If the device was free, it waits for a packet */
    device->transfer_status = SLINK_WAITING_FOR_SYNC;
    device->error_messages = SLINK_NO_ERROR;
    break;

  case SLINK_NEED_OF_NEW_PAGE :    /* Ask for a new page to continue dam; allow dma init on the new page */   
     break;       

  case SLINK_WAITING_FOR_SYNC :    /* Transfer under way, don't allow dma init */
  case SLINK_RECEIVING :
    return( SLINK_TRANSFER_NOT_FINISHED);
    break;

  default :
    return( SLINK_BAD_STATUS);
    break;
  }

  /* Sets the initial size and inital address */

  device->initial_address = address;
  device->initial_size = size;

  /* If a CPU is asked  */

  if( device->dma != SLINK_ENABLE) {
    /* Sets the current size and current address */

    device->number_words = size >> 2;
    device->current_address =( long *) address;
  } 
  return( SLINK_OK);
}

/****************************************************************************/
/*                                                                          */
/* Name           : SSP_ControlAndStatus                                    */
/*                                                                          */
/* Description    : Gives the current status of the reading operation       */
/*                                                                          */
/* Parameters     :                                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/* Name                | Type               | Function                      */
/* ------------------------------------------------------------------------ */
/* device              | SLINK_device *     | Pointer to the chip structure */
/* ------------------------------------------------------------------------ */
/* status              | int *              | The current status            */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Returned value : An  error message                                       */
/*                                                                          */
/* Notes          :                                                         */
/* Modified       : Jun -1997 by M. Niculescu                               */
/* - when check add on fifo read directly the S5933 registers, don't use    */
/*   the flag fifo_empty                                                    */ 
/* - whenever cpu reads the add on fifo check before that fifo is realy not */ 
/*   empty                                                                  */
/* - when reading the packets by using CPU, do the reading loop until       */
/*   number_words > 0 and stop_flag = 0                                     */
/* Modified       : Nov. -1997 by M. Niculescu                              */
/* - ask for a new page to continue dam transfer                            */
/* - initialise dma to continue the transfer on the new page                */
/****************************************************************************/

int SSP_ControlAndStatus( SLINK_device *device, int *status)
{
  register volatile s5933_regs *s5933_regs;
  static int words_in_fifo;
  static int control_line, error_line;
  static int old_control_line;
  static int dma, timeout;
  static dword control_word;
  static int init_page = 0;
  static int number_words, words_to_read;
  static long * address;
  static int stop_flag, i, nretry=0;
  int word;

  /* Only for POINTER_CHECK : Checks if the device pointer is right */

#ifdef POINTER_CHECK
  if( SSP_bad_structure( device) == SLINK_TRUE)
    {
      return( SLINK_BAD_POINTER);
    }
#endif

  /* Loads the address of registers */

  s5933_regs = device->regs;

  /* Only for LDOWN_CHECK : Checks if the device is down, if so
     exits */

#ifdef LDOWN_CHECK

  /* Checks LDOWN# */

  if( ((SSP_in_register) 
       (( dword)(s5933_regs->SSP_IN_REG.all))).bit.LDOWN == 0)
     return( SLINK_DEVICE_DOWN);
    
#endif

  /* Analyses current transfer status */

  switch( device->transfer_status) {

  case SLINK_RECEIVING :            /* accept all of them */
  case SLINK_NEED_OF_NEW_PAGE :     
  case SLINK_WAITING_FOR_SYNC :
    /* Normal modes : continue */
    break;

  case SLINK_FREE :
    return( SLINK_TRANSFER_NOT_STARTED);
    break;

  default :
    return( SLINK_BAD_STATUS);
    break;
  }

  /* Gets the DMA flag */

  dma = device->dma;

  /* Gets the timeout */

  timeout = device->params.timeout;

  /* Initializes the type of word flag */

  control_line = device->specific.ssp.previous_in.bit.LCTRL;

  /* Loop on the timeout */

  do {

    /*--------------------------------------------------------------------*/
    /* Part I : Commutation of the expected lines EXPLCTRL# and EXPLDERR# */
    /*--------------------------------------------------------------------*/

    /* Checks that the FIFO is not empty to continue */

    if( s5933_regs->mcsr.bit.a2pfe == 0){
#ifdef DEBUG
      printf("part I/1, fifo not empty \n");
#endif
    }
    else { /* 1, fifo empty for the moment, check it more */

      /* Reads the in-going register */

      device->specific.ssp.in.all = s5933_regs->SSP_IN_REG.all;
      nretry++;

      /* Compares the values of LCTRL# and LDERR# to their previous values */
    
      if( device->specific.ssp.previous_in.transfer_bits.LINES == \
	  device->specific.ssp.in.transfer_bits.LINES) {

	/* If they didn't change  the fifo is really empty */
      }
      else { /* 2 , expected lines != actual lines, but fifo still not empty */

	/* Checks the fifo a second time */

	if( s5933_regs->mcsr.bit.a2pfe == 0) {
	  /* The fifo is no more empty */
#ifdef DEBUG
          printf("part I/2, fifo not empty \n");
#endif
	}

	/* If the fifo is still empty, the expected transfer lines really need
	   to be changed */

	else { /* 3 , fifo is realy empty and we will change the expected lines to go further */
	  /* Saves the value of control_line */

	  old_control_line = control_line;

	  /* Gets the flags for the transfer lines LCTRL# and LDERR# */

          control_line = (device->specific.ssp.in.bit.LCTRL);
          error_line = (device->specific.ssp.in.bit.LDERR );

	  /* Signals an error in the current packet if it's the case */

	  if( error_line == SLINK_WRONG_WORD) {
	    device->specific.ssp.current_error = 1;
	  }

	  /* Saves the new ingoing-mailbox in the old copy */

	  device->specific.ssp.previous_in.all = \
	    device->specific.ssp.in.all;

	  /* If the control flag has changed */

	  if( dma == SLINK_ENABLE) {
	    if( control_line != old_control_line) {
#ifdef DEBUG
            printf("part I/3, control_line = 0x%x old_control_line =0x%x \n",control_line, old_control_line);
#endif
	      /* Disable DMA if we got control words */

	      if( control_line == SLINK_CONTROL_WORD)
		{
		device->mcsr.bit.wte = 0; /* fifo is empty and the next words will be control words read by CPU, not DMA */
                /* Writes to the MCSR register */
                s5933_regs->mcsr.all = device->mcsr.all;
#ifdef DEBUG
                printf("part I/3, Disable DMA \n");
#endif
		} 
	    } 
	  } 
      

	  /* Modifies the expected transfer lines in the out-going mailbox */
	  
	  device->specific.ssp.out.transfer_bits.EXPLINES = \
	    device->specific.ssp.in.transfer_bits.LINES;

	  s5933_regs->SSP_OUT_REG.all = device->specific.ssp.out.all;

	} /* 3 */
      } /* 2 */
    } /* 1 */

    /*-----------------------------------*/
    /* Part II : Synchronisation control */
    /*-----------------------------------*/

    /* Are we receiving control words ? */

    if( control_line == SLINK_CONTROL_WORD) {
      if( s5933_regs->mcsr.bit.a2pfe == 0) {
	/* ... we know there is a control word in the fifo and we read it */
	control_word = s5933_regs->fifo;
#ifdef DEBUGB
        printf("part II, Receive control word 0x%x\n",control_word);
#endif

	/* We can analyse the control word */

	/* If the word is a starting word for a data packet */

#ifdef LINK_4BITS
  if( (control_word & 0x80000000) == (device->params.start_word & 0x80000000) ) { 
#else
  if( (control_word & ~0xF) == (device->params.start_word & ~0xF) ) {
#endif

	  /* If a previous packet was still being received */

	  if( device->transfer_status == SLINK_RECEIVING) {

	    /* ... then put the error in statistics */

	    device->stats.lost_ends++;
	  }

	  /* We are now transferring a new packet */

	  device->transfer_status = SLINK_RECEIVING;

	  /* Resets the packet size */

	  device->packet_size = 0;

	  /* We assumes there is no error in the current packet */

	  device->specific.ssp.current_error = 0;

	  /* Signals we have to restart the transfer at the beginning of
	     the page. */

	  init_page = 1;  /* Beginning pf a new packet */
#ifdef DEBUG
          printf("part II, Beginning of a new packet \n");
#endif
	}

	/* If the control word signals the end of a data packet */

#ifdef LINK_4BITS
	else if( (control_word & 0x80000000) == (device->params.stop_word & 0x80000000) ) {
/*      else if (1) { */
#else
        else if( (control_word & ~0xF) == (device->params.stop_word & ~0xF) ) {
/*      else if (1) { */
#endif

	  /* If we were transferring a data packet before */

	  if( device->transfer_status == SLINK_RECEIVING) {

	    /* If a transmission error occured in this data packet */

	    if( device->specific.ssp.current_error != 0) {

	      /* Inserts it in statistics */

	      device->stats.transmission_errors++;

	      /* If corrupt packets have to be rejected */

	      if( device->params.keep_bad_packets == SLINK_DISABLE) {

		/* Aborts the transfer of the packet and waits for a new one */

		device->transfer_status = SLINK_WAITING_FOR_SYNC;
	      }
	    }

	    /* If we have kept the packet */

	    if( device->transfer_status == SLINK_RECEIVING) {

	      /* Stops the transfer */

	      device->transfer_status = SLINK_FREE;

	      /* Computes the size of the packet */

	      /* If the dma was chosen */

	      if( dma){

		/* Reads the DMA counter */

		device->packet_size += device->initial_size - \
		  bswap( s5933_regs->mwtc);
#ifdef DEBUG
	      printf("part II,End of packet\n");
#endif
	      }

	      /* If the CPU was used */

	      else {

		/* Computes the size */

		device->packet_size += device->initial_size - \
		  ( device->number_words << 2);
	      }

	      /* Increments statistics */

	      device->stats.good_packets++;

	      /* Exits from the function and informs the transfer
		 is finished. */

	      *status = SLINK_FINISHED;

	      return( SLINK_OK);

	    } /* End of test about transmission errors */
	  }
	  else {
	   /* If no transfer was begun, it means we lost the beginning  of a packet */
           /* Continue the transfer until a the control word for the begining of a new packet will appear */
           /* The first transfer, for the uncomplite packet will be overwritten by the good packet which follows */ 
	    device->stats.lost_starts++;
#ifdef DEBUG
            printf("part II, rec. packet without start, device status =%d\n", device->transfer_status);
#endif
	  } /* End of test about transfer status */
	}

	/* If we receive another control word */

	else {

	  /* Signals it in the statistics */

	  device->stats.unknown_control_words++;

	} /* End of test about the read control word */
      } /* End of test about fifo */
    } /* End of test about control flag */

    /*--------------------*/
    /* Part III : Reading */
    /*--------------------*/

    /*---------------------------------------------------*/
    /* The reading algorithm depends on using DMA or not */
    /*---------------------------------------------------*/

    if( dma) {

      /* If we are reading data words */

      if( control_line == SLINK_DATA_WORD) {
#ifdef DEBUG
         printf(" part III, Reading data words under DMA \n");
#endif

	/* Reads the MCSR register */

	device->mcsr.all = s5933_regs->mcsr.all;

	/* Checks the DMA counter equal 0 */

	if( device->mcsr.bit.a2ptc == 1) {

	  /* If the fifo isn't empty */

	  if( device->mcsr.bit.a2pfe == 0) {

#ifdef DEBUG
	   printf("part III, dma is finished, but there are still data into fifo\n"); 
           printf("part III, device status = %d\n", device->transfer_status);
#endif 
                      
            if(device->transfer_status == SLINK_WAITING_FOR_SYNC)
             /* allow a dma dummy data transfer to empty the fifo, help to start the syncronization */
               init_page =1;
	    if( device->transfer_status == SLINK_RECEIVING) 
            /* Ask for a new page to continue the dma transfer */
               {
	       /* Need of a new page to continue dma transfer of the current packet */

	       device->transfer_status = SLINK_NEED_OF_NEW_PAGE;
               *status = device->transfer_status;
               device->packet_size += device->initial_size; /* update the size of the current  received data */
#ifdef DEBUG
          printf("part III, dma finished in the current page, read %d bytes\n", device->packet_size);
#endif
               return (SLINK_OK);
	       } /* End of if status is transfer */

            if( device->transfer_status == SLINK_NEED_OF_NEW_PAGE)
              {
              init_page =1;       /* Beginning of a new page in the transfer of the current packet */
              device->transfer_status = SLINK_RECEIVING;
              } 

	  } /* End of fifo isn't empty */
	} /* End of if DMA counter equal 0 */
      } /* End of if we read data words */

      /* If we have to initialize transfer on the beginning of the page */

      if( init_page) { /* for all: dummy data,new packet ,new page of the current packet */

	 /* Resets the flag  */

	init_page = 0;

	/*  Initializes & enable  DMA  */

	s5933_regs->mwar = bswap( (long) device->initial_address);
	s5933_regs->mwtc = bswap( device->initial_size);
        s5933_regs->mcsr.bit.wte = 1; /* enable DMA */
#ifdef DEBUG
       printf("part III, At beginning of the packet init. & enable  DMA \n"); 
#endif
      } 

    } /* End of DMA reading */

    /*---------------------------------*/
    /* If the CPU directly reads words */
    /*---------------------------------*/

    else {

      /* If we are reading data words */

      if( control_line == SLINK_DATA_WORD) {

	/* Gets the current transfer parameters */

	address = device->current_address;
	number_words = device->number_words;

#ifdef DEBUG
        printf("CPU is writing %d data words to address %x\n",number_words,address);
#endif
	/* We don't want immediately to go out from the loop */

	stop_flag = 0;

	do { /* do while number_words >0 and stop_flag =0 */

	  /* If the init_page flag is not set */

	  if( init_page == 0) {

	    /* If the page is full */

	    if( number_words == 0) {

	      /* If the fifo isn't empty */

	      if( s5933_regs->mcsr.bit.a2pfe == 0) {

		/* If we were receiving a data packet */

		if( device->transfer_status == SLINK_RECEIVING) {

		  /* If we allow a one packet transfer on several pages */

		  if( device->specific.ssp.multiple_pages_transfer ==
		      SLINK_ENABLE) {

		    /* Computes the new packet size */

		    device->packet_size += device->initial_size;

		    /* Asks for a new page */

		    device->transfer_status = SLINK_NEED_OF_NEW_PAGE;

		    /* Stores the event in statistics */

		    device->stats.asks_for_new_page++;

		    /* Exits from the function and returns the status. */

		    *status = device->transfer_status;

		    return( SLINK_OK);

		  }
		  else {

		    /* Initalizes the transfer at the beginning of the page */

		    init_page = 1;

		    /* Aborts the transfer */

		    device->transfer_status = SLINK_WAITING_FOR_SYNC;

		    /* Signals it to statistics */

		    device->stats.overflown_buffers++;

		  } /* End of else of if multiple_pages_transfer == enable */
		} /* End of if status = SLINK_RECEIVING */

		/* If we weren't receiving a packet */

		else {

		  /* Initalizes the transfer at the beginning of the page */

		  init_page = 1;

		} /* End of else of if status is transfer */
	      } /* End of fifo isn't empty */
	    } /* End of if number words == 0 */
	  } /* End of if init_page == 0 */

	  /* If we have to initialize transfer at the beginning of the page */

	  if( init_page) {

	    /* Resets the flag */

	    init_page = 0;

	    /* Initializes the CPU reading */

	    address = (long *) device->initial_address;
	    number_words = (device->initial_size >> 2);
#ifdef DEBUG
            printf("part III, Initialise  CPU writing of %d data words to %x address \n",number_words,address);
#endif
	  } /* End of if init_page */

	  /* Reads the MCSR register */

	  device->mcsr.all = s5933_regs->mcsr.all;

	  /* Checks the state of the S5933 fifo */

	  if( device->mcsr.bit.a2pff == 1){
	    words_in_fifo = 8;
	  }
	  else {
	    if( device->mcsr.bit.a2pf4 == 1){
	      words_in_fifo = 4;
	    }
	    else {
	      if( device->mcsr.bit.a2pfe == 0){
		words_in_fifo = 1;
	      }
	      else {
		words_in_fifo = 0;

		/* Because the fifo is empty we have to see what it happens,
		   so we go out from the reading loop */

		stop_flag = 1;
#ifdef DEBUG
              printf("part III, now stop fifo empty\n");
#endif
	      }
	    }
	  }

	  /* By regarding the number of words in the fifo, and the remaining
	     words to write, reads a given amount of words */

	  words_to_read = ( words_in_fifo > number_words) ? number_words : \
	    words_in_fifo;

	  /* Reads the words in the fifo until we lost the right to do it */

	  for( i=0; (i<words_to_read); i++) {
            if(s5933_regs->mcsr.bit.a2pfe==0){ /* only if fifo not empty read  the data */
                word = s5933_regs->fifo;	
	    	*( address) = word; 
#ifdef DEBUGB
                printf("part III, write to %x address , %x data\n",address,word);
#endif
                address++;
                }
            else {
                printf("part III, break because fifo empty\n"); 
            	break; }
	  }

	  /* Calculates the remaining size */

	  number_words += -( words_to_read);

	  /* Start again the loop */
#ifdef DEBUG
         printf("part III,small loop again, remaining %d words with %d timeout\n",number_words,timeout);
#endif
	} while((number_words > 0) && ( !stop_flag));

	/* Saves the current transfer parameters */

	device->current_address = address;
	device->number_words = number_words;
#ifdef DEBUG
        printf(" part III, Current %d data words to write to %x address\n",number_words,  address);
#endif
      } /* End of if we read data words */

    } /* End of CPU reading */
#ifdef DEBUG
    printf(" part III, Do big loop again with %d timeout\n",timeout);
#endif
  } while( --timeout > 0);
 
  /* Until timeout is exhausted */

  *status = device->transfer_status;

  return( SLINK_OK);
}
