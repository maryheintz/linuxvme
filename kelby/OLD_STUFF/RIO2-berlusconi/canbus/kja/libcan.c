#include <types.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#define CANSET 11
#define CANGET 12
#define ACK 51
unsigned swch=0; /* value of dip switch on integrator adc card */
int Base;
time_t usec;
unsigned int base = 0xffff0000;

reset_can()
 { unsigned short *adr;
   int k,sjw,brp,sam,tseg2,tseg1;
   int octp1,octn1,ocpol1,octp0,octn0,ocpol0,ocmode1,ocmode0;

   printf("entered reset_can\n");
   Base=(swch<<6);

   k = 0xffff6001; /* CONTROL REGISTER */
   adr = (unsigned short*) k;
   *adr = 0x01;  /*RESET HIGH*/
   
   k = 0xffff600b; /* ACCEPTANCE MASK */
   adr = (unsigned short*) k;
   *adr = 0xff;
   
   k = 0xffff600b; /* ACCEPTANCE CODE REGISTER */
   adr = (unsigned short*) k;
   *adr = 0;
   
   k = 0xffff600d; /* BTR0 */
   adr = (unsigned short*) k;
   sjw = 1;
   brp = 0;
   *adr = (sjw<<6) | brp;  

   k = 0xffff600f; /* BTR1 */
   adr = (unsigned short*) k;
   sam = 0;
   tseg2 = 6;
   tseg1 = 7;
   *adr = (sam<<7) | (tseg2<<4) | tseg1;  /*BTR 1*/

   k = 0xffff6011;  /* OCR */
   adr = (unsigned short*) k;
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
   adr = (unsigned short*) k;
   *adr = 0x0;  /* Reset Request low */
   printf("returning from reset_can\n");
}

can_tx(id,dlc,rtr,data)
int id;
char dlc,rtr;
char data[8];
 { int i;
   unsigned int k;
   unsigned short*adr;
   adr = (unsigned short*)0xffff6015;
   *adr = (id >> 3);
   adr = (unsigned short*)0xffff6017;
   *adr = (id << 5) | ((rtr&1)<<4) | (dlc&0xf);
   k = 0xffff6019;
   for(i=0;i<dlc;i++)
    {adr = (unsigned short*)k;
     *adr = data[i];
     k = k+2;
    }
   adr = (unsigned short*)0xffff6003;
   *adr = 0x01;
}
can_rx(id,dlc,rtr,data)
int *id;
char *dlc,*rtr;
char data[8];
 { int i,k;
   unsigned int kk;
   unsigned char status,dat1,dat2;
   unsigned short *adr;
   adr = (unsigned short *)0xffff6005;
wait_rx:
   status = *adr;
   if(status&1 == 0) goto wait_rx;
   adr = (unsigned short *)0xffff6029;
   dat1 = *adr;
   adr = (unsigned short *)0xffff602b;
   dat2 = *adr;
   *id = (dat1<<3) | (dat2>>5);
   *rtr = (dat2>>4) & 1;
   k = (dat2 & 0xf);
   *dlc = k;
   kk = 0xffff602f;
   for(i=0;i<k;i++)
    { adr = (unsigned short *)kk;
      data[i] = *adr;
      kk = kk+2;
    }
}


long can_3in1set(dat)
unsigned short dat;
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	/* Send the command  */
	data[0] = CANSET;
	data[1] = (unsigned char)( (dat >> 8) &0x00FF );
	data[2] = (unsigned char)(  dat       &0x00FF );
	status  = can_tx(Base+1, 3, 0, data);

	/* Wait for the acknowledgement  */
	/* if we don't get ACK then status is set to -1  */
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	if (status == 0) if (data[0] != ACK) status = -1;

	/* If the command was written and acknowledged properly, return 0
	   Otherwise return -1  */

	return status;
}

/* Read a 3in1 pattern form the mother board epld  */

can_3in1get(dat)
unsigned short *dat;
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];
	unsigned short var1, var2;

	/* Send the read command  */
	data[0] = CANGET;
	status  = can_tx(Base+1, 1, 0, data);
	/* Get back the result  */
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	/* If we don't get back a REQUEST command  */
	/* If we don't get back the right parameter code  */
	/*		then error is set to 1  */
	if (status == 0) if (data[0] != CANGET) status = 1;
	
	/* Format the returned parameter  */
	var1 = ((unsigned short)data[1])&0x00FF;
	var2 = ((unsigned short)data[2])&0x00FF;
	*dat = (var1 << 8) + var2;

	/* Returns 0 if all was OK
	   Returns 1 if the command or code parameter that were received back are wrong
	   Returns -1 if there was a communication error  */

	return status;
}
can_swln1(i)
int i;
  { can_set(0xac,i); }

can_swln2(i)
int i;
  { can_set(0xb4,i); }

can_tp_high()
  { can_set(0x80,1); }

can_tp_low()
  { can_set(0x80,0); }

can_zone(zone)
int zone;
  { can_set(0x84,zone); }

can_sector(sector)
int sector;
  { can_set(0x88,sector); }

can_tube(tube)
int tube;
  { can_set(0x8c,tube); }

can_multi_high()
  { can_set(0x90,0xff); }

can_multi_low()
  { can_set(0x90,0); }

can_rxw_high()
  { can_set(0x94,1); }

can_rxw_low()
  { can_set(0x94,0); }

can_bkld_high()
  { can_set(0x98,1); }

can_bkld_low()
  { can_set(0x98,0); }

can_load_out()
  { can_set(0x9c,0); }

can_reset_state()
  { can_set(0xa0,0); }

can_reset()
  { can_set(0xa4,0); }

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
     i1 = 0xe0 | (code <<2)  | (data>>8) & 3;
     i2 = data & 0xff;
     can_set(i1,i2);
/*     if(sw(8) == 1) printf(" subadr=%x  data=%x\n",i1,i2);  */
   }

can_set(subadr,data)
int subadr,data;
  { unsigned short i16;
    int i;
    i16 = (subadr<<8) | data;
/*    if(sw(8) == 1) printf(" can_set i16=%x\n",(int)i16);  */
    can_3in1set(i16);
  }   

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
