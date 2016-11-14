// SwitchBox.h: interface to switches on test pulser

#ifndef _SWITCHBOX_H
#define _SWITCHBOX_H

class SwitchBox {
public:
  SwitchBox(unsigned long VMEBaseAddress);
  bool GetSwitch(int i);
private:
  unsigned long *address;
};

#endif // _SWITCHBOX_H not defined
