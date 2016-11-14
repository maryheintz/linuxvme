// BOOTSTRAP LOADER FOR IRI-2000
// JANUARY 2000

// CONFIGURE CAN AT 500KBPS
// CONFIGURE BUFFERS 0 FOR RECEPTION OF PC COMMANDS WITH ID = 1
// CONFIGURE BUFFER 13 FOR TRANSMISSION OF REPORTS TO PC   WITH ID = 14
// CONFIGURE BUFFERS 1 TO 12 FOR SAMPLING DATA TRANSFER    WITH ID = 2 To 13
// AT 20MHZ CPU CLOCK, Ntimer = 0xBEE4 for 100Hz DAQ

// CARD SELECT IS WRITTEN IN AN EXTERN ASSEMBLER MODULE

// STATE 1: WAIT FOR INIT COMMAND (TESTED)

//	INIT 1 -> CALL FB SUBROUTINE      (TESTED)
//	INIT 2 -> CALL ISP SUBROUTINE     (TESTED)

// STATE ISP
//	WAIT FOR ERASE COMMAND               (TESTED)
//      REPORT ON ERASE: SUCCESS OR FAILURE  (TESTED)
//
//	WAIT FOR PROGRAM COMMAND             (TESTED)
//	REPORT ON PROGRAM: SUCCESS OR FAILURE(TESTED)
//
//	WAIT FOR READBACK COMMAND	     (TESTED)
//	REPORT ON READBACK: DATA READ BACK   (TESTED)

// STATE FB
//	SETxGET TIMER	(TESTED)
//	SETxGET NPMT	(TESTED)
//	SETxGET MAXSCAN (TESTED)
// 	SETxGET DACSET  (TESTED)
//	SETxGET PMTLIST (TESTED)
//	SETxGET DELAY   (TESTED)
//	FUNC	REQUEST (TESTED)
//	FUNC 	CANSET	(TESTED)
//	FUNC 	CANGET  (UNTESTED)
//  	FUNC	CONVERT (TESTED)
//  	FUNC    TRIGGER (TESTED)
//  	FUNC    TIMER1  (TESTED)
//  	FUNC    START   (TESTED)
//  	FUNC    STOP    (TESTED)

// P1 ACCESSES 3in1 CONTROL LINES FOR PROTOTYPE ONLY
//	P1.0	CLK3in1
//	P1.1	3in1TOADC
//	P1.2	3in1TOMB
//	P1.3	LATCH3in1


// P3 ACCESSES OTHER IO LINES FOR PROTOTYPE ONLY
// P3.5    BUSY*

#define VERSION	1

#define VOID	0
#define INIT	1
#define ERASE	2
#define WRITE	3
#define READ	4
#define TIMER   5
#define NPMT    6
#define MAXSCAN 7
#define PMTLIST 8
#define DACSET  9
#define DELAY   10

#define CANSET  11
#define CANGET  12
#define REQUEST 14
#define CONVERT 15
#define TRIGGER 16
#define START   17
#define STOP	18

#define RESET   50
#define ACK	51

#define GO_FB	101
#define GO_ISP  102
#define ERASE_SUCCESS 103
#define ERASE_FAILURE 104
#define WRITE_SUCCESS 105
#define WRITE_FAILURE 106

#define ST_NOTINITIALIZED    -1
#define ST_INITIALIZED	      1
#define ST_ISP		      2

#pragma DEBUG OBJECTEXTEND CODE

#include <stdio.h>
#include <reg51.h>
#include "SAE81C90.H"
#include <stdlib.h>
#include <intrins.h>

void iri_daq(void);	// Flash B subroutine declaration
void acknowledge(void);              
extern void asmcardsel(unsigned int card);

xdata unsigned char s1[0x4000] _at_ 0x0000;
xdata unsigned char s2[0x4000] _at_ 0x4000;
xdata unsigned char dacreg     _at_ 0xC000;
xdata unsigned char adcreg[2]  _at_ 0xD000;
xdata unsigned char divH            _at_ 0x8002;
xdata unsigned char divL 	    _at_ 0x8003;
 data unsigned int  state;
 data unsigned char command;
 data unsigned char error;
 data unsigned int  adr;
 data unsigned char npmt;	    
 data unsigned char param1;
 data unsigned char param2;
xdata unsigned char param3 	    _at_ 0x800E;
xdata unsigned char param4 	    _at_ 0x800F;
xdata unsigned char param5 	    _at_ 0x8010;
xdata unsigned char param6	    _at_ 0x8011;
xdata unsigned char param7	    _at_ 0x8012;
 data unsigned int  maxscan;
xdata unsigned int  pmt[48]	    _at_ 0x8015;
xdata unsigned char dacset	    _at_ 0x8200;
 data unsigned int  delay;	     
 data unsigned char i,j;
 data unsigned int  k;
 data unsigned char Base;
 data unsigned char result[2];
 data unsigned char tr1, tr2;
xdata unsigned int  tr;
 data unsigned int  scan;
 data unsigned int  card;
 data unsigned int  shift;
 data unsigned char n;  
 data unsigned char d0;
 data unsigned char d1;

void main(void)
{

 /* Configure the board */

  P3 = 0xF7;   /* Set P3 ports to enable 
	       	P3.7 = /RD 
	       	P3.6 = /WR
		P3.5 = /BUSY
	        P3.4 = /CANRST
		P3.2 = /INT0  
		P3.1 = /SLOWMODE
		P3.0 = FA_DATA  */

  P1 = 0x02;	// RESET 3in1 PORT with P1.1 configured as input

  Base = 0x00;	// FOR PROTOTYPE ONLY

 // Initial state of the device

  state   = ST_NOTINITIALIZED;	
  command = VOID;

 /* Configure the CAN port for 500KBPS */

  CAN.MOD  = 0x03;	// Hard software reset
  CAN.CTRL = 0x00;      // Enable sinitializing mode and resets 
  CAN.INT  = 0x00;	// Resets all interrupts sources

  CAN.BRP = 0x00;	// Max baud rate
  CAN.BL1 = 0X34;	// TS1=3 TS2=4
  CAN.BL2 = 0X41;	// SJW=1
  
  CAN.RRR1 = 0X00;	// CLEAR RECEIVE REGISTERS
  CAN.RRR2 = 0X00;
  CAN.IMSK = 0X01;	// ENABLE IRQ ON RECEIVE 
  CAN.RIMR1 = 0X0F;
  CAN.RIMR2 = 0X00;	// RECEPTION IN BUFFERS 0 TO 3 GENERATES /INT0

  CAN.CC = 0X80;
  CAN.CC = 0X00;	// 10MHZ SHOULD BE AT FCLKOUT

  CAN.DESCRIPTOR[0]  = (((unsigned int)Base) << 11) | 0x0028;	// MSG 1 IN BUFFER 0 WITH 8 DATA BYTES
  CAN.DESCRIPTOR[1]  = (((unsigned int)Base) << 11) | 0x0048; 	// MSG 2 to 13 
  CAN.DESCRIPTOR[2]  = (((unsigned int)Base) << 11) | 0x0068;     //  IN BUFFERS 1 TO 12
  CAN.DESCRIPTOR[3]  = (((unsigned int)Base) << 11) | 0x0088;     //  CONTAIN SAMPLING DATA
  CAN.DESCRIPTOR[4]  = (((unsigned int)Base) << 11) | 0x00A8;
  CAN.DESCRIPTOR[5]  = (((unsigned int)Base) << 11) | 0x00C8;
  CAN.DESCRIPTOR[6]  = (((unsigned int)Base) << 11) | 0x00E8;
  CAN.DESCRIPTOR[7]  = (((unsigned int)Base) << 11) | 0x0108;
  CAN.DESCRIPTOR[8]  = (((unsigned int)Base) << 11) | 0x0128;
  CAN.DESCRIPTOR[9]  = (((unsigned int)Base) << 11) | 0x0148;
  CAN.DESCRIPTOR[10] = (((unsigned int)Base) << 11) | 0x0168;
  CAN.DESCRIPTOR[11] = (((unsigned int)Base) << 11) | 0x0188;
  CAN.DESCRIPTOR[12] = (((unsigned int)Base) << 11) | 0x01A8;
  CAN.DESCRIPTOR[13] = (((unsigned int)Base) << 11) | 0x01C8;  // MSG 14 in BUFFER 13 WITH 8 DATA BYTES

  CAN.OC = 0X18;	// OUTPUT CONTROL SET TO DEFAULT
  CAN.MOD = 0X00;	// ENDS INITIALIZATION PROCEDURE

  /* Enable interrupt calls on /INT0 */

  IT0 = 0;
  EX0 = 1;		// ENABLE EXTERNAL INTERRUPT CALL 0
  EA  = 1;		// DISABLE IRQ WIDE MASK
  IP = IP | 0X01;	// HIGH PRIORITY ON /INT0

// Global loop

while(1)
{

  // Wait for command

  while (command == VOID);

  // Process INIT command
            
  if (command == INIT)
  {

	// Change state and acknowledge to host

        if ((param1 == GO_FB)  || (param1 == GO_ISP))
	{ 

       	 CAN.DESCRIPTOR[0] = 0X0028;	// MSG 1 IN BUFFER 0 WITH 8 DATA BYTE

	 MSG0[1] = 'R';
	 MSG0[2] = 'I';
	 MSG0[3] = '2';
	 MSG0[4] = '0';
	 MSG0[5] = '0';
	 MSG0[6] = '0';
	 MSG0[7] = VERSION;
       	 MSG0[0] = 'I';

         CAN.TRS1 = 0X01;		// TRANSMITS MESSAGE 1 IN BUFFER 0
         while ( (CAN.TRS1 & 0x01) == 0x01); // WAIT FOR COMPLETION
	}

	// Section to launch Flash B application

  	if ( param1 == GO_FB ) 
	{
 
	 // Acknowledge command must be implemented in flash b

	 iri_daq();     // Call the Flash B subroutine
	
	}

	// Section to reprogram Flash B application

	else if ( param1 == GO_ISP )
 	{

	 // Acknowledge command

         CAN.DESCRIPTOR[0] = 0x0028;
         MSG0[1] = GO_ISP;
	 MSG0[0] = INIT;
	 CAN.TRS1 = 0x01;
         while ( (CAN.TRS1 & 0x01) == 0x01); // WAIT FOR COMPLETION

	 state = ST_ISP;

	}
        command = VOID;
  }

  // Process ERASE S2 command, param1 and param2 contain the wait timer setting

  if ( (command == ERASE) && (state == ST_ISP) )
  {

    P3 = 0xF5;	// SLOW DOWN

    // Erase embedded algo

    s2[0x1555] = 0xAA;
    s1[0x2AAA] = 0x55;
    s2[0x1555] = 0x80;
    s2[0x1555] = 0xAA;
    s1[0x2AAA] = 0x55;
    s2[0]      = 0x30;

    P3 = 0xF7;  // SPEED UP

 CAN.DESCRIPTOR[0] = 0x0028;
 MSG0[0] = ACK;
 CAN.TRS1 = 0x01;
 while ( (CAN.TRS1 & 0x01) == 0x01); // WAIT FOR COMPLETION

    command  = VOID;

  }

  // Process WRITE command, param1 and param2 contain the adress, param3 contain the data

  if ( (command == WRITE) && (state == ST_ISP) )
  {

    // Calculate adress and data

    adr = ( (((unsigned int)param1)<<8) + (unsigned int)param2) - 0x4000;
    d0 = param3;

    P3 = 0xF5;

    // Byte program embedded algo

    s2[0x1555] = 0xAA;
    s1[0x2AAA] = 0x55;
    s2[0x1555] = 0xA0;
    s2[adr]    = d0;

    P3 = 0xF7;
  

 CAN.DESCRIPTOR[0] = 0x0028;
 MSG0[0] = ACK;
 CAN.TRS1 = 0x01;
 while ( (CAN.TRS1 & 0x01) == 0x01); // WAIT FOR COMPLETION

    command = VOID;

  }

  // Process READ command, param1 and param2 contain the adress

  if ( (command == READ) && (state == ST_ISP) )
  {

    // Calculate adress

    adr = ( (((unsigned int)param1)<<8) + (unsigned int)param2) - 0x4000;

    // Report on readback command

    CAN.DESCRIPTOR[0] = 0x0028;
    MSG0[1] = param1;
    MSG0[2] = param2;
    MSG0[3] = s2[adr];
    MSG0[0] = READ;
    CAN.TRS1 = 0x01;
    while ( (CAN.TRS1 & 0x01) == 0x01); // WAIT FOR COMPLETION

    command = VOID;
  }

}
}

void CAN_COMMAND (void) interrupt 0
{

 unsigned char i, rcv_dada[8];
 unsigned char shadowmemdump; 

	if (( CAN.INT & 0x01) == 0x01)
 	{
  		if ((CAN.RRR1 & 0x01 ) == 0X01)	// MSG 1 IN BUFFER 0
  		{
    		shadowmemdump = MSG0[7];
    	   	for (i=0;i<8;i++)
		{
			rcv_dada[i] = MSG0[i];
		}
		command = rcv_dada[0];
		param1  = rcv_dada[1]; 
    		param2  = rcv_dada[2];
		param3  = rcv_dada[3];
		param4  = rcv_dada[4];
		param5  = rcv_dada[5];
		param6  = rcv_dada[6];
		param7  = rcv_dada[7];
    
    		CAN.RRR1 = 0x00;		// RESETS THE RECEIVE READY REG
    		CAN.INT = 0x00;	         	// RESETS INTERRUPT REQUEST BITS
    		IE0 = 0;

    		CAN.DESCRIPTOR[0] = 0X0028;	// RESET MSG 1 IN BUFFER 0 TO RECEIVE MORE DATA  
 		}
	}

}

void timer1(void) interrupt 3
{
	TH1 = divH;
	TL1 = divL;
	command = TRIGGER;
}

void acknowledge(void)
{

 CAN.DESCRIPTOR[0] = 0x0028;
 MSG0[0] = ACK;
 CAN.TRS1 = 0x01;
 while ( (CAN.TRS1 & 0x01) == 0x01); // WAIT FOR COMPLETION

 return;
}

void iri_daq(void)
{

 // Acknowledge startup of Flash B code

 CAN.DESCRIPTOR[0] = 0x0028;
 MSG0[1] = GO_FB;
 MSG0[0] = INIT;
 CAN.TRS1 = 0x01;
 while ( (CAN.TRS1 & 0x01) == 0x01); // WAIT FOR COMPLETION
 state = ST_INITIALIZED;
 command = VOID;

 // Default Timer setting for 100Hz

 divH = 0xBE;
 divL = 0xE4;
 TMOD = 0x10;	 	// TIMER 1: GATE OFF MODE 1
 TH1  = divH;
 TL1  = divL;
 ET1  = 0;		// DISABLE TIMER 1 OVERFLOW IRQ SOURCE
 EA   = 1;		// DISABLE IRQ WIDE MASK
 PT1  = 0;		// LOW PRIORITY TIMER 1 IRQ SOURCE
 TR1  = 0;		// DO NOT START THE TIMER YET

 scan = 0;		// Reset Scan counter

 // Can return to FA code upon reception of RESET command

 while (command != RESET)
 {

 // Start application here

  command = VOID;
  while (command == VOID);	     // WAIT FOR NEW COMMAND
 
  switch(command)
  {
   case TIMER:
	divH = param1;
	divL = param2;
	TH1  = divH;
	TL1  = divL;
	acknowledge();
	break;

   case NPMT:
	npmt = param1;
	tr1 = 0;
	tr2 = 0;
	tr  = 0;
	param1 = param1 -1;
	if (npmt > 0)
	{
		param1 = param1 >> 2;	// divide by 4
		param1 = param1 + 1;	// for ceil function
		for (i=0;i<param1;i++)
		{
		 tr = (tr << 1) & 0x0001;
		 tr++;
		}
	}
	tr1 = (unsigned char)(tr & 0x00FE);
	tr2 = (unsigned char)( (tr>>8) & 0x001F);
	acknowledge();
	break;

   case MAXSCAN:
	maxscan = (((unsigned int)param1)<<8) + ((unsigned int)param2);
	acknowledge();
	break;

   case DACSET:
	dacset = param1;
        dacreg = dacset;
	acknowledge();
	break;

   case DELAY:
	delay = (((unsigned int)param1)<<8) + ((unsigned int)param2);
	acknowledge();
	break;

   case PMTLIST:
	// param1 = pos
	// param2 = zone
	// param3 = sector
	// param4 = card

	pmt[param1]=( (((unsigned int)param2)<<13) + (((unsigned int)param3)<<6) + ((unsigned int)param4) );
	acknowledge();
	break;

   case CANSET:
	// INLINE Data bits shifted through P1.2

	shift = ( ((unsigned int)param2)<<8 ) + ((unsigned int)param1);
        asmcardsel(shift);
	acknowledge();
	break;

   case CANGET:

	CAN.DESCRIPTOR[13] = 0x01C8;
	param1 = 0;
	param2 = 0;
	P1 = P1 | 0x08   | 0x02; // LATCH HIGH with P1.1 as input
	P1 = (P1 & 0xF7) | 0x02; // LATCH LOW
	for (i=0;i<8;i++)
	{
		param1 = param1 << 1;
         	param1 = param1 | ( (P1 >> 1)&0x01); // LOAD DATA BIT
		P1 = P1 | 0x01   | 0x02; // CLK HIGH with P1.1 as input
		P1 = (P1 & 0xFE) | 0x02; // CLK LOW		
	}
	for (i=0;i<8;i++)
	{
		param2 = param2 << 1;
         	param2 = param2 | ( (P1 >> 1)&0x01); // LOAD DATA BIT
		P1 = P1 | 0x01   | 0x02; // CLK HIGH with P1.1 as input
		P1 = (P1 & 0xFE) | 0x02; // CLK LOW		
	}
	MSG13[1] = param1;  // High Byte
	MSG13[2] = param2;  // Low Byte
	MSG13[0] = CANGET; // First byte indicates we are reporting on a CANGET
 	CAN.TRS2 = 0x20;
   	while ( (CAN.TRS2 & 0x20) == 0x20); // WAIT FOR COMPLETION
	break;

   case CONVERT:

	CAN.DESCRIPTOR[13] = 0x01C8;
    	MSG13[1] = param1;
	MSG13[2] = param2;

    // Card Selection

	shift = ( ((unsigned int)param2)<<8 ) + ((unsigned int)param1);
        asmcardsel(shift);
	for(k=0;k<delay;k++);

	// Trigger by reading previous low byte (dummy data)

	result[0] = adcreg[0];	

	// Wait until BUSY* goes high

	while ( (P3 & 0x20) == 0x00);

	// Recover result from adcregs

	MSG13[3] = adcreg[1];  // High byte first
	MSG13[4] = adcreg[0];  // Low byte then, also retriggers but not used

	// Dump shadow memory and transmit

	MSG13[0] = CONVERT;
 	CAN.TRS2 = 0x20;
    	while ( (CAN.TRS2 & 0x20) == 0x20); // WAIT FOR COMPLETION
	break;

   case TRIGGER:

     if (scan++ < maxscan)
     {

	// Set up the first descriptor of msg 1
	CAN.DESCRIPTOR[1] = 0x0028;
	// First card select
 	asmcardsel(pmt[0]);
	// Wait for stabilization
	for (k=0;k<delay; k++);
	// Trigger first time
	result[0] = adcreg[0];
	i=0;

	while (i<npmt)
	{
      
	   while ( (P3 & 0x20) == 0x00); // Wait for BUSY* to be high
	   // Recover last conversion's high and low byte + retrigger
	   // Data is saved in the temp d1 and d0 regs to avoid waiting for EOT

	   result[1] = adcreg[1];             // Still transmitting
	   i++;
 	   asmcardsel(pmt[i]);
	   for(k=0;k<delay;k++);
	   result[0] = adcreg[0];             // Still transmitting

	   if (i<npmt)
	   {
		while ( (P3 & 0x20) == 0x00); // Wait for BUSY* to be high
		d0 = adcreg[1];               // Still transmitting	
		i++;
	        asmcardsel(pmt[i]);
 		for (k=0;k<delay; k++);
		while(CAN.TRS1 != 0x00); // just in case it's too fast :-) 
		MSG1[3] = adcreg[0];
   		MSG1[2] = d0;
	   }
	   if (i<npmt)
	   {
		while ( (P3 & 0x20) == 0x00); // Wait for BUSY* to be high
		MSG1[4] = adcreg[1];             	
		i++;
	        asmcardsel(pmt[i]);
 		for (k=0;k<delay; k++);
                MSG1[5] = adcreg[0];
	   }	   
	   if (i<npmt)
	   {
		while ( (P3 & 0x20) == 0x00); // Wait for BUSY* to be high
		MSG1[6] = adcreg[1];             	
		i++;
	        asmcardsel(pmt[i]);
 		for (k=0;k<delay; k++);
                MSG1[7] = adcreg[0];
	   }
		  
           CAN.DESCRIPTOR[1] += 0x0020;	
	   MSG1[1] = result[0];
	   MSG1[0] = result[1]; // Dump shadow memory
	   
            // Transmit the current msg 1
           CAN.TRS1 = 0x02;
	 }
	// Wait for the last transmission to complete
	while (CAN.TRS1 != 0x00);
     }
     else
     {
     	// stop the automatic triggers
     	ET1 = 0;
     	TR1 = 0;
     }
     break;

   case START:
	   scan = 0;
	   ET1  = 1;
	   TR1  = 1;
	   break;

   case STOP:
	   ET1  = 0;
	   TR1  = 0;
	   acknowledge();
           break;

   case REQUEST:
	CAN.DESCRIPTOR[13] = 0x01C8;
	MSG13[1] = param1;  // First parameter indicates what is returned

	switch(param1)
	{
        	case TIMER:
			MSG13[2] = divH;
			MSG13[3] = divL;
			break;
		case NPMT:
			MSG13[2] = npmt;
			break;
		case MAXSCAN:
			MSG13[2] = (unsigned char)((maxscan>>8) & 0x00FF);
		       	MSG13[3] = (unsigned char)(maxscan & 0x00FF);
		       	break;
		case DACSET:
			MSG13[2] = dacset;
			break;
		case DELAY:
			MSG13[2] = (unsigned char)((delay>>8) & 0x00FF);
			MSG13[3] = (unsigned char)( delay     & 0x00FF);
			break;
		case PMTLIST:
			MSG13[2] = param2;   // POS NUMBER
			MSG13[3] = ( (unsigned char)( (pmt[param2]>>13) &0x0007) );
			MSG13[4] = ( (unsigned char)( (pmt[param2]>>6 ) &0x007F) );
			MSG13[5] =   (unsigned char)( (pmt[param2]    ) &0x003F)  ;
			break;

	}
	
	MSG13[0] = REQUEST; // First byte indicates we are reporting on a REQUEST
 	CAN.TRS2 = 0x20;
        while ( (CAN.TRS2 & 0x20) == 0x20); // WAIT FOR COMPLETION
	break;
  }
 
 }
 return;
}


	
