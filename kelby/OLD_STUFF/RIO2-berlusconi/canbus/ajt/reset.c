/************************************************************
 reset.c  -- resets the TIP 810 for use on a CANBUS of VME crates

   1) Switches the reset bit in the control register high.
   2) Sets the bit timing registers (BTR's) to run the CAN bus
      at 1 Mbaud.
       BTR0:
	Synchronization Jump Width (SJW) = 0
	Baud Rate Prescaler (BRP) = 0
       BTR1:
	Sampling Rate (SAM) = LOW
	Time Segment 2 (TSEG2) = 1
	Time Segment 1 (TSEG1) = 4
   3) Sets the Output Control Register (OCR) to 0xDA, as instructed by 
	the WIENER CANbus Interface User's Manual.
   4) Sets the Reset Request bit back to 0.  Enables Interrupts.

   Coded by: A.J. Tolland.  Last modified:  3/32/1998
   ajt@hep.uchicago.edu     
*/


#include <stdio.h>

main() {

   unsigned char *adr;
   int k;

   k = 0xdffe6001; /* CONTROL REGISTER */
   adr = (unsigned char*) k;
   *adr = 0x01;  /*RESET HIGH*/
   
   k = 0xdffe600d; /* BTR0 */
   adr = (unsigned char*) k;
   *adr = 0x00;  

   k = 0xdffe600f; /* BTR1 */
   adr = (unsigned char*) k;
   *adr = 0x14;  /*BTR 1*/

   k = 0xdffe6011;  /* OCR */
   adr = (unsigned char*) k;
   *adr = 0xda;
 
   k = 0xdffe6001;  /* COMMAND REGISTER */
   adr = (unsigned char*) k;
   *adr = 0x0e;  /* Reset Request low.  Enable interrupts */
}

 
