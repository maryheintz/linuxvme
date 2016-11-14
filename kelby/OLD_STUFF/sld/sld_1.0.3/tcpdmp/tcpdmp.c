#include <unistd.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <pwd.h>

#include "ant.h"

#define TCP_SIZE        (1024*64)
#define TCP_PORT        0x3001

static void  usage     (char *str);

main (int argc, char *argv[], char *envp[])
{
    int  event = {0};
    int  size, sock;
    char buf [TCP_SIZE];
    char pro [ANT_MAX_NAME];

    switch (argc)
    {
      default:
      {
	usage ("Wrong number of argumets");
	break;
      }
      case 2:
      {
	if (sscanf(*++argv, "%s", pro) != 1)
	  usage ("No valid first argument");

	break;
      }
    }

    ANT_log (ANT_LOG_INF, "Tcpdmp waiting for connection");
    sock = TEC_setup_client (pro,  TCP_PORT, TEC_WAIT);
    ANT_log (ANT_LOG_INF, "Tcpdmp established connection");

    while (1)
    {
      size = TEC_read (sock, buf, TCP_SIZE);
      if (size <= 0)
	ANT_log (ANT_LOG_FAT, "Error return TEC_read %d", size);

      ANT_time (size);

      if (!(++event % 1000))
      {
	printf ("\rEvent %8d\t", event);
	fflush (stdout);
      }
    }
}

    void
usage (char *str)
{
    ANT_log (ANT_LOG_INF, "Usage: tcpdmp producing_machine_name");
    ANT_log (ANT_LOG_FAT, "%s", str);
}


