// EventBuffer.cpp -- implementation of Buffer for events coming from SLINK.
// Note on exception handling:  At this time, SLINK_error exceptions thrown
// from these functions are not expected to be caught.  Like in previous
// code, they print the error message associated with the return code and
// if they are uncaught, the program terminates.  But in the future they can
// be caught and recovery and/or further error-checking is possible through
// that mechanism.

#include <iostream.h>
#include <mem.h>
#include "atdaq_types.h"
#include "s5933lib.h"
#include "SLINK.h"
#include "EventBuffer.h"

EventBuffer::EventBuffer() {
  SLINK_parameters slink_parameters;
  SLINK_InitParameters(&slink_parameters);
  slink_parameters.start_word = 0x00000000;
  slink_parameters.stop_word = 0xfffffff0;
  // should throw here if SSP_open returns an error.
  SSP_Open(&slink_parameters, &dev);
  uio_open();
  uio_calloc(&buf_desc, 9000 * sizeof(int));
}

EventBuffer::~EventBuffer() {
  uio_cfree(&buf_desc);
  uio_close();
  SSP_Close(dev);
}

void EventBuffer::ReadEvent() {
  // use DMA
  char *address = (char*)(buf_desc.paddr | 0x80000000);
  int size = buf_desc.size;
  SSP_InitRead(dev, address, size, true, 0);
  int code;
  for (int status = !SLINK_FINISHED; status != SLINK_FINISHED;
       code = SSP_ControlAndStatus(dev, &status)) {
    if (code != SLINK_OK) {
      throw SLINK_error(code);
    }
  }
  // Now check for parity and other errors in packet.  We need a format spec.!
}

// Exception constructor

SLINK_error::SLINK_error(int inCode) : code(inCode) {
  SLINK_PrintErrorMessage(code);
}
