#define CANSET 11
#define CANGET 12
#define CONVERT 15
#define DELAY 10
#define DACSET 9
#define REQUEST 14
#define ACK 51
#define GO_FB 101
#define GO_ISP 102
#define INIT 1
#include <stdio.h>
#include <math.h>
int Base;

/*-----------------------------------------------*/
/* set deskew of clocks                          */
/*-----------------------------------------------*/
     ClockDes1(ttcadr,time)  /* fine deskewing of clockdes1 for L1A */
      int ttcadr,time;
         { SendLongCommand(ttcadr,0,0,time);
	 }
      
      ClockDes2(ttcadr,time)  /* fine deskewing of clockdes2 for TP drop */
      int ttcadr,time;
         { SendLongCommand(ttcadr,0,1,time);
	 }
      
      CoarseDelay(ttcadr,d1,d2)  /* coarse deskew of clock40des1 & 2 */
      int ttcadr,d1,d2;
         { int delay;
	   delay = d2<<4 | (d1 & 0xf);
           SendLongCommand(ttcadr,0,2,delay);
	 }

     tp_high()     
/*-----------------------------------------------*/
/* set tp high                                   */
/*-----------------------------------------------*/
      { SendLongCommand(0,1,0x80,0x00); }

     tp_low()     
/*-----------------------------------------------*/
/* set tp low                                    */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0x80,0x01); }
 
     tp_low_bc()     
/*-----------------------------------------------*/
/* set tp low with broadcast command             */
/*-----------------------------------------------*/
     { BroadcastShortCommand(0xc0); }
 
     set_zone(zone)     
/*-----------------------------------------------*/
/* set the zone field                            */
/*-----------------------------------------------*/
     int zone;
     { SendLongCommand(0,1,0x84,zone); }

     set_sector(sect)     
/*-----------------------------------------------*/
/* set the sector field                          */
/*-----------------------------------------------*/
     int sect;
     { SendLongCommand(0,1,0x88,sect); }

     set_tube(tube)     
/*-----------------------------------------------*/
/* set the tube field                            */
/*-----------------------------------------------*/
     int tube;
     { SendLongCommand(0,1,0x8c,tube); }

     multi_high()     
/*-----------------------------------------------*/
/* set multi_sel high                            */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0x90,0); }

     multi_low()     
/*-----------------------------------------------*/
/* set multi_sel low                             */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0x90,1); }

     rxw_high()     
/*-----------------------------------------------*/
/* set rxw high                                  */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0x94,1); }

     rxw_low()     
/*-----------------------------------------------*/
/* set rxw low                                   */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0x94,0); }

     back_high()     
/*-----------------------------------------------*/
/* set back_load high                            */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0x98,1); }

     back_low()     
/*-----------------------------------------------*/
/* set back_load low                             */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0x98,0); }

     load_can()     
/*-----------------------------------------------*/
/* load the CAN output register                  */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0x9c,0); }

     reset_state()     
/*-----------------------------------------------*/
/* reset the state machines                      */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0xa0,0); }

     reset_can(i)
     int i;
     
/*-----------------------------------------------*/
/* set CANRST lne to the integrator card         */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0xa4,i); }

     no_op()     
/*-----------------------------------------------*/
/* send a no_op instruction to the state machine */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0xdc,0); }

     intg_rd_high()     
/*-----------------------------------------------*/
/* set intg_rd high                              */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0xe0,1); }

     intg_rd_low()     
/*-----------------------------------------------*/
/* set intg_rd low                              */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0xe0,0); }

     itr_high()     
/*-----------------------------------------------*/
/* set itr high                              */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0xe4,1); }

     itr_low()     
/*-----------------------------------------------*/
/* set itr low                              */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0xe4,0); }

     set_intg_gain(i)     
/*-----------------------------------------------*/
/* set intg gain switches s1-s4                  */
/*-----------------------------------------------*/
     unsigned int i;
     { unsigned int adr,data;
       adr = 0xe8;
       data = i & 0xf;
       SendLongCommand(0,1,adr,data); 
     }

     mse_high()     
/*-----------------------------------------------*/
/* set mse high                              */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0xec,1); }

     mse_low()     
/*-----------------------------------------------*/
/* set mse low                              */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0xec,0); }

     small_cap_enable()     
/*-----------------------------------------------*/
/* enable small capacitor calibration            */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0xf0,1); }

     small_cap_disable()     
/*-----------------------------------------------*/
/* disable small capacitor calibration           */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0xf0,0); }


     large_cap_enable()     
/*-----------------------------------------------*/
/* enable large capacitor calibration            */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0xf4,1); }

     large_cap_disable()     
/*-----------------------------------------------*/
/* disable large capacitor calibration           */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0xf4,0); }

     set_dac(i)     
/*-----------------------------------------------*/
/* set dac                                       */
/*-----------------------------------------------*/
     unsigned int i;
     { unsigned int subadr,data;
       subadr = 0xf8 | ((i>>8) & 0x3);
       data = i & 0xff;
       SendLongCommand(0,1,subadr,data);
     }

     trig_enable()     
/*-----------------------------------------------*/
/* enable trigger output                         */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0xfc,1); }

     trig_disable()     
/*-----------------------------------------------*/
/* disable trigger output                        */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0xfc,0); }

     set_mb_time(l,i)
/*-----------------------------------------------*/
/* set timer on board l to 8-bit value i         */
/*-----------------------------------------------*/
     unsigned int l,i;
     { if(l == 1)
        { set_tube(0x38);
          SendLongCommand(0,1,0xf8,i);  
          set_tube(0x39); }
       if(l == 2)
        { set_tube(0x3a);
          SendLongCommand(0,1,0xf8,i);  
          set_tube(0x3b); }
       if(l == 3)
        { set_tube(0x3c);
          SendLongCommand(0,1,0xf8,i);  
          set_tube(0x3d); }
       if(l == 4)
        { set_tube(0x3e);
          SendLongCommand(0,1,0xf8,i);  
          set_tube(0x3f); }
     }

/*-----------------------------------------------*/
/* CANbus commands - via the integrator ADC card */
/*-----------------------------------------------*/
can_tp_high()
  { long status;
    status = can_3in1set(0x8001);
    if(status != 0) printf("CANbus error can_tp_high\n");
  }

can_tp_low()
  { long status;
    status = can_3in1set(0x8000);
    if(status != 0) printf("CANbus error can_tp_low\n");
  }

can_zone(zone)
int zone;
  { long status;
    unsigned short dat;
    dat = 0x8400 | zone;
    status = can_3in1set(dat);
    if(status != 0) printf("CANbus error can_zone\n");
  }

can_sector(sector)
int sector;
  { long status;
    unsigned short dat;
    dat = 0x8400 | sector;
    status = can_3in1set(dat);
    if(status != 0) printf("CANbus error can_sector\n");
  }

can_tube(tube)
int tube;
  { long status;
    unsigned short dat;
    dat = 0x8c00 | tube;
    status = can_3in1set(dat);
    if(status != 0) printf("CANbus error can_tube\n");
  }

can_multi_high()
  { long status;
    status = can_3in1set(0x9000);
    if(status != 0) printf("CANbus error can_multi_high\n");
  }

can_multi_low()
  { long status;
    status = can_3in1set(0x90ff);
    if(status != 0) printf("CANbus error can_multi_low\n");
  }

can_rxw_high()
  { long status;
    status = can_3in1set(0x9401);
    if(status != 0) printf("CANbus error can_rxw_high\n");
  }

can_rxw_low()
  { long status;
    status = can_3in1set(0x9400);
    if(status != 0) printf("CANbus error can_rxw_low\n");
  }

can_bkld_high()
  { long status;
    status = can_3in1set(0x9801);
    if(status != 0) printf("CANbus error can_bkld_high\n");
  }

can_bkld_low()
  { long status;
    status = can_3in1set(0x9800);
    if(status != 0) printf("CANbus error can_bkld_low\n");
  }

can_load_out()
  { long status;
    status = can_3in1set(0x9c00);
    if(status != 0) printf("CANbus error can_load_out\n");
  }

can_intg_rd_high()
  { can_send_3in1(0,1); }

can_intg_rd_low()
  { can_send_3in1(0,0); }

can_itr_high()
  { can_send_3in1(1,1); }

can_itr_low()
  { can_send_3in1(1,0); }

can_mse_high()
  { can_send_3in1(3,1); }

can_mse_low()
  { can_send_3in1(3,0); }

can_intg_gain(i)
int i;
   { can_send_3in1(2,i); }

can_dac(i)
int i;
   { can_send_3in1(6,i); }

can_small_cap_enable()
   { can_send_3in1(4,1); }

can_small_cap_disable()
   { can_send_3in1(4,0); }

can_large_cap_enable()
   { can_send_3in1(5,1); }

can_large_cap_disable()
   { can_send_3in1(5,0); }

can_trig_enable()
   { can_send_3in1(7,1); }

can_trig_disable()
   { can_send_3in1(7,0); }

can_send_3in1(code,data)
int code,data;
   { int i1,i2;
     long status;
     i1 = 0xe0 | (code <<2)  | (data>>8) & 3;
     i2 = data & 0xff;
     status = can_3in1set(i1,i2);
     if(status != 0) printf("CANbus error can_tp_high\n");
   }

can_init()
{  int i,j;
   char version[8];
   unsigned short dat;
   long stat;
   Base = 0;
   reset_ctrl_can();    /* set up the TIP810 canbus controller */
   reset_can(0);
   tsleep(0x80000005);
   reset_can(1);
   sleep(1);
   reset_can(0);
   tsleep(0x80000005);
   printf("calling can_adc_init GO_FB\n");  
   can_adc_init(GO_FB,&version[0]);
   for (i=0;i<7;i++) printf("%c", version[i]);
   printf(" - VERSION %d\n", (int)version[7]);  
   tsleep(0x80000002);
}

reset_ctrl_can()
 { unsigned char *adr,stat;
   int k,sjw,brp,sam,tseg2,tseg1;
   int octp1,octn1,ocpol1,octp0,octn0,ocpol0,ocmode1,ocmode0;

   k = 0xffff6001; /* CONTROL REGISTER */
   adr = (unsigned char*) k;
   *adr = 0x01;  /*RESET HIGH*/
   
   k = 0xffff6009; /* ACCEPTANCE CODE REGISTER */
   adr = (unsigned char*) k;
   *adr = 0;  
   
   k = 0xffff600b; /* ACCEPTANCE MASK */
   adr = (unsigned char*) k;
   *adr = 0xff;  
   
   k = 0xffff600d; /* BTR0 */
   adr = (unsigned char*) k;
   sjw = 1;
   brp = 0;  
   *adr = (sjw<<6) | brp;  

   k = 0xffff600f; /* BTR1 */
   adr = (unsigned char*) k;
   sam = 0;
   tseg2 = 6;
   tseg1 = 7;
   *adr = (sam<<7) | (tseg2<<4) | tseg1;     /*BTR 1*/

   k = 0xffff6011;  /* OCR */
   adr = (unsigned char*) k;
   octp1 = 0;
   octn1 = 0;
   ocpol1 = 0;
   octp0 = 1;
   octn0 = 1;
   ocpol0 = 0;
   ocmode1 = 1;
   ocmode0 = 0;
   *adr = (octp1<<7) | (octn1<<6) | (ocpol1<<5) | (octp0<<4)
        | (octn0<<3) | (ocpol0<<2) | (ocmode1<<1) | ocmode0;  
 
   k = 0xffff6001;  /* COMMAND REGISTER */
   adr = (unsigned char*) k;
   *adr = 0;  /* Reset Request low */
   
}
can_adc_init(action,version)
 char action;
 char version[];
  { long status;
    long id;
    char dlc, rtr;
    char data[8];
    int i;
    unsigned char stat,*adr;

/* Send the init command  */
    data[0] = INIT;
    data[1] = action;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    can_tx(Base+1, 2, 0 , data);

/* Wait for the adc acknowledgement  */
    status = can_rx(&id, &dlc, &rtr, version);  
/* Wait for the transition indicator  */
    status = can_rx(&id, &dlc, &rtr, data);
    printf(" trans id=%x  dlc=%x  rtr=%x\n",id,dlc,rtr); 
    printf(" trans data= %x %x %x %x %x %x %x %x\n",
        data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]); 
    printf(" trans data= %c %c %c %c %c %c %c %c\n",
        data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]); 
	
/* If Status equals 0 command was processed -> return 0
	   Otherwise there was a problem -> return -1   */
   adr = (unsigned char *)0xffff6005;
   stat = *adr;
/*   printf(" status at adc_init exit %x\n",stat);
   if(data[0] !=1) exit(0);
   if(data[1] != 101) exit(0);    */
}

can_tx(id,dlc,rtr,data)
int id;
char dlc,rtr;
char data[8];
 { int i;
   unsigned int k,l;
   unsigned char *adr,value,status;
/* check we are clear to put data in the xmit buffer */
   adr = (unsigned char *)0xffff6005;
busy:
   status = *adr;
/*   printf("TX status=%x\n",status);  */
   if( (status&0xc) == 0) goto busy;
   tsleep(0x80000002);
   adr = (unsigned char *)0xffff6015;
   value = (id >> 3);
   *adr = value;
   /* printf(" sending adr=%x  *adr= %x\n",(int)adr,value);  */  
   adr = (unsigned char *)0xffff6017;
   value = (id << 5) | ((rtr&1)<<4) | (dlc&0xf);
   *adr = value;
   /* printf(" sending adr=%x  *adr= %x\n",(int)adr,value);  */  
   k = 0xffff6019;
   for(i=0;i<8;i++)
    {adr = (unsigned char *)k;
     *adr = data[i];
     /* printf(" sending adr=%x  *adr= %x   data=%x\n",
          (int)adr,*adr,(int)data[i]);  */
     k = k+2;
    }

   adr = (unsigned char *)0xffff6003;
   *adr = 1;   /* send it */
   tsleep(0x80000002);
   l=0;
loop:
   l++;
   adr = (unsigned char *)0xffff6005;
   status = *adr;
/*   if(l<10) printf("complete status=%x\n",status);    */  
   if((status&8) == 0) goto loop;  /* wait for xmit complete */
   tsleep(0x80000002);
   return(0);
   }
can_rx(id,dlc,rtr,data)
int *id;
char *dlc,*rtr;
char data[8];
 { int i,k;
   long status;
   unsigned int kk;
   unsigned char dat1,dat2;
   unsigned char *adr;

   adr = (unsigned char *)0xffff6005;
wait_rx:
   status = *adr;
   if((status&1) == 0) goto wait_rx;  /* wait for message available */

   adr = (unsigned char *)0xffff6029;
   dat1 = *adr;
   adr = (unsigned char *)0xffff602d;
   dat2 = *adr;
   *id = (dat1<<3) | (dat2>>5);
   *rtr = (dat2>>4) & 1;
   k = (dat2 & 0xf);
   *dlc = k;
   kk = 0xffff602d;
   for(i=0;i<k;i++)
    { adr = (unsigned char *)kk;
      data[i] = *adr;
      kk = kk+2;  
    }
   adr = (unsigned char *)0xffff6003;
   *adr = 4;  /* release read buffer */
   tsleep(0x80000002);
   return(0);
}

long can_3in1set(dat)
unsigned short dat;
{
	long status,stat;
	long id;
	char dlc, rtr;
	unsigned char data[8];

	/* Send the command  */
	data[0] = CANSET;
	data[1] = (unsigned char)( dat &0x00FF );
	data[2] = (unsigned char)( (dat >> 8 )&0x00FF );
	can_tx(Base+1, 3, 0, data);

	/* Wait for the acknowledgement  */
	/* if we don't get ACK then status is set to -1  */
	status = can_rx(&id, &dlc, &rtr, &data[0]);
	stat = 0;
	if (data[0] != ACK) 
	  { stat = -1;
	    printf(" ACK  id=%x  dlc=%x  rtr=%x\n",id,dlc,rtr);
	    printf(" data= %x %x %x %x %x %x %x %x\n",
            data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]); 
          }

	/* If the command was written and acknowledged properly, return 0
	   Otherwise return -1  */
        if(stat != 0) printf("can_3in1set error status=%x\n",status);
        return(stat);
}

/* Read a 3in1 pattern form the mother board epld  */

long can_3in1get(dat)
unsigned short *dat;
{
	long status,stat;
	long id;
	char dlc, rtr;
	char data[8];
	unsigned short var1, var2;

	/* Send the read command  */
	data[0] = CANGET;
	can_tx(Base+1, 1, 0, data);
	/* Get back the result  */
	status = can_rx(&id, &dlc, &rtr, data);
	/* If we don't get back a REQUEST command  */
	/* If we don't get back the right parameter code  */
	/*		then error is set to 1  */
	stat = 0;
	if (data[0] != CANGET) stat = 1;
	
	/* Format the returned parameter  */
	var1 = ((unsigned short)data[1])&0x00FF;
	var2 = ((unsigned short)data[2])&0x00FF;
	*dat = (var1 << 8) + var2;

	/* Returns 0 if all was OK
	   Returns 1 if the command or code parameter that were received back are wrong
	   Returns -1 if there was a communication error  */
        if(stat != 0) printf("can_3in1get error status=%x  %x\n",status,stat);
        return(stat);
}

/*  Do a single convertion
    The ADC shifts into the mother board epld the formatted pattern 
    to select the card then waits for DELAY
    then performs the conversion and send back the result  */

long adc_convert(zone,sector,card,result)
  char zone,sector,card;
  unsigned short *result;
  { long status;
    long id;
    char dlc, rtr;
    char data[8];
    char d1, d2;
    unsigned short pattern;

    pattern = (((unsigned short)zone)<<13) + (((unsigned short)sector)<<6) + ((unsigned short)card);

/* Send the convert command */
    data[0] = CONVERT;
    data[1] = (unsigned char)( (pattern >> 8) &0x00FF );
    data[2] = (unsigned char)(  pattern       &0x00FF );
    status  = can_tx(Base+1, 3, 0, data);

    d1 = data[1];
    d2 = data[2];
   if(status != 0) 
     { printf(" convert send error status=%x\n",status);
       return(-1); }
/* Get back the result  */
    status = can_rx(&id, &dlc, &rtr, data);
/* If we don't get back a REQUEST command
   If we don't get back the right parameter code
      then error is set to 1  */

   if(status != 0) 
      { printf(" convert back error status=%x\n",status);
        return(-1); }
   if( (data[0] != CONVERT) || (data[1] != d1) ) status=1;
	
/* Format the returned parameter  */
    *result = ((unsigned short)data[3])*256 + ((unsigned short)data[4]);
	
/* Returns 0 if all was OK
   Returns 1 if the command or code parameter that were received back are wrong
   Returns -1 if there was a communication error  */

   return(status);
}

/* set up a programable delay
    The delay is placed between the card select operation and the 
    adc trigger for any adc convertion request
    DELAY = 0 gives 8 usec delay
    Each increment of DELAY gives 8 additional usecs delay
    Note that in automatic scan mode, DELAY should not exceed 6.
    For other modes, any value can be set  */

long adc_delay(delay)
unsigned short delay;
  {	long status;
	long id;
	char dlc, rtr;
	char data[8];

	/* Send the command */
	data[0] = DELAY;
	data[1] = (unsigned char)( (delay >> 8) &0x00FF );
	data[2] = (unsigned char)(  delay       &0x00FF );
	status  = can_tx(Base+1, 3, 0, data);
   if(status != 0) 
     { printf(" set delay send error status=%x\n",status);
       return(-1); }

	/* Wait for the acknowledgement 
	   if we don't get ACK then status is set to -1  */
	status = can_rx(&id, &dlc, &rtr, data);
   if(status != 0) 
     { printf(" set delay ACK error status=%x\n",status);
       return(-1); }
	if (data[0] != ACK) status = -1;

	/* If the command was written and acknowledged properly, return 0
	   Otherwise return -1 */

	return(status);
}

long adc_get_delay(delay)
unsigned short *delay;
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	/* Send the read command */
	data[0] = REQUEST;
	data[1] = DELAY;
	status  = can_tx(Base+1, 2, 0, data);
   if(status != 0) 
     { printf(" set delay read error status=%x\n",status);
       return(-1); }
	/* Get back the result */
	status = can_rx(&id, &dlc, &rtr, data);
   if(status != 0) 
     { printf(" set delay read REQ error status=%x\n",status);
       return(-1); }
	/* If we don't get back a REQUEST command
	   If we don't get back the right parameter code
			then error is set to 1  */
	if ((data[0] != REQUEST) || (data[1] != DELAY)) status = 1;
	
	/* Format the returned parameter */
	*delay = (((unsigned short)data[2])*256) + ((unsigned short)data[3]);

	/* Returns 0 if all was OK
	  Returns 1 if the command or code parameter that were received back are wrong
	  Returns -1 if there was a communication error */

	return(status);
}

/* Set up the DAC that controls the global pedestal
   DAC=0 sets the maximum pedestal (0.5V)
   DAC=255 sets the minimum pedestal (-0.5V)
   Note that this parameter cannot be read back  */

long adc_dacset(dac)
unsigned char dac;
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	/* Send the command  */
	data[0] = DACSET;
	data[1] = dac;
	status  = can_tx(Base+1, 2, 0, data);
   if(status != 0) 
     { printf(" set set ADC dac send error status=%x\n",status);
       return(-1); }

	/* Wait for the acknowledgement
	   if we don't get ACK then status is set to -1 */
	status = can_rx(&id, &dlc, &rtr, data);
   if(status != 0) 
     { printf(" set ADC dac ACK error status=%x\n",status);
       return(-1); }
	if (data[0] != ACK) status = -1;

	/* If the command was written and acknowledged properly, return 0
	   Otherwise return -1  */

	return(status);	
}


/*-----------------------------------------------*/
/* test module functions                         */
/*-----------------------------------------------*/
sw(i)
int i;
{ int k,l,shift;
  unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xf0aa000c;
  k = *adr;
  shift = i-1;
  l = (k>>shift) & 1;
  return(l);
}
latch()
{ int k,l;
  unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xf0aa000c;
  k = *adr;
  l = (k>>15) & 1;
  return(l);
}
setime(c1,f1,c2,f2)
int c1,f1,c2,f2;
{unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xf0aa000a;
 *adr = (unsigned short)f1;               /* latch fine timer 1 value */
  (unsigned short *)adr = (unsigned short *)0xf0aa0012;
 *adr = (unsigned short)f2;             /* latch fine timer 2 value */
  (unsigned short *)adr = (unsigned short *)0xf0aa0008;
 *adr = (unsigned short)c1;               /* write coarse timer 1 to latch */
  (unsigned short *)adr = (unsigned short *)0xf0aa0010;
 *adr = (unsigned short)c2;               /* write coarse timer 1 to latch */
  (unsigned short *)adr = (unsigned short *)0xf0aa0004;
 *adr = 0;                        /* load fine timers with latched values */
  (unsigned short *)adr = (unsigned short *)0xf0aa0000;
 *adr = 0;                        /* xfer latched to counters (coarse) */
}
restim()
{unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xf0aa0006;
 *adr = 0;              /* clear fine timer done bits */
  (unsigned short *)adr = (unsigned short *)0xf0aa0000;
 *adr = 0;              /* reset timers back to latched values */
}
stvme()
{unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xf0aa000e;
 *adr = 0;              /* start the timers from a vme null write */
}
rlatch()
{unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xf0aa0002;
 *adr = 0;              /* reset the latch bit */
}

cardchk(k)
int k;
  {int i,status;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
   status=0;
   multi_low();
   tsleep(0x80000002);
   back_low();
   tsleep(0x80000002);
   set_tube(k);
   tsleep(0x80000002);
   set_intg_gain(5);   /* s1=s3=1 s2=s4=0 */
   tsleep(0x80000002);
   itr_high();           /* itr=1 */
   tsleep(0x80000002);
   intg_rd_low();        /* ire=0 */
   tsleep(0x80000002);
   mse_high();           /* mse=1 */
   tsleep(0x80000002);
   small_cap_disable();  /* tplo=0 */
   tsleep(0x80000002);
   large_cap_enable();   /* tphi=1 */
   tsleep(0x80000002);
   trig_disable();       /* trig=0 */
   tsleep(0x80000002);
   can_bkld_high();
   tsleep(0x80000002);
   can_bkld_low();
   tsleep(0x80000002);
   status=can_3in1set(0x9c00);   /* fetch from 3in1 and load can output reg */
   tsleep(0x80000005);
   status=can_3in1get(&shrt);
   tsleep(0x80000005);
   i = (int)shrt;
   trig = i & 1;
   s1 = (i>>1) & 1;
   s2 = (i>>2) & 1;
   s3 = (i>>3) & 1;
   s4 = (i>>4) & 1;
   itr = (i>>5) & 1;
   ire = (i>>6) & 1;
   mse = (i>>7) & 1;
   tplo = (i>>8) & 1;
   tphi = (i>>9) & 1;
   if((s1==1) && (s2==0)  && (s3==1) && (s4==0) && (itr==1) &&
      (ire==0) && (mse==1) && (tplo==0) && (tphi==1) && 
      (trig==0) ) status=1;
   return(status);
  }
