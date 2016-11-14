// SwitchBox.cpp: Implementation for reading switches on test pulser

#include <sys/types.h>
#include <ces/vmelib.h>
#include "auxvme.h"
#include "SwitchBox.h"

SwitchBox::SwitchBox(unsigned long VMEBaseAddress) {
  unsigned long length = 400;
  unsigned long adrmodifier = 0x39;
  unsigned long offset = 0;
  unsigned long size = 0;
  pdparam_master param = {
    1,   // VME Iack (must be 1 for all normal pages)
    0,   // VME Read Prefetch Option
    0,   // VME Write Posting Option
    1,   // VME Swap Option
    0,   // page number return
    0,   // reserved
    0,   // reserved
    0
  };
  address = (unsigned long*)find_controller(VMEBaseAddress, length,
					    adrmodifier, offset, size, &param);
  // Throw exception if unable to map address
}

bool SwitchBox::GetSwitch(int sw) {
  // Should throw exception if not between 1 and 8.
  return (*address >> (23 + sw)) & 1;
}
