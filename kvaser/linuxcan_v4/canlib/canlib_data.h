/*
** Copyright 2002-2006 KVASER AB, Sweden.  All rights reserved.
*/

/* Kvaser Linux Canlib */

#ifndef _CANLIB_DATA_H_
#define _CANLIB_DATA_H_


#include "linkedlist.h"

#include "osif_user.h"

#if !LINUX
#include "osif_kernel.h"
#include "vcanevt.h"
#endif

#define DEVICE_NAME_LEN 32
#define N_O_CHANNELS 2

typedef LinkedList HandleList;

#include <canlib.h>
#define CANLIB_MAJOR_VERSION 4
#define CANLIB_MINOR_VERSION 72
#define LAPCAN_TICKS_PER_MS 125

struct HandleData;
/* Hardware dependent functions that do the actual work with the card
 * The functions are given a HandleData struct */
//typedef struct HWOps
typedef struct CANOps
{
  /* Read a channel and flags e.g canWANT_EXCLUSIVE and return a file descriptor
   * to read messages from.
   */
  canStatus (*openChannel) (struct HandleData *);
  /* Read a callback function and flags that defines which events triggers it */
  canStatus (*setNotify) (struct HandleData*, /*void (*callback)(canNotifyData *)*/OS_IF_SET_NOTIFY_PARAM, unsigned int);
  canStatus (*busOn) (struct HandleData *);
  canStatus (*busOff) (struct HandleData *);

  canStatus (*setBusParams) (struct HandleData *, long, unsigned int, unsigned int,
                             unsigned int, unsigned int, unsigned int);
  canStatus (*getBusParams) (struct HandleData *, long *, unsigned int *, unsigned int *,
                             unsigned int *, unsigned int *, unsigned int *);
  canStatus (*read) (struct HandleData *, long *, void *, unsigned int *,
                     unsigned int *, unsigned long *);
  canStatus (*readWait) (struct HandleData *, long *, void *, unsigned int *,
                         unsigned int *, unsigned long *, long);
  canStatus (*setBusOutputControl) (struct HandleData *, unsigned int);
  canStatus (*getBusOutputControl) (struct HandleData *, unsigned int *);
  canStatus (*accept) (struct HandleData *, const long, const unsigned int);
  canStatus (*write) (struct HandleData *, long, void *, unsigned int, unsigned int);
  canStatus (*writeWait) (struct HandleData *, long, void *,
                          unsigned int, unsigned int, long);
  canStatus (*writeSync) (struct HandleData *, unsigned long);
  canStatus (*getNumberOfChannels) (struct HandleData *, int *);
  canStatus (*readTimer) (struct HandleData *, unsigned long *);
  canStatus (*readErrorCounters) (struct HandleData *, unsigned int *,
                                  unsigned int *, unsigned int *);
  canStatus (*readStatus) (struct HandleData *, unsigned long *);
  canStatus (*getChannelData) (int , int, void *, size_t);
  canStatus (*ioCtl) (struct HandleData* , unsigned int, void *, size_t);

//} HWOps;
} CANOps;

// This struct is associated with each handle
// returned by canOpenChannel
typedef struct HandleData
{
  OS_IF_FILE_HANDLE fd;
  char               deviceName[DEVICE_NAME_LEN];
  char               deviceOfficialName[150];
  int                channelNr; // Absolute ch nr i.e. it can be >2 for lapcan
  canHandle          handle;
  unsigned char      isExtended;
  unsigned char      wantExclusive;
  unsigned char      readIsBlock;
  unsigned char      writeIsBlock;
  long               readTimeout;
  long               writeTimeout;
  unsigned long      currentTime;
#if LINUX
  void               (*callback) (canNotifyData*);
#else
  HWND               notify_hwnd;
  HANDLE             notify_die;
  int                notified;
  OS_IF_EVENT        notify_ack;
  VCAN_EVENT         notify_msg;
#endif
  canNotifyData      notifyData;
  OS_IF_FILE_HANDLE  notifyFd;
#if LINUX
  void               *notifyThread;
#endif
  unsigned int       notifyFlags;
  CANOps             *canOps;
} HandleData;

//typedef struct CanBusTiming

#endif
