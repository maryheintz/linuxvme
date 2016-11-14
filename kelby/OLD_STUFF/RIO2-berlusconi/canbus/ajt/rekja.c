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

   unsigned char *adr,back;

   int k;

   k = 0xdffe6001; /* CONTROL REGISTER */
   adr = (unsigned char*) k;
   *adr = 0x01;  /*RESET HIGH*/
   back = *adr;
   printf(" after 0x01 dffe6001  back=%x\n",back);

   k = 0xdffe600d; /* BTR0 */
   adr = (unsigned char*) k;
   *adr = 0x00;  
   back = *adr;
   printf(" after 0x00 dffe600d  back=%x\n",back);


   k = 0xdffe600f; /* BTR1 */
   adr = (unsigned char*) k;
   *adr = 0x14;  /*BTR 1*/
   back = *adr;
   printf(" after 0x14 dffe600f  back=%x\n",back);


   k = 0xdffe6011;  /* OCR */
   adr = (unsigned char*) k;
   *adr = 0xda;
   back = *adr;
   printf(" after 0xda dffe6011  back=%x\n",back);


   k = 0xdffe6001;  /* COMMAND REGISTER */
   adr = (unsigned char*) k;
   *adr = 0x0e;  /* Reset Request low.  Enable interrupts */
   back = *adr;
   printf(" after 0x0e dffe6001  back=%x\n",back);

}

 
