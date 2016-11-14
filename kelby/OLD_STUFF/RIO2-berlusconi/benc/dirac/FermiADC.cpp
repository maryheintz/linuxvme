// FermiADC.cpp: Implementation of FERMI ADC behavior

#include <iostream.h>
#include <sys/types.h>
#include <ces/vmelib.h>
#include "auxvme.h"
#include "Signal.h"
#include "FermiADC.h"

FermiADC::FermiADC(unsigned long VMEBaseAddress) {
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
  adc = (FermiADCBlock*)find_controller(VMEBaseAddress, length, adrmodifier,
					offset, size, &param);
  if (adc == (FermiADCBlock*)(-1)) {
    cout << "Unable to map VME address" << endl;
    exit(0);
  }
  // Throw exception if unable to map address
  Reset(); // Issue FERMI reset
  adc->sample = 255; // set to full 255 samples
  adc->delay = 2; // set delay
}

void FermiADC::Reset() {
  adc->reset = 0;
}

bool FermiADC::Trigger() {
  return (adc->status & 4);
}

bool FermiADC::WaitTrigger() {
  int lcnt;
  for (lcnt = 0; lcnt < 1000; ++lcnt) {
    if (Trigger()) break;
  }  
  return (lcnt < 1000);
}

int FermiADC::GetSample(int channel) {
  return (adc->rd[channel - 1] & 0x3ff);
}

void FermiADC::FillSignal(Signal& sig, int channel, int numsamples) {
  sig.Clear();
  for (int i = 0; i < numsamples; ++i) {
    sig.AddSample(GetSample(channel));
  }
}
