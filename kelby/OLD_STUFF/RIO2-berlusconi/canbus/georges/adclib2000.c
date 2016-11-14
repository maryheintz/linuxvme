// iostream for file IO
// stdio.h for standard io
// nican.h only when using the PCI-CAN2 board from natinst

/* #include <iostream.h>
#include <conio.h>  */
#include <stdio.h>
#include <math.h>
#include "nican.h"
#include "adclib2000.h"

// ADC board Base adress 

long Base;

// PCI-CAN2 board handler

unsigned long ObjHandle;

// CAN communication functions declarations

long can_tx(int  id, char  dlc, char  rtr, char data[8]);
long can_rx(int *id, char *dlc, char *rtr, char data[8]);

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// ADC protocol communication functions
// Use the CAN communication function defined below


// Send the initialization command to the adc
// the action can be either
//		GO_ISP to enter the in system programmability mode
//		GO_FB  to start the firmware
// The in system programability mode allows to download custom firmwares before starting it.
// The adc indicates it is alive by returning a know version stream:
//		version[0..6]	= 'IRI2000'
//		version[7]		= version number

long adc_init(char action, char version[8])
{
	long status;
	long id;
	char dlc, rtr;
    char data[8];
	int i;

	// Send the init command
	data[0] = INIT;
	data[1] = action;
	status = can_tx(Base+1, 2, 0 , data);
	
	// Wait for the adc acknowledgement
    if (status == 0) status = can_rx(&id, &dlc, &rtr, version);
	
	// Wait for the transition indicator
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	
	// If Status equals 0 command was processed -> return 0
	// Otherwise there was a problem -> return -1 

	return status;
}


// Send the erase embedded algorithm command
// Used in ISP mode to erase the Flash B before 
// loading a new custom firmware

long adc_erase(void)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	data[0] = ERASE;

	// send the erase command
	status = can_tx(Base+1, 1, 0, data);

	// wait for the adc acknowledgment
	// if we don't get ACK then status is set to -1
  	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	//if (status == 0) if (data[0] != ACK) status = -1;

	// If Status equals 0 command was processed -> return 0
	// Otherwise there was a problem -> return -1 
	
	return status;

}

// Blank check algorithm
// Used in ISP mode to check the Flash B was properly erased
// MUST be used before loading a new custom firmware
// In case of error, *adr will contain the memory location that caused the error

long adc_blankcheck(unsigned int *adr)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	int i;
	int checkadr;
	int error;

	char dat;

	i= 0;
	error = 0;
	
	do 
	{
		// Calculates the memory location to check and read it
		
		*adr  = 0x4000 + i;
		status = adc_readmem(i+0x4000, &dat);
	
		// Check the returned data
		// If we don't get back a 0xFF data
		//		then error is set to 1

		if (status == 0) if ((((int)dat)&0xFF) != 0xFF) error = 1;
			
		// Increment the adress pointer
		i++;

		// Close the loop
		// Exit if there was a bad communication or a bad returned data
	} while( (status == 0) && (i<0x4000) && (error == 0) );
			
	// Return values 
	if (error != 0) return +1;			// Blank check error
	if (status != 0) return status;		// Communication error
	return 0;							// blank check success

}

// Program a memory location in the flash memories.

long adc_writemem( int adr, char data)
{
	long status;
	long id;
	char dlc, rtr;
	char dat[8];

	// Send the write command
	dat[0] = WRITE;
	dat[1] = (unsigned char)( (adr >> 8) &0x00FF);
	dat[2] = (unsigned char)(  adr       &0x00FF);
	dat[3] = data;
	status = can_tx(Base + 1, 4, 0, dat);

	// Wait for the acknowledgement
	// if we don't get ACK then status is set to -1
	if (status == 0) status = can_rx(&id, &dlc, &rtr, dat);
	if (status == 0) if (dat[0] != ACK) status = -1;

	// If the command was written and acknowledged properly, return 0
	// Otherwise return -1

	return status;
}

// Read a memory location in the flash memories

long  adc_readmem( unsigned int adr, char *data)
{
	long status;
	unsigned int var1, var2;
	unsigned int checkadr;
	long id;
	char dlc, rtr;
	char dat[8];

	// Send the read command
	dat[0] = READ;
	dat[1] = (unsigned char)( (adr >> 8)&0x00FF);
	dat[2] = (unsigned char)(  adr      &0x00FF);
	status  = can_tx(Base+1, 3, 0, dat);

	// Get back the result
	if (status == 0) status = can_rx(&id, &dlc, &rtr, dat);
	
	// If we don't get back a READ command
	// If we don't get back the right adress
	//		then error is set to 1
	if (status == 0)
	{
		var1 = ((unsigned int)dat[1]) & 0x00FF;
		var2 = ((unsigned int)dat[2]) & 0x00FF;
		checkadr = (var1 << 8) + var2;
		if ((dat[0] != READ) || (checkadr != adr) ) status = 1;
	}
	
	if (status == 1) printf(" adr %x versus checkadr %x\n", adr, checkadr);

	// Set up the *data pointer
	*data = dat[3];

	// Returned values:
	//		0 if sucecssuful operation, *data contains the returned  data
	//		1 if the returned adress or the returned command were wrong
	//		-1 if a communication error occured

	return status;
}

// Set up the trigger rate 

long adc_rate(unsigned short timer)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	// Send the command
	data[0] = TIMER;
	data[1] = (char)( (timer >> 8) &0x00FF );
	data[2] = (char)(  timer       &0x00FF );
	status  = can_tx(Base+1, 3, 0, data);

	// Wait for the acknowledgement
	// if we don't get ACK then status is set to -1
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	if (status == 0) if (data[0] != ACK) status = -1;

	// If the command was written and acknowledged properly, return 0
	// Otherwise return -1

	return status;	
}

// Retrieve the trigger rate

long adc_get_rate(unsigned short *timer)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];
	unsigned short var1, var2;

	// Send the read command
	data[0] = REQUEST;
	data[1] = TIMER;
	status  = can_tx(Base+1, 2, 0, data);
	// Get back the result
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	// If we don't get back a REQUEST command
	// If we don't get back the right parameter code
	//		then error is set to 1
	if (status == 0) if ((data[0] != REQUEST) || (data[1] != TIMER)) status = 1;

	// Format the returned parameter

	var1 = ((unsigned short)data[2])&0x00FF;
	var2 = ((unsigned short)data[3])&0x00FF;
	
	*timer = (var1<<8) + var2;

	// Returns 0 if all was OK
	// Returns 1 if the command or code parameter that were received back are wrong
	// Returns -1 if there was a communication error

	return status;

}

// Set up the number of pmts of the pmt list

long adc_npmt(char npmt)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	// Send the command
	data[0] = NPMT;
	data[1] = npmt;
	status  = can_tx(Base+1, 2, 0, data);

	// Wait for the acknowledgement
	// if we don't get ACK then status is set to -1
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	if (status == 0) if (data[0] != ACK) status = -1;

	// If the command was written and acknowledged properly, return 0
	// Otherwise return -1

	return status;	
}

// Retrieve the number of pmts in the pmt list

long adc_get_npmt(char *npmt)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	// Send the read command
	data[0] = REQUEST;
	data[1] = NPMT;
	status  = can_tx(Base+1, 2, 0, data);
	// Get back the result
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	// If we don't get back a REQUEST command
	// If we don't get back the right parameter code
	//		then error is set to 1
	if (status == 0) if ((data[0] != REQUEST) || (data[1] != TIMER)) status = 1;
	
	// Format the returned parameter
	*npmt = data[2];

	// Returns 0 if all was OK
	// Returns 1 if the command or code parameter that were received back are wrong
	// Returns -1 if there was a communication error

	return status;
}

// Set the maximum number of scans to be performed before doing an automatic stop

long adc_maxscans(unsigned short maxscans)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	// Send the command
	data[0] = MAXSCAN;
	data[1] = (unsigned char)( (maxscans >> 8) &0x00FF );
	data[2] = (unsigned char)(  maxscans       &0x00FF );
	status  = can_tx(Base+1, 3, 0, data);

	// Wait for the acknowledgement
	// if we don't get ACK then status is set to -1
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	if (status == 0) if (data[0] != ACK) status = -1;

	// If the command was written and acknowledged properly, return 0
	// Otherwise return -1

	return status;
}

// Retrieve the maxcsans setting

long adc_get_maxscans(unsigned short *maxscans)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];
	unsigned short var1, var2;

	// Send the read command
	data[0] = REQUEST;
	data[1] = MAXSCAN;
	status  = can_tx(Base+1, 2, 0, data);
	// Get back the result
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	// If we don't get back a REQUEST command
	// If we don't get back the right parameter code
	//		then error is set to 1
	if (status == 0) if ((data[0] != REQUEST) || (data[1] != MAXSCAN)) status = 1;

	// Format the returned parameter
	var1 = ((unsigned short)data[2])&0x00FF;
	var2 = ((unsigned short)data[3])&0x00FF;
	*maxscans = (var1 << 8) + var2;

	// Returns 0 if all was OK
	// Returns 1 if the command or code parameter that were received back are wrong
	// Returns -1 if there was a communication error

	return status;
}

// Configure one position of the pmtlist.
//	Parameters passed are the position (0 to 47)
//						  the zone, sector and card for that position

long adc_pmtlist(char pos, char zone, char sector, char card)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	// Send the command
	data[0] = PMTLIST;
	data[1] = pos;
	data[2] = zone;
	data[3] = sector;
	data[4] = card;
	status  = can_tx(Base+1, 5, 0, data);

	// Wait for the acknowledgement
	// if we don't get ACK then status is set to -1
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	if (status == 0) if (data[0] != ACK) status = -1;

	// If the command was written and acknowledged properly, return 0
	// Otherwise return -1

	return status;	
}

// Retrieves the programmed pmt for a given position
//		The returned values are the zone, sector and card for the position pos

long adc_get_pmtlist(char pos, char *zone, char *sector, char *card)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	// Send the read command
	data[0] = REQUEST;
	data[1] = PMTLIST;
	data[2] = pos;
	status  = can_tx(Base+1, 3, 0, data);
	// Get back the result
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	// If we don't get back a REQUEST command
	// If we don't get back the right parameter code
	//		then error is set to 1
	if (status == 0) if ((data[0] != REQUEST) || (data[1] != PMTLIST) || (data[2] != pos)) status = 1;
	
	// Format the returned parameter
	*zone = data[3];
	*sector = data[4];
	*card = data[5];

	// Returns 0 if all was OK
	// Returns 1 if the command or code parameter that were received back are wrong
	// Returns -1 if there was a communication error

	return status;

}

// Set up the DAC that controls the global pedestal
//	DAC=0 sets the maximum pedestal (0.5V)
//  DAC=255 sets the minimum pedestal (-0.5V)
//  Note that this parameter cannot be read back

long adc_dacset(char dac)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	// Send the command
	data[0] = DACSET;
	data[1] = dac;
	status  = can_tx(Base+1, 2, 0, data);

	// Wait for the acknowledgement
	// if we don't get ACK then status is set to -1
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	if (status == 0) if (data[0] != ACK) status = -1;

	// If the command was written and acknowledged properly, return 0
	// Otherwise return -1

	return status;	
}

// set up a programable delay
//	The delay is placed between the card select operation and the 
//	adc trigger for any adc convertion request
//	DELAY = 0 gives 8 usec delay
//	Each increment of DELAY gives 8 additional usecs delay
//	Note that in automatic scan mode, DELAY should not exceed 6.
//  For other modes, any value can be set

long adc_delay(unsigned short delay)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	// Send the command
	data[0] = DELAY;
	data[1] = (unsigned char)( (delay >> 8) &0x00FF );
	data[2] = (unsigned char)(  delay       &0x00FF );
	status  = can_tx(Base+1, 3, 0, data);

	// Wait for the acknowledgement
	// if we don't get ACK then status is set to -1
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	if (status == 0) if (data[0] != ACK) status = -1;

	// If the command was written and acknowledged properly, return 0
	// Otherwise return -1

	return status;
}

long adc_get_delay(unsigned short *delay)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	// Send the read command
	data[0] = REQUEST;
	data[1] = DELAY;
	status  = can_tx(Base+1, 2, 0, data);
	// Get back the result
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	// If we don't get back a REQUEST command
	// If we don't get back the right parameter code
	//		then error is set to 1
	if (status == 0) if ((data[0] != REQUEST) || (data[1] != DELAY)) status = 1;
	
	// Format the returned parameter
	*delay = (((unsigned short)data[2])*256) + ((unsigned short)data[3]);

	// Returns 0 if all was OK
	// Returns 1 if the command or code parameter that were received back are wrong
	// Returns -1 if there was a communication error

	return status;
}

// Send a 3in1 pattern into the mother board epld

long adc_3in1set(unsigned short dat)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	// Send the command
	data[0] = CANSET;
	data[1] = (unsigned char)( (dat >> 8) &0x00FF );
	data[2] = (unsigned char)(  dat       &0x00FF );
	status  = can_tx(Base+1, 3, 0, data);

	// Wait for the acknowledgement
	// if we don't get ACK then status is set to -1
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	if (status == 0) if (data[0] != ACK) status = -1;

	// If the command was written and acknowledged properly, return 0
	// Otherwise return -1

	return status;
}

// Read a 3in1 pattern form the mother board epld

long adc_3in1get(unsigned short *dat)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];
	unsigned short var1, var2;

	// Send the read command
	data[0] = CANGET;
	status  = can_tx(Base+1, 1, 0, data);
	// Get back the result
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	// If we don't get back a REQUEST command
	// If we don't get back the right parameter code
	//		then error is set to 1
	if (status == 0) if (data[0] != CANGET) status = 1;
	
	// Format the returned parameter
	var1 = ((unsigned short)data[1])&0x00FF;
	var2 = ((unsigned short)data[2])&0x00FF;
	*dat = (var1 << 8) + var2;

	// Returns 0 if all was OK
	// Returns 1 if the command or code parameter that were received back are wrong
	// Returns -1 if there was a communication error

	return status;
}

// Do a single convertion
//  The ADC shifts into the mother board epld the formatted apttern to select the card
//  then waits for DELAY
//	then performs the conversion and send back the result

long adc_convert(char zone, char sector, char card, unsigned short *result)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];
	char d1, d2;
	unsigned short pattern;

	pattern = (((unsigned short)zone)<<13) + (((unsigned short)sector)<<6) + ((unsigned short)card);

	// Send the convert command
	data[0] = CONVERT;
	data[1] = (unsigned char)( (pattern >> 8) &0x00FF );
	data[2] = (unsigned char)(  pattern       &0x00FF );
	status  = can_tx(Base+1, 3, 0, data);

	d1 = data[1];
	d2 = data[2];

	// Get back the result
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	// If we don't get back a REQUEST command
	// If we don't get back the right parameter code
	//		then error is set to 1

	if (status == 0) if ((data[0] != CONVERT) || (data[1] != d1 ) 
											  || (data[2] != d2 ) ) status = 1;
	
	// Format the returned parameter
	*result = ((unsigned short)data[3])*256 + ((unsigned short)data[4]);
	
	// Returns 0 if all was OK
	// Returns 1 if the command or code parameter that were received back are wrong
	// Returns -1 if there was a communication error

	return status;
}

// This function triggers a full scan and is used to synchronize
//  the full scans of different ADCs
// The number of messages that must be received back is function of npmt
// Each received message will conatin up to 4 results
// Maximum 12 messages are expected for npmt=48

long adc_trigger(char npmt, unsigned short result[48])
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	int i, j, k;
	int nmsg;

	// Calculates the expetcted number of messages

	nmsg = (int)(ceil( ((double)npmt)/((double)4) ) );

	// Send the command
	data[0] = TRIGGER;
	status  = can_tx(Base+1, 1, 0, data);

	// Receive the messages
	i=0;
	j=0;
	while ((i<nmsg)&&(status==0))
	{
		status = can_rx(&id, &dlc, &rtr, data);
		k = (id - 2) & 0x001F;
		if (status == 0) 
		{
			for (j=0;j<4;j++) 
			{
				result[k*4 + j] = ((unsigned short)data[j*2])*256 + ((unsigned short)data[j*2 + 1]);
			}
		}
	}
	
	// If the command was written properly and the expected messages were received, then return 0
	// Otherwise return -1

	return status;
}

// start the automatic triggers
// No acknowledgement expected: data is being sent immediately

long adc_start(void)
{
	long status;
	char data[8];

	// Send the command
	data[0] = START;
	status  = can_tx(Base+1, 1, 0, data);

	// If the command was written properly, return 0
	// Otherwise return -1

	return status;
}

// Recovers data in automatic scan mode

long adc_auto(char npmt, unsigned short result[48])
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	int i, j, k;
	int nmsg;

	// Calculates the expetcted number of messages

	nmsg = (int)(ceil( ((double)npmt)/((double)4) ) );

	// Receive the messages
	i=0;
	j=0;
	status = 0;

	while ((i<nmsg)&&(status==0))
	{
		status = can_rx(&id, &dlc, &rtr, data);
		i++;
		k = (id - 2) & 0x001F;
		if (status == 0) 
		{
			for (j=0;j<4;j++) 
			{
				result[k*4 + j] = ((unsigned short)data[j*2])*256 + ((unsigned short)data[j*2 + 1]);
			}
		}
	}
	
	// If the command was written properly and the expected messages were received, then return 0
	// Otherwise return -1

	return status;
}

// Stops automatic trigger
//	the ADC sends back an ACK

long adc_stop(void)
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	// Send the command
	data[0] = STOP;
	status  = can_tx(Base+1, 1, 0, data);

	// Empty the input buffer until ACK is received
	do
	{
		// Wait for the acknowledgement
		// if we don't get ACK then status is set to -1
		if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
		if (status == 0) if (data[0] != ACK) status = -1;
	} while ( (status == 0) && (data[0] != ACK) );


	// If the command was written properly, return 0
	// If the ACK was received return 0
	// Otherwise return -1

	return status;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// CAN communication functions
// Id is the 11 bits identifier
// DLC is the data length code (0 to 8)
// RTR is the Remote Frame Flag
// Data[8]

long can_tx(int id, char dlc, char rtr, char data[8])
{
	int i;
	
	NCTYPE_CAN_FRAME		FrameS;
	NCTYPE_STATUS			Status;
	NCTYPE_STATE			State;
	
	FrameS.ArbitrationId	=	id;
	FrameS.IsRemote			=	rtr;
	FrameS.DataLength		=	dlc;
	for (i=0;i<dlc;i++) FrameS.Data[i] = data[i];
	
	// Transmission Frame Request
	Status = ncWrite(ObjHandle, sizeof(FrameS), &FrameS);
	
	// Wait for end of CAN transmission
	if (Status == NC_SUCCESS)
	{
		Status = ncWaitForState(ObjHandle, (NC_ST_WRITE_SUCCESS | NC_ST_ERROR), (NC_DURATION_1SEC), &State);
		
	}
	
	// If Status equals NCSUCCESS message was transitted -> return 0
	// Otherwise there was a problem -> return -1
	
	if (Status == NC_SUCCESS) return 0;
	else return -1;
}

long can_rx(int *id, char *dlc, char *rtr, char data[8])
{
	int i;

	NCTYPE_CAN_FRAME_TIMED	FrameR;
	NCTYPE_STATUS			Status;
	NCTYPE_STATE			State;

	// Wait until there's something in the incoming queue
	Status = ncWaitForState(ObjHandle, (NC_ST_READ_AVAIL | NC_ST_ERROR), (NC_DURATION_1SEC), &State);

	// Recover the message
	if (Status == NC_SUCCESS)
	{
		Status  = ncRead(ObjHandle, sizeof(FrameR), &FrameR);
		*id		= FrameR.ArbitrationId;
		*dlc	= FrameR.DataLength;
		*rtr	= FrameR.IsRemote;
		for (i=0;i<8; i++) data[i]=FrameR.Data[i];
	}

	// If Status equals NCSUCCESS message was properly recovered -> returns 0
	// Otherwise there was problem -> returns -1

	if (Status == NC_SUCCESS) return 0;
	else return -1;

}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// CANbus initialization
// Adress is the ADC board diup switch setting
// Setup 500kbps
// Setup rx and tx queues for PCICAN2 board
// Disable masks and filters

long init(int adress, char * branchname)
{



   NCTYPE_STATUS		Status;
   NCTYPE_ATTRID		AttrIdList[8];
   NCTYPE_UINT32		AttrValueList[8];

   // The upper 5 bits of the identifiers are the ADC dip switches settings
   Base = (adress << 6);

   // Reset the PCICAN2 board
   ncReset(branchname, 0);

   // setup variables

   AttrIdList[0] =         NC_ATTR_BAUD_RATE;   
   AttrValueList[0] =      NC_BAUD_500K;
   AttrIdList[1] =         NC_ATTR_START_ON_OPEN;
   AttrValueList[1] =      NC_TRUE;
   AttrIdList[2] =         NC_ATTR_READ_Q_LEN;
   AttrValueList[2] =      300;
   AttrIdList[3] =         NC_ATTR_WRITE_Q_LEN;
   AttrValueList[3] =      10;
   AttrIdList[4] =         NC_ATTR_CAN_COMP_STD;
   AttrValueList[4] =      0; 
   AttrIdList[5] =         NC_ATTR_CAN_MASK_STD;
   AttrValueList[5] =      NC_CAN_MASK_STD_DONTCARE;
   AttrIdList[6] =         NC_ATTR_CAN_COMP_XTD;
   AttrValueList[6] =      0; 
   AttrIdList[7] =         NC_ATTR_CAN_MASK_XTD;
   AttrValueList[7] =      NC_CAN_MASK_XTD_DONTCARE;

	// Initializes CANbus connected to PCICAN2
   Status = ncConfig(branchname, 8, AttrIdList, AttrValueList);

   // open the device driver and gets the object handler
   if (Status == NC_SUCCESS) Status = ncOpenObject(branchname, &ObjHandle);
   
   // If Status == NCSUCCESS the CANbus was initialized properly -> returns 0
   // Otherwise return -1
   if (Status == NC_SUCCESS) return 0;
   else return -1;
   
}

