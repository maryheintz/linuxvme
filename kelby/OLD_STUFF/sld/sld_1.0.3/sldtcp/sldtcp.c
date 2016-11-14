#include <unistd.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>

#include "ant.h"
#include "sld.h"

#define TCPDMP_PORT     0x3001

#define SLD_log ANT_log

static  void    cleanUp (int arg);
static  void    sigAll  (void);
static  void    usage   (char *str);

int             sldFd;
void           *sldMem;
unsigned long   sldSize;
int             sldSock;

int             dataDump        = {0};
int             fullDataDump    = {0};
int             controlDump     = {0};
int             infoDump        = {0};
int             dataCheck       = {0};
int             setReturnLines  = {0};

  static int
waitForData (int sldFd)
{
  fd_set readFd;
  int    ret;

  /* sometimes we get errors and need to restart the select () */
  do
  {
    FD_ZERO ( &readFd );
    FD_SET  ( 0, &readFd ); /* stdin */
    FD_SET  ( sldFd, &readFd );
    ret = select (sldFd + 1, &readFd, NULL, NULL, NULL);

    if (ret < 0)
      ret = -errno;

    switch (ret)
    {
      case -EINTR:    /* signal; restart select */
	ret = 0;
	break;
      case 0:         /* timeout?! */
	break;
      case 2:         /* both descriptors have input */
	return (sldFd);
      case 1:
      {
	/* check which one */
	if      (FD_ISSET (sldFd, &readFd))
	  return (sldFd);
	else if (FD_ISSET (0,     &readFd))
	  return (0);
	else
	  ret = 0;
	break;
      }
      default: /* strange... */
	ret = 0;
	break;
    }
  }
  while (1);
}

  static void
dumpData (const infoblock_t *ibPtr)
{
  int  i;
  int  size;
  int *intPtr;

  printf ("data: buffer=%p, size=%d, message=%s.\n",
	   ibPtr->u.buffer.ptr,
	   ibPtr->u.buffer.size,
	   getMessageStr (ibPtr->message));

  if (fullDataDump)
  {
    intPtr = (int *)ibPtr->u.buffer.ptr;
    size = ibPtr->u.buffer.size / 4;
    for (i = 0; i < size; i++, intPtr++)
      printf ("data: %2.2d %8.8x\n", i, *intPtr);
  }
}

  static void
checkData (const infoblock_t *ibPtr)
{
  int  size;
  int *intPtr;

  intPtr = (int *)ibPtr->u.buffer.ptr;
  size   = ibPtr->u.buffer.size / 4;
}

  static void
handleBlock (const infoblock_t *ibPtr)
{
  switch (ibPtr->type)
  {
    case statistics:
      SLD_log (ANT_LOG_FAT, "We don't expect any statistics");
      break;

    case information:
      if (infoDump)
	SLD_log (ANT_LOG_INF, "information: message=%s.", getMessageStr( ibPtr->message ) );
      break;

    case slink_data:
      if (dataDump)
	dumpData (ibPtr);
      else if (ibPtr->message != NoMessage)
	SLD_log (ANT_LOG_INF, "data: message=%s.", getMessageStr (ibPtr->message));

      if (dataCheck)
	checkData (ibPtr);

      {
	int size;
	int first;

	first = ibPtr->u.buffer.size + sizeof (int);
	size  = TEC_write (sldSock, &first, sizeof (first));
	if (size != sizeof (int))
	  SLD_log (ANT_LOG_FAT, "Error return TEC_write %d (expected %d)", size, sizeof (first));

	size = TEC_write (sldSock,  ibPtr->u.buffer.ptr,  ibPtr->u.buffer.size);
	if (size != ibPtr->u.buffer.size)
	  SLD_log (ANT_LOG_FAT, "Error return TEC_write %d (expected %d)", size, ibPtr->u.buffer.size);
      }
      break;

    case slink_control:
      if (controlDump)
	SLD_log (ANT_LOG_INF, "control: data=%8.8x, message=%s", ibPtr->u.data,
			       getMessageStr (ibPtr->message));
      break;

    default:
      break;
  }
}


  int
main (int argc, char *argv[], char *envp[])
{
  infoblock_t *ibPtr;
  int          ret;
  int          dataFd;
  int          count = 0;

  switch (argc)
  {
    default:
    {
      usage ("Wrong number of arguments");
      break;
    }
    case 1:
    {
      break;
    }

#ifdef  NOT_DEFINED
    case 2:
    {
      if (sscanf(*++argv, "%d", &size) != 1)
	usage ("No valid first argument");

      break;
    }
#endif
  }

  sigAll ();

  sldFd = open ("/dev/sld0", O_RDWR);
  if (sldFd < 0)
    SLD_log (ANT_LOG_FAT, "Cannot open /dev/sld0");

  ret = ioctl (sldFd, SLD_IOCGMAPSIZE, &sldSize);
  if (ret < 0)
    SLD_log (ANT_LOG_FAT, "Cannot do ioctl for: map size");

  sldMem = mmap (0, sldSize, PROT_READ | PROT_WRITE, MAP_SHARED, sldFd, 0);
  if (sldMem == MAP_FAILED)
    SLD_log (ANT_LOG_FAT, "Cannot map");

  SLD_log (ANT_LOG_INF, "Waiting for connection with 'dump' program");

  sldSock = TEC_connection (TCPDMP_PORT, 0, 0);
  SLD_log (ANT_LOG_INF, "Connected to dump");

  while (1)
  {
    dataFd = waitForData (sldFd);

    if (dataFd == sldFd)
    {
      /* get the first infoblock */
      ibPtr = getFullInfoBlock (sldMem);
      while (ibPtr != NULL)
      {
	/* control return lines */
	if (setReturnLines && (ibPtr->type == slink_data))
	{
	  count++;
	  ret = ioctl (sldFd, SLD_IOCSRETL, &count);
	  if (ret < 0)
	    perror ("ioctl");
	}

	handleBlock   (ibPtr);
	freeInfoBlock (sldMem, ibPtr);
	ibPtr = getFullInfoBlock (sldMem);
      }
    }
  }
  cleanUp (0);
  return (0);
}

    static void
cleanUp (int arg)
{
  int ret;

  if (sldMem)
  {
    SLD_log (ANT_LOG_INF, "Unmap");
    ret = munmap (sldMem, sldSize);
    if (ret < 0)
      SLD_log (ANT_LOG_FAT, "Cannot unmap");
  }
  if (sldFd)
  {
    SLD_log (ANT_LOG_INF, "Close");
    ret = close (sldFd);
    if (ret < 0)
      SLD_log (ANT_LOG_FAT, "Cannot close driver");
  }
  exit (-1);
}

    void
sigAll (void)
{
  struct sigaction  sa;

  sa.sa_handler = cleanUp;
  sa.sa_flags   = 0;
  sigemptyset (&sa.sa_mask);
  sigaction (SIGINT,  &sa, NULL);

  sa.sa_handler = cleanUp;
  sa.sa_flags   = 0;
  sigemptyset (&sa.sa_mask);
  sigaction   (SIGTERM, &sa, NULL);

  sa.sa_handler = cleanUp;
  sa.sa_flags   = 0;
  sigemptyset (&sa.sa_mask);
  sigaction   (SIGBUS, &sa, NULL);
}

    void
usage (char *str)
{
    ANT_log (ANT_LOG_INF, "Usage: sldtcp");
    ANT_log (ANT_LOG_FAT, "%s", str);
}

