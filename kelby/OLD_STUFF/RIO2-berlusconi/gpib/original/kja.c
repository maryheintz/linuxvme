/************************************************************************
 *
 * Engineering Software Package Example Application Program
 * Rev A.1
 * (c) Copyright 1984, 1992 National Instruments
 * All rights reserved.
 *
 * This program is an example application program which
 * controls a hypothetical digital voltmeter.  It must
 * be compiled and linked to the ESP488 module in order
 * to run.  The primary function of this program is to
 * illustrate the use of the GPIB functions of the ESP488
 * package.  The codes and formats used for communicating
 * with the voltmeter are representative of such devices
 * available on the market but are not necessarily correct
 * for any particular voltmeter.
 *
 * The interface is assumed to be at GPIB address 0
 * (talk address 0100 or '@', and listen address 040 or
 * ' '), and the voltmeter at GPIB address 1 (talk address
 * 0101 or 'A', and listen address 043 or '#').
 *
 ************************************************************************/

#include "ugpib.h"		/* user GPIB application include file	*/

char result[320000];	/* measurement from voltmeter (ASCII) */

main()
{
	char stb;		/* serial poll status byte */
	double v;		/* measurement in floating point format	*/

	/* Initialize the interface */
        printf("calling ibonl\n");
	ibonl(1);

        printf("calling ibsic\n");
	/* Send IFC to clear the GPIB */
	ibsic();

        printf("calling ibsre\n");
	/* Set REN to prepare for placing the voltmeter in remote mode */
	ibsre(1);

	/* Send the LLO command followed by the voltmeter listen address,
	 * the DCL command, and the interface board talk address to lockout
	 * the front panel, place the voltmeter under remote control, clear
	 * the internal voltmeter functions, and prepare to send it data.
	 */
	/* Conduct a serial poll to determine if an error occurred by
	 * sending the UNL, UNT, followed by the interface
	 * listen address and the voltmeter talk address.
	 */
        printf("calling ibcmd(?_ C, 4)\n");
        ibcmd("?_ \101", 4);
	/* Read the status byte from the scope: if it is 0xC0 there is
	 * a valid reading available, otherwise a fault condition exists.
	 */
/*	ibrd(&stb, 1);
        printf("stb=%x\n",(int)stb);
	if (stb != (char)(0xC0))
		dvmerr();   */


	/* Read the measurement from the scope (it is still addressed
	 * to talk and the interface is still addressed to listen).
	 */
        printf("calling ibrd\n");
	ibrd(result, 320000);
        printf("ibrd return ibcnt=%d\n",ibcnt);
	result[ibcnt] = 0;		/* make it a null-terminated string */

        printf("%c%c%c%c%c%c%c%c%c%c\n",
	  result[0],result[1],result[2],result[3],result[4],
          result[5],result[6],result[7],result[8],result[9]);
	/* Terminate GPIB activity by clearing the voltmeter and the GPIB */
	ibcmd("\024", 1);
	ibsic();

	/* Disable the interface */
	ibonl(0);
}


dvmerr()
{
	printf(" Print out a DVM error message here\n");
	exit();
}
