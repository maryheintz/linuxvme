// EventBuffer.h -- Interface to the SigBuffer object, which collects words
// from the S-link, checks errors, and allows access to samples.  Also
// has a debugging feature to dump to the screen.

#ifndef _EVENTBUFFER_H
#define _EVENTBUFFER_H

#include <iostream.h>
#include <ces/uiocmd.h>
#include "atdaq_types.h"
#include "s5933lib.h"
#include "SLINK.h"

class EventBuffer {
public:
  EventBuffer() throw (SLINK_error);
  ~EventBuffer() throw (SLINK_error);
  void ReadEvent() throw (SLINK_error);
  int GetSample(int board, int channel, int sample);
  void Dump(ostream& ost); // Hex dump
private:
  // Implementation not finalized.
  SLINK_device *dev;
  uio_mem_t buf_desc;
};

// "Formatted Print" operator

ostream& operator<<(ostream& ost, const EventBuffer& buf);

// Exception for S-Link errors: wrapper for return code.

class SLINK_error {
public:
  SLINK_error(int inCode);
  int code;
};

#endif // _SIGBUFFER_H not defined
