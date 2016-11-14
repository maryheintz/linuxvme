// TTCvi.cpp: implementation of TTCvi behavior

#include <iostream.h>
#include <iomanip.h>
#include <sys/types.h>
#include <ces/vmelib.h>
#include "auxvme.h"
#include "TTCvi.h"

TTCvi::TTCvi(unsigned long VMEBaseAddress) {
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
  // Add 0x80 to skip EEPROM
//   ttcadr = (TTCviBlock*)(find_controller(VMEBaseAddress, length, adrmodifier,
// 					 offset, size, &param) + 0x80);
//   // Should throw exception if unable to map address
//   if (ttcadr == (TTCviBlock*)(-1)) {
//     cout << "Unable to map VME address." << endl;
//   }
//   cout << hex << (unsigned long) ttcadr << endl;
  ttcadr = (TTCviBlock*)(0xdeb00080);
}

// Level-1 Accept parameter accessors

unsigned short TTCvi::GetTriggerSelect() {
  return ttcadr->csr1 & 0x7;
}

void TTCvi::SetTriggerSelect(unsigned short tsel) {
  ttcadr->csr1 = (ttcadr->csr1 & 0xfff8) | (tsel & 0x7);
}

void TTCvi::SendL1A() {
  unsigned short saved = GetTriggerSelect();
  SetTriggerSelect(4); // VME L1A
  ttcadr->sw_L1A = 0;
  SetTriggerSelect(saved); // Switch back
}

unsigned short TTCvi::GetRandTrigRate() {
  return (ttcadr->csr1 >> 12) & 0x7;
}

void TTCvi::SetRandTrigRate(unsigned short rate) {
  ttcadr->csr1 = (ttcadr->csr1 & 0x8fff) | ((rate & 0x7) << 12);
}

// Functions to control the inhibit channels

unsigned short TTCvi::GetInhibitDelay(int channel) {
  return ttcadr->b_go[channel].inhibit_delay;
}

void TTCvi::SetInhibitDelay(int channel, unsigned short delay) {
  ttcadr->b_go[channel].inhibit_delay = delay & 0xfff;
}

unsigned short TTCvi::GetInhibitDuration(int channel) {
  return ttcadr->b_go[channel].inhibit_duration;
}

void TTCvi::SetInhibitDuration(int channel, unsigned short duration) {
  ttcadr->b_go[channel].inhibit_duration = duration & 0xff;
}

// The asynchronous immediate cycles.

void TTCvi::BroadcastShortCommand(unsigned char command) {
  ttcadr->short_cycle = (unsigned short)command;
}

void TTCvi::SendLongCommand(unsigned short address, bool external,
			    unsigned char subaddr, unsigned char data) {
  ttcadr->long_cycle = (1ul << 31) | ((address & 0x3fff) << 17)
    | (external << 16) | (subaddr << 8) | data;
}

// Pre-loaded cycles

unsigned char TTCvi::GetBMode(int channel) {
  return ttcadr->b_go[channel].mode;
}

void TTCvi::SetBMode(int channel, unsigned char mode) {
  ttcadr->b_go[channel].mode = mode & 0xf;
}

void TTCvi::SetRetransmit(int channel, bool retransmit) {
  ttcadr->csr2 = (ttcadr->csr2 & ~(1u << 8 + channel))
    | (!retransmit << 8 + channel);
}

void TTCvi::ResetFIFO(int channel) {
  ttcadr->csr2 = (ttcadr->csr2) | (1u << 12 + channel);
}

void TTCvi::BPutShort(int channel, unsigned char command) {
  ttcadr->bgo_param[channel] = (0ul << 31) | (command << 23);
}

void TTCvi::BPutLong(int channel, unsigned short address, bool external,
		     unsigned char subaddr, unsigned char data) {
  ttcadr->bgo_param[channel] = (1ul << 31) | ((address & 0x3fff) << 17)
    | (external << 16) | (subaddr << 8) | data;
}

void TTCvi::BGoSignal(int channel) {
  ttcadr->b_go[channel].sw_go = 0;
}

// unsigned short TTCvi::GetClockDelay() {
//   return 2 * (~(ttcadr->csr1 >> 8) & 0xf);
// }
