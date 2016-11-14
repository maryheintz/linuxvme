#define CANSET 11
#define CANGET 12
#define ACK 51
#define GO_FB 101
#define GO_ISP 102
#define INIT 1
#include <stdio.h>
#include <math.h>
#include <time.h>
/* #define swch 0  */    /* value of dip switch on integrator adc card */
/* static int Base={swch<<6}; */
int Base;
;
time_t usec;
long can_3in1set(unsigned short);
long can_3in1get(unsigned short *);
main()
{ 
   int i,j;
   char version[8];
   unsigned short dat;
   long stat;
/*   extern long can_3in1get();  */
   Base = 0;
   ttcvi_map();
   printf("calling reset_can\n");
/*   reset_can(1);  */ /* set can reset bit on via TTC */
   reset_ctrl_can();    /* set up the TIP810 canbus controller */
   j=0;


   printf("calling can_adc_init GO_FB\n");  
   can_adc_init(GO_FB,&version[0]);
   for (i=0;i<7;i++) printf("%c", version[i]);
   printf(" - VERSION %d\n", (int)version[7]);  
   usleep(1000);

   stat = can_3in1set(0x8400);  /* set zone 0 */
loop:
   usleep(1000);
   stat = can_3in1set(0x8800);  /* set sector 0 */
   usleep(1000);
   can_3in1set(0x8801);  /* set sector 1 */
   usleep(1000);
   stat = can_3in1get(&dat);
   printf("3in1_get data=%x  stat=%x\n",dat,stat);
   goto loop;  
}

reset_ctrl_can()
 { unsigned char *adr,stat;
   int k,sjw,brp,sam,tseg2,tseg1;
   int octp1,octn1,ocpol1,octp0,octn0,ocpol0,ocmode1,ocmode0;


   k = 0xdffe6001; /* CONTROL REGISTER */
   adr = (unsigned char*) k;
   *adr = 0x01;  /*RESET HIGH*/
   
   k = 0xdffe6009; /* ACCEPTANCE CODE REGISTER */
   adr = (unsigned char*) k;
   *adr = 0;  
   
   k = 0xdffe600b; /* ACCEPTANCE MASK */
   adr = (unsigned char*) k;
   *adr = 0xff;  
   
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
   *adr = (sam<<7) | (tseg2<<4) | tseg1;     /*BTR 1*/

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
   adr = (unsigned char *)0xdffe6005;
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
   adr = (unsigned char *)0xdffe6005;
busy:
   status = *adr;
/*   printf("TX status=%x\n",status);  */
   if( (status&0xc) == 0) goto busy;
   usleep(2);
   adr = (unsigned char *)0xdffe6015;
   value = (id >> 3);
   *adr = value;
   /* printf(" sending adr=%x  *adr= %x\n",(int)adr,value);  */  
   adr = (unsigned char *)0xdffe6017;
   value = (id << 5) | ((rtr&1)<<4) | (dlc&0xf);
   *adr = value;
   /* printf(" sending adr=%x  *adr= %x\n",(int)adr,value);  */  
   k = 0xdffe6019;
   for(i=0;i<8;i++)
    {adr = (unsigned char *)k;
     *adr = data[i];
     /* printf(" sending adr=%x  *adr= %x   data=%x\n",
          (int)adr,*adr,(int)data[i]);  */
     k = k+2;
    }

   adr = (unsigned char *)0xdffe6003;
   *adr = 1;   /* send it */
   usleep(2);
   l=0;
loop:
   l++;
   adr = (unsigned char *)0xdffe6005;
   status = *adr;
/*   if(l<10) printf("complete status=%x\n",status);    */  
   if((status&8) == 0) goto loop;  /* wait for xmit complete */
   usleep(2);
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

   adr = (unsigned char *)0xdffe6005;
wait_rx:
   status = *adr;
   if((status&1) == 0) goto wait_rx;  /* wait for message available */

   adr = (unsigned char *)0xdffe6029;
   dat1 = *adr;
   adr = (unsigned char *)0xdffe602b;
   dat2 = *adr;
   *id = (dat1<<3) | (dat2>>5);
   *rtr = (dat2>>4) & 1;
   k = (dat2 & 0xf);
   *dlc = k;
   kk = 0xdffe602d;
   for(i=0;i<k;i++)
    { adr = (unsigned char *)kk;
      data[i] = *adr;
      kk = kk+2;  
    }
   adr = (unsigned char *)0xdffe6003;
   *adr = 4;  /* release read buffer */
   usleep(2);
   return(status);
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


