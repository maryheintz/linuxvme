#define CANSET 11
#define CANGET 12
#define ACK 51
#include <stdio.h>
unsigned swch=1; /* value of dip switch on integrator adc card */
int Base;

reset_can()
 { unsigned char *adr;
   int k,sjw,brp,sam,tseg2,tseg1;
   int octp1,octn1,ocpol1,octp0,octn0,ocpol0,ocmode1,ocmode0;

   Base=(swch<<6);

   k = 0xdffe6001; /* CONTROL REGISTER */
   adr = (unsigned char*) k;
   *adr = 0x01;  /*RESET HIGH*/
   
   k = 0xdffe600b; /* ACCEPTANCE MASK */
   adr = (unsigned char*) k;
   *adr = 0xff;
   
   k = 0xdffe600b; /* ACCEPTANCE CODE REGISTER */
   adr = (unsigned char*) k;
   *adr = 0;
   
   k = 0xdffe600d; /* BTR0 */
   adr = (unsigned char*) k;
   sjw = 1;
   brp = 0;
   *adr = (sjw<<6) | brp;  

   k = 0xdffe600f; /* BTR1 */
   adr = (unsigned char*) k;
   sam = 0;
   tseg2 = 6;
   tseg1 = 7;
   *adr = (sam<<7) | (tseg2<<4) | tseg1;  /*BTR 1*/

   k = 0xdffe6011;  /* OCR */
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
 
   k = 0xdffe6001;  /* COMMAND REGISTER */
   adr = (unsigned char*) k;
   *adr = 0x0;  /* Reset Request low */
}

can_tx(id,dlc,rtr,data)
int id;
char dlc,rtr;
char data[8];
 { int i;
   unsigned int k;
   unsigned char *adr;
   adr = (unsigned char *)0xdffe6015;
   *adr = (id >> 3);
   adr = (unsigned char *)0xdffe6017;
   *adr = (id << 5) | ((rtr&1)<<4) | (dlc&0xf);
   k = 0xdffe6019;
   for(i=0;i<dlc;i++)
    {adr = (unsigned char *)k;
     *adr = data[i];
     k = k+2;
    }
   adr = (unsigned char *)0xdffe6003;
   *adr = 0x01;
}
can_rx(id,dlc,rtr,data)
int *id;
char *dlc,*rtr;
char data[8];
 { int i,k;
   unsigned int kk;
   unsigned char status,dat1,dat2;
   unsigned char *adr;
   adr = (unsigned char *)0xdffe6005;
wait_rx:
   status = *adr;
   if(status&1 == 0) goto wait_rx;
   adr = (unsigned char *)0xdffe6029;
   dat1 = *adr;
   adr = (unsigned char *)0xdffe602b;
   dat2 = *adr;
   *id = (dat1<<3) | (dat2>>5);
   *rtr = (dat2>>4) & 1;
   k = (dat2 & 0xf);
   *dlc = k;
   kk = 0xdffe602f;
   for(i=0;i<k;i++)
    { adr = (unsigned char *)kk;
      data[i] = *adr;
      kk = kk+2;
    }
}


long adc_3in1set(dat)
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

adc_3in1get(dat)
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
