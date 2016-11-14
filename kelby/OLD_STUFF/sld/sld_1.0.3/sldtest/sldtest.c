
/*
 * sldtest.c: main program used to test the sld driver
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



#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>


#include "sld.h"



/* global variables for access to driver */
int sldFd;
void *sldMem;
unsigned long sldSize;

/* do we want all data printed? */
int dataDump = 0;       /* Was 0        */
int controlDump = 0;    /* Was 1        */
int informationDump = 1;
int statDump = 1;
int fullDataDump = 0;
int fullStatDump = 0;
int dataCheck = 0;
int expectedData = 1;
int setReturnLines = 0;
int errorDump = 0;
unsigned long long dataErrors = 0;

/* remember the previous statistics */
sldstat_t prevStat;

const char *binary( int x )
{
  unsigned int mask;
  int i;
  static char binBuf[ 33 ];

  for ( i = 0, mask = (1 << 31 );
	mask != 0;
	i++, mask >>= 1 )
    {
      binBuf[ i ] = (x & mask) ? '1' : '0';
    }
  binBuf[ 32 ] = '\0';

  return( binBuf );
}


static int waitForData( int sldFd )
{
  fd_set readFd;
  int ret;

  /* sometimes we get errors and need to restart the select() */
  do
    {
      FD_ZERO( &readFd );
      FD_SET( 0, &readFd ); /* stdin */
      FD_SET( sldFd, &readFd );
      ret = select( sldFd + 1, &readFd, NULL, NULL, NULL );

      if ( ret < 0 )
	ret = -errno;

      switch( ret )
	{
	case -EINTR: /* signal; restart select */
	  ret = 0;
	  break;
	case 0: /* timeout?! */
	  break;
	case 2: /* both descriptors have input */
	  return( sldFd );
	case 1:
	  {
	    /* check which one */
	    if ( FD_ISSET( sldFd, &readFd ) )
	      return( sldFd );
	    else if ( FD_ISSET( 0, &readFd ) )
	      return( 0 );
	    else
	      ret = 0;
	    break;
	  }
	default: /* strange... */
	  ret = 0;
	  break;
	}
    }
  while( 1 );
}

static void displayStats( sldstat_t *statPtr )
{
  struct timeval timeDiff;
  unsigned long long countDiff;
  unsigned long long bytesDiff;
  unsigned long long irqDiff;
  float timeFloat;
  float countThru;
  float bytesThru;
  float irqThru;
  float errThru;
  float errMB;
  static int bytesDiv;
  static char *bytesStr;

  if ( prevStat.stamp.tv_sec == 0 )
    {
      prevStat = *statPtr;
      bytesDiv = 0;
      bytesStr = "bytes";
      return;
    }

  /* calculate things we always need */
  timersub( &statPtr->stamp, &prevStat.stamp, &timeDiff );
  countDiff = statPtr->dataInput - prevStat.dataInput;
  bytesDiff = statPtr->dataBytes - prevStat.dataBytes;
  irqDiff = statPtr->interrupt - prevStat.interrupt;

  /* make floating point time */
  timeFloat = timeDiff.tv_usec;
  timeFloat = timeFloat / 1000000;
  timeFloat += timeDiff.tv_sec;

  /* calculate throughputs */
  countThru = countDiff / timeFloat;
  bytesThru = bytesDiff / timeFloat;
  irqThru = irqDiff / timeFloat;
  errThru = dataErrors / timeFloat;
  errMB = (float)dataErrors * (1024 * 1024) / bytesDiff;

  /* set dividers */
  if ( (bytesDiff >> (bytesDiv + 17)) != 0 )
    {
      bytesDiv += 10;
      switch( bytesDiv )
	{
	case 0: bytesStr = "bytes"; break;
	case 10: bytesStr = "KB"; break;
	case 20: bytesStr = "MB"; break;
	case 30: bytesStr = "GB"; break;
	case 40: bytesStr = "TB"; break;
	case 50: bytesStr = "PB"; break;
	default: bytesStr = "?B"; break;
	}
    }

  if ( fullStatDump )
    {
      printf( "stats: timestamp=%ld.%6.6ldsec\n",
	      statPtr->stamp.tv_sec,
	      statPtr->stamp.tv_usec );
      printf( "stats: interrupts=%llu master abort=%lu, target abort=%lu, overflow=%lu\n",
	      statPtr->interrupt,
	      statPtr->masterAbort,
	      statPtr->targetAbort,
	      statPtr->overflow );
      printf( "stats: data queue get=%lu full=%lu info queue get=%lu\n",
	      statPtr->dataQueueGet,
	      statPtr->dataQueueFull,
	      statPtr->infoQueueGet );
      printf( "stats: count data=%lu, control=%lu, info=%lu\n",
	      statPtr->dataInput, statPtr->controlInput, statPtr->infoInput );
      printf( "stats: data bytes queued: %llu (%llukB)\n",
	      statPtr->dataBytes, statPtr->dataBytes / 1024 );
    }

  if ( !dataDump && !controlDump && !fullStatDump )
    printf( "\n" );

  /* print the difference */
  printf( "stats: timediff=%ld.%6.6ldsec, data queue full=%lu, data errors=%llu\n",
	  timeDiff.tv_sec, timeDiff.tv_usec,
	  statPtr->dataQueueFull,
	  dataErrors );
  printf( "stats: totals: int.=%llu, count=%llu, bytes=%llu%s\n",
	  irqDiff,
	  countDiff,
	  bytesDiff >> bytesDiv, bytesStr );

  /* throughput */
  printf( "stats: thru: int.=%.0f/s, count=%.0f/s, bytes=%.0fkB/s\n",
	  irqThru,
	  countThru,
	  bytesThru / 1024 );

  /* errors */
  printf( "stats: errors: %.3e/s, %.3e/MB\n",
	  errThru, errMB );
	  
}

static void displayData( const infoblock_t *ibPtr )
{
  int i;
  int size;
  int *intPtr;

  printf( "data: buffer=%p, size=%d, message=%s.\n",
	  ibPtr->u.buffer.ptr,
	  ibPtr->u.buffer.size,
	  getMessageStr( ibPtr->message ) );
  if ( fullDataDump )
    {
      intPtr = ibPtr->u.buffer.ptr;
      size = ibPtr->u.buffer.size / 4;
      for ( i = 0; i < size; i++, intPtr++ )
	{
	  printf( "data: %2.2d %8.8x %s\n", i, *intPtr, binary( *intPtr ) );
	}
    }
}

static void checkData( const infoblock_t *ibPtr )
{
  int i;
  int size;
  int *intPtr;
  int count;

  /* we never have expectedData == 0 */
  if ( expectedData == 0 )
    expectedData = 1;

  intPtr = ibPtr->u.buffer.ptr;
  size = ibPtr->u.buffer.size / 4;
  for ( i = 0; i < size; i++, intPtr++, expectedData<<=1 )
    {
      if ( expectedData == 0 )
	expectedData = 1;

      if ( *intPtr != expectedData )
	{
	  dataErrors++;
	  if ( errorDump )
	    printf( "received data not correct: index=%d, expected=%8.8x, received=%8.8x\n",
		    i, expectedData, *intPtr );
	  count = 0;
	  do
	    {
	      expectedData <<= 1;
	      if ( expectedData == 0 )
		expectedData = 1;
	      count++;
	    }
	  while ( (*intPtr != expectedData) && (count < 32) );
	  if ( errorDump )
	    printf( "skipped %d forward\n", count );
	}
    }

}

static void handleBlock( const infoblock_t *ibPtr )
{
  switch ( ibPtr->type )
    {
    case statistics:
      /* print just received statistics */
      if ( statDump )
	displayStats( ibPtr->u.buffer.ptr );
      break;

    case information:
      if ( informationDump )
	printf( "information: message=%s.\n",
		getMessageStr( ibPtr->message ) );
      break;

    case slink_data:
      if ( dataDump )
	{
	  displayData( ibPtr );
	}
      else if ( ibPtr->message != NoMessage )
	{
	  printf( "data: message=%s.\n",
		  getMessageStr( ibPtr->message ) );
	}
      if ( dataCheck )
	checkData( ibPtr );
      break;

    case slink_control:
      expectedData = 0;
      if ( controlDump )
	{
	  printf( "control: data=%8.8x, message=%s\n",
		  ibPtr->u.data,
		  getMessageStr( ibPtr->message ) );
	}
      break;

    default:
      break;
    }
}

static int handleKey( void )
{
  char c;
  int i;
  int ret;
  int val;

  /* read from stdin */
  ret = read( 0, &c,1  );
  if ( ret < 0 )
    return( 0 );

  switch( c )
    {
    case '\n':
      /* ignore */
      break;

    case 'm':
      printf( "\nDump menu:\n"
	      " d - display data toggle\n"
	      " c - display control toggle\n"
	      " i - display information toggle\n"
	      " s - display statistics toggle\n"
	      " r - set return line toggle\n"
	      " D - toggle full data dump\n"
	      " e - check data from S-Link for errors\n"
	      " E - display errors toggle\n"
	      " S - toggle full statistics dump\n"
	      " R - request return line status from driver\n"
	      " L - request LDOWN status from driver\n"
	      " w - wait for the next keypress\n"
	      " 0 - (zero) turn off statistics generation\n"
	      " 1-9 - turn on statistics generation every X*500msec\n"
	      " q - quit\n" );
      break;

    case 's':
      statDump = !statDump;
      break;

    case 'c':
      controlDump = !controlDump;
      break;

    case 'i':
      informationDump = !informationDump;
      break;

    case 'E':
      errorDump = !errorDump;
      break;

    case 'd':
      dataDump = !dataDump;
      break;

    case 'e':
      dataCheck = !dataCheck;
      dataErrors = 0;
      break;

    case 'r':
      setReturnLines = !setReturnLines;
      break;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      prevStat.stamp.tv_sec = 0;
      dataErrors = 0;
      val = c - '0';
      val *= 5;
      ret = ioctl( sldFd, SLD_IOCSSTAT, &val );
      if ( ret < 0 )
	perror( "ioctl" );
      break;

    case 'D':
      fullDataDump = !fullDataDump;
      break;

    case 'S':
      fullStatDump = !fullStatDump;
      break;

    case 'w':
      {
	/* read CR */
	ret = read( 0, &c,1  );

	/* now wait for next keypress */
	ret = read( 0, &c,1  );
	if ( ret < 0 )
	  perror( "wait for data" );
	break;
      }

    case 'R':
      if ( ioctl( sldFd, SLD_IOCGRETL, &i ) < 0 )
	perror( "ioctl" );      
      printf( "return lines are: %1.1x\n", i );
      break;

    case 'L':
      if ( ioctl( sldFd, SLD_IOCGDOWN, &i ) < 0 )
	perror( "ioctl" );      
      printf( "link is %s\n", i ? "down" : "up" );
      break;

    case 'q':
      return( 1 );

    default:
      printf( "Unknown menu choice %c.\n", c );
      break;
    }

  return( 0 );
}




int main( void )
{
  infoblock_t *ibPtr;
  int ret;
  int quit;
  int dataFd;
  int count = 0;

  printf( "SldTest version " SLD_VERSION ", Copyright (C) 2000 by Nikhef\n"
	  "Written by J.E. van Grootheest for Nikhef, Netherlands\n"
	  "SldTest comes with ABSOLUTELY NO WARRANTY. This is free\n"
	  "software, and you are welcome to redistribute it under\n"
	  "certain conditions. Refer to the included file COPYING.\n\n" );

  /* open device */
  sldFd = open( "/dev/sld0", O_RDWR );
  if ( sldFd < 0 )
    {
      perror( "open" );
      exit( 1 );
    }

  /* get mmap size */
  ret = ioctl( sldFd, SLD_IOCGMAPSIZE, &sldSize );
  if ( ret < 0 )
    {
      perror( "ioctl get size" );
      exit( 1 );
    }
  printf( "test: SLD requires %lu bytes mmaped\n", sldSize );

  /* do mmap */
  sldMem = mmap( 0, sldSize, PROT_READ | PROT_WRITE, MAP_SHARED, sldFd, 0 );
  if ( sldMem == MAP_FAILED )
    {
      perror( "mmap" );
      close( sldFd );
      exit( 1 );
    }
  printf( "test: SLD mapped at %p\n", sldMem );

  quit = 0;
  while ( !quit )
    {
      dataFd = waitForData( sldFd );

      if ( dataFd == sldFd )
	{
	  /* get the first infoblock */
	  ibPtr = getFullInfoBlock( sldMem );
	  while( ibPtr != NULL )
	    {
	      /* control return lines */
	      if ( setReturnLines && (ibPtr->type == slink_data) )
		{
		  count++;
		  ret = ioctl( sldFd, SLD_IOCSRETL, &count );
		  if ( ret < 0 )
		    perror( "ioctl" );
		}

	      /* handle and free the infoblock */
	      handleBlock( ibPtr );
	      freeInfoBlock( sldMem, ibPtr );

	      /* get the next infoblock */
	      ibPtr = getFullInfoBlock( sldMem );
	    }
	}

      if ( dataFd == 0 )
	{
	  quit = handleKey( );
	}      
    }

  ret = munmap( sldMem, sldSize );
  if ( ret < 0 )
    perror( "munmap" );

  ret = close( sldFd );
  if ( ret < 0 )
    perror( "close" );

  return( 0 );
}


