// FermiADC.h: Interface to FERMI ADC functions

#ifndef _FERMIADC_H
#define _FERMIADC_H

#include "Signal.h"

// Memory space abstraction

struct FermiADCBlock {
  unsigned short b0;        // base+0
  unsigned short b2;        // base+2
  unsigned short b4;        // base+4
  unsigned short b6;        // base+6
  unsigned short b8;        // base+8
  unsigned short ba;        // base+a
  unsigned short bc;        // base+c
  unsigned short be;        // base+e
  unsigned short rd[3];     // base+10 - base+15
  unsigned short b16;       // base+16
  unsigned short all;       // base+18
  unsigned short b1a;       // base+1a
  unsigned short b1c;       // base+1c
  unsigned short delay;     // base+1e
  unsigned short sample;    // base+20
  unsigned short status;    // base+22
  unsigned short reset;     // base+24
  unsigned short trig;      // base+26
};

// Class interface

class FermiADC {
public:
  // Constructor maps address space
  FermiADC(unsigned long VMEBaseAddress);
  // Member functions
  void Reset();
  bool Trigger();
  bool WaitTrigger();
  int GetSample(int channel);
  void FillSignal(Signal& sig, int channel, int numsamples);
private:
  FermiADCBlock *adc;
};

#endif // _FERMIADC_H not defined
