#include <iostream.h>
#include <stdio.h>
#include <math.h>
#include <vector.h>
#include <graphics.h>
#include "A3in1Controller.h"
#include "SwitchBox.h"
#include "FermiADC.h"
#include "Signal.h"

int main() {

  // Variable declarations
  A3in1Controller control(1, 1, 0x560000);
  SwitchBox pulser(0xcf0000);
  FermiADC adc(0x0);
  int i; // General loop counter

  vector<int> found;
  for (i = 1; i <= 48; ++i) {
    if (control.TestCard(i)) {
      found.push_back(i);
    }
  }
  int size = found.size();
  // If no cards are found, print a message and exit.
  if (size == 0) {
    cout << "No cards found." << endl;
    return 0;
  }
  // If just one is found, use it.
  int card;
  if (size == 1) {
    card = found[0];
    cout << "Card " << found[0] << " found." << endl;
  } else { // if more than one card is found, give user a choice
    cout << "Cards found:" << endl;
    for (i = 0; i < size; ++i) {
      cout << "  " << found[i] << endl;
    }
    cout << "Choose a card: ";
    cin >> card;
  }
  control.SelectCard(card);
  control.Send12(2, 0, 0);   // s1=s2=s3=s4=0
  control.Send4(1, 0);   // itr=0
  control.Send4(0, 0);   // ire=0
  control.Send4(3, 0);   // mse=0
  control.SetDrawerTimer(1, 0);
  control.SetDrawerTimer(2, 0);
  control.SetDrawerTimer(3, 0);
  control.SetDrawerTimer(4, 0);
  control.SetTimer1(10); // tim1_set(tc);   /* coarse */
  control.SetTimer2(100); // tim2_set(tf);   /* fine */

  control.SelectCard(card); // card_sel(cadr);
  // Choose a capacitor.  For lowgain, use 220 pF
  int icap;
  cout << "Which capacitor?  (0) small; (1) large" << endl;
  cin >> icap;
  if (icap == 0) {
    control.SetCapacitor(small);
  } else {
    control.SetCapacitor(large);
  }
  // Get Charge
  double charge;
  cout << "Enter Charge: ";
  cin >> charge;

  // loop
  control.SelectCard(card); // card_sel(cadr);  
  Signal lowsig, highsig;
  // bool running = true;
  cin.get();
  while (true) {
    adc.Reset();
    control.Inject(charge);
    // Wait for trigger
    if (adc.WaitTrigger()) {
      // Fill the signals
      if (pulser.GetSwitch(1)) {
	adc.FillSignal(lowsig, 2, 40);
	// Make lowgain plot
	TekPlotter plot(cout);
	plot.SetTekMode(true);
	lowsig.Plot(&plot, "Low gain signal", 0, 15);
	plot.PlotString("Type <cr> for next plot, or q<cr> to quit: ",
			 0, 0, 0, 0);
	cout.flush();
	if (cin.get() == 'q') break;
	plot.SetTekMode(false);
      }
      if (pulser.GetSwitch(2)) {
	adc.FillSignal(highsig, 1, 40);
	// Make highgain plot
	TekPlotter plot(cout);
	plot.SetTekMode(true);
	highsig.Plot(&plot, "High gain signal", 0, 15);
	plot.PlotString("Type <cr> for next plot, or q<cr> to quit: ",
			 0, 0, 0, 0);
	if (cin.get() == 'q') break;
	plot.SetTekMode(false);
      }
    }
  }
}
