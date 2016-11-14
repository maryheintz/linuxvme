
/*
 * queue.c: queueing mechanism used in the buffer module
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

#include "queue.h"



/* Queue initialization */
int queue_init( queue_t *q,
		unsigned int queueSize,
		void *userData,
		void *kernelData,
		unsigned int order )
{
  q->head = 0;
  q->tail = 0;
  q->size = queueSize;
  q->userData = userData;
  q->kernelData = kernelData;
  q->order = order;

  // check that queuesize is power of 2
  if ( (queueSize & ~(queueSize - 1)) != queueSize )
    {
      q->size = 0;
      return( -1 );
    }
  else
    {
      return( 0 );
    }
}



unsigned int queue_getCurrentFree( queue_t *q )
{
  return( q->head );
}

unsigned int queue_getCurrentData( queue_t *q )
{
  return( q->tail );
}

