/*
** Copyright 2002-2006 KVASER AB, Sweden.  All rights reserved.
*/

//
// Linux/WinCE USBcanII driver
//

#ifndef _USBCAN_HW_IF_H_
#define _USBCAN_HW_IF_H_


#if WIN32
#include "vcanevt.h"
#include "VCanOSif.h"
#endif

#include "osif_kernel.h"
#include "helios_cmds.h"

/*****************************************************************************/
/* defines */
/*****************************************************************************/

#if LINUX
#define DEVICE_NAME_STRING              "usbcanII"
#endif
#define MAX_CHANNELS                             2
#define KV_USBCAN_MAIN_RCV_BUF_SIZE             16
#define KV_USBCAN_TX_CMD_BUF_SIZE               16
#define USBCAN_CMD_RESP_WAIT_TIME              200
#define USBCANII_TICKS_PER_MS                  100


// Bits in the CxSTRH register in the M16C.
#define M16C_BUS_RESET    0x01    // Chip is in Reset state
#define M16C_BUS_ERROR    0x10    // Chip has seen a bus error
#define M16C_BUS_PASSIVE  0x20    // Chip is error passive
#define M16C_BUS_OFF      0x40    // Chip is bus off



#if DEBUG
#   define USBCAN_Q_CMD_WAIT_TIME                 800
#else
#   define USBCAN_Q_CMD_WAIT_TIME                 200
#endif


typedef struct UsbcanWaitNode {
  struct list_head list;
  OS_IF_SEMAPHORE   waitSemaphore;
  heliosCmd *replyPtr;
  unsigned char cmdNr;
  unsigned char transId;
  unsigned char timedOut;
} UsbcanWaitNode;



/* Channel specific data */
typedef struct UsbcanChanData
{
  /* These are the number of outgoing packets residing in the device */
  unsigned int outstanding_tx;
  OS_IF_LOCK   outTxLock;
} UsbcanChanData;



/*  Cards specific data */
typedef struct UsbcanCardData {

  // qqq Max_outstanding_tx is received from the card
  CAN_MSG          current_tx_message[128];
  unsigned int     max_outstanding_tx;

  OS_IF_LOCK       timeHi_lock;

  OS_IF_LOCK       replyWaitListLock;
  struct list_head replyWaitList;

  /* Structure to hold all of our device specific stuff */
  OS_IF_WQUEUE                *txTaskQ;
  OS_IF_TASK_QUEUE_HANDLE     txWork;

  heliosCmd          txCmdBuffer[KV_USBCAN_TX_CMD_BUF_SIZE]; /* Control messages */
#if 0
  unsigned int       txCmdBufHead;   /* Where we write outgoing control messages */
  unsigned int       txCmdBufTail;   /* The messages are sent from this end */

  OS_IF_WAITQUEUE_HEAD  txCmdWaitQ;    /* WaitQ for sending commands */
#else
  Queue              txCmdQueue;
#endif

  // busparams
  unsigned long freq;
  unsigned char sjw;
  unsigned char tseg1;
  unsigned char tseg2;
  unsigned char samples;

#if LINUX
  struct usb_device       *udev;               // save off the usb device pointer
  struct usb_interface    *interface;          // the interface for this device
#else
  LPCUSB_FUNCS            usb_funcs;
#endif

  unsigned char *         bulk_in_buffer;      // the buffer to receive data
  size_t                  bulk_in_size;        // the size of the receive buffer
  __u8                    bulk_in_endpointAddr;// the address of the bulk in endpoint
#if LINUX
  unsigned int            bulk_in_MaxPacketSize;
#else
  int                     bulk_in_MaxPacketSize;
#endif

#if LINUX
  unsigned char *         bulk_out_buffer;     // the buffer to send data
#endif
  size_t                  bulk_out_size;       // the size of the send buffer
#if LINUX
  unsigned int            bulk_out_MaxPacketSize;
#else
  int                     bulk_out_MaxPacketSize;
#endif

  struct urb *            write_urb;           // the urb used to send data
  struct urb *            read_urb;            // the urb used to receive data
  __u8                    bulk_out_endpointAddr;//the address of the bulk out endpoint
  OS_IF_SEMAPHORE         write_finished;       // wait for the write to finish

  volatile int            present;              // if the device is not disconnected
  OS_IF_SEMAPHORE         sem;                  // locks this structure

  VCanCardData           *vCard;
} UsbcanCardData;


int usbcan_init_driver(void);
int usbcan_set_busparams(VCanChanData *vChd, VCanBusParams *par);
int usbcan_get_busparams(VCanChanData *vChd, VCanBusParams *par);
int usbcan_set_silent(VCanChanData *vChd, int silent);
int usbcan_set_trans_type(VCanChanData *vChd, int linemode, int resnet);
int usbcan_bus_on(VCanChanData *vChd);
int usbcan_bus_off(VCanChanData *vChd);
int usbcan_get_tx_err(VCanChanData *vChd);
int usbcan_get_rx_err(VCanChanData *vChd);
int usbcan_outstanding_sync(VCanChanData *vChan);
int usbcan_close_all(void);
int usbcan_proc_read(char *buf, char **start, off_t offset,
                     int count, int *eof, void *data);
int usbcan_get_chipstate(VCanChanData *vChd);
unsigned long usbcan_get_time(VCanCardData *vCard);
int usbcan_flush_tx_buffer(VCanChanData *vChan);
int usbcan_schedule_send(VCanCardData *vCard, VCanChanData *vChan);
unsigned long usbcan_get_hw_rx_q_len(VCanChanData *vChan);
unsigned long usbcan_get_hw_tx_q_len(VCanChanData *vChan);
int usbcan_translate_and_send_message(VCanChanData *vChan, CAN_MSG *m);
#if !LINUX
int usbcan_plugin(USBCAN_CONTEXT *usbcan_context);
void usbcan_remove(USBCAN_CONTEXT *usbcan_context);
#endif

#endif  /* _USBCAN_HW_IF_H_ */
