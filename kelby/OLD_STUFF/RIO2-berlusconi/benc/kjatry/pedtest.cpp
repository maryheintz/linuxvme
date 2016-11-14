// pedtest.cpp: Getting pedestal distributions of <...> digitizers

#include <iostream.h>
#include <fstream.h>
#include <vector.h>
#include "Signal.h"
#include "A3in1Controller.h"
#include "TTCvi.h"
#include "DigBoard.h"

int main() {

  // Variable declarations
  // A3in1Controller control(1, 1, 0x560000);
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
  int pipeline;
  cout << "Pipeline length: ";
  cin >> pipeline;
  ttc.SetTriggerSelect(1);
  adc.SetMode(DIG_CALIB);
  adc.SetSamples(15);
  adc.SetDelay(pipeline);
  adc.SignalReset();

  // Select a channel
  int channel;
  cout << "Channel: ";
  cin >> channel;

  // open file
  char filename[50];
  cout << "Filename: ";
  cin.get();
  cin.getline(filename, 50);
  ofstream file(filename);

  // Loop
  EventBuffer *events[6400];
  for (i = 0; i < 6400; ++i) {
    events[i] = new EventBuffer(15, DIG_CALIB);
  }
  cout << "Done Allocating" << endl;
  // cout << "OK2" << endl;
  // control.SetCapacitor(large);
  Signal losig, hisig;
  // cin.get();
  for (i = 0; i < 6400; ++i) {
    adc.ReadEvent(*events[i], 0);
    if (i % 100 == 0) {
      cout << i << endl;
    }
  }

  // Now write
  for (i = 0; i < 6400; ++i) {
    (*events[i]).FillSignal(hisig, channel, 15, highgain);
    // (*events[i]).FillSignal(losig, channel, 15, lowgain);
    file << i / 25 << "     " << hisig.GetPeak() << endl;
    delete events[i];
  }

}
    
