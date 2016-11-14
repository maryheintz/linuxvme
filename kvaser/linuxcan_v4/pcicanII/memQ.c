/*
** Copyright 2002-2006 KVASER AB, Sweden.  All rights reserved.
*/

//////////////////////////////////////////////////////////////////////////////////
// FILE: memQ.c                                /
//////////////////////////////////////////////////////////////////////////////////
// memQ  --  Structs and functions for manipulating memQ             /
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------
// NOTE! module_versioning HAVE to be included first
#include "module_versioning.h"
//--------------------------------------------------


#include <asm/io.h>

#include "osif_functions_kernel.h"
#include "helios_cmds.h"
#include "memq.h"

/////////////////////////////////////////////

#define MEM_Q_TX_BUFFER       1
#define MEM_Q_RX_BUFFER       2

/////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////

int MemQSanityCheck (PciCanIICardData *ci)
{
    unsigned long p;
    unsigned long irqFlags;

    os_if_spin_lock_irqsave(&ci->memQLock, &irqFlags);

    p = readl((void *)(ci->baseAddr + DPRAM_HOST_WRITE_PTR));
    if (p > 10000)
        goto error;

    p = readl((void *)(ci->baseAddr + DPRAM_HOST_READ_PTR));    
    if (p > 10000)
        goto error;

    p = readl((void *)(ci->baseAddr + DPRAM_M16C_WRITE_PTR));    
    if (p > 10000)
        goto error;

    p = readl((void *)(ci->baseAddr + DPRAM_M16C_READ_PTR));    
    if (p > 10000)
        goto error;

    os_if_spin_unlock_irqrestore(&ci->memQLock, irqFlags);

    return 1;

error:
    os_if_spin_unlock_irqrestore(&ci->memQLock, irqFlags);

    return 0;
}

/////////////////////////////////////////////////////////////////////////
//help fkt for Tx/Rx- FreeSpace(...)
/////////////////////////////////////////////////////////////////////////

static int AvailableSpace (unsigned int cmdLen, unsigned int rAddr,  
                           unsigned int wAddr,  unsigned int bufStart, 
                           unsigned int bufSize) 
{
    int remaining;
    //cmdLen in bytes

    // Note that "remaining" is not actually number of remaining bytes.
    // Also, the writer never wraps for a single message (it actually
    // guarantees that the write pointer will never be too close to
    // the top of the buffer, so the first case below will always be ok).
    if (wAddr >= rAddr) {
      remaining = bufSize - (wAddr - bufStart) - cmdLen;
    } else {
      remaining = (rAddr - wAddr - 1) - cmdLen;
    }

    // This supposedly never happens, but shout if it does.
    if (remaining < 0)
      printk("<1> Too little space remaining in queue: %d\n", remaining);

    return remaining >= 0;
}

////////////////////////////////////////////////////////////////////////////
//returns true if there is enough room for cmd in Tx-memQ, else false
////////////////////////////////////////////////////////////////////////////
static int TxAvailableSpace (PciCanIICardData *ci, unsigned int cmdLen) 
{
    unsigned long hwp, crp;
    int tmp;

    hwp = readl((void *)(ci->baseAddr + DPRAM_HOST_WRITE_PTR));  
    crp = readl((void *)(ci->baseAddr + DPRAM_M16C_READ_PTR));    
    tmp = AvailableSpace(cmdLen,
                         (unsigned long)(ci->baseAddr + crp),
                         (unsigned long)(ci->baseAddr + hwp),
                         (unsigned long)(ci->baseAddr + DPRAM_TX_BUF_START),
                         DPRAM_TX_BUF_SIZE);

    return tmp;
}


//////////////////////////////////////////////////////////////////////////

typedef struct _tmp_context {
    PciCanIICardData *ci;
    heliosCmd *cmd;
    int res;
} TMP_CONTEXT;


int QCmd (PciCanIICardData *ci, heliosCmd *cmd) 
{
    int           i;
    unsigned long p;
    unsigned long hwp, crp;
    unsigned long *tmp;
    unsigned long addr = ci->baseAddr;
    unsigned long irqFlags;

    os_if_spin_lock_irqsave(&ci->memQLock, &irqFlags);

    if (!TxAvailableSpace(ci, cmd->head.cmdLen)) {
        os_if_spin_unlock_irqrestore(&ci->memQLock, irqFlags);
        return MEM_Q_FULL;
    }

    hwp = readl((void *)(addr + DPRAM_HOST_WRITE_PTR));    
    crp = readl((void *)(addr + DPRAM_M16C_READ_PTR));    

    p = addr + hwp;
    tmp = (unsigned long*) cmd;

    for (i = 0; i < cmd->head.cmdLen; i += 4) {
        writel(*tmp++, (void *)p);
        p += 4;
    }

    p = hwp + cmd->head.cmdLen;
 
    if ((p + MAX_CMD_LEN) > (DPRAM_TX_BUF_START + DPRAM_TX_BUF_SIZE)) 
        p = DPRAM_TX_BUF_START;

    writel(p, (void *)(addr + DPRAM_HOST_WRITE_PTR));

    os_if_spin_unlock_irqrestore(&ci->memQLock, irqFlags);

    return MEM_Q_SUCCESS;
}


/////////////////////////////////////////////////////////////////////
int GetCmdFromQ (PciCanIICardData *ci, heliosCmd *cmdPtr) 
{
    unsigned long p;
    unsigned long hrp, cwp;
    unsigned long *tmp;
    int empty;
    unsigned long addr = ci->baseAddr;
    unsigned long irqFlags;

    os_if_spin_lock_irqsave(&ci->memQLock, &irqFlags);

    hrp = readl((void *)(addr + DPRAM_HOST_READ_PTR));    
    cwp = readl((void *)(addr + DPRAM_M16C_WRITE_PTR));    

    empty = (hrp == cwp);

    if (!empty) {
        int len;

        p = addr + hrp;
        tmp = (unsigned long *)cmdPtr;
        *tmp++ = readl((void *)p);
        len = cmdPtr->head.cmdLen - 4;
        p += 4;

        while (len > 0) {
            *tmp++ = readl((void *)p);
            len -= 4;
            p += 4;
        }

        p = hrp + cmdPtr->head.cmdLen;

        if ((p + MAX_CMD_LEN) > (DPRAM_RX_BUF_START + DPRAM_RX_BUF_SIZE)) 
            p = DPRAM_RX_BUF_START;

        writel(p, (void *)(addr + DPRAM_HOST_READ_PTR));

        if (empty) {
            printk("<1> GetCmdFromQ - // maybe in some way...qqq\n");
            //maybe in some way...qqq
        }

        os_if_spin_unlock_irqrestore(&ci->memQLock, irqFlags);

        return MEM_Q_SUCCESS;
    }
    else {
        //printk("<1> GetCmDFromQ - MEM_Q_EMPTY\n");
        os_if_spin_unlock_irqrestore(&ci->memQLock, irqFlags);

        return MEM_Q_EMPTY;
    }
}
