// ttctest.cpp simple program to test TTCvi library.  Will change often.

#include <iostream.h>
#include "TTCvi.h"
#include "DigBoard.h"

void main() {
  // Initialize the address space
  TTCvi ttc(0xb00000);
  DigBoard dig(&ttc, 1);
  int ns;
  cout << "Number of samples: ";
  cin >> ns;
  dig.SetSamples(ns);
  dig.SignalReset();
}
