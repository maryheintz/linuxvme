#ifndef _BUFINTERN_H_
#define _BUFINTERN_H_


/*
 * bufintern.h: internal definitions for the buffer module
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


#include <linux/mm.h>
#include "sld.h"
#include "sldpriv.h"



#ifdef __KERNEL__

/* Opening the buffer can only be done from kernel space. */

/*
 * Function:	buf_open
 *
 * Description:	This function initializes the buf module. It allocates the
 *		databuffers of the right size and prepares the queues
 *		with infoblock_t's and databuffers.
 *		After this function, the kernel side of things is fully
 *		operational.
 *
 * Parameters:	sldpriv_t *sldData
 *			pointer to the driver's private data, where buf also
 *			needs to store its things.
 *
 * Returns:	int
 * 			0 if successful, some errno number otherwise.
 */
int buf_open( sldpriv_t *sldData );



/*
 * Function:	buf_close
 *
 * Description:	Stop and clean the buffers. After this it'd be better not to
 *		call any other buffer functions, as they are not properly
 *		initialized.
 */
int buf_close( sldpriv_t *sldData );



/*
 * Function:	buf_mmap
 *
 * Description:	Map all necessary memory pages into the users address space.
 *		This also requires calculating the pointers to the databuffers
 *		in the users memory space.
 *
 * Parameters:	vm_area_struct *vmap
 *			the address space where the pages are to be mapped;
 *			the first page is taken by the private data, so
 *			that must be skipped.
 *
 * Returns:	int errno
 */
int buf_mmap( sldpriv_t *sldData, struct vm_area_struct *vmap );



/*
 * Function:	getDataBuffer
 *
 * Description:	This function will return a free dataBuffer to the caller.
 *		If there is no databuffer available, an infoblock with an error
 *		will be queued automatically.
 *
 * Returns:	A pointer to the databuffer, or NULL if there is no buffer
 *		available (ugly situation).
 */
void *getDataBuffer( sldpriv_t *sldData );



/*
 * Function:	putInfo
 *
 * Description:	Allocate an infoblock from the queue and copy the passed
 *		information in it. If it is a type of information that uses a
 *		databuffer, then also the databuffer is marked as in use.
 *		In the case that the infoblock_t queue is full, a flag is
 *		used to remember this for later and when an infoblock_t is
 *		released by the user, it is used to report this error.
 *
 * Parameters:	void * privData
 *			the regular pointer of private data, see buf_open
 *
 *		infoblock_t infoBlock
 *			the information to be queued to the user
 *
 * Returns:	int
 *			The number of infoblocks that were put in the queue.
 *			This is not necessarily 1, since it can happen that
 *			statistics were queued too.
 *
 * Remark:	This function may only be executed in an interrupts-off
 *		environment! It makes assumptions about that!
 */
int putInfo( sldpriv_t *sldData, infoblock_t *infoBlock );

#endif /* __KERNEL__ */


#endif
