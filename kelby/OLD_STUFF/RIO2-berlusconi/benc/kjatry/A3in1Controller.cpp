// A3in1Controller.cpp: implementation of ATLAS 3 in 1 controller behavior

#include <time.h>
#include <sys/types.h>
#include <ces/vmelib.h>
#include "auxvme.h"
#include "A3in1Controller.h"

A3in1Controller::A3in1Controller(unsigned short inZone,
				 unsigned short inSector,
				 unsigned long VMEBaseAddress) :
  zone(inZone),
  sector(inSector) {
  // Throw exception if inputs are bad
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
  ctrladr = (A3in1Block*)find_controller(VMEBaseAddress, length, adrmodifier,
					 offset, size, &param);
  // Throw exception if unable to map address
}

// card_sel lines are used to address a single 3in1 card at a time.  This
// is necessary for operations that drive the data_out line since only one
// card can drive the data_out line at a time.
void A3in1Controller::SelectCard(unsigned short card) {
  // Throw on bad input
  ctrladr->address = (zone << 12) | (sector << 6) | card;
}

// Read value in controller shift register
unsigned short A3in1Controller::GetShift() {
  return ctrladr->shift12;
}

// Set controller shift register to sval
void A3in1Controller::SetShift(unsigned short sval) {
  ctrladr->shift12 = sval;
}

void A3in1Controller::SetTimer1(unsigned short tval) {
  ctrladr->set_timer1 = tval;
}

void A3in1Controller::SetTimer2(unsigned short tval) {
  ctrladr->set_timer2 = tval;
  ctrladr->latch_timer2 = 0; // latch value into the timer
}

/*---------------------------------------------*/
/*   start timer 1                             */
/*   this enables timer 1 to start on the next */
/*   fermi clock cycle (front panel ecl input) */
/*   at the timer start the front panel ttl    */
/*   signal FS (fermi start) also goes high    */
/*   this allows phasing the charge injection  */
/*   with the fermi readout                    */
/*---------------------------------------------*/
void A3in1Controller::StartTimer() {
  ctrladr->enable_timer = 0;
}

/*---------------------------------------------*/
/*   reset timer done lines                    */
/*---------------------------------------------*/
void A3in1Controller::ResetTimer() {
  ctrladr->reset_timer = 0;
}

/*--------------------------------------------------------------------*/
/*   tp line low: discharge state, normal operations state            */
/*   note: timer done also sets tp low                                */
/* ------------------------------------------------------------------ */
/*   tp line high: charge up start.  This charges up the calibration  */
/*   capacitor C3.  When tp drops back low, a pulse is injected into  */
/*   the shaper.  tp goes low on SetTP(low) (computer control) or on  */
/*   timer2 done (phased with fermi clock).                           */
/*--------------------------------------------------------------------*/
void A3in1Controller::SetTP(LineState state) {
  if (state == low) {
    ctrladr->tp_low = 0;
  } else {
    ctrladr->tp_high = 0;
  }
}

/*-------------------------------------------------------*/
/*   sets the multi_sel line to low                      */
/* ----------------------------------------------------- */
/*   sets the multi_sel line to high                     */
/*   this allows commands to be executed in parallel     */
/*   on enabled 3in1 cards                               */
/*   when multi_sel is high, all cards with their mse    */
/*   (multi select enable) bit high will respond to      */
/*   commands on the bus                                 */
/*   commands that return data will not operate with     */
/*   multi_sel since only one 3in1 card is allowed to    */
/*   drive the data_out line at a time                   */
/*-------------------------------------------------------*/
void A3in1Controller::SetMultiSel(LineState state) {
  if (state == low) {
    ctrladr->multi_sel_low = 0;
  } else {
    ctrladr->multi_sel_high = 0;
  }
}

/*--------------------------------------------*/
/*   sets the rxw line to low                 */
/*   with rxw low, the data_in line value     */
/*   is shifted in to the 3in1 shift register */
/*   as the clock line goes high              */
/*--------------------------------------------*/
/*   sets the rxw line to high          */
/*   when rxw is high as the 3in1 shift */
/*   register is clocked circularly     */
/*   this allowes one to load the 3in1  */
/*   shift register (with rxw low)      */
/*   then read back the stored value    */
/*   (with rxw high)  then execute the  */
/*   function if the read back agrees   */
/*   with the sent value                */
/*--------------------------------------*/
void A3in1Controller::SetRXW(LineState state) {
  if (state == low) {
    ctrladr->rxw_low = 0;
  } else {
    ctrladr->rxw_high = 0;
  }
}

/*--------------------------------------------*/
/*   sets the back_load line to low           */
/*--------------------------------------------*/
/*   sets the back_load line to high             */
/*   this loads the 3in1 shift register with     */
/*   internal state bits (like reading a status) */
/*-----------------------------------------------*/
void A3in1Controller::SetBackLoad(LineState state) {
  if (state == low) {
    ctrladr->back_load_low = 0;
  } else {
    ctrladr->back_load_high = 0;
  }
}

/*-----------------------------------------*/
/*   sets the enable line to low           */
/*-----------------------------------------*/
/*   sets the enable line to high              */
/*   this causes selected 3in1 cards to        */
/*   execute the function code currently       */
/*   in the 3in1 shift register                */
/*   a 3in1 card is selected if its card_sel   */
/*   is high or if multi_sel is high and       */
/*   mse on the card is set high               */
/*---------------------------------------------*/
void A3in1Controller::SetEnable(LineState state) {
  if (state == low) {
    ctrladr->enable_low = 0;
  } else {
    ctrladr->enable_high = 0;
  }
}

/*----------------------------------------*/
/*   sets the clock line to low           */
/*----------------------------------------*/
/*------------------------------------------*/
/*   sets the clock line to high            */
/*   clk_high() followed by clk_low()       */
/*   clocks the shift register of selected  */
/*   3in1 cards by 1 bit                    */
/*------------------------------------------*/
void A3in1Controller::SetClock(LineState state) {
  if (state == low) {
    ctrladr->clock_low = 0;
  } else {
    ctrladr->clock_high = 0;
  }
}

/*-------------------------------------------*/
/*   clock the controller shift register     */
/*   by 1 bit                                */
/*   one alternatly clocks the 3in1 clock    */
/*   and the controller clock to move data   */
/*   from the controller shift register to   */
/*   the 3in1 shift register of visa versa   */   
/*-----------------------------------------  */
void A3in1Controller::Clock_ctrl() {
  ctrladr->ctrl_clock = 0;
}

/*----------------------------------------*/
/*    read back the 3in1 shift register   */
/*     (full 12 bits)                     */
/*----------------------------------------*/
unsigned short A3in1Controller::Read3in1Shift() {
  SetBackLoad(low);
  SetRXW(high);
  for (int i = 0; i < 12; ++i) {
    Clock_ctrl();
    SetClock(high);
    SetClock(low);
  }
  return GetShift();
}

/*--------------------------------------------------*/
/*   send n bits of sval to the 3in1 shift register */
/*--------------------------------------------------*/
void A3in1Controller::Write3in1Shift(int n, unsigned short sval) {
  SetBackLoad(low);
  SetRXW(low);
  SetShift(sval);
  for (int i = 0; i < n; ++i) {
    SetClock(high);
    SetClock(low);
    Clock_ctrl();
  }
}

/*---------------------------------------------------------------*/
/*    send and execute a single action bit function code         */
/*     to the selected 3in1 card                                 */
/*     f0 - set intg_rd                                          */
/*     f1 - set itr                                              */
/*     f3 - set mse                                              */
/*     f4 - set tpe                                              */
/*     f5 - set lcal_enable                                      */
/*---------------------------------------------------------------*/
void A3in1Controller::Send4(int fcn, int abit) {
  Write3in1Shift(4, (abit << 11) | (fcn << 8));
  SetEnable(high);
  SetEnable(low);
}

/*---------------------------------------------------------------*/
/*    send and execute a multi bit function code                 */
/*     f2 - set s1 s2 s3 s4 = data                               */
/*     f6 - set dac  voltage = data                              */
/*         abit=0 coarse                                         */
/*         abit=1 fine                                           */
/*---------------------------------------------------------------*/
void A3in1Controller::Send12(int fcn, int abit, int data) {
  Write3in1Shift(12, (data << 4) | (abit << 3) | fcn);
  SetEnable(high);
  SetEnable(low);
}

/*----------------------------------------*/
/*    reset control lines and 3in1        */
/*    to default data taking state        */
/*    note: card_sel(n) must be called    */
/*    to select the card being addressed  */
/*----------------------------------------*/
void A3in1Controller::ResetCard() {
  SetMultiSel(high);
  SetBackLoad(low);
  SetEnable(low);
  Send4(0,0);     /* send f0 abit=0 intg_rd --> 0 */
  Send4(1,0);     /* send f1 abit=0 itr --> 0 */
  Send4(4,0);     /* send f4 abit=0 tpe --> 0 */
  Send4(5,0);     /* send f5 abit=0 lcal_enable --> 0 */
  /* note: lcal_enable is not used on the prototype 3in1 cards */
  Send12(2,0,0);  /* send f2 abit=0 data=0 s1=s2=s3=s4 --> 0 */
  Send4(3,0);     /* send f3 abit=0 mse --> 0 */
  Send12(6,1,0);  /* send f6 abit=1 data=0 dac fine --> 0 */
  Send12(6,0,0);  /* send f6 abit=0 data=0 dac coarse --> 0 */
}

/*----------------------------------------*/
/*    read back the 3in1 internal state   */
/*       s1 = (sval>>3) & 1;              */
/*       s2 = (sval>>4) & 1;              */
/*       s3 = (sval>>5) & 1;              */
/*       s4 = (sval>>6) & 1;              */
/*       itr = (sval>>7) & 1;             */
/*       ire = (sval>>8) & 1;             */
/*       mse = (sval>>9) & 1;             */
/*       tpe = (sval>>10) & 1;            */
/*       lcal = (sval>>11) & 1;           */
/*----------------------------------------*/
unsigned short A3in1Controller::ReadStatus() {
  unsigned short rbck;
  SetBackLoad(high);
  SetBackLoad(low);
  SetRXW(high);
  for (int i = 0; i < 12; ++i) {
    Clock_ctrl();  /* now clock the controller shift register once */
    SetClock(high);  /* clock the 3in1 shift register once */
    SetClock(low);
  }
  // compliment since data_out inverted in 3in1 wiring mistake
  return ~GetShift();
}

void A3in1Controller::SetDrawerTimer(int section, int time) {
  unsigned short card;
  SetRXW(low);
  switch (section) {
  case 1:
    card = 56;
    break;
  case 2:
    card = 58;
    break;
  case 3:
    card = 60;
    break;
  case 4: 
    card = 63;
    break;
  }
  SelectCard(card);
  Write3in1Shift(8, time << 4);
  SetEnable(high);
  SetEnable(low);
  SelectCard(card + 1);
  SetEnable(high);
  SetEnable(low);
}

void A3in1Controller::SetDac(int dacvalue) {
  Send12(6, 0, dacvalue & 0xff);   /* load low 8 bits */
  Send12(6, 1, dacvalue >> 8);   /* load high 2 bits */
}

void A3in1Controller::SetCapacitor(CapType type) {
  if (type == small) {
    capacitance = 5.2;
    Send4(4, 1); // Enable small capacitor
    Send4(5, 0); // Disable large capacitor
  } else {
    capacitance = 100.0;
    Send4(4, 0); // Disable small capacitor
    Send4(5, 1); // Enable large capacitor
  }
}

double A3in1Controller::Inject(double charge) {
  int dacvalue = (int)((charge * 1023) / (8 * capacitance));
  if (dacvalue > 1023) dacvalue = 1023;
  SetDac(dacvalue);
  ResetTimer();
  SetTP(high); // Charge up capacitor
  timespec rqtp = { 0, 25000 };
  timespec rmtp;
  nanosleep(&rqtp, &rmtp); // Wait 25us for capacitor to charge
  StartTimer();
  return capacitance * (8 * dacvalue / 1023);
}

bool A3in1Controller::TestCard(int card) {
  SelectCard(card);
  Write3in1Shift(12, 0x555);
  if ((Read3in1Shift() & 0xfff) != 0x555) {
    return false;
  }
  Write3in1Shift(12, 0xaaa);
  if ((Read3in1Shift() & 0xfff) != 0xaaa) {
    return false;
  }
  return true;
}
