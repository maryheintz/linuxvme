/****************************************************************************/
/*                                                                          */
/* Header file :                                                            */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/* File Name   : SLINK.h                                                    */
/*                                                                          */
/* Title       : Header file for the library libSLINK.c                     */
/*                                                                          */
/* Version     : 1.0                                                        */
/*                                                                          */
/* Description :                                                            */
/*               Only useful to use the SPS and SSP S-LINK interfaces       */
/*               Small set of functions designed for the prototype -1       */
/*               in the ATLAS experiment.                                   */
/*                                                                          */
/* Author      : F.Pennerath                                                */
/*                                                                          */
/* Notes       :                                                            */
/*                                                                          */
/****************************************************************************/

#ifndef SLINK_v1
#define SLINK_v1 0

/****************************************************************************/
/*                                                                          */
/*                       Includes header files                              */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*                       Declaration of constants                           */
/*                                                                          */
/****************************************************************************/

#define NULL           0
#define SLINK_ENABLE   1
#define SLINK_DISABLE  0
#define SLINK_TRUE     1
#define SLINK_FALSE    0

#define SLINK_MASK_START_WORD 1
#define SLINK_MASK_STOP_WORD 2

/****************************************************************************/
/*                                                                          */
/*                  Default values for control words                        */
/*                                                                          */
/****************************************************************************/

#define SLINK_DEFAULT_START_CONTROL_WORD 0x11111110
#define SLINK_DEFAULT_STOP_CONTROL_WORD 0xFFFFFFF0

/****************************************************************************/
/*                                                                          */
/*                            S-LINK packet formats                         */
/*                                                                          */
/****************************************************************************/

typedef enum {
  SLINK_NO_CONTROL_WORD = 0,
  SLINK_START_CONTROL_WORD = 1,
  SLINK_STOP_CONTROL_WORD = 2,
  SLINK_BOTH_CONTROL_WORDS = 3
} SLINK_packet_format;

/****************************************************************************/
/*                                                                          */
/*                            S-LINK device types                           */
/*                                                                          */
/****************************************************************************/

typedef enum {
  SPS,
  SSP,
  UNKNOWN
} SLINK_device_type;

/****************************************************************************/
/*                                                                          */
/*                     Error codes returned by functions                    */
/*                                                                          */
/****************************************************************************/

typedef enum {
  SLINK_OK = 0,
  SLINK_BAD_OPEN,
  SLINK_DEVICE_NOT_FOUND,
  SLINK_FORBIDDEN_ACCESS,
  SLINK_NOT_ENOUGH_MEMORY,
  SLINK_BAD_POINTER,
  SLINK_BAD_DATA_WIDTH,
  SLINK_BAD_PARAMETERS,
  SLINK_DEVICE_DOWN,
  SLINK_FIFO_FULL,
  SLINK_FIFO_EMPTY,
  SLINK_DETACH_FAILED,
  SLINK_TRANSFER_NOT_FINISHED,
  SLINK_TRANSFER_NOT_STARTED,
  SLINK_DOWN_LINK,
  SLINK_LOST_IN_FUNCTION,
  SLINK_NO_PAGE,
  SLINK_BAD_STATUS,

  /* Be careful, inserts new codes before this one */

  SLINK_INSERT_NEW_ERROR_CODES_BEFORE_THIS_ONE
} SLINK_error_types;

/****************************************************************************/
/*                                                                          */
/*                     Status codes returned by functions                   */
/*                                                                          */
/****************************************************************************/

typedef enum {
  SLINK_FREE,
  SLINK_WAITING_FOR_SYNC,
  SLINK_REJECT,
  SLINK_RECEIVING,
  SLINK_SENDING,
  SLINK_NEED_OF_NEW_PAGE,
  SLINK_FINISHED,

  /* Be careful, inserts new codes before this one */

  SLINK_INSERT_NEW_STATUS_CODES_BEFORE_THIS_ONE
} SLINK_status_types;

/****************************************************************************/
/*                                                                          */
/*                    Events recored in statistics                          */
/*                                                                          */
/****************************************************************************/

typedef enum {
  SLINK_NO_ERROR,
  SLINK_INTERFACE_DOWN,
  SLINK_TRANSMISSION_ERROR,
  SLINK_PACKET_SMASHED,
  SLINK_OVERFLOW,
  SLINK_UNKNOWN_CONTROL_WORD,

  /* Be careful, inserts new codes before this one */

  SLINK_INSERT_NEW_EVENT_CODES_BEFORE_THIS_ONE
} SLINK_event_types;

/****************************************************************************/
/*                                                                          */
/*                    Constants to specify the data width                   */
/*                                                                          */
/****************************************************************************/

#define SLINK_8BITS	2
#define SLINK_16BITS	1
#define SLINK_32BITS	0

/****************************************************************************/
/*                                                                          */
/*                 Constants to specify the type of words                   */
/*                                                                          */
/****************************************************************************/
 
#define SLINK_CONTROL_WORD 0
#define SLINK_DATA_WORD    1

/****************************************************************************/
/*                                                                          */
/*                 Constants to specify the error line values               */
/*                                                                          */
/****************************************************************************/

#define SLINK_WRONG_WORD   0
#define SLINK_RIGHT_WORD   1

/****************************************************************************/
/*                                                                          */
/*          Constants to create typical delay of one microsecond            */
/*                                                                          */
/****************************************************************************/

#define SLINK_MICROSECOND 5

/****************************************************************************/
/*                                                                          */
/*                         Declaration of macros                            */
/*                                                                          */
/****************************************************************************/

/* To fill the parameters structure */

#define SLINK_InitParameters( params) \
  ( params)->position = 0; \
  ( params)->data_width = SLINK_32BITS; \
  ( params)->byte_swapping = SLINK_ENABLE; \
  ( params)->timeout = 10; \
  ( params)->start_word = ( SLINK_DEFAULT_START_CONTROL_WORD) & ~0xF; \
  ( params)->stop_word =  ( SLINK_DEFAULT_STOP_CONTROL_WORD) & ~0xF; \
  ( params)->keep_bad_packets = SLINK_ENABLE

/* To wait about a micro second */

#define SLINK_WaitOneMicro( t) \
  for( t=0; (t)<SLINK_MICROSECOND; (t)++)

/* To set the timeout parameter */

#define SLINK_Timeout( dev, timeout_value) \
  ( dev)->params.timeout = ( timeout_value)

/* To set the control word put at the beginning of data packet */

#define SLINK_StartControlWord( dev, word_value) \
  ( dev)->params.start_word = ( word_value) & ~0xF

/* To set the control word put at the end of data packet */

#define SLINK_StopControlWord( dev, word_value) \
  ( dev)->params.stop_word = ( word_value) & ~0xF

/* To set the keep/reject flag */

#define SSP_KeepBadPacket( dev, flag) \
  ( dev)->params.keep_bad_packets = ( flag)

/* To get the packet size */

#define SSP_PacketSize( dev) \
    (( dev)->packet_size)

/* To continue a one S-LINK packet transfer in several buffers */

#define SSP_ContinueRead( dev, addr, size) \
    SSP_InitRead( (dev), (addr), (size), 0, 0)

/* To get the transfer status */

#define SLINK_Status( dev) \
  (( dev)->transfer_status)


/****************************************************************************/
/*                                                                          */
/*                          Declaration of types                            */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*              S-LINK registers for the SPS and SSP interfaces             */
/*                                                                          */
/****************************************************************************/

typedef union {
  struct {
    dword       : 27;
    dword LDOWN : 1;
    dword LRLs  : 4;
  } bit;
  dword all;
} SPS_in_register;

typedef union {
  struct{
    dword        : 3;
    dword UDW    : 2;
    dword URESET : 1;
    dword UTEST  : 1;
    dword UCTRL  : 1;
    dword        : 24;
  } bit;
  long all;
} SPS_out_register;

typedef union {
  struct {
    dword       : 28;
    dword OVFLW : 1;
    dword LDOWN : 1;
    dword LCTRL : 1;
    dword LDERR : 1;
  } bit;
  struct {
    dword       : 30;
    dword LINES : 2;
  } transfer_bits;
  dword all;
} SSP_in_register;

typedef union {
  struct {
    dword          : 3;
    dword UDW      : 2;
    dword URESET   : 1;
    dword UTDO     : 1;
    dword FRESET   : 1;
    dword          : 4;
    dword URLs     : 4;
    dword          : 14;
    dword EXPLCTRL : 1;
    dword EXPLDERR : 1;
  } bit;
  struct {
    dword          : 30;
    dword EXPLINES : 2;
  } transfer_bits;
  dword all;
} SSP_out_register;


/****************************************************************************/
/*                                                                          */
/*              S-LINK structure to submit parameters at opening           */
/*                                                                          */
/****************************************************************************/

typedef struct {
  int position;	    
                  /* position of the given interface on the PCI bus  */
  int data_width;
                  /* the data width to set on the S-LINK link.Can be */
                  /* equal to SLINK_8BITS, SLINK_16BITS or         */
                  /* SLINK_32BITS                                   */
  int byte_swapping;
	          /* To enable or disable the byte swapping on the    */
		  /* hardware chip ( if this feature exists)         */
		  /* Can be equal to SLINK_ENABLE or SLINK_DISABLE */
  int timeout;
                  /* Timeout used if the dma is not used but */
		  /* the CPU                                 */
  dword start_word;
                  /* Control word at the beginning of a
		     data pattern */
  dword stop_word;
                  /* Control word at the end of a
		     data pattern */
  int keep_bad_packets;
                  /* Flag to keep or reject corrupt packets */

} SLINK_parameters;

/****************************************************************************/
/*                                                                          */
/*           SLINK structure for statistics about errors                    */
/*                                                                          */
/****************************************************************************/

typedef struct {
  int good_packets;
  int transmission_errors;
  int lost_ends;
  int lost_starts;
  int unknown_control_words;
  int overflown_buffers;
  int asks_for_new_page;
} SLINK_statistics;

/****************************************************************************/
/*                                                                          */
/*           SLINK structure for specific fields for SPS and SSP            */
/*                                                                          */
/****************************************************************************/

typedef struct {
  SPS_out_register out;         /* Out-going mailbox copy */
  SPS_in_register in;           /* In-going mailbox copy */
  SLINK_packet_format packet_format; 
                                /* To know if we have to embed data packets
				   with control words. */
  int where_in_packet;          /* To know where we are in sending data
				   packets */
  int phase;                    /* A substate of where_in_packet */
} SPS_specific;

typedef struct {
  SSP_out_register out;         /* Out-going mailbox copy */
  SSP_in_register in;           /* In-going mailbox copy */
  SSP_in_register previous_in;  /* Previous in-going mailbox copy */
  int multiple_pages_transfer;  /* To do a one packet transfer
				   on several pages*/
  int current_error;
} SSP_specific;

/****************************************************************************/
/*                                                                          */
/*                 SLINK structure to manage the S-LINK device             */
/*                                                                          */
/****************************************************************************/

typedef struct {
  long struct_id;               /* To check coherency of pointer */
  SLINK_parameters params;	/* Parameters for the device */
				/* Includes data width, position on PCI, */
				/* and byte swapping enable flag          */
  int pci_slot;			/* Occupied slot on the PCI bus */
  s5933_regs *regs;             /* Pointer on the S5933's registers */
  s5933_mcsr mcsr;              /* To store the mcsr register */
  s5933_intcsr intcsr;          /* To store the intcsr register */
  SLINK_statistics stats;       /* To store statistics about errors */
  int transfer_status;		/* Status of the current transfer */
  int error_messages;		/* The last error which occured */
  int initial_size;             /* The initial size of the buffer */
  int number_words;		/* The number of words to write or read */
  int packet_size;              /* The real size of the packet */
  int dma;			/* Says if the dma feature has to be used */
				/* for the current transfer               */
  char *initial_address;        /* The PCI physical address where the buffer
				   begins or its virtual address */
  long *current_address;	/* Current address where to write or to read */
				/* only used when the CPU is used,           */
  union {                       /* Union for specific fields for SPS and SSP */
    SPS_specific sps;
    SSP_specific ssp;
  } specific;
} SLINK_device;

/****************************************************************************/
/*                                                                          */
/*                        Declaration of functions                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*                For a general usage aroung S-LINK library                 */
/*                                                                          */
/****************************************************************************/

char * SLINK_GetErrorMessage( int error_code);
char * SLINK_GetStatusMessage( int status_code);
int SLINK_PrintErrorMessage( int error_code);
int SLINK_PrintState( SLINK_device *device);
SLINK_device_type SLINK_WhichTypeOfDevice( SLINK_device *device);

/****************************************************************************/
/*                                                                          */
/*            For the SPS interface ( i.e. Simple PMC to S-LINK)            */
/*                                                                          */
/****************************************************************************/

int SPS_Open( SLINK_parameters *params, SLINK_device ** device);
int SPS_Close( SLINK_device *device);
int SPS_Reset( SLINK_device *device);
int SPS_WriteOneWord( SLINK_device *device, long word, int type_of_word);
int SPS_InitWrite( SLINK_device *device, char *address,
		   int size, SLINK_packet_format packet_format, int dma);
int SPS_ControlAndStatus( SLINK_device *device, int *status);

/****************************************************************************/
/*                                                                          */
/*            For the SSP interface ( i.e. Simple S-LINK to PMC)            */
/*                                                                          */
/****************************************************************************/

int SSP_Open( SLINK_parameters *params, SLINK_device ** device);
int SSP_Close( SLINK_device *device);
int SSP_Reset( SLINK_device *device);
int SSP_ReadOneWord( SLINK_device *device, long *word, int *type_of_word);
int SSP_InitRead( SLINK_device *device, char *address, int size, int dma, int multiple_pages);
int SSP_ControlAndStatus( SLINK_device *device, int *status);
#endif

/* Prototypes that were missing in the original implemntation of this */
/* library. I don't know which section they were supposed to go it... */
int LSC_Reset( SLINK_device *);
