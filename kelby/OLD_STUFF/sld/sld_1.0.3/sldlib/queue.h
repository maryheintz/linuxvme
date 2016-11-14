#ifndef _QUEUE_H_
#define _QUEUE_H_

/*
 * queue.h: definitions for queues
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


/*
 * Struct:	queue
 *
 * Description:	Hold all information about a queue.
 *
 *		The queue is organized as an array and the head and
 *		tail are the indexes to the head and tail of the queue.
 *		The number of items in the queue has to be a power of two,
 *		so the wrapping of the indexes can be done with a simple
 *		mask operation.
 *
 *		The two pointers are the pointers to the (one!) array of data.
 *		There are two pointers, since the kernel interface needs
 *		to use kernel addresses, while the user interface needs to
 *		use user space addresses (which are only valid for in the
 *		user process!). So these are simply two different addresses
 *		for the same data.
 *
 *		All operations on the queue return indexes into the array.
 */
typedef struct
{
  unsigned int head, tail;	/* to keep track of queue status */
  unsigned int size;		/* multiple of 2, to be used for masking bits */
  void *kernelData;		/* where the real queue data is in kernel space */
  void *userData;		/* where the real queue data is in user space */
  unsigned int order;		/* order of allocated pages of memory */
} queue_t;



/*
 * Macro:	QUEUE_GETNEXTFREE
 * Macro:	QUEUE_GETNEXTDATA
 * Macro:	QUEUE_ALLOCNEXTFREE
 * Macro:	QUEUE_ALLOCNEXTDATA
 *
 * Description:	These macros can be used to retrieve the next free/data
 *		item. In case of the ALLOC versions, the item is also
 *		allocated.
 *		The _K suffix ones give kernel addresses.
 *
 *		With these you can use the queue like this (if you make sure
 *		beforehand that there is a free item):
 *			foo_t *fooPtr;
 *			fooPtr = &QUEUE_GETNEXTFREE( foo_t, &fooQueue );
 *			fillFooItem( fooPtr );
 *			queue_allocNextFree( &fooQueue );
 *
 *		Or, if you're sure that fillFooItem doesn't do weird things,
 *		this code is a bit faster and does the same:
 *			foo_t *fooPtr;
 *			fooPtr = &QUEUE_ALLOCNEXTFREE( foo_t, &fooQueue );
 *			fillFooItem( fooPtr );
 */
#define QUEUE_GETNEXTFREE( dataType, queuePtr ) \
(((dataType *)(queuePtr)->userData)[ queue_getNextFree( queuePtr ) ])

#define QUEUE_GETNEXTDATA( dataType, queuePtr ) \
(((dataType *)(queuePtr)->userData)[ queue_getNextData( queuePtr ) ])

#define QUEUE_ALLOCNEXTFREE( dataType, queuePtr ) \
(((dataType *)(queuePtr)->userData)[ queue_allocNextFree( queuePtr ) ])

#define QUEUE_ALLOCNEXTDATA( dataType, queuePtr ) \
(((dataType *)(queuePtr)->userData)[ queue_allocNextData( queuePtr ) ])



#ifdef __KERNEL__

#define QUEUE_GETNEXTFREE_K( dataType, queuePtr ) \
(((dataType *)(queuePtr)->kernelData)[ queue_getNextFree( queuePtr ) ])

#define QUEUE_GETNEXTDATA_K( dataType, queuePtr ) \
(((dataType *)(queuePtr)->kernelData)[ queue_getNextData( queuePtr ) ])

#define QUEUE_ALLOCNEXTFREE_K( dataType, queuePtr ) \
(((dataType *)(queuePtr)->kernelData)[ queue_allocNextFree( queuePtr ) ])

#define QUEUE_ALLOCNEXTDATA_K( dataType, queuePtr ) \
(((dataType *)(queuePtr)->kernelData)[ queue_allocNextData( queuePtr ) ])

#endif



/*
 * Function:	queue_init
 *
 * Description:	Initialize the queue structure.
 *		This also checks that the size is a power of 2. If it is
 *		not, the queue will not work.
 *		On success, 0 is returned, otherwise -1.
 */
extern int queue_init( queue_t *q,
		       unsigned int queueSize,
		       void *userData,
		       void *kernelData,
		       unsigned int order );



/*
 * Function:	queue_nextItem
 *
 * Description:	calculate the next item of the circular queue
 */
extern inline unsigned int queue_nextItem( unsigned int currentItem,
					   unsigned int queueSize )
{
  return( (currentItem + 1) & (queueSize - 1) );
}



/*
 * Function:	queue_isEmpty
 * Function:	queue_isFull
 *
 * Description:	These two functions indicate that the queue is empty or
 *		full, respectively.
 */
extern inline int queue_isEmpty( queue_t *q )
{
  return( q->head == q->tail );
}

extern inline int queue_isFull( queue_t *q )
{
  return( queue_nextItem( q->head, q->size ) == q->tail );
}



/*
 * Function:	queue_getNextFree
 * Function:	queue_allocNextFree
 *
 * Description:	These two functions work on the head of the queue. Both will
 *		return the index of the next free item of the queue, or -1
 *		if the queue is full. If isFull returned false, both functions
 *		will always succeed.
 *		The difference between the two is that allocNextFree will also
 *		advance the head of the queue, thereby indicating that the
 *		previous head item contains data.
 */
extern inline unsigned int queue_getNextFree( queue_t *q )
{
  unsigned int nextItem;

  if ( queue_isFull( q ) )
    {
      return( -1 );
    }

  nextItem = queue_nextItem( q->head, q->size );
  return( nextItem );
}

extern inline unsigned int queue_allocNextFree( queue_t *q )
{
  unsigned int nextItem;

  if ( queue_isFull( q ) )
    {
      return( -1 );
    }

  nextItem = queue_nextItem( q->head, q->size );
  q->head = nextItem;
  return( nextItem );
}



/*
 * Function:	queue_getNextData
 * Function:	queue_freeNextData
 *
 * Description:	These two functions match the two above, but work on the tail
 *		of the queue. Both return the index of a queue item with data
 *		in it, or -1 if the queue is empty.
 */
extern inline unsigned int queue_getNextData( queue_t *q )
{
  unsigned int nextItem;

  if ( queue_isEmpty( q ) )
    {
      return( -1 );
    }

  nextItem = queue_nextItem( q->tail, q->size );
  return( nextItem );
}

extern inline unsigned int queue_freeNextData( queue_t *q )
{
  unsigned int nextItem;

  if ( queue_isEmpty( q ) )
    {
      return( -1 );
    }

  nextItem = queue_nextItem( q->tail, q->size );
  q->tail = nextItem;
  return( nextItem );
}



/*
 * Function:	queue_getCurrentFree
 * Function:	queue_getCurrentData
 *
 * Description:	These functions return simply the current head and tail. These
 *		are to be used with caution, since if the queue is full,
 *		currentFree will still return a free item. The same for
 *		currentData when the queue is empty...
 *		On the other hand, they cannot fail...
 *
 *		Oh, and they're also not inline.
 */
extern unsigned int queue_getCurrentFree( queue_t *q );
extern unsigned int queue_getCurrentData( queue_t *q );



#endif
