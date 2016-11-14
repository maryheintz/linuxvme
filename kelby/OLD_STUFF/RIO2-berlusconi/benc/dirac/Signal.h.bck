// Signal.h: Class for processing ADC signals.

#include <vector.h>
#include <graphics.h>

#ifndef _SIGNAL_H
#define _SIGNAL_H

struct pedinfo {
  double mean;
  double variance;
};

class Signal {
public:
  // Constructors: default, and from FERMI ADC object
  Signal();
  // Member functions
  int operator[](int sample);
  int GetPeaksample();
  int GetPeak();
  pedinfo Pedestal(int first, int last);
  void Clear();
  void AddSample(int counts);
  void Plot(GraphPort *port, const char *title, int pbegin, int pend);
private:
  vector<int> samples;
  int peaksample;
};

#endif // _SIGNAL_H not defined
