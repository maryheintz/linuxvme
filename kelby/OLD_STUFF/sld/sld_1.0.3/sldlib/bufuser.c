
/*
 * bufuser.c: user-space buffer management
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

#ifdef __KERNEL__
/*
 * This makes the code usable in kernel and user space,
 * but obviously you need to recompile.
 */
#include <linux/config.h>
#if defined(CONFIG_MODVERSIONS) && !defined(MODVERSIONS)
#define MODVERSIONS
#endif
#if defined(MODVERSIONS)
#include <linux/modversions.h>
#endif

#else /* __KERNEL__ */

/*
 * Provide the printf prototype so we prevent a warning on that.
 * Unfortunately, it's impossible to include stdio since it clashes
 * badly with kernel includes on some systems (RedHat).
 * Kernel includes come in via sldpriv.h.
 */
extern int printf( const char *format, ... );
#define printk printf
#define NULL (0L)
#define KERN_CRIT
#define KERN_DEBUG

#endif /* __KERNEL__ */

#include "sldpriv.h"



/* SLD_DEBUG debugging level */
static const int debuglevel = 0;


infoblock_t *getFullInfoBlock( void *privData )
{
  sldpriv_t *sldData = privData;
  infoblock_t *nextInfo;
  bufdatabuf_t *nextBuffer;

  /* check that we actually did get the private data */
  if ( !SLD_VERIFY_SIG )
    {
      printk( "BUF (" __FUNCTION__ "): passed incorrect pointer to private data!.\n" );
      return( NULL );
    }

  if ( queue_isEmpty( &sldData->buf.infoQueue ) )
    {
      return( NULL );
    }

  /* data is now available */
  nextInfo = &QUEUE_GETNEXTDATA( infoblock_t, &sldData->buf.infoQueue );

  /* translate pointer to buffer from kernel to userspace */
  if ( BUF_TYPE_WITH_BUFFER( nextInfo->type ) )
    {
      SLD_DEBUG( 4, "processing databuffer\n" );

      /* find the next databuffer */
      nextBuffer = &QUEUE_GETNEXTDATA( bufdatabuf_t, &sldData->buf.dataQueue );
      while ( nextBuffer->kernel == NULL )
	{
	  queue_freeNextData( &sldData->buf.dataQueue );
	  nextBuffer = &QUEUE_GETNEXTDATA( bufdatabuf_t, &sldData->buf.dataQueue );
	}

      /* and check that it is the right one. */
      if ( nextBuffer->kernel != nextInfo->u.buffer.ptr )
	{
	  /* something went terribly wrong! no possibility to continue */
	  SLD_PRINT( "infoblock: %p, type=%s\n", nextInfo, getTypeStr( nextInfo->type ) );
	  SLD_PRINT( "databuffer is %p, expected is %p\n",
		     nextInfo->u.buffer.ptr, nextBuffer->kernel );
	  SLD_PRINT( "queue: head=%d, tail=%d, size=%d\n",
		     sldData->buf.dataQueue.head,
		     sldData->buf.dataQueue.tail,
		     sldData->buf.dataQueue.size );
	  SLD_PRINT( "queue: kernel=%p, user=%p\n",
		     sldData->buf.dataQueue.kernelData,
		     sldData->buf.dataQueue.userData );
#ifdef __KERNEL__
	  panic( "S-Link databuffers not consistent!" );
#else
	  SLD_PRINT( "S-Link databuffers not consistent!" );
	  exit( 1 );
#endif
	}

      /* convert the datapointer to userspace */
      nextInfo->u.buffer.ptr = nextBuffer->user;
    }

  SLD_DEBUG( 1, "returning infoblock at %p, type is %s\n",
	     nextInfo, getTypeStr( nextInfo->type ) );
  return( nextInfo );
}



void freeInfoBlock( void *privData, infoblock_t *infoBlock )
{
  sldpriv_t *sldData = privData;
  infoblock_t *nextInfo;

  SLD_DEBUG( 2, "got infoblock at %p, type is %s\n",
	     infoBlock, getTypeStr( infoBlock->type ) );

  /* check that we actually did get the private data */
  if ( !SLD_VERIFY_SIG )
    {
      printk( "BUF (" __FUNCTION__ "): passed incorrect pointer to private data!\n" );
      return;
    }

  /* check that the right infoblock_t was passed */
  nextInfo = &QUEUE_GETNEXTDATA( infoblock_t, &sldData->buf.infoQueue );
  if ( nextInfo != infoBlock )
    {
      printk( "BUF (" __FUNCTION__ "): to-be-released infoBlock is not the same as the current data infoblock.\n" );
      return;
    }

  /* check if we need to release a databuffer as well */
  if ( BUF_TYPE_WITH_BUFFER( infoBlock->type ) )
    {
      /* there's a databuffer in there! */
      bufdatabuf_t *nextBuffer;

      nextBuffer = &QUEUE_GETNEXTDATA( bufdatabuf_t, &sldData->buf.dataQueue );
      SLD_DEBUG( 4, "next databuffer in the queue is %p (user %p)\n",
		 nextBuffer->kernel, nextBuffer->user );
      while ( nextBuffer->kernel == NULL )
	{
	  queue_freeNextData( &sldData->buf.dataQueue );
	  nextBuffer = &QUEUE_GETNEXTDATA( bufdatabuf_t, &sldData->buf.dataQueue );
	}
      SLD_DEBUG( 3, "next databuffer in the queue is %p (user %p)\n",
		 nextBuffer->kernel, nextBuffer->user );

      /* check that the databuffer is in the infoblock */
      if ( nextBuffer->user != infoBlock->u.buffer.ptr )
	{
	  printk( "BUF (" __FUNCTION__ "): to-be-released databuffer is not the same as the current data in the infoblock.\n" );
	  return;
	}

      queue_freeNextData( &sldData->buf.dataQueue );
    }

  /* release the infoblock_t too */
  queue_freeNextData( &sldData->buf.infoQueue );
}


const char *getTypeStr( infotype_t type )
{
  switch( type )
    {
    case unused: return( "unused" );
    case slink_data: return( "slink_data" );
    case slink_control: return( "slink_control" );
    case information: return( "information" );
    case statistics: return( "statistics" );
    default: return( "unknown" );
    }
}

const char *getMessageStr( infomessage_t message )
{
  switch( message )
    {
    case NoMessage: return( "No Message" );
    case S_LinkDown: return( "S-Link down" );
    case S_LinkUp: return( "S-Link up" );
    case S_LinkError: return( "S-Link error" );
    case S_LinkAbort: return( "S-Link abort" );
    case S_LinkOverflow: return( "S-Link overflow" );
    case NoFreeDataBuffer: return( "no free databuffer" );
    case NoFreeInfoBlock: return( "no free infoblock" );
    case TargetAbort: return( "target abort" );
    case MasterAbort: return( "master abort" );
    default: return( "unknown" );
    }
}
