/******************************************************************************
*
* Filename: 	thread.c
* 
* Description:	Linux VME Utility thread functions.
*
* $Revision: 1.2 $
*
* $Date: 2006/08/24 09:06:58 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme_util/thread.c,v $
*
* Copyright 2000 - 2005 Concurrent Technologies.
*
******************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/mman.h>
#include <asm/page.h>
#include <semaphore.h>

#include "main.h"
#include "thread.h"

#ifdef DEBUG
#include "debug.h"
#endif

static sem_t start_sem;				/* thread startup semahore */
static pthread_mutex_t disp_mutex;	/* mutex to synchronise display updates */
									/* only one thread at a time can write to display */

static pthread_mutex_t dev_mutex;	/* mutex to synchronise device data transfers */

static ENTRYITEM waitIntsData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"00 VOWN     : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"01 VIRQ1    : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"02 VIRQ2    : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"03 VIRQ3    : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"04 VIRQ4    : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"05 VIRQ5    : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"06 VIRQ6    : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"07 VIRQ7    : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"08 DMA      : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"09 LERR     : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"10 VERR     : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"11 Reserved : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"12 SW_IACK  : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"13 SW_INT   : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"14 SYSFAIL  : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"15 ACFAIL   : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"16 MBOX0    : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"17 MBOX1    : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"18 MBOX2    : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"19 MBOX3    : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"20 LM0      : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"21 LM1      : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"22 LM2      : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"23 LM3      : ",'u',32, 0, 0, 0xffffffff, NULL},
	{"\0",0,0,0,0,0,NULL},
};


/******************************************************************************
*
* calcChecksum
*
* Calculate checksum of given buffer.
*
*
*  
*
* RETURNS: checksum value.
*
******************************************************************************/
static UINT32 calcChecksum( UINT8 *buf, int len )
{
	int i;
	UINT32 result = 0;


	for( i = 0; i < len; i++ )
	{	
		result += buf[i];
	}

	return result;
}


/******************************************************************************
*
* initBuffer
*
* Initialise content of given buffer.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void initBuffer( int threadNum, int pattern, UINT8 *buf, 
							int len, UINT8 data )
{
	int i = 0;


	while ( i < len )
	{
		if ( pattern != 0 )
		{
			buf[i] = i + threadNum;
		}
		else
		{
			buf[i] = data;
		}

		i++;
	}
}


/******************************************************************************
*
* updateBuffer
*
* Update content of given buffer.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void updateBuffer( UINT8 *buf, int len )
{
	int i = 0;


	while ( i < len )
	{
		buf[i] += 1;
		i++;
	}
	
}


/******************************************************************************
*
* threadCleanup
*
* Cleanup resources used by thread.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void threadCleanup( void *arg )
{
	TDATA *tPtr;


	tPtr = (TDATA *) arg;

	if ( tPtr != NULL )
	{
		if ( tPtr->bPtr != NULL )
		{
			free( tPtr->bPtr );
		}

		free( tPtr );
		tPtr = NULL;
	}

	refresh();
	pthread_mutex_unlock( &disp_mutex );	/* make sure display is unlocked */
}


/******************************************************************************
*
* readDeviceData
*
* Reads data from device.
*
*
*  
*
* RETURNS: Number of bytes read if successful else an error code.
*
******************************************************************************/
static int readDeviceData( TDATA *tPtr )
{
	int result = 0;
	UINT32 offsetIntoPage;
	UINT8 *rPtr;
	UINT32 checksum = 0;
#ifdef DEBUG
	UINT32 start;
	UINT32 stop;
#endif
	
	/* second half of buffer is used for reads */
	rPtr = tPtr->bPtr + tPtr->length;
	memset( rPtr, 0, tPtr->length );

#ifdef DEBUG
	start = clockGetMicroSec();
#endif

	/* if ioremapped */
	if ( tPtr->userMemAddrs == 0 )
	{
		result = vme_read( tPtr->devHandle, tPtr->offset, 
							rPtr, tPtr->length );

		if ( result <= 0 )
		{
			pthread_mutex_lock( &disp_mutex );
			mvprintw( STATUS_LINE, 10, "Error (%d) device read failed (%s)", 
							result, strerror(errno));
			clrtoeol();
			refresh();
			pthread_mutex_unlock( &disp_mutex );
		}
		else
		{
			if ( tPtr->checksum !=  0 )
			{
				checksum = calcChecksum( rPtr, tPtr->length );

				if ( checksum != tPtr->checksum )
				{
					pthread_mutex_lock( &disp_mutex );
					mvprintw( STATUS_LINE, 10, "Error checksum failed [0x%x 0x%x]", 
								checksum, tPtr->checksum);
					clrtoeol();
					refresh();
					pthread_mutex_unlock( &disp_mutex );

					result = -1;
				}
			}
		}
	}
	else /* device is memory mapped */
	{
		offsetIntoPage = tPtr->offset % PAGE_SIZE;
		memcpy( rPtr, (UINT8 *) (tPtr->userMemAddrs + offsetIntoPage),
					 tPtr->length );

		if ( tPtr->checksum != 0 )
		{
			checksum = calcChecksum( rPtr, tPtr->length );

			if ( checksum != tPtr->checksum )
			{
				pthread_mutex_lock( &disp_mutex );
				mvprintw( STATUS_LINE, 10, "Error checksum failed [0x%x 0x%x]", 
							checksum, tPtr->checksum);
				clrtoeol();
				refresh();
				pthread_mutex_unlock( &disp_mutex );

				result = -1;
			}
			else
			{
				result = tPtr->length;
			}
		}
		else
		{
			result = tPtr->length;
		}
		
	}

#ifdef DEBUG
	stop = clockGetMicroSec();
	tPtr->readTime += (stop - start);
#endif

	if ( result > 0 )
	{
		pthread_mutex_lock( &disp_mutex );

		if ( tPtr->numThreads > 1 )
		{
			if ( result <= 16 ) /* can only display 32 bytes */
			{
				printBuf( stdscr, (START_LINE + 1) + (tPtr->threadNum * 4), 
							tPtr->offset, rPtr, result );
			}
			else
			{
				printBuf( stdscr, (START_LINE + 1) + (tPtr->threadNum * 4), 
							tPtr->offset, rPtr, 32 );
			}
		}
		else
		{
			if ( result <= 256 ) /* can only display 256 bytes */
			{
				printBuf( stdscr, START_LINE + 2, tPtr->offset, rPtr, result );
			}
			else
			{
				printBuf( stdscr, START_LINE + 2, tPtr->offset, rPtr, 256 );
			}

			
		}

#if 0
		mvprintw( STATUS_LINE, 10, "Checksums 0x%x 0x%x", 
							checksum, tPtr->checksum);
		clrtoeol();
		refresh();
#endif

		pthread_mutex_unlock( &disp_mutex );	
	}

	return result;
}


/******************************************************************************
*
* writeDeviceData
*
* Writes data from device.
*
*
*  
*
* RETURNS: Number of bytes written if successful else an error code.
*
******************************************************************************/
static int writeDeviceData( TDATA *tPtr )
{
	int result;
	UINT32 offsetIntoPage;
#ifdef DEBUG
	UINT32 start;
	UINT32 stop;
#endif
	
	if ( tPtr->mode & WR_DEVICE )
	{
#ifdef DEBUG
		start = clockGetMicroSec();
#endif
		/* if ioremapped */
		if ( tPtr->userMemAddrs == 0 )
		{
			result = vme_write( tPtr->devHandle, tPtr->offset, 
								tPtr->bPtr, tPtr->length );

			if ( result < 0 )
			{
				pthread_mutex_lock( &disp_mutex );
				mvprintw( STATUS_LINE, 10,	"Error (%d) device write failed (%s)", 
							result, strerror(errno));
				clrtoeol();
				refresh();
				pthread_mutex_unlock( &disp_mutex );
			}
		}
		else /* device is memory mapped */
		{
			offsetIntoPage = tPtr->offset % PAGE_SIZE;

			memcpy( (UINT8 *) (tPtr->userMemAddrs + offsetIntoPage),
					 tPtr->bPtr, tPtr->length );
			result = tPtr->length;
		}

#ifdef DEBUG
		stop = clockGetMicroSec();
		tPtr->writeTime += (stop - start);
#endif

		if ( tPtr->useTestPattern == 1 )
		{
			updateBuffer( tPtr->bPtr, tPtr->length );

			if ( tPtr->checksum != 0 )
			{
				tPtr->checksum = calcChecksum( tPtr->bPtr, tPtr->length );
			}
		}
	}
	else
	{
		result = 0;
	}


	return result;
}


/******************************************************************************
*
* doDmaTransfer
*
* Initiates a DMA transfer if necessary.
*
*
*  
*
* RETURNS: 0 successful else an error code.
*
******************************************************************************/
static int doDmaTransfer( TDATA *tPtr )
{
	int result = 0;
#ifdef DEBUG
	UINT32 start;
	UINT32 stop;
#endif

	if ( tPtr->mode & DMA_DIRECT )
	{
#ifdef DEBUG
		start = clockGetMicroSec();
#endif

		if ( tPtr->mode & WR_DEVICE )
		{
			tPtr->txfer.direct.direction = UNI_DMA_WRITE;
					
			result = vme_dmaDirectTransfer( tPtr->devHandle, 
										(VME_DIRECT_TXFER *) &tPtr->txfer.direct );
		}

		if ( result == 0 )
		{
			tPtr->txfer.direct.direction = UNI_DMA_READ;
					
			result = vme_dmaDirectTransfer( tPtr->devHandle, 
										(VME_DIRECT_TXFER *) &tPtr->txfer.direct );

			if ( result < 0 )
			{
				pthread_mutex_lock( &disp_mutex );
				mvprintw( STATUS_LINE, 10, "Error (%d) DMA read failed (%s)", 
							result, strerror(errno));
				clrtoeol();
				refresh();
				pthread_mutex_unlock( &disp_mutex );
			}
		}
		else
		{
			pthread_mutex_lock( &disp_mutex );
			mvprintw( STATUS_LINE, 10, "Error (%d) DMA write failed (%s)", 
							result, strerror(errno));
			clrtoeol();
			refresh();
			pthread_mutex_unlock( &disp_mutex );
		}

#ifdef DEBUG
		stop = clockGetMicroSec();
		tPtr->txferTime += (stop - start);
#endif
	}

	else if ( (tPtr->mode & DMA_LIST) && (tPtr->threadNum == 0) )
	{

#ifdef DEBUG
		start = clockGetMicroSec();
#endif

		result = vme_dmaListTransfer( tPtr->devHandle, 
										(VME_TXFER_PARAMS *) &tPtr->txfer.list );

		if ( result < 0 )
		{
			pthread_mutex_lock( &disp_mutex );
			mvprintw( STATUS_LINE, 10, "Error (%d) DMA list transfer failed (%s)", 
							result, strerror(errno));
			clrtoeol();
			refresh();
			pthread_mutex_unlock( &disp_mutex );
		}

#ifdef DEBUG
		stop = clockGetMicroSec();
		tPtr->txferTime += (stop - start);
#endif

	}

	return result;
}


/******************************************************************************
*
* accessDevice
*
* Accesses the device.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void accessDevice( TDATA *tPtr )
{
	int result = 0;
	UINT32 count = 0;
	

	while ( (result >= 0) && 
			((count < tPtr->numCycles) || (tPtr->numCycles == 0)) )
	{
		pthread_mutex_lock( &dev_mutex );
		
		count++;

		result = writeDeviceData( tPtr );

		if ( result >= 0 )
		{
			result = doDmaTransfer( tPtr );
		}

		if ( result >= 0 )
		{
			result = readDeviceData( tPtr );
		}

		pthread_mutex_unlock( &dev_mutex );	

		pthread_mutex_lock( &disp_mutex );

		mvprintw( (START_LINE + (tPtr->threadNum * 4)), 2, "%02u Cycle count: %u", tPtr->threadNum, count );
		
		if ( tPtr->threadNum == 0 )
		{
			if ( tPtr->userMemAddrs == 0 )
			{
				mvprintw( START_LINE, 63, "I/O remapped" );
			}
			else
			{
				mvprintw( START_LINE, 48, "Memory mapped at 0x%X", tPtr->userMemAddrs );
			}
		}

#ifdef DEBUG
		if ( result >= 0 )
		{
			tPtr->averCount++;

			if ( (tPtr->averCount >= 100) && ( tPtr->numThreads == 1) &&
				( tPtr->numThreads == 1) )
			{
				mvprintw( STATUS_LINE, 6, "Timings:  Write: %04u us  DMA transfer: %04u us  Read: %04u us", 
						(tPtr->writeTime/tPtr->averCount), (tPtr->txferTime/tPtr->averCount), 
							(tPtr->readTime/tPtr->averCount) );

				tPtr->txferTime = 0;
				tPtr->writeTime = 0;
				tPtr->readTime = 0;
				tPtr->averCount = 0;
			}
		}
#endif

		if ( tPtr->threadNum == 0 )
		{
			refresh();
		}

		pthread_mutex_unlock( &disp_mutex );

		if ( result >= 0 )
		{
			usleep( (tPtr->cycleRate * 1000) );	/* cycle rate in milliseconds */
		}

		pthread_testcancel();	/* test for pending cancellation */
    }

	/* our work is done, wait for cancel */
	while(1)
	{
		sleep(1);
		pthread_testcancel();	/* test for pending cancellation */
	}
}


/******************************************************************************
*
* deviceAccessThread
*
* Device access thread function.
*
*
*  
*
* RETURNS: NULL.
*
******************************************************************************/
static void *deviceAccessThread( void *arg ) 
{
	TDATA *tPtr = NULL;

	
	pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
	pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, NULL );

	tPtr = (TDATA *) malloc( sizeof(TDATA) );
	if (tPtr != NULL)
	{
		pthread_cleanup_push( threadCleanup, tPtr );

		memcpy( tPtr, (TDATA *) arg, sizeof(TDATA) );
		sem_post( &start_sem );

#ifdef DEBUG
		tPtr->txferTime = 0;
		tPtr->writeTime = 0;
		tPtr->readTime = 0;
		tPtr->averCount = 0;
#endif
		
		/* allocate enough memory for both read and write data buffers */
		tPtr->bPtr = malloc( (tPtr->length * 2) );

		if ( tPtr->bPtr != NULL )
		{
			if ( tPtr->mode & WR_DEVICE )
			{
				initBuffer( tPtr->threadNum, tPtr->useTestPattern,  
								tPtr->bPtr, tPtr->length, tPtr->userData );

				tPtr->checksum = calcChecksum( tPtr->bPtr, tPtr->length );
			}
			else
			{
				tPtr->checksum = 0;
			}

			/* modify the parameters for this thread */
			tPtr->offset = tPtr->offset + (tPtr->length * tPtr->threadNum);
				
			if ( tPtr->mode & DMA_DIRECT )
			{
				tPtr->txfer.direct.offset = tPtr->offset;
				tPtr->txfer.direct.vmeAddress += (tPtr->offset * tPtr->threadNum);
			}

			/* start device access, doesn't return */
			accessDevice( tPtr );
		}
		else
		{
			pthread_mutex_lock( &disp_mutex );
			mvprintw( STATUS_LINE, 10, "Error - failed to alloc thread buffers");
			clrtoeol();
			refresh();
			pthread_mutex_unlock( &disp_mutex );
		}
		
		pthread_cleanup_pop( 1 );
	}
	else
	{
		pthread_mutex_lock( &disp_mutex );
		mvprintw( STATUS_LINE, 10, "Error - failed to alloc thread resource");
		clrtoeol();
		refresh();
		pthread_mutex_unlock( &disp_mutex );
		sem_post( &start_sem );
	}

	pthread_exit( 0 );			/* cleanup and never return */

	return NULL;				/* keep compiler happy */
}


#ifdef DEBUG
static void *timeThread( void *arg )
{

	pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
	pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, NULL );

	showElapsedTime( &disp_mutex );

	pthread_exit( 0 );			/* cleanup and never return */

	return NULL;				/* keep compiler happy */
}
#endif


static void *waitIntThread( void *arg )
{
	int i;
	int result;
	UINT32 intNum;
	UINT8 count;
	TINTDATA *tPtr;
#ifdef DEBUG
	UINT32 start;
	UINT32 stop;
#endif


	tPtr = (TINTDATA *) arg;

	pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
	pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, NULL );

	for ( i = 0; i < 24; i++ )
	{
		waitIntsData[i].value = 0;
	}

	intNum = 0;
	result = 0;
	
	mvprintw( START_LINE, 10, "Selected interrupts: 0x%08X", tPtr->selectedInts );

	while( result == 0 )
	{
#ifdef DEBUG
		start = clockGetMicroSec();
#endif
		drawItems( waitIntsData, -1, 1, 12 );

		result = vme_waitInterrupt( tPtr->devHandle, tPtr->selectedInts, 
										tPtr->timeout, &intNum );
		if ( result == 0 )
		{
			count = 0;
			while ( (count < 24) && (((intNum >> count) & 0x1) == 0) )
			{
				count++;
			}

			if ( count < 24 )
			{
				waitIntsData[count].value++;				
			}
			else
			{
				mvprintw( STATUS_LINE, 10, "Invalid interrupt number returned 0x%08X", intNum );
				result = -1;
			}
		}
		else
		{
			mvprintw(STATUS_LINE, 10, "Error (%d) wait interrupt failed (%s)",
							result, strerror(errno));
		}

		pthread_testcancel();	/* test for pending cancellation */

#ifdef DEBUG
		stop = clockGetMicroSec();
		mvprintw( (STATUS_LINE - 2), 10, "Timings: wait interrupt: %04u us", (stop - start) );
		clrtoeol();
#endif
	}

	refresh();

	/* our work is done, wait for cancel */
	while(1)
	{
		sleep(1);
		pthread_testcancel();	/* test for pending cancellation */
	}

	pthread_exit( 0 );			/* cleanup and never return */

	return NULL;				/* keep compiler happy */
}


/******************************************************************************
*
* startDeviceAccess
*
* Creates device access thread and waits for user to cancel operation.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
void startDeviceAccess( TDATA *tPtr )
{
	int result = 0;
	int count = 0;
	pthread_t a_thread[NUM_THREADS+1];
	void *thread_result;
	UINT32 pageOffset;
	UINT32 numOfPages;
	UINT32 totalSize;


	/* memory map device if selected*/
	if ( tPtr->mapping == 1 )
	{
		pageOffset = (tPtr->offset / PAGE_SIZE) * PAGE_SIZE;
		totalSize = (tPtr->offset % PAGE_SIZE) + tPtr->length;

		numOfPages = (totalSize / PAGE_SIZE);
		if ( (totalSize % PAGE_SIZE) != 0 )
		{
			numOfPages++;
		}

		result = vme_mmap( tPtr->devHandle, pageOffset, 
							(numOfPages * PAGE_SIZE), &tPtr->userMemAddrs );

		if ( result != 0 )
		{
			mvprintw(STATUS_LINE, 10, "Error (%d) memory mapping failed (%s)", 
							result, strerror(errno));
			clrtoeol();
			getReturn( 10 );
		}
	}

	if ( result == 0 )
	{
		result = sem_init( &start_sem, 0, 0 );

		if ( result == 0 )
		{
			result = pthread_mutex_init( &disp_mutex, NULL );

			if ( result == 0 )
			{
				result = pthread_mutex_init( &dev_mutex, NULL );
			}
		}

		while( ( result == 0 ) && (count < tPtr->numThreads) && (count < NUM_THREADS) )
		{
			tPtr->threadNum = count;
			result = pthread_create( &a_thread[count], NULL, deviceAccessThread, (void *) tPtr );
    		if ( result != 0 ) 
			{
				pthread_mutex_lock( &disp_mutex );
				mvprintw( STATUS_LINE, 10, "Error - thread creation failed" );
				clrtoeol();
				refresh();
				getReturn( 10 );
				pthread_mutex_unlock( &disp_mutex );
    		}
			else
			{
				sem_wait( &start_sem );
				count++;				
			}
		}

#ifdef DEBUG
		result = pthread_create( &a_thread[count], NULL, timeThread, NULL );
		count++;
#endif

		if ( result == 0 ) 
		{
			getReturn( 10 );	
		}

		while( count > 0 )
		{
			count--;

			mvprintw( STATUS_LINE, 0, " ");
			clrtoeol();
			mvprintw( STATUS_LINE, 10, "Waiting for thread %d to terminate...", count );
			refresh();

			result = pthread_cancel( a_thread[count] );
			
			if ( result == 0 ) 
			{
				result = pthread_join(a_thread[count], &thread_result);
			}

			if ( result != 0 )
			{
				mvprintw( STATUS_LINE, 10, "Error - terminating thread %d", count);
				clrtoeol();
				getReturn( 10 );
			}
		}

		sem_destroy( &start_sem );
		pthread_mutex_destroy( &disp_mutex );
		pthread_mutex_destroy( &dev_mutex );
	}

	if ( tPtr->userMemAddrs != 0 )
	{
		result = vme_unmap( tPtr->devHandle, tPtr->userMemAddrs,
							(numOfPages * PAGE_SIZE) );
		if ( result != 0 )
		{
			mvprintw(STATUS_LINE, 10, "Error (%d) unmap failed (%s)", 
						result, strerror(errno));
			clrtoeol();
			getReturn( 10 );
		}
	}
}


void startWaitInt( TINTDATA *tPtr )
{
	int result;
	pthread_t a_thread;
	void *thread_result;


	result = pthread_create( &a_thread, NULL, waitIntThread, (void *) tPtr );

	if ( result == 0 ) 
	{
		getReturn( 10 );

		mvprintw( STATUS_LINE, 0, " ");
		clrtoeol();
		mvprintw( STATUS_LINE, 10, "Waiting for thread to terminate..." );
		refresh();

		result = pthread_cancel( a_thread );
			
		if ( result == 0 ) 
		{
			result = pthread_join(a_thread, &thread_result);
		}

		if ( result != 0 )
		{
			mvprintw( STATUS_LINE, 10, "Error - terminating thread");
			clrtoeol();
			getReturn( 10 );
		}	
	}
}
