/*************************************************************
  CANremote.c -- Remote control software for accessing a VME
	 	 crate over a CAN Bus, using a TEWS TIP810
		 controller.

  Coded by:  A.J. Tolland (ajt@hep.uchicago.edu)
  Created: 3/31/98 	Last modified:
  
  Description:  A loop + menu interface allows the user to 
	select a function.  The program prompts the user for
	data, where necessary.  Then it writes the necessary
	data to the TIP810, and reads back the remote create's
	response, when necessary.

*************************************************************/

#include <stdio.h>

#define IP 0xdffe6000

/* typedef unsigned char uchar;  */

typedef struct {
   uchar data[10];
} Buffer;

uchar r_read(int addr);
uchar getaddr();

main()
{
   int choice1 = 1;

   printf("VME Crate Remote Interface\n\n");

   while(choice1 != 3) {
     reset_can();  /* reset everything */
     printf("\n\nMain Menu:\n");
     printf("\n1. Read data from a crate.\n");     
     printf("2. Write data to a crate.\n");
     printf("3. Quit.\n\n");
     printf("\tEnter your choice (1 - 3): ");
     scanf("%d", &choice1);

     switch(choice1) {
       case 1: {readmenu(); break;}
       case 2: {writemenu(); break;}
       case 3: {break;}
       default: {printf("%d is not a valid input.\n\n", choice1); break;}
     }
   }
}



/***************************************************************
 readmenu() -- Prompts the user for the address of the crate
	to read data from, and for the sort of data to retrieve.
***************************************************************/

int readmenu() 
{
   int i, choice2;
   uchar addr;
   Buffer rbuf;  

   addr = (uchar) getaddr();
   if(((int) addr) == 0) return 0;
   choice2 = 0;
   while(choice2 < 1 || choice2 > 6) {
     printf("\n\nWhat would you like to do?");
     printf("\n\n1. Get the crate status.");
     printf("\n2. Get the measured voltages.");
     printf("\n3. Get the voltage settings.");
     printf("\n4. Get the fan speed data.");
     printf("\n5. Get the temperature data.");
     printf("\n6. Get the configuration settings.");
     printf("\n\tEnter your choice (1 - 6): ");
     scanf("%d", &choice2);
   
     switch(choice2) {
       case 1: {c_stat_read(addr, &rbuf); 
                printf("\n\nCrate Status Data:\n");
	   	if(rbuf.data[2] & 0x01) printf("\nPower is On.");
   		  else printf("\nPower is Off.");
   		if(rbuf.data[2] & 0x02) printf("\nExternal power is not inhibited.");
   		  else printf("\nExternal power is inhibited.");
	   	if(rbuf.data[2] & 0x04) printf("\nAC is within safe limits.");
   		  else printf("\nAC power failure.");
   		if(rbuf.data[2] & 0x10) printf("\nFans are OK.");
   	  	else printf("\nFans are broken.");
   		if(rbuf.data[2] & 0x20) printf("\nCrate set to trip off if fans are disabled.");
   		  else printf("\nCrate is not set to trip off if fans are disabled.");
   		if(rbuf.data[2] & 0x40) printf("\nCrate not set to trip off if any errors occur.");
 	  	  else printf("\nCrate set to trip off if any errors occur.");
   		if(rbuf.data[2] & 0x80) printf("\nVME bus signal sysfail active.");
   		  else printf("\nVME bus signal sysfail inactive.");
   		if(rbuf.data[3] & 0x80) printf("\nHardware is write protected.");
   		  else printf("\nNo write protect.\n");
   		if(rbuf.data[2] & 0x08) printf("\nNo Errors.\n");   
     		else {
     		for(i = 0; i < 8; i++) {
        	  if(rbuf.data[4] & (0x01 << i)) printf("\nUndervoltage error on Channel %d.", i);}     
     		for(i = 0; i < 8; i++) {
     	  	  if(rbuf.data[5] & (0x01 << i)) printf("\nOvervoltage error on Channel %d.", i);   }
     		for(i = 0; i < 8; i++) {
    	   	  if(rbuf.data[6] & (0x01 << i)) printf("\nMinimum current error on Channel %d", i);}
   	  	for(i = 0; i < 8; i++) {
   	    	  if(rbuf.data[7] & (0x01 << i)) printf("\nOver current error on Channel %d.", i);} 
   	  	for(i = 0; i < 8; i++) {
   	    	  if(rbuf.data[8] & (0x01 << i)) printf("\nOVP error on Channel %d.");}
     		for(i = 0; i < 8; i++) {
       		  if(rbuf.data[9] & (0x01 << i)) printf("\nTemperature error.");}
     		}
		break;}
       case 2: {m_volt_read(addr); break;}
       case 3: {volt_set_read(addr); break;}
       case 4: {fan_read(addr, &rbuf); 
		printf("\n\nFan Speed Data:");
		printf("\n\nMiddle Fan Speed: %d revs/sec", rbuf.data[2]);
		printf("\nNominal Fan Speed:  %d revs/sec", rbuf.data[3]);
		printf("\nSpeed of Fan #1:    %d revs/sec", rbuf.data[4]);
		printf("\nSpeed of Fan #2:    %d revs/sec", rbuf.data[5]);	
		printf("\nSpeed of Fan #3:    %d revs/sec", rbuf.data[6]);   
		break;}
       case 5: {temp_read(addr); break;}
       case 6: {config_read(addr); break;}
       default: {printf("\n\nInvalid choice");break;}
     }
   }
}

/*************************************************************
   c_stat_read(addr) -- retrieves and prints out status data
from addr
*************************************************************/

c_stat_read(addr, bp)
uchar addr;
Buffer* bp;
{
   r_write(IP + 0x15, (addr >> 3));
   r_write(IP + 0x17, ((addr << 5) + 0x18));
   r_write(IP + 0x03, 0x01);

   read_buf(IP + 0x29, bp);   
}

/*************************************************************
  m_volt_read(addr) -- reads the measured voltage on whichever
	channel the user requests.
*************************************************************/

m_volt_read(addr)
uchar addr;
{
   int channel;   
   Buffer rbuf;
   short int voltage, current;

   printf("\n\nMeasured Voltages & Currents from Power Supply:");
   printf("\nWhich channel do you wish to get data on?");
   printf("\n\tEnter channel number (0 - 6): ");

   scanf("%d", &channel);

   r_write(IP + 0x17, (addr << 5) + 0x18);
     
switch(channel) {
     case 0: {r_write(IP + 0x15, 0x20 + (addr >> 3)); break;}
     case 1: {r_write(IP + 0x15, 0x30 + (addr >> 3)); break;}         
     case 2: {r_write(IP + 0x15, 0x40 + (addr >> 3)); break;}
     case 3: {r_write(IP + 0x15, 0x50 + (addr >> 3)); break;}
     case 4: {r_write(IP + 0x15, 0x20 + (addr >> 3)); break;}
     case 5: {r_write(IP + 0x15, 0x30 + (addr >> 3)); break;}
     case 6: {r_write(IP + 0x15, 0x40 + (addr >> 3)); break;}
     default: {printf("\n\n%d is not a valid channel.\n", channel);return 0;}
   }
   r_write(IP + 0x03, 0x01);
   read_buf(IP + 0x29, &rbuf);

   switch(channel) {
     case 0: {voltage = rbuf.data[2] + rbuf.data[3]*256;
              current = rbuf.data[4] + rbuf.data[5]*256;break;}
     case 1: {voltage = rbuf.data[2] + rbuf.data[3]*256;
              current = rbuf.data[4] + rbuf.data[5]*256;break;}
     case 2: {voltage = rbuf.data[2] + rbuf.data[3]*256;
              current = rbuf.data[4] + rbuf.data[5]*256;break;}
     case 3: {voltage = rbuf.data[2] + rbuf.data[3]*256;
              current = rbuf.data[4] + rbuf.data[5]*256;break;}   
     case 4: {voltage = rbuf.data[6] + rbuf.data[7]*256;
              current = rbuf.data[8] + rbuf.data[9]*256;break;}
     case 5: {voltage = rbuf.data[6] + rbuf.data[7]*256;
              current = rbuf.data[8] + rbuf.data[9]*256;break;}
     case 6: {voltage = rbuf.data[6] + rbuf.data[7]*256;
              current = rbuf.data[8] + rbuf.data[9]*256;break;}
     }

     printf("\nMeasured Voltage on Channel %d: %d", channel, voltage/100);
     printf("\nMeasured Current on Channel %d: %d", channel, current/100);
}

/**************************************************************
  volt_set_read(addr) -- Reads the voltage settings
*************************************************************/

volt_set_read(addr)
uchar addr;
{
   Buffer rbuf;
   int choice, channel;
   short int i;  /*Stores current or voltage */

   printf("\n\nVoltage Settings Menu:");
   printf("\n1. Output Voltage Settings");
   printf("\n2. Current Limit Settings");
   printf("\n3. Undervoltage Compare Settings");
   printf("\n4. Overvoltage Compare Settings");
   printf("\n5. Current Compare Settings"); 
   printf("\n6. Overcurrent Compare Settings");
   printf("\n7. Overvoltage Protection Settings");
   printf("\n\nEnter your choice (1-7): ");
   scanf("%d", &choice);
   printf("\nWhich channel's settings do you wish to view?  \n\ntEnter channel number: ");
   scanf("%d", &channel);

   r_write(IP + 0x15, 0xa0 + (addr >> 3));
   r_write(IP + 0x17, (addr << 5) + 0x01);

   switch(choice) {
   case 1: {r_write(IP + 0x19, channel*16 + 0x80);break;}
   case 2: {r_write(IP + 0x19, channel*16 + 0x81); break;}
   case 3: {r_write(IP + 0x19, channel*16 + 0x82); break;}
   case 4: {r_write(IP + 0x19, channel*16 + 0x83); break;}
   case 5: {r_write(IP + 0x19, channel*16 + 0x84); break;}
   case 6: {r_write(IP + 0x19, channel*16 + 0x85); break;}
   case 7: {r_write(IP + 0x19, channel*16 + 0x86); break;}
   default: {printf("\n\nThat's not a valid option."); break; return 0;}
   }
   r_write(IP + 0x03, 0x01);
   read_buf(IP + 0x29, &rbuf);
   i = rbuf.data[4]*256 + rbuf.data[3];
   switch(choice) {
   case 1: {printf("\nOutput Voltage on Channel %d Set to %.2f (V)", channel, (float) (i/100));break;}
   case 2: {printf("\nCurrent Limit on Channel %d Set to %.2f(A)", channel, (float)(i/100));break;}
   case 3: {printf("\nUndervoltage on Channel %d Set to %.2f (V)", channel, (float) (i/100));break;}
   case 4: {printf("\nOvervoltage on Channel %d Set to %.2f (A)", channel, (float)(i/100));break;}
   case 5: {printf("\nMinimum Current on Channel %d Set to %.2f (V)", channel,(float) (i/100));break;}
   case 6: {printf("\nOvercurrent Limit on Channel %d Set to %.2f (A)", channel, (float) (i/100));break;}
   case 7: {printf("\nOvervoltage Protection on Channel %d Set to %.2f (A)", channel, (float)(i/100));break;}
   }
}

/*************************************************************
   fan_read(addr) -- retrieves and prints out data from addr
*************************************************************/

fan_read(addr, bp)
uchar addr;
Buffer* bp;
{
   uchar desc0, desc1;

   desc0 = (0x6 << 4) + (addr >> 3);
   desc1 = (addr << 5) + 0x18;
   
   r_write(IP + 0x15, desc0);
   r_write(IP + 0x17, desc1);
   r_write(IP + 0x03, 0x01);

   read_buf(IP + 0x29, bp);   
}

/*************************************************************
  temp_read(addr) -- retrieves and prints out temperature data
	from addr
*************************************************************/

temp_read(addr)
uchar addr;
{
   uchar desc0, desc1;
   Buffer temps;
  
   desc0 = (0x7 << 4) + (addr >> 3);
   desc1 = (addr << 5) + 0x18;
   
   r_write(IP + 0x15, desc0);
   r_write(IP + 0x17, desc1);
   r_write(IP + 0x03, 0x01);

   read_buf(IP + 0x29, &temps);

   printf("\n\nTemperature Data:\n\n");
   printf("Fan Air Inlet Temperature:    %d Celsius\n", temps.data[2]);
   printf("Power Supply Air Temperature: %d Celsius\n", temps.data[3]);
}   

/*************************************************************
  config_read(addr)  -- Asks the user which data he wishes to 
	retrieve, retrieves data
*************************************************************/

config_read(addr)
uchar addr;
{
   uchar desc0, desc1;
   int choice, i;
   Buffer rbuf;
 
   printf("\n\nConfiguration Data Retrieval Menu:\n");
   printf("\nWhich Data do you wish to retrieve?\n");
   printf("1. CAN Crate Control Software Version.\n");
   printf("2. Fan Software Version.\n");
   printf("3. Power Supply Version.\n");
   printf("4. Fan ID String.\n");
   printf("5. Power Supply ID String.\n");
   printf("Enter your choice (1-5): ");
   scanf("%d", &choice);

   desc0 = 0xc0 + (addr >> 3);
   desc1 = (addr << 5) + 0x01;
   
   r_write(IP + 0x15, desc0);
   r_write(IP + 0x17, desc1);
   
   switch(choice) {
   case 1: {r_write(IP + 0x19, 0x80);   /* Get software version */
	    r_write(IP + 0x03, 0x01);
	    read_buf(IP + 0x29, &rbuf);
	    printf("\n\nCAN Crate Control Software Version: ");
  	    for(i = 2; i < 10; i++) printf("%c", rbuf.data[i]); 
            break;}
   case 2: {r_write(IP + 0x19, 0x81);   /* Get ID's 0 - 6) */
            r_write(IP + 0x03, 0x01);
            read_buf(IP + 0x29, &rbuf);
            printf("\n\nFan Software Version: ");
            for(i = 3; i < 10; i++) printf("%c ", rbuf.data[i]);
            r_write(IP + 0x19, 0x82);   /* Get ID's 7 -13 */
            r_write(IP + 0x03, 0x01);
 	    read_buf(IP + 0x29, &rbuf);
            for(i = 3; i < 10; i++) printf("%c ", rbuf.data[i]);
	    break;}
   case 3: {r_write(IP + 0x19, 0x83);   /* Get ID's 0 - 6) */
            r_write(IP + 0x03, 0x01);
            read_buf(IP + 0x29, &rbuf);
            printf("\n\nPower Supply Software Version: ");
            for(i = 3; i < 10; i++) printf("%c ", rbuf.data[i]);
            r_write(IP + 0x19, 0x84);   /* Get ID's 7 -13 */
            r_write(IP + 0x03, 0x01);
 	    read_buf(IP + 0x29, &rbuf);
            for(i = 3; i < 10; i++) printf("%c ", rbuf.data[i]);
            break;}
   case 4: {r_write(IP + 0x19, 0x88);   /* Get ID's 0 - 6) */
            r_write(IP + 0x03, 0x01);
            read_buf(IP + 0x29, &rbuf);
            printf("\n\nFan ID String: ");
            for(i = 3; i < 10; i++) printf("%c ", rbuf.data[i]);
            r_write(IP + 0x19, 0x89);   /* Get ID's 7 - 13 */
            r_write(IP + 0x03, 0x01);
 	    read_buf(IP + 0x29, &rbuf);
            for(i = 3; i < 10; i++) printf("%c ", rbuf.data[i]);
            r_write(IP + 0x19, 0x8a);   /* Get ID's 14 - 20) */
            r_write(IP + 0x03, 0x01);
            read_buf(IP + 0x29, &rbuf);
            for(i = 3; i < 10; i++) printf("%c ", rbuf.data[i]);
            r_write(IP + 0x19, 0x8b);   /* Get ID's 21 - 27 */
            r_write(IP + 0x03, 0x01);
 	    read_buf(IP + 0x29, &rbuf);
            for(i = 3; i < 10; i++) printf("%c ", rbuf.data[i]);
            break;}
   case 5: {r_write(IP + 0x19, 0x8c);   /* Get ID's 0 - 6) */
            r_write(IP + 0x03, 0x01);
            read_buf(IP + 0x29, &rbuf);
            printf("\n\nPower Supply ID String: ");
            for(i = 3; i < 10; i++) printf("%c ", rbuf.data[i]);
            r_write(IP + 0x19, 0x8d);   /* Get ID's 7 - 13 */
            r_write(IP + 0x03, 0x01);
 	    read_buf(IP + 0x29, &rbuf);
            for(i = 3; i < 10; i++) printf("%c ", rbuf.data[i]);
            r_write(IP + 0x19, 0x8e);   /* Get ID's 14 - 20) */
            r_write(IP + 0x03, 0x01);
            read_buf(IP + 0x29, &rbuf);
            for(i = 3; i < 10; i++) printf("%c ", rbuf.data[i]);
            r_write(IP + 0x19, 0x8f);   /* Get ID's 21 - 27 */
            r_write(IP + 0x03, 0x01);
 	    read_buf(IP + 0x29, &rbuf);
            for(i = 3; i < 10; i++) printf("%c ", rbuf.data[i]);
            break;}
   default: {printf("\n\nInvalid Choice.\n\n");break;} 
   }
  
}

/*************************************************************
  writemenu() -- Prompts user for crate address, and offers
	user a menu of data input functions.  Passes control 
	to the function which the user chooses.
*************************************************************/

writemenu() 
{
   uchar addr;
   int choice;
   Buffer rbuf;

   addr = (uchar) getaddr();
   if( ((int) addr) == 0) return 0;  

/*kja   c_stat_read(addr, &rbuf);  */
 
   printf("\n\nData Writing Operations");
   printf("\nWhich one do you want?");
   printf("\n1. Toggle crate's power on/off.");
     if(rbuf.data[2] & 0x01) printf("\t\t\tCrate is currently ON.");
     else printf("\t\t\tCrate is currently OFF.");
   printf("\n2. Generate VME reset.");
   printf("\n3. Toggle error trip on/off");
     if(rbuf.data[2] & 0x40) printf("\t\t\tError Trip is currently OFF");
     else printf("\t\t\tError Trip is currently ON.");
   printf("\n4. Change the nominal fan speed.");
     fan_read(addr, &rbuf);
     printf("\t\t\tCurrent Nominal speed: %d revs/sec", rbuf.data[3]);     
   printf("\n5. Change the voltage settings.");
   printf("\n6. Change the fan ID string.");
   printf("\n7. Change the power supply ID string.");
   printf("\nEnter your choice (1 - 7): ");
   scanf("%d", &choice);

   switch(choice) {
     case 1: {on_off(addr);break;}
     case 2: {vme_reset(addr);break;}
     case 3: {error_on_off(addr);break;}
     case 4: {fan_change(addr);break;}
     case 5: {voltage_change(addr);break;}
     case 6: {fan_ID_change(addr);break;} 
     case 7: {Power_ID_change(addr);break;}
     default: {printf("\nn\nThat's not a valid choice.\n");break;} 
   }
}

/*************************************************************
 on_off(addr) -- toggles the crate on and off.
*************************************************************/

on_off(addr)
uchar addr;
{
   Buffer rbuf;
   c_stat_read(addr, &rbuf);
   
   r_write(IP + 0x15, (0x01 << 4) + (addr >> 3));
   r_write(IP + 0x17, (addr << 5) + 0x01);
   if(rbuf.data[2] & 0x01) r_write(IP + 0x19, 0x41);
   else r_write(IP + 0x19, 0x43);
   r_write(IP + 0x03, 0x01);
}

/*************************************************************
  vme_reset(addr) -- generates a VME Reset
*************************************************************/


vme_reset(addr)
uchar addr;
{
   Buffer rbuf;
   c_stat_read(addr, &rbuf);

   r_write(IP + 0x15, (0x01 << 4) + (addr >> 3));
   r_write(IP + 0x17, (addr << 5) + 0x01);
   if(rbuf.data[2] & 0x40) r_write(IP + 0x19, 0x04);
   else r_write(IP + 0x19, 0x44);
   r_write(IP + 0x03, 0x01);
} 

/*************************************************************
 error_on_off(addr) -- toggles the crate on and off.
*************************************************************/

error_on_off(addr)
uchar addr;
{
   Buffer rbuf;

   c_stat_read(addr, &rbuf);

   r_write(IP + 0x15, (0x01 << 4) + (addr >> 3));
   r_write(IP + 0x17, (addr << 5) + 0x01);
   if(rbuf.data[2] & 0x40) r_write(IP + 0x19, 0x40);
   r_write(IP + 0x03, 0x01);
}

/*************************************************************
  fan_change(addr) -- Lets the user change the fan speed.
*************************************************************/

fan_change(addr)
uchar addr;
{
   int new_speed;
   Buffer rbuf;

   c_stat_read(addr, &rbuf);   

   printf("\n\nEnter the new fan speed (revs/sec): ");
   scanf("%d", &new_speed);
  
   r_write(IP + 0x15, (0x01 << 4) + (addr >> 3));
   r_write(IP + 0x17, (addr << 5) + 0x02);
   if(rbuf.data[2] & 0x40) r_write(IP + 0x19, 0x80);
   else r_write(IP + 0x19, 0xc0);
   r_write(IP + 0x1b, (uchar) new_speed); 
   r_write(IP + 0x03, 0x01);
}

/*************************************************************
  voltage_change(addr) -- Presents a list of voltage changes 
	the user can make, implements the change user chooses.
*************************************************************/

voltage_change(addr)
uchar addr;
{
   Buffer rbuf;
   int choice, channel, volt;
   uchar byte1, byte2;  /*Stores current or voltage */
   

   printf("\n\nWhich Voltage Settings do you wish to alter?");
   printf("\n1. Output Voltage Settings");
   printf("\n2. Current Limit Settings");
   printf("\n3. Undervoltage Compare Settings");
   printf("\n4. Overvoltage Compare Settings");
   printf("\n5. Current Compare Settings"); 
   printf("\n6. Overcurrent Compare Settings");
   printf("\n7. Overvoltage Protection Settings");
   printf("\n\nEnter your choice (1-7): ");
   scanf("%d", &choice);
   printf("\nWhich channel's settings do you wish to view?  \n\ntEnter channel number: ");
   scanf("%d", &channel);

   printf("Enter the new Voltage or current (V or A): ");
   scanf("%d", &volt);
   
   byte1 = ((volt & 0xff00) >> 8);
   byte2 = (volt & 0x00ff);
 
   r_write(IP + 0x15, 0xa0 + (addr >> 3));
   r_write(IP + 0x17, (addr << 5) + 0x08);

   switch(choice) {
   case 1: {r_write(IP + 0x19, channel*16 + 0x00); break;}
   case 2: {r_write(IP + 0x19, channel*16 + 0x01); break;}
   case 3: {r_write(IP + 0x19, channel*16 + 0x02); break;}
   case 4: {r_write(IP + 0x19, channel*16 + 0x03); break;}
   case 5: {r_write(IP + 0x19, channel*16 + 0x04); break;}
   case 6: {r_write(IP + 0x19, channel*16 + 0x05); break;}
   case 7: {r_write(IP + 0x19, channel*16 + 0x06); break;}
   default: {printf("\n\nThat's not a valid option."); break; return 0;}
   }
   r_write(IP + 0x1b, byte1);
   r_write(IP + 0x1d, byte2);
   r_write(IP + 0x03, 0x01);
   read_buf(IP + 0x29, &rbuf); /*Clears the Crate confirm */
}

/*************************************************************
  fan_ID_change(addr) -- User enters a new ID String, which is
 	then written to addr
*************************************************************/

fan_ID_change(addr)
uchar addr;
{
    uchar newID[28];    
    int i, j;    

    printf("\n\nEnter the new Fan ID: ");
    gets(newID);
  
    for(j = 0; j < 4; j++) {
      r_write(IP + 0x15, 0xc0 + (addr >> 3));
      r_write(IP + 0x17, (addr << 5) + 0x08);
      r_write(IP + 0x19, 0x08 + j);
      for(i = 7*j; i < 7*(j+1); i++) r_write(IP + 0x1b + 2*i, newID[i]);
      r_write(IP + 0x03, 0x04);
    }
}

/*************************************************************
  Power_ID_change(addr) -- User enters a new ID String, which is
 	then written to addr
*************************************************************/

Power_ID_change(addr)
uchar addr;
{
    uchar newID[28];    
    int i, j;    

    printf("\n\nEnter the new Power Supply ID: ");
    gets(newID);
  
    for(j = 0; j < 4; j++) {
      r_write(IP + 0x15, 0xc0 + (addr >> 3));
      r_write(IP + 0x17, (addr << 5) + 0x08);
      r_write(IP + 0x19, 0x0c + j);
      for(i = 7*j; i < 7*(j+1); i++) r_write(IP + 0x1b + 2*i, newID[i]);
      r_write(IP + 0x03, 0x04);
    }
}

/*************************************************************
  read_buf(int, fp) -- copies the data in the ten bytes 
	starting with int into the pointer fp.
*************************************************************/

read_buf(addr, bp)
int addr;
Buffer* bp;
{
  int i;
  while(!(r_read(IP + 0x05) & 0x01)) {}
  for(i = 0; i < 10; i++) {
    bp->data[i] = r_read(addr + 2*i);
  }
  r_write(IP + 0x03, 0x04);
}

/*************************************************************
  getaddr() -- returns the crate address inputed by the user,
	after checking that the address is valid.
*************************************************************/


uchar getaddr()
{
   int addr, yesno;
   uchar adr;
   addr = yesno = 0;   

   while( addr < 1 || addr >  127 ) {
     printf("\nWhich crate do you wish to interface with?\n");
     printf("\tEnter the crate's address: ");
     scanf("%d", &addr);
      
     if(addr < 1 || addr > 127) {printf("Invalid address.\n");}
   
     if(addr == 127) {
       while(yesno < 1 || yesno > 2) {
         printf("Are you sure you want a general broadcast?\n"); 
         printf("\tYes (1) or No (2): ");
         scanf("%d", &yesno);
       }  
     }
   if(yesno == 2) return 0;
   }
   adr = (uchar) addr;
   return(adr);
}

/************************************************************
 r_write(address, command) -- writes the unsigned character
 command to the address specified.
************************************************************/


r_write(address, command) 
int address;
uchar command;
{
   uchar* addr;
   addr = (uchar*) address;
   *addr = command;
}

/****************************************************************
  r_read(addr) -- returns the byte held at addr
****************************************************************/

uchar r_read(addr)
int addr;
{
   uchar* adr;
   adr = (uchar*) addr;
   return(*adr);
}

reset_can()
  {
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

