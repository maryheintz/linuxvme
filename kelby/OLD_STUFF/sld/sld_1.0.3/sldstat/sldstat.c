
/*
 * sldstat.c: stat program like iostat and vmstat
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
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include "sld.h"


int main( int argc, char **argv )
{
  int sldFd;
  int sleepTime;	/* time to sleep() in seconds */
  int lineCount;
  sldstat_t prevStat;
  sldstat_t currentStat;
  int ret;
  struct timeval timeDiff;

  printf( "SldStat version " SLD_VERSION ", Copyright (C) 2000 by Nikhef\n"
	  "Written by J.E. van Grootheest for Nikhef, Netherlands\n"
	  "SldStat comes with ABSOLUTELY NO WARRANTY. This is free\n"
	  "software, and you are welcome to redistribute it under\n"
	  "certain conditions. Refer to the included file COPYING.\n\n" );

  /* handle parameter */
  sleepTime = 1;
  if ( argc == 2 )
    sleepTime = atoi( argv[ 1 ] );
  if ( sleepTime < 1 )
    {
      printf( "Error in the parameter.\n" );
      exit( 1 );
    }

  /* prepare device */
  sldFd = open( "/dev/sld0", 0 );
  if ( sldFd < 0 )
    {
      perror( "open" );
      exit( 1 );
    }

  ioctl( sldFd, SLD_IOCGSTAT, &prevStat );
  for ( ; ; )
    {
      /* print header every now and then; this works till sleepTime==10
       *                1         2         3         4         5         6         7         8
       *       12345678901234567890123456789012345678901234567890123456789012345678901234567890
       *       12.123456 | 1234567 | 123456789 123456 |  123456 | 123 12 12 12
       */
      printf( "     time |     int |             data | control |       errors\n"
	      "    (sec) |         |     bytes packet |         | dqf ma ta ov\n" );

      for ( lineCount = 0; lineCount < 22; lineCount++ )
	{
	  sleep( sleepTime );
	  ret = ioctl( sldFd, SLD_IOCGSTAT, &currentStat );
	  if ( ret < 0 )
	    {
	      printf( "Error retrieving the statistics\n" );
	      continue;
	    }

	  timersub( &currentStat.stamp, &prevStat.stamp, &timeDiff );

#define DIFF( X ) (currentStat.##X - prevStat.##X)

	  printf( "%2.2ld.%6.6ld | %7.7llu | %9.9llu %6.6lu |  %6.6lu | %3.3lu %2.2lu %2.2lu %2.2lu\n",
		  timeDiff.tv_sec,
		  timeDiff.tv_usec,
		  DIFF( interrupt ),
		  DIFF( dataBytes ),
		  DIFF( dataInput ),
		  DIFF( controlInput ),
		  DIFF( dataQueueFull ),
		  DIFF( masterAbort ),
		  DIFF( targetAbort ),
		  DIFF( overflow ) );

	  prevStat = currentStat;
	}
    }

  close( sldFd );
}
