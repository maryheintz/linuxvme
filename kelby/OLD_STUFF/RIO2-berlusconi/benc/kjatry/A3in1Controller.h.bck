// A3in1Controller.h: Software abstraction of ATLAS 3in1 Controller

#ifndef _A3IN1CONTROLLER_H
#define _A3IN1CONTROLLER_H

#include "auxvme.h"

// Struct specifying controller address space

struct A3in1Block {
  unsigned short enable_high;        // base+0
  unsigned short enable_low;         // base+2
  unsigned short tp_high;            // base+4
  unsigned short tp_low;             // base+6
  unsigned short clock_high;         // base+8
  unsigned short clock_low;          // base+a
  unsigned short rxw_high;           // base+c
  unsigned short rxw_low;            // base+e
  unsigned short unused1;            // base+10
  unsigned short unused2;            // base+12
  unsigned short multi_sel_high;     // base+14
  unsigned short multi_sel_low;      // base+16
  unsigned short back_load_high;     // base+18
  unsigned short back_load_low;      // base+1a
  unsigned short ctrl_clock;         // base+1c
  unsigned short enable_timer;       // base+1e
  unsigned short reset_timer;        // base+20
  unsigned short shift12;            // base+22
  unsigned short address;            // base+24
  unsigned short set_timer1;         // base+26
  unsigned short set_timer2;         // base+28
  unsigned short latch_timer2;       // base+2a
};

// Enumeration describing capacitor types

enum CapType { small, large };

// Controller class

class A3in1Controller {
public:
  // Constructor: specify zone and sector, assign address
  A3in1Controller(unsigned short inZone, unsigned short inSector,
		  unsigned long VMEBaseAddress);
  // Member functions: Select 3in1 card on motherboard to access
  void SelectCard(unsigned short card);
  // Accessors for shift register on controller
  unsigned short GetShift();
  void SetShift(unsigned short sval);
  // Timers
  void SetTimer1(unsigned short tval);
  void SetTimer2(unsigned short tval);
  void StartTimer();
  void ResetTimer();
  // Line states
  void SetTP(LineState state);
  void SetMultiSel(LineState state);
  void SetRXW(LineState state);
  void SetBackLoad(LineState state);
  void SetEnable(LineState state);
  void SetClock(LineState state);
  // Sending data to/from 3in1 cards
  void Clock_ctrl();
  unsigned short ClockBack();
  unsigned short Read3in1Shift();
  void Write3in1Shift(int n, unsigned short sval);
  void Send4(int fcn, int abit);
  void Send12(int fcn, int abit, int data);
  // General Members
  void ResetCard();
  unsigned short ReadStatus();
  void SetDrawerTimer(int section, int time);
  void SetDac(int dacvalue);
  void SetCapacitor(CapType type);
  double Inject(double charge); // returns actual charge injected
  bool TestCard(int card);
private:
  A3in1Block *ctrladr;
  unsigned short zone;
  unsigned short sector;
  double capacitance;
};

#endif // _A3IN1CONTROLLER_H not defined
