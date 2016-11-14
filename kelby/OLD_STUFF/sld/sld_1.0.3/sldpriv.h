#ifndef _SLDPRIV_H_
#define _SLDPRIV_H_

/*
 * sldpriv.h: internal global definitions
 * Copyright (C) 2000 by J.E. van Grootheest
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#include <linux/pci.h>
#include <linux/tqueue.h>
#include <linux/timer.h>
#include <linux/time.h>

#include "sld.h"
#include "queue.h"
#include "amcc.h"


#ifndef FALSE
#define FALSE (0)
#define TRUE (!FALSE)
#endif


/*
 * Macro:	SLD_MAX_DEVICES
 *
 * Description:	This defines how many devices we support in one system.
 *		It is currently 4, but that's just an arbitrary number.
 */
#define SLD_MAX_DEVICES		4



/*
 * Macro:	MAX_DATAMEMORY_REGIONS
 *
 * Description:	BUF needs to maintain its databuffer memory regions. This
 *		number defines how many it can handle. Increase if you
 *		have problems.
 */
#define MAX_DATAMEMORY_REGIONS	50



/*
 * Struct:	memblock_t
 *
 * Description:	Structure used by BUF to maintain memory regions in use
 *		as databuffers. The kernel and user space pointers are here
 *		and the order of pages there. The last item is needed again
 *		when the pages are freed.
 */
typedef struct
{
  void *userPtr;
  void *kernelPtr;
  unsigned int order;
} memblock_t;



/*
 * Struct:	bufprivdata_t
 *
 * Description:	Internal data to BUF.
 *
 * Fields:	queue_t dataQueue
 *		queue_t infoQueue
 *			queues with, respectively, databuffers and infoblocks
 *
 *		int dataQueueFull
 *			Counter for the times when the data queue is full.
 *			It is incremented when a databuffer is requested,
 *			but not available.
 *
 *		int doStats
 *			flag from the DDF timer to us that the statistics must
 *			be put in the queue.
 *
 *		memblock_t dataMem
 *			the memory regions used for databuffers are here.
 *
 *		unsigned int mmapPages
 *			the number of pages that need to be mmaped; the user
 *			needs the size of the mmaped region and this is used
 *			when calculating that.
 *
 *		void *kUserPtr
 *			pointer to the user databuffer queue
 *
 * Remark:	The layout has been optimized for 16 byte cache-lines;
 *		this *does* make a small difference in execution speed of
 *		the driver.
 *		The dummy int is added to make this struct a multiple of
 *		16 bytes
 */
typedef struct
{
  /* often used part; try to keep things 16-byte aligned */
  queue_t dataQueue;				/* 0; 24 bytes */
  int dataQueueFull;				/* 24; 4 bytes */
  int doStats;					/* 28; 4 bytes */
  queue_t infoQueue;				/* 32; 24 bytes */

  /* less often used part */
  memblock_t dataMem[ MAX_DATAMEMORY_REGIONS ];	/* 56; 12 * 50 = 600 */
  unsigned int mmapPages;			/* 656; 4 bytes */

  int dummy[ 3 ];				/* 660; 8 bytes */

  /* 672 = 42 * 16 */

} bufprivdata_t;



/*
 * Struct:	ddfprivdata_t
 *
 * Description:	The DDF private data.
 *
 * Fields:	int opened
 *			counter how often the device was opened
 *
 *		struct wait_queue *waitQueue
 *			queue used when processes need to wait for data to
 *			become available; used in the poll operation
 *
 *		struct tq_struct task
 *			the task that's queued in the immediate queue
 *
 *		unsigned int statTime
 *			the time between putting statistics in the infoblock
 *			queue, this is in 100millisec units
 *
 *		struct timer_list statTimer
 *			like the task, this is queued in the timer queue
 */
typedef struct
{
  int opened;

  spinlock_t lock;

  struct wait_queue *waitQueue;	/* used in the poll operation */
  struct tq_struct bhTask;	/* bottom-half task */
  unsigned int statTime;	/* statistics time, unit is 100msec */
  struct timer_list statTimer;	/* statistics timer */
  struct tq_struct timerTask;	/* timer task */

  void *deviceMemAddress;

} ddfprivdata_t;




typedef struct
{
  /* pointer to access the AMCC chip */
  amcc_operation_registers_t volatile *amccRegs;

  /* local copies of amcc registers; used to cache their values */
  int imb4;
  int intcsr;

  /*
   * dmaBusy indicates if we're waiting for DMA to finish,
   * but implicitly also indicates if we're handling datawords or
   * controlwords currently
   */
  int dmaBusy;
  void *dmaPtr;

  /* task things */
  int waitingForBuffer;
  int waitingForControlword;
  int waitingForResetEnd;

  int started;
  int resetTime;

} hwdprivdata_t;



/*
 * Struct:	sldpriv_t
 *
 * Description:	All internal data relevant for one interface is in this
 *		structure; it is passed around everywhere.
 *		Obviously a lot of data here only makes sense in the kernel!
 *
 * Fields:	int sig1
 *		int sig2
 *			fields used to make sure that the right pointer was
 *			passed to the functions used in the user application
 *
 *		struct pci_dev *device
 *			pointer to the linux structure describing the device
 *			we manage
 *
 *		ddfprivdata_t ddf
 *		bufprivdata_t buf
 *		hwdprivdata_t buf
 *			DDF, HWD and BUF private data
 *
 * Remark:	If this were a general purpose device, the buf field would
 *		have been a pointer to the struct. However, this is not a
 *		general purpose device.
 *		So, to ease debugging, the whole sldpriv_t is exposed into
 *		user space.
 *		It would only be trivial to change this, of course.
 *
 * Remark:	The DDF private data can vary in size between user and kernel
 *		space, so it is put at the end. Anyway the DDF fields are not
 *		supposed to be used in user space.
 *		Items that may vary are the spinlock and other kernel specific
 *		types.
 */
typedef struct
{
  sldstat_t	stat;		/* statistics */
  bufprivdata_t	buf;		/* BUF local data */
  hwdprivdata_t hwd;		/* HWD local data */

  int sig1;

  struct pci_dev *device;	/* information about our device */

  int sig2;

  ddfprivdata_t ddf;		/* DDF local data */

} sldpriv_t;

/*
 * Macro:	SLD_SIGNATURE
 * Macro:	SLD_VERIFY_SIG
 *
 * Description:	The first macro defines the value that is used in the sig1 and
 *		sig2 fields. The second one is an expression that checks that
 *		both fields contain the correct data.
 */
#define SLD_SIGNATURE		((int) 0x00FACE00)
#define SLD_VERIFY_SIG ( sldData->sig1 == SLD_SIGNATURE && sldData->sig2 == SLD_SIGNATURE )


/*
 * Struct:	bufdatabuf_t
 *
 * Description:	There are two addresses where databuffers are: one
 *		in kernel space and one in the process space of the
 *		process using the driver.
 *	       	This structure holds both pointers for one buffer.
 */
typedef struct
{
  void *kernel;
  void *user;
} bufdatabuf_t;


/*
 * Variable:	unsigned int sizeOfBuffers
 *
 * Description:	The configured size of databuffers. This is passed as a
 *		configuration item at module load time.
 *		Although not really necessary, there is a sanity check
 *		that this number is 128 or bigger. A practical maximum is
 *		probably PAGE_SIZE * 8 (that's 32KB on i386), the theoretical
 *		absolute maximum is PAGE_SIZE * 2^^5.
 */
extern unsigned int sld_bufferSize;

/*
 * Variable:	unsigned int numberOfBuffers
 *
 * Description:	The configured number of databuffers.
 *		As above, there's a check that this is 10 or bigger. The
 *		maximum is defined by factors such as kernel size, available
 *		memory and such. If too much is reserved for buffers, linux
 *		WILL start swapping heavily, affecting speed, obviously.
 */
extern unsigned int sld_nrOfBuffers;



/*
 * Macro:	SLD_PRINT
 * Macro:	SLD_DEBUG
 *
 * Description:	Macros used for output. This output ends up in the logfiles
 *		via klogd and syslogd.
 *		The debuglevel in SLD_DEBUG works like this: each file has
 *		a static int debuglevel which is set to a value. Each
 *		SLD_DEBUG that is lower or equal to the debuglevel is printed.
 *		If the variable is also const, then the compiler can and will
 *		optimize the if statement away if the parameter x is also
 *		constant.
 *		The format can be used like any other printf call.
 */

#define SLD_PRINT( format... ) printk( KERN_CRIT "S-Link: " format )

#define SLD_DEBUG( x, format... )				\
do								\
{								\
  if ( (x) <= debuglevel )					\
    {								\
      printk( KERN_CRIT "S-Link: " __FUNCTION__ ": " format );	\
    }								\
}								\
while( 0 )



#define PCI_TRIGGER( x ) sldData->hwd.amccRegs->omb[ 1 ].dword = ((x) << 16) + (__LINE__)


#endif /* _SLDPRIV_H_ */

