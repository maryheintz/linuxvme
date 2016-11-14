// findcard: A simple program to test cards, 3in1 controller hardware and
// library

#include <iostream.h>
#include <vector.h>
#include "A3in1Controller.h"
#include "SwitchBox.h"

void main() {
  unsigned short zone, sector;
  cout << "Zone: ";
  cin >> zone;
  cout << "Sector: ";
  cin >> sector;
  A3in1Controller control(zone, sector, 0x560000); // Maps addresses
  vector<int> found;
  for (int i = 1; i <= 48; ++i) {
    if (control.TestCard(i)) {
      found.push_back(i);
    }
  }
  int size = found.size();
  if (size != 0) {
    cout << endl << "Cards Found:" << endl;
    for (int j = 0; j < size; ++j) {
      cout << found[j] << endl;
    }
  } else { // found is empty
    cout << endl << "No cards found." << endl;
  }
  cout << endl;
  SwitchBox pulser(0xcf0000);
  for (int sw = 1; sw <= 8; ++sw) {
    if (pulser.GetSwitch(sw)) {
      cout << "Switch " << sw << " is on" << endl;
    } else {
      cout << "Switch " << sw << " is off" << endl;
    }
  }
}
