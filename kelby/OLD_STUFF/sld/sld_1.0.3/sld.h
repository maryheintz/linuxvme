#ifndef __SLD_H__
#define __SLD_H__

/*
 * sld.h: definition of the user interface
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
 * Macro:	SLD_IOC*
 *
 * Description:	The ioctl commands are generated by several Linux defined
 *		macros (_IO, _IOW, _IOR and _IOWR). They need a magic
 *		that is used internally, a sequence number and (for R, W
 *		and WR) the type of the data.
 */

#define SLD_IOC_MAGIC	'S'

/* Return line handling; only four lowest bits are used/valid */
#define SLD_IOCSRETL	_IOW( SLD_IOC_MAGIC, 0, int )
#define SLD_IOCGRETL	_IOR( SLD_IOC_MAGIC, 1, int )

/* get LDOWN status; also done via buffers */
#define SLD_IOCGDOWN	_IOR( SLD_IOC_MAGIC, 2, int )

/* get the size of the memory that's needed when using mmap() */
#define SLD_IOCGMAPSIZE	_IOR( SLD_IOC_MAGIC, 10, int )

/* Set the statistics time; 0 means off, any other value is in 100msec units */
#define SLD_IOCSSTAT	_IOW( SLD_IOC_MAGIC, 11, int )

/* Get statistics */
#define SLD_IOCGSTAT	_IOR( SLD_IOC_MAGIC, 11, int )



/*
 * Enum:	infotype
 *
 * Description:	This enum indicates what type of information is stored in
 *		an infoblock_t (see below). So it tells you which fields
 *		to use and how to implement them.
 */
typedef enum
{
  unused,		/* value 0: marked as not in use */
  slink_data,		/* use buffer (ptr and size) */
  slink_control,	/* use data */
  information,		/* no data at all */
  statistics		/* use buffer */
} infotype_t;

/*
 * Macro:	BUF_TYPE_WITH_BUFFER
 *
 * Description:	This macro indicates if a databuffer is used with this type of infoblock.
 */
#define BUF_TYPE_WITH_BUFFER( infoType ) ( ((infoType) == slink_data) || ((infoType) == statistics) )



/*
 * Enum:	infomessage
 *
 * Description:	This enum defines the different informational messages (and
 *		errors) that can be generated by the driver.
 */
typedef enum
{
  NoMessage,

  /* information */
  S_LinkDown,		/* S-Link went down */
  S_LinkUp,		/* S-Link went up */

  /* errors */
  S_LinkError = 100,	/* S-Link hardware indicated receive error */
  S_LinkAbort,		/* S-Link hardware aborted the transfer */
  S_LinkOverflow,	/* S-Link indicated overflow */
  NoFreeDataBuffer,	/* No databuffer available; means that some S-Link data was lost */
  NoFreeInfoBlock,	/* No infoblock available to store information. (this error is only generated for S-Link data) */
  TargetAbort,		/* PCI DMA abort */
  MasterAbort		/* PCI DMA abort; perhaps data was lost */
} infomessage_t;



/*
 * Struct:	infoblock_t
 *
 * Description:	Main type of communication between kerneldriver and user
 *		application.
 *
 * Fields:	infotype type
 *			Indicates what type of information is held here.
 *			Use this with BUF_TYPE_WITH_BUFFER to determine if
 *			there's a databuffer attached.
 *
 *		infomessage message
 *			informational field about the data in this message or
 *			about the S-Link
 *
 *		int u.data
 *			for some types of information, this field holds the data
 *
 *		void *u.buffer.ptr, int u.buffer.size
 *			for some other types of information, ptr points to the
 *			data and size indicates how much of data there is
 */
typedef struct
{
  infotype_t type;
  infomessage_t message;
  union
  {
    int data;
    struct
    {
      void *ptr;
      int size;
    } buffer;
  } u;
} infoblock_t;



/*
 * Struct:	stat_t
 *
 * Description:	The statistics in the driver are counted in this structure.
 *		If the infoType is statistics, this structure is in the
 *		buffer.
 *
 * Remark:	This structure has a layout that takes 16 byte cache-lines
 *		into consideration.
 */
typedef struct
{
  unsigned long long interrupt;	/* number of interrupts */

  unsigned long dataQueueGet;	/* number of databuffer gets */
  unsigned long dataQueueFull;	/* number of times no databuffer available */

  unsigned long infoQueueGet;	/* number of infoblock gets */

  /* offset: 20 bytes */
  unsigned long long dataBytes;	/* count of datawords */
  unsigned long dataInput;	/* number of dataword packets */
  unsigned long controlInput;	/* number of controlword */
  unsigned long infoInput;

  /* offset: 40 bytes */
  struct timeval stamp;		/* record the time of the statistics */

  /* offset: 48 bytes */
  unsigned long masterAbort;	/* number of DMA master aborts */
  unsigned long targetAbort;	/* number of DMA target aborts */

  unsigned long overflow;	/* simplex S-Link overflows */

  /* offset: 60 bytes */
  unsigned long fifoRead;	/* number of reads from AMCC fifo to finish data */

} sldstat_t;

#ifdef  __cplusplus
extern  "C" {
#endif



/*
 * Function:	getFullInfoBlock
 *
 * Description:	Get an infoblock_t from the queue with information in it.
 *
 * Parameter:	void *privData
 *			This is the address that was received from the
 *			mmap() of the driver.
 *
 * Returns:	infoblock_t *
 *			pointer to an infoblock_t full of information or NULL
 *			if there is no data available
 */
infoblock_t *getFullInfoBlock( void *privData );



/*
 * Function:	freeInfoBlock
 *
 * Description:	After handling the information in an infoblock_t, this
 *		function is used to release the infoblock_t and mark it
 *		as free.
 */
void freeInfoBlock( void *privData, infoblock_t *infoBlock );



/*
 * Function:	getTypeStr
 * Function:	getMessageStr
 *
 * Description:	Turn a infotype (infoMessage) into an appropriate
 *		human-readable string.
 */
const char *getTypeStr( infotype_t type );
const char *getMessageStr( infomessage_t message );

#ifdef  __cplusplus
}
#endif

#endif /* __SLD_H__ */