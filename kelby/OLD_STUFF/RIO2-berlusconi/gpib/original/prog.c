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
 * ' '), and the voltmeter at GPIB address 3 (talk address
 * 0103 or 'C', and listen address 043 or '#').
 *
 ************************************************************************/

#include "ugpib.h"		/* user GPIB application include file	*/


main()
{
	char stb;		/* serial poll status byte */
	char result[32];	/* measurement from voltmeter (ASCII) */
	double v;		/* measurement in floating point format	*/

	/* Initialize the interface */
	ibonl(1);

	/* Send IFC to clear the GPIB */
	ibsic();

	/* Set REN to prepare for placing the voltmeter in remote mode */
	ibsre(1);

	/* Send the LLO command followed by the voltmeter listen address,
	 * the DCL command, and the interface board talk address to lockout
	 * the front panel, place the voltmeter under remote control, clear
	 * the internal voltmeter functions, and prepare to send it data.
	 */
	ibcmd("\021#\024@", 4);

	/* Send the function, range, and trigger source instructions */
	ibwrt("F3R7T3", 6);

	/* Send the GET command to trigger a measurement reading */
	ibcmd("\010", 1);

	/* Wait for voltmeter to set SRQ indicating measurement is ready */
	ibwait(SRQI);

	/* Conduct a serial poll to determine if an error occurred by
	 * sending the UNL, UNT, and SPE commands followed by the interface
	 * listen address and the voltmeter talk address.
	 */
	ibcmd("?_\030 C", 5);

	/* Read the status byte from the voltmeter: if it is 0xC0 there is
	 * a valid reading available, otherwise a fault condition exists.
	 */
	ibrd(&stb, 1);
	if (stb != (char)(0xC0))
		dvmerr();

	/* Terminate the serial poll by sending the SPD message */
	ibcmd("\031", 1);

	/* Read the measurement from the voltmeter (it is still addressed
	 * to talk and the interface is still addressed to listen).
	 */
	ibrd(result, 31);
	result[ibcnt] = 0;		/* make it a null-terminated string */

	/* Extract the measurement value and process it */
	sscanf(result, "%f", &v);
	process(v);

	/* Terminate GPIB activity by clearing the voltmeter and the GPIB */
	ibcmd("\024", 1);
	ibsic();

	/* Disable the interface */
	ibonl(0);
}


dvmerr()
{
	/* Print out a DVM error message here */
	exit();
}


process(v) double v;
{
	/* Process the DVM measurement here */
}

