/*
** Copyright 2002-2006 KVASER AB, Sweden.  All rights reserved.
*/

//--------------------------------------------------
// NOTE! module_versioning HAVE to be included first
#include "module_versioning.h"
//--------------------------------------------------

//
// Kvaser CAN driver virtual hardware specific parts
// virtual functions
//

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/ptrace.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <asm/system.h>
#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <linux/pci.h>


// Kvaser definitions
#include "VCanOsIf.h"
#include "virtualcan.h"
#include "osif_kernel.h"
#include "osif_functions_kernel.h"
#include "queue.h"

#include "hwnames.h"

//
// If you do not define VIRTUAL_DEBUG at all, all the debug code will be
// left out.  If you compile with VIRTUAL_DEBUG=0, the debug code will
// be present but disabled -- but it can then be enabled for specific
// modules at load time with a 'debug_level=#' option to insmod.
// i.e. >insmod kvvirtual debug_level=#
//

#ifdef VIRTUAL_DEBUG
static int debug_level = VIRTUAL_DEBUG;
#   if !LINUX_2_6
        MODULE_PARM(debug_level, "i");
#   else
        MODULE_PARM_DESC(debug_level, "virtual debug level");
        module_param(debug_level, int, 0644);
#   endif
#   define DEBUGPRINT(n, args...) if (debug_level>=(n)) printk("<" #n ">" args)
#else
#   define DEBUGPRINT(n, args...)
#endif

//======================================================================
// HW function pointers
//======================================================================
#if LINUX_2_6
VCanHWInterface hwIf = {
    .initAllDevices     = virtualInitAllDevices,
    .setBusParams       = virtualSetBusParams,
    .getBusParams       = virtualGetBusParams,
    .setOutputMode      = virtualSetOutputMode,
    .setTranceiverMode  = virtualSetTranceiverMode,
    .busOn              = virtualBusOn,
    .busOff             = virtualBusOff,
    .txAvailable        = virtualTxAvailable,
    .transmitMessage    = virtualTransmitMessage,
    .procRead           = virtualProcRead,
    .closeAllDevices    = virtualCloseAllDevices,
    .getTime            = vCanTime,
    .flushSendBuffer    = vCanFlushSendBuffer,
    .getTxErr           = virtualGetTxErr,
    .getRxErr           = virtualGetRxErr,
    .rxQLen             = virtualRxQLen,
    .txQLen             = virtualTxQLen,
    .requestChipState   = virtualRequestChipState,
    .requestSend        = virtualRequestSend
};
#elif LINUX
VCanHWInterface hwIf = {
     initAllDevices:      virtualInitAllDevices,
     setBusParams:        virtualSetBusParams,
     getBusParams:        virtualGetBusParams,
     setOutputMode:       virtualSetOutputMode,
     setTranceiverMode:   virtualSetTranceiverMode,
     busOn:               virtualBusOn,
     busOff:              virtualBusOff,
     txAvailable:         virtualTxAvailable,
     transmitMessage:     virtualTransmitMessage,
     procRead:            virtualProcRead,
     closeAllDevices:     virtualCloseAllDevices,
     getTime:             vCanTime,
     flushSendBuffer:     vCanFlushSendBuffer,
     getTxErr:            virtualGetTxErr,
     getRxErr:            virtualGetRxErr,
     rxQLen:              virtualRxQLen,
     txQLen:              virtualTxQLen,
     requestChipState:    virtualRequestChipState,
     requestSend:         virtualRequestSend
};
#else // wince
VCanHWInterface hwIf = {
    /*initAllDevices:*/     hwIfInitDriver,
    /*setBusParams:  */     hwIfSetBusParams,
    /*getBusParams:  */     hwIfGetBusParams,
    /*setOutputMode: */     hwIfSetOutputMode,
    /*setTranceiverMode: */ hwIfSetTranceiverMode,
    /*busOn:           */   hwIfBusOn,
    /*busOff:          */   hwIfBusOff,
    /*txAvailable:      */  hwIfTxAvailable,
    /*transmitMessage:  */  hwIfPrepareAndTransmit,
    /*procRead:         */  hwIfProcRead,
    /*closeAllDevices:  */  hwIfCloseAllDevices,
    /*getTime:          */  hwIfTime,
    /*flushSendBuffer:  */  hwIfFlushSendBuffer,
    /*getTxErr:         */  hwIfGetTxErr,
    /*getRxErr:         */  hwIfGetRxErr,
    /*rxQLen:           */  hwIfHwRxQLen,
    /*txQLen:           */  hwIfHwTxQLen,
    /*requestChipState: */  hwIfRequestChipState,
    /*requestSend:      */  hwIfRequestSend
};
#endif


// prototype:
int virtualSend (void *void_chanData);



//======================================================================
// /proc read function
//======================================================================
int virtualProcRead (char *buf, char **start, off_t offset,
                    int count, int *eof, void *data)
{
    int len = 0;
    len += sprintf(buf+len,"\ntotal channels %d\n", NR_CHANNELS*NR_VIRTUAL_DEV);
    *eof = 1;
    return len;
}


//======================================================================
//  Can we send now?
//======================================================================
int virtualTxAvailable (VCanChanData *vChd)
{
    virtualChanData *hChd = vChd->hwChanData;
    return (atomic_read(&hChd->outstanding_tx) < VIRTUAL_MAX_OUTSTANDING);
} // virtualTxAvailable


//======================================================================
// Find out some info about the H/W
//======================================================================
int virtualProbe (VCanCardData *vCd)
{
    int i;

    vCd->nrChannels = NR_CHANNELS;
    DEBUGPRINT(1, "Kvaser virtual with %d channels found\n", vCd->nrChannels);

    for (i = 0; i < vCd->nrChannels; ++i) {
        VCanChanData *vChd = vCd->chanData[i];
        vChd->channel = i;
        vChd->transType  = VCAN_TRANSCEIVER_TYPE_251;
        memset(vChd->ean, 0, sizeof(vChd->ean));
        vChd->serialLow  = 0;
        vChd->serialHigh = 0;

        vChd->lineMode = VCAN_TRANSCEIVER_LINEMODE_NORMAL;
    }
    vCd->cardPresent = 1;

    return 0;
} // virtualProbe


//======================================================================
// Enable bus error interrupts, and reset the
// counters which keep track of the error rate
//======================================================================
static void virtualResetErrorCounter (VCanChanData *vChd)
{
    vChd->errorCount = 0;

    vChd->errorTime = hwIf.getTime(vChd->vCard);
} // virtualResetErrorCounter


//======================================================================
//  Set bit timing
//======================================================================
int virtualSetBusParams (VCanChanData *vChd, VCanBusParams *par)
{
    virtualChanData *virtualChan = (virtualChanData*)vChd->hwChanData;
    //save busparams
    virtualChan->busparams.freq = par->freq;
    virtualChan->busparams.sjw = par->sjw;
    virtualChan->busparams.tseg1 = par->tseg1;
    virtualChan->busparams.tseg2 = par->tseg2;
    virtualChan->busparams.samp3 = par->samp3;

    return 0;
} // virtualSetBusParams


//======================================================================
//  Get bit timing
//======================================================================
static int virtualGetBusParams (VCanChanData *vChd, VCanBusParams *par)
{
    virtualChanData *virtualChan = (virtualChanData*)vChd->hwChanData;
    //read saved busparams
    par->freq = virtualChan->busparams.freq;
    par->sjw = virtualChan->busparams.sjw;
    par->tseg1 = virtualChan->busparams.tseg1;
    par->tseg2 = virtualChan->busparams.tseg2;
    par->samp3 = virtualChan->busparams.samp3;

    return 0;
} // virtualGetBusParams


//======================================================================
//  Set silent or normal mode
//======================================================================
int virtualSetOutputMode (VCanChanData *vChd, int silent)
{
    virtualChanData *virtualChan = vChd->hwChanData;
    VCanCardData    *vCard       = vChd->vCard;
    int             i;

    if (virtualChan->silentmode && (silent == 1)) {
        virtualChan->silentmode = silent;

        //  Try sending all unsent messages on all channels except the specified channel
        for (i = 0; i < vCard->nrChannels; i++) {
            //se if other chans have stuff to send now that we are not alone anymore...
             if ((vCard->chanData[i] != NULL) && (vCard->chanData[i] != vChd) && (vCard->chanData[i]->isOnBus)) {
                //DEBUGPRINT(1, "virtualsend from bus on. %d\n", vChd->channel);
                virtualSend((void *)vCard->chanData[i]);
            }
        }
    }
    else {
      virtualChan->silentmode = silent;
    }

    return 0;
} // virtualSetOutputMode


//======================================================================
//  Line mode
//======================================================================
int virtualSetTranceiverMode (VCanChanData *vChd, int linemode, int resnet)
{
    // qqq what to do here?
    return 0;
} // virtualSetTranceiverMode


//======================================================================
//  Query chip status
//======================================================================
int virtualRequestChipState (VCanChanData *vChd)
{
    // qqq what here?
    return 0;
} // virtualRequestChipState


//======================================================================
//  Go bus on
//======================================================================
int virtualBusOn (VCanChanData *vChd)
{
    virtualChanData *virtualChan = vChd->hwChanData;
    VCanCardData *vCard = vChd->vCard;
    int i;

    vChd->isOnBus = 1;
    vChd->overrun = 0;
    atomic_set(&virtualChan->outstanding_tx, 0);
    virtualResetErrorCounter(vChd);
    vChd->chipState.state = CHIPSTAT_ERROR_ACTIVE;

    //  Try sending all unsent messages on all channels except the specified channel
    for (i = 0; i < vCard->nrChannels; i++) {
        //se if other chans have stuff to send now that we are on bus...
         if ((vCard->chanData[i] != NULL) && (vCard->chanData[i] != vChd) && (vCard->chanData[i]->isOnBus)) {
            DEBUGPRINT(1, "virtualsend from bus on. %d\n", vChd->channel);
            virtualSend((void *)vCard->chanData[i]);
        }
    }

    return 0;
} // virtualBusOn



//======================================================================
//  Go bus off
//======================================================================
int virtualBusOff (VCanChanData *vChd)
{
    vChd->isOnBus = 0;

    virtualRequestChipState(vChd);
    return 0;
} // virtualBusOff




//======================================================================
//  Read transmit error counter
//======================================================================
int virtualGetTxErr (VCanChanData *vChd)
{
    return 0; // qqq what do we do....
}


//======================================================================
//  Read transmit error counter
//======================================================================
int virtualGetRxErr (VCanChanData *vChd)
{
    return 0; // qqq what do we do....
}


//======================================================================
//  Read receive queue length in hardware/firmware
//======================================================================
unsigned long virtualRxQLen (VCanChanData *vChd)
{
    // qqq Why _tx_ channel buffer?
    return queue_length(&vChd->txChanQueue);
}


//======================================================================
//  Read transmit queue length in hardware/firmware
//======================================================================
unsigned long virtualTxQLen (VCanChanData *vChd)
{
    int qLen = 0;

    // return zero because we don't have any hw-buffers.
    return qLen;
}





//======================================================================printk("<1>Nothing for ch%d \n", chd->channel);
// Request send
//======================================================================
int virtualRequestSend (VCanCardData *vCard, VCanChanData *vChan)
{
    if (vChan->isOnBus) {
        int ret;
        if (!virtualTxAvailable(vChan))
            return -1;
        ret = virtualSend(vChan);
        return ret;
    }
    return -1;
}

//======================================================================
// send
//======================================================================
int virtualSend (void *void_chanData)
{
    VCanChanData *chd = (VCanChanData *)void_chanData;

    // Send Messages
    while (1) {
        if (!queue_empty(&chd->txChanQueue)) {
#if 0
            if (virtualTransmitMessage(chd, &(chd->txChanBuffer[queue_front(&chd->txChanQueue)]))) {
#else
            int queuePos = queue_front(&chd->txChanQueue);
            if (queuePos < 0) {   // Did we actually get anything from queue?
              queue_release(&chd->txChanQueue);
              continue;
            }
            if (virtualTransmitMessage(chd, &(chd->txChanBuffer[queuePos]))) {
#endif
                //DEBUGPRINT(1, "NS:%d \n", chd->channel);
                queue_release(&chd->txChanQueue);
                return -2;
            }
            queue_pop(&chd->txChanQueue);
        }
        else if (atomic_read(&chd->waitEmpty)) {
            atomic_set(&chd->waitEmpty, 0);
            os_if_wake_up_interruptible(&chd->flushQ);
            break;
        }
        else {
            queue_wakeup_on_space(&chd->txChanQueue);
            //DEBUGPRINT(1, "Nothing for ch%d \n", chd->channel);
            break;
        }
    }
    return 0;
}


//======================================================================
//  virtualTransmit
//======================================================================
static int virtualTransmitMessage (VCanChanData *vChd, CAN_MSG *m)
{
    int               i;
    VCanCardData      *vCard        = vChd->vCard;
    int               isDispatched  = 0;
    virtualChanData   *virtualChan = NULL;

    // qqq is it ok to just pass the ptr on?
    VCAN_EVENT *e = (VCAN_EVENT *)m;

    // fake reception
    e->tag = V_RECEIVE_MSG;
    e->timeStamp = vCanTime(vCard);
    for (i = 0; i < vCard->nrChannels; i++) {
        // distribute the msgs to all valid channels on a specific card (not ourselves though
        if ((vCard->chanData[i] != NULL) && (vCard->chanData[i] != vChd) && (vCard->chanData[i]->isOnBus)) {
            vCanDispatchEvent(vCard->chanData[i], e);
            virtualChan = vCard->chanData[i]->hwChanData;
            if (!virtualChan->silentmode) {
                isDispatched++;
            }
        }
    }

    virtualChan = vChd->hwChanData; // point virtualChan to sending chan
    if (!isDispatched)
        atomic_add(1, &virtualChan->outstanding_tx);
    else if (atomic_read(&virtualChan->outstanding_tx)) {
        atomic_sub(1, &virtualChan->outstanding_tx);
    }

    return isDispatched ? 0 : -1;
} // virtualTransmitMessage

//======================================================================
//  Initialize H/W specific data
//======================================================================
int virtualInitData (VCanCardData *vCard)
{
    int chNr;
    vCanInitData(vCard);
    for (chNr = 0; chNr < vCard->nrChannels; chNr++){
        virtualChanData *hChd;
        hChd = vCard->chanData[chNr]->hwChanData;
        hChd->silentmode = 0;
    }
    return 0;
}


//======================================================================
// Initialize the HW for one card
//======================================================================
int virtualInitOne (void)
{
    // Helper struct for allocation
    typedef struct {
        VCanChanData *dataPtrArray[MAX_CHANNELS];
        VCanChanData vChd[MAX_CHANNELS];
        virtualChanData hChd[MAX_CHANNELS];
    } ChanHelperStruct;

    ChanHelperStruct   *chs;
    int                chNr;
    VCanCardData       *vCard;

    // Allocate data area for this card
    vCard = os_if_kernel_malloc(sizeof(VCanCardData) + sizeof(virtualCardData));
    if (!vCard) goto card_alloc_err;
    memset(vCard, 0, sizeof(VCanCardData) + sizeof(virtualCardData));

    // hwCardData is directly after VCanCardData
    vCard->hwCardData = vCard + 1;

    // Allocate memory for n channels
    chs = os_if_kernel_malloc(sizeof(ChanHelperStruct));
    if (!chs) goto chan_alloc_err;
    memset(chs, 0, sizeof(ChanHelperStruct));

    // Init array and hwChanData
    for (chNr = 0; chNr < MAX_CHANNELS; chNr++){
        chs->dataPtrArray[chNr] = &chs->vChd[chNr];
        chs->vChd[chNr].hwChanData = &chs->hChd[chNr];
        chs->vChd[chNr].minorNr    = -1;   // No preset minor number
    }
    vCard->chanData = chs->dataPtrArray;

    // Find out type of card i.e. N/O channels etc
    if (virtualProbe(vCard)) {
        DEBUGPRINT(1, "virtualProbe failed");
        goto probe_err;
    }

    // Init channels
    virtualInitData(vCard);

    // Insert into list of cards
    os_if_spin_lock(&canCardsLock);
    vCard->next = canCards;
    canCards = vCard;
    os_if_spin_unlock(&canCardsLock);

    return 1;

chan_alloc_err:
probe_err:
    os_if_kernel_free(vCard);
card_alloc_err:
    return 0;
} // virtualInitOne


//======================================================================
// Find and initialize all cards
//======================================================================
int virtualInitAllDevices (void)
{
    int i;

    driverData.deviceName = DEVICE_NAME_STRING;
    for (i = 0; i < NR_VIRTUAL_DEV; i++) {
        virtualInitOne();
    }

    DEBUGPRINT(1, "Kvaser Virtual Initialized.\n");
    return 0;
} // virtualInitAllDevices


//======================================================================
// Shut down and free resources before unloading driver
//======================================================================
int virtualCloseAllDevices (void)
{
    // qqq check for open files
    VCanCardData *vCard;

    os_if_spin_lock(&canCardsLock);
    vCard = canCards;
    while (vCard) {
        vCard = canCards->next;
        os_if_kernel_free(canCards->chanData);
        os_if_kernel_free(canCards);
        canCards = vCard;
    }
    os_if_spin_unlock(&canCardsLock);
    DEBUGPRINT(1, "Kvaser Virtual Closed.\n");
    return 0;
} // virtualCloseAllDevices

