// TTCvi.h: Interface to TTCvi functions
// Note:  Documentation on the TTC-VMEbus interface can be found in the
// RD12 working document "TTC-VMEbus Interface: TTCvi."  This and lots
// of other useful info about the LHC TTC systems can be found via the
// web page <http://www.cern.ch/TTC/intro.html>.

// Bit-fields representing control status registers.  This avoids writing
// to read-only bits, and acts as shorthand for using lots of bitwise
// operators.

#ifndef _TTCVI_H
#define _TTCVI_H

union CSR1 {
  unsigned short full;
  struct {
    unsigned short l1_trig_sel  : 3;  // (r/w) L1a<0-3>, VME if 4, random if 5
    unsigned short orb_sig_sel  : 1;  // (r/w) external orbit if 0
    unsigned short l1_fifofull  : 1;  // (r) L1A FIFO full if 1
    unsigned short l1_fifoempty : 1;  // (r) L1A FIFO empty if 1
    unsigned short l1_fiforst   : 1;  // (w) L1A FIFO reset if set to 1
    unsigned short vme_trpend   : 1;  // (r) VME transfer pending if 1
    unsigned short bcdelay      : 4;  // (r) read BC delay switch value
    unsigned short rantrigrt    : 3;  // (r/w) random trigger rate setting
    unsigned short unused       : 1;  // Highest bit is here
  } bits;
};

struct CSR2 {
  unsigned short rst_bgo : 4; // (w) Reset B-Go FIFO <3..0> if set to 1
  unsigned short rtx_bgo : 4; // (r/w) Retransmit B-Go FIFO if 0 when empty
  unsigned char status;       // (r) FIFO 3 full/empty..FIFO 0 full/empty
};


// structure of "B-Go" channel controls

struct B_Go {
  unsigned short mode;
  unsigned short inhibit_delay;
  unsigned short inhibit_duration;
  unsigned short sw_go;
};
  
// TTCvi address space

struct TTCviBlock {
  // Skip the 128-byte Configuration EEPROM memory.
  unsigned short csr1;
  unsigned short csr2;
  unsigned short sw_rst;
  unsigned short sw_L1A;
  unsigned long evt_count;
  unsigned long unused;
  B_Go b_go[4];
  unsigned long bgo_param[4];
  unsigned long long_cycle;
  unsigned short short_cycle;
};

// Class interface to TTCvi

class TTCvi {
public:
  // Default constructor: maps address
  TTCvi(unsigned long VMEBaseAddress);
  // Level-1 Accept controls
  unsigned short GetTriggerSelect();
  void SetTriggerSelect(unsigned short tsel);
  void SendL1A();
  unsigned short GetRandTrigRate();
  void SetRandTrigRate(unsigned short rate);
  // B-channel controls
  // Inhibit signals: delay and duration given in number of clock cycles.
  unsigned short GetInhibitDelay(int channel);
  void SetInhibitDelay(int channel, unsigned short delay);
  unsigned short GetInhibitDuration(int channel);
  void SetInhibitDuration(int channel, unsigned short duration);
  // Asynchronous immediate cycles
  void BroadcastShortCommand(unsigned char command);
  void SendLongCommand(unsigned short address, bool external,
		       unsigned char subaddr, unsigned char data);
  // Pre-Loaded cycles
  unsigned char GetBMode(int channel);
  void SetBMode(int channel, unsigned char mode);
  void SetRetransmit(int channel, bool retransmit);
  void ResetFIFO(int channel);
  void BPutShort(int channel, unsigned char command);
  void BPutLong(int channel, unsigned short address, bool external,
		unsigned char subaddr, unsigned char data);
  void BGoSignal(int channel);
  // Other functions
  // unsigned short GetClockDelay();
private:
  TTCviBlock *ttcadr;
};

#endif // _TTCVI_H not defined
