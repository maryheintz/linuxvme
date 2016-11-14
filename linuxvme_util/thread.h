/******************************************************************************
*
* Filename: 	thread.h
* 
* Description:	Header file for Linux VME Utility thread functions.
*
* $Revision: 1.2 $
*
* $Date: 2006/08/24 09:06:58 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme_util/thread.h,v $
*
* Copyright 2000 - 2005 Concurrent Technologies.
*
******************************************************************************/
#ifndef __INCthread
#define __INCthread

#define RD_DEVICE	1
#define WR_DEVICE	2
#define DMA_DIRECT	4
#define DMA_LIST	8

#define NUM_THREADS 4

typedef struct 
{
	int numThreads;
	int devHandle;
	UINT8 mode;
	UINT32 offset;
	UINT32 length;
	UINT32 mapping;
	UINT32 numCycles;
	UINT32 cycleRate;
	UINT32 userMemAddrs;
	UINT8 *bPtr;
	UINT32 checksum;
	UINT8 userData;
	UINT32 useTestPattern;
	int threadNum;
	union 
	{
		VME_TXFER_PARAMS list;
		VME_DIRECT_TXFER direct;
	} txfer;

#ifdef DEBUG
	UINT32 txferTime;
	UINT32 writeTime;
	UINT32 readTime;
	UINT32 averCount;
#endif

} TDATA; 


typedef struct 
{
	int devHandle;
	UINT32 selectedInts;
	UINT32 timeout;

} TINTDATA; 


/* Function Prototypes */
void startDeviceAccess( TDATA *tPtr );
void startWaitInt( TINTDATA *tPtr );

#endif	/* __INCthread */
