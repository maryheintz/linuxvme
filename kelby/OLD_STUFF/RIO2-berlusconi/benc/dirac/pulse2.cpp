#include <iostream.h>
#include <stdio.h>
#include <math.h>
#include <vector.h>
#include <graphics.h>
#include "Signal.h"
#include "A3in1Controller.h"
#include "SwitchBox.h"
#include "FermiADC.h"
#include "TTCvi.h"
#include "DigBoard.h"

int main() {

  // Variable declarations
  // A3in1Controller control(1, 1, 0x560000);
  // SwitchBox pulser(0xcf0000);
  // FermiADC fermi(0x0);
  TTCvi ttc(0xb00000);
  ttc.ResetFIFO(0);
  DigBoard adc(&ttc, 6);
  int i; // General loop counter

  // Choose a card
//   vector<int> found;
//   for (i = 1; i <= 48; ++i) {
//     if (control.TestCard(i)) {
//       found.push_back(i);
//     }
//   }
//   int size = found.size();
//   // If no cards are found, print a message and exit.
//   if (size == 0) {
//     cout << "No cards found." << endl;
//     return 0;
//   }
//   // If just one is found, use it.
//   int card;
//   if (size == 1) {
//     card = found[0];
//     cout << "Card " << found[0] << " found." << endl;
//   } else { // if more than one card is found, give user a choice
//     cout << "Cards found:" << endl;
//     for (i = 0; i < size; ++i) {
//       cout << "  " << found[i] << endl;
//     }
//     cout << "Choose a card: ";
//     cin >> card;
//   }
//   control.SelectCard(card);
//   control.Send12(2, 0, 0);   // s1=s2=s3=s4=0
//   control.Send4(1, 0);   // itr=0
//   control.Send4(0, 0);   // ire=0
//   control.Send4(3, 0);   // mse=0
//   control.SetDrawerTimer(1, 0);
//   control.SetDrawerTimer(2, 0);
//   control.SetDrawerTimer(3, 0);
//   control.SetDrawerTimer(4, 0);
//   control.SetTimer1(10); // tim1_set(tc);   /* coarse */
//   control.SetTimer2(100); // tim2_set(tf);   /* fine */
//   control.SelectCard(card); // card_sel(cadr);

  // Setup TTC and digitizers
  ttc.SetTriggerSelect(1);
  adc.SetMode(DIG_NORMAL);
  int board;
  cout << "Board: ";
  cin >> board;
  int channel;
  cout << "Channel: ";
  cin >> channel;
  int numsamples;
  cout << "Number of samples: ";
  cin >> numsamples;
  adc.SetSamples(numsamples);
  int pipeline;
  cout << "Pipeline length: ";
  cin >> pipeline;
  adc.SetDelay(pipeline);
  adc.SignalReset();

  // Choose a capacitor.  For lowgain, use 220 pF
//   int icap;
//   cout << "Which capacitor?  (0) small; (1) large: ";
//   cin >> icap;
//   if (icap == 0) {
//     control.SetCapacitor(small);
//   } else {
//     control.SetCapacitor(large);
//   }
//   // Get Charge
//   double charge;
//   cout << "Enter Charge: ";
//   cin >> charge;

  // loop
  // control.SelectCard(card); // card_sel(cadr);
  EventBuffer event(numsamples, DIG_NORMAL);
  Signal lowsig, highsig;
  // bool running = true;
  cin.get();
  while (true) {
    // fermi.Reset();
    // control.Inject(charge);
    cout << "OK: ready to read" << endl;
    // try {
    adc.ReadEvent(event, board);
    // }
//     catch (Data_error err) {
//       cout << adc << endl;
//       cout << "Error " << err.type << " occurred: "
// 	   << "Set " << err.set << ", Sample " << err.sample << endl;
//       if ((err.sample != -2) && (err.type != READ_ERROR)) {
// 	throw;
//       }
//     }
    cout << "OK: read successfully" << endl;

    // Now for the ????? digtizers

    // Fill the signals
    if (true) {
      event.FillSignal(lowsig, channel, numsamples, lowgain);
      // Make lowgain plot
      TekPlotter plot(cout);
      plot.SetTekMode(true);
      lowsig.Plot(&plot, "Low gain signal", 0, 4);
      plot.PlotString("Type <cr> for next plot, or q<cr> to quit: ",
		      0, 0, 0, 0);
      cout.flush();
      if (cin.get() == 'q') break;
      plot.SetTekMode(false);
    }
    if (true) {
      event.FillSignal(highsig, channel, numsamples, highgain);
      // Make highgain plot
      TekPlotter plot(cout);
      plot.SetTekMode(true);
      highsig.Plot(&plot, "High gain signal", 0, 4);
      plot.PlotString("Type <cr> for next plot, or q<cr> to quit: ",
		      0, 0, 0, 0);
      if (cin.get() == 'q') break;
      plot.SetTekMode(false);
    }
  }
}

