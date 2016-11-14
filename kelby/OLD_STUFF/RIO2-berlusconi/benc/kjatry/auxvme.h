/* auxvme.h: some useful declarations for dealing with vmebus cards  */

#ifndef _AUXVME_H
#define _AUXVME_H

#include <sys/types.h>
#include <ces/vmelib.h>
#include <ces/uiocmd.h>

/* Functions that should be prototyped in the CES headers, but aren't  */

extern "C" {
  unsigned long find_controller(unsigned long, unsigned long, unsigned long,
				unsigned long, unsigned long, pdparam_master*);
  int uio_open();
  int uio_close();
  int uio_calloc(uio_mem_t *dsc, int size);
  int uio_cfree(uio_mem_t *dsc);
	   }

enum LineState { low, high };

#endif /* _AUXVME_H not defined  */
