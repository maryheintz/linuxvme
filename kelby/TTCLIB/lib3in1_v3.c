#define IDALLOC 107
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
#include <string.h>
#include <math.h>
#include "vme_base.h"
#include "cberr.h"
int lwait;
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

     tp_high(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* set tp high                                   */
/*-----------------------------------------------*/
      { SendLongCommand(ttcadr,1,0xc0,0x00); }

     tp_low(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* set tp low                                    */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xc0,0x01); }
 
     tp_low_bc()     
/*-----------------------------------------------*/
/* set tp low with broadcast command             */
/*-----------------------------------------------*/
     { BroadcastShortCommand(0xc0); }
 
     tp_high_bc()     
/*-----------------------------------------------*/
/* set tp high with broadcast command            */
/*-----------------------------------------------*/
     { BroadcastShortCommand(0xc4); }
 

     set_tube(ttcadr,tube)     
/*-----------------------------------------------*/
/* set the tube field                            */
/*-----------------------------------------------*/
     int ttcadr,tube;
     { SendLongCommand(ttcadr,1,0xc4,tube); }

     multi_high(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* set multi_sel high                            */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xc8,0); }

     multi_low(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* set multi_sel low                             */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xc8,1); }

     rxw_high(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* set rxw high                                  */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xcc,1); }

     rxw_low(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* set rxw low                                   */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xcc,0); }

     back_high(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* set back_load high                            */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xd0,1); }

     back_low(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* set back_load low                             */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xd0,0); }

     load_can(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* load the CAN output register                  */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xd4,0); }

     reset_state(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* reset the state machines                      */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xd8,0); }

     reset_can(ttcadr,i)
     int ttcadr,i;     
/*-----------------------------------------------*/
/* send reset signal to integrator card          */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xdc,i); }

     intg_rd_high(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* set intg_rd high                              */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xe0,1); }

     intg_rd_low(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* set intg_rd low                              */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xe0,0); }

     itr_high(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* set itr high                              */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xe4,1); }

     itr_low(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* set itr low                              */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xe4,0); }

     set_intg_gain(ttcadr,i)     
/*-----------------------------------------------*/
/* set intg gain switches s1-s4                  */
/*-----------------------------------------------*/
     unsigned int ttcadr,i;
     { unsigned int adr,data;
       adr = 0xe8;
       data = i & 0xf;
       SendLongCommand(ttcadr,1,adr,data); 
     }

     mse_high(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* set mse high                              */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xec,1); }

     mse_low(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* set mse low                              */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xec,0); }

     small_cap_enable(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* enable small capacitor calibration            */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xf0,1); }

     small_cap_disable(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* disable small capacitor calibration           */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xf0,0); }


     large_cap_enable(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* enable large capacitor calibration            */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xf4,1); }

     large_cap_disable(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* disable large capacitor calibration           */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xf4,0); }

     set_dac(ttcadr,i)     
/*-----------------------------------------------*/
/* set dac                                       */
/*-----------------------------------------------*/
     unsigned int ttcadr,i;
     { unsigned int subadr,data;
       subadr = 0xf8 | ((i>>8) & 0x3);
       data = i & 0xff;
       SendLongCommand(ttcadr,1,subadr,data);
     }

     trig_enable(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* enable trigger output                         */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xfc,1); }

     trig_disable(ttcadr)     
     int ttcadr;
/*-----------------------------------------------*/
/* disable trigger output                        */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xfc,0); }

     set_mb_time(ttcadr,l,i)
/*-----------------------------------------------*/
/* set timer on board l to 8-bit value i         */
/*-----------------------------------------------*/
     unsigned int ttcadr,l,i;
     { if(l == 1)
        { set_tube(ttcadr,0x38);
          SendLongCommand(ttcadr,1,0xf8,i);  
          set_tube(ttcadr,0x39); }
       if(l == 2)
        { set_tube(ttcadr,0x3a);
          SendLongCommand(ttcadr,1,0xf8,i);  
          set_tube(ttcadr,0x3b); }
       if(l == 3)
        { set_tube(ttcadr,0x3c);
          SendLongCommand(ttcadr,1,0xf8,i);  
          set_tube(ttcadr,0x3d); }
       if(l == 4)
        { set_tube(ttcadr,0x3e);
          SendLongCommand(ttcadr,1,0xf8,i);  
          set_tube(ttcadr,0x3f); }
     }
/*-----------------------------------------------*/
/* CANbus commands - via the integrator ADC card */
/*-----------------------------------------------*/
long can_3in1set(id,dat)
int id;
unsigned short dat;
{
	long status,stat;
	long idrd;
	unsigned char dlc, rtr;
	unsigned char data[8];

	/* Send the command  */
	data[0] = CANSET;
	data[1] = (unsigned char)( (dat >> 8 )&0x00FF );
	data[2] = (unsigned char)( dat &0x00FF );  
/*	data[2] = (unsigned char)( (dat >> 8 )&0x00FF );
	data[1] = (unsigned char)( dat &0x00FF );  */
	can_tx(id, 3, 0, data);

	/* Wait for the acknowledgement  */
	/* if we don't get ACK then status is set to -1  */
	status = can_rx(&idrd, &dlc, &rtr, &data[0]);
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

long can_3in1get(id,dat)
int id;
unsigned short *dat;
{
	long status,stat;
	long idrd;
	unsigned char dlc, rtr;
	unsigned char data[8];
	unsigned short var1, var2;

	/* Send the read command  */
	data[0] = CANGET;
	can_tx(id, 1, 0, data);
	lwait = kjawait(100); /*1 usleep(0); */
	/* Get back the result  */
	status = can_rx(&idrd, &dlc, &rtr, data);
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

can_tp_high(id)
int id;
  { long status;
    status = can_3in1set(id,0xc001);
    if(status != 0) printf("CANbus error can_tp_high\n");
  }

can_tp_low(id)
int id;
  { long status;
    status = can_3in1set(id,0xc000);
    if(status != 0) printf("CANbus error can_tp_low\n");
  }

can_tube(id,tube)
int id,tube;
  { long status;
    unsigned short dat;
    dat = 0xc400 | tube;
    status = can_3in1set(id,dat);
    if(status != 0) printf("CANbus error can_tube\n");
  }

can_multi_high(id)
int id;
  { long status;
    status = can_3in1set(id,0xc800);
    if(status != 0) printf("CANbus error can_multi_high\n");
  }

can_multi_low(id)
int id;
  { long status;
    status = can_3in1set(id,0xc8ff);
    if(status != 0) printf("CANbus error can_multi_low\n");
  }

can_rxw_high(id)
int id;
  { long status;
    status = can_3in1set(id,0xcc01);
    if(status != 0) printf("CANbus error can_rxw_high\n");
  }

can_rxw_low(id)
int id;
  { long status;
    status = can_3in1set(id,0xcc00);
    if(status != 0) printf("CANbus error can_rxw_low\n");
  }

can_bkld_high(id)
int id;
  { long status;
    status = can_3in1set(id,0xd001);
    if(status != 0) printf("CANbus error can_bkld_high\n");
  }

can_bkld_low(id)
int id;
  { long status;
    status = can_3in1set(id,0xd000);
    if(status != 0) printf("CANbus error can_bkld_low\n");
  }

can_load_out(id)
int id;
  { long status;
    status = can_3in1set(id,0xd400);
    if(status != 0) printf("CANbus error can_load_out\n");
  }

can_intg_rd_high(id)
int id;
  { can_send_3in1(id,0,1); }

can_intg_rd_low(id)
int id;
  { can_send_3in1(id,0,0); }

can_itr_high(id)
int id;
  { can_send_3in1(id,1,1); }

can_itr_low(id)
int id;
  { can_send_3in1(id,1,0); }

can_mse_high(id)
int id;
  { can_send_3in1(id,3,1); }

can_mse_low(id)
int id;
  { can_send_3in1(id,3,0); }

can_intg_gain(id,i)
int id,i;
   { can_send_3in1(id,2,i); }

can_dac(id,i)
int id,i;
   { can_send_3in1(id,6,i); }

can_small_cap_enable(id)
int id;
   { can_send_3in1(id,4,1); }

can_small_cap_disable(id)
int id;
   { can_send_3in1(id,4,0); }

can_large_cap_enable(id)
int id;
   { can_send_3in1(id,5,1); }

can_large_cap_disable(id)
int id;
   { can_send_3in1(id,5,0); }

can_trig_enable(id)
int id;
   { can_send_3in1(id,7,1); }

can_trig_disable(id)
int id;
   { can_send_3in1(id,7,0); }

can_send_3in1(id,code,data)
int id,code,data;
   { unsigned int i1,i2,kk;
     long status;
     i1 = 0xe0 | (code <<2)  | ((data>>8) & 3);
     i2 = data & 0xff;
     kk = (i1<<8) | i2;
     status = can_3in1set(id,kk);
     if(status != 0) printf("CANbus error can_tp_high\n");
   }

can_reset(ttcadr)
int ttcadr;
  {reset_can(ttcadr,0);
   usleep(10);  /* kja */
   reset_can(ttcadr,1);
   usleep(10);
   reset_can(ttcadr,0);
/*   usleep(1000000); */
   sleep(1);
  }
    
can_init(board,serial)
int board;
unsigned char serial[6];
{  int i,j,id;
   unsigned char version[8];
   unsigned short dat;
   long stat,status;
   unsigned char data[8],datarx[8];
   unsigned char dlc,rtr;
   int idrd,id_adc;
/*   printf("serial=%s  board=%d\n",serial,board); */
   id = (board<<6) | 1;
   data[0] = IDALLOC;
   data[1] = serial[0];
   data[2] = serial[1];
   data[3] = serial[2];
   data[4] = serial[3];
   data[5] = serial[4];
   data[6] = serial[5];
   data[7] = board;
   can_tx(0,8,0,data);
/*   printf("can_tx completed\n"); */
   status=can_rx(&idrd, &dlc, &rtr, datarx);
/*   printf("can rx complete\n"); */ 
   id_adc = datarx[7];
/*   printf("id adc = %x\n", id_adc);  */

   printf("calling can_adc_init GO_FB\n");
   can_adc_init(id,GO_FB,&version[0]);
   for (i=0;i<7;i++) printf("%c", version[i]);
   printf(" - VERSION %d\n", (int)version[7]);  
   usleep(20);
}

reset_ctrl_can()
 { unsigned char *adr,stat;
   unsigned int k,sjw,brp,sam,tseg2,tseg1;
   unsigned int octp1,octn1,ocpol1,octp0,octn0,ocpol0,ocmode1,ocmode0;

   k = vme16base + 0x6001; /* CONTROL REGISTER */
   adr = (unsigned char *) k;
   *adr = 0x01;  /*RESET HIGH*/
   usleep(1);
      
   k = vme16base + 0x6009; /* ACCEPTANCE CODE REGISTER */
   adr = (unsigned char *) k;
   *adr = 0;  
   usleep(1); 
     
   k = vme16base + 0x600b; /* ACCEPTANCE MASK */
   adr = (unsigned char *) k;
   *adr = 0xff;  
   usleep(1);
   
   k = vme16base + 0x600d; /* BTR0 */
   adr = (unsigned char *) k;
   sjw = 1;
   brp = 0;  
   *adr = (sjw<<6) | brp;  
   usleep(1);

   k = vme16base + 0x600f; /* BTR1 */
   adr = (unsigned char *) k;
   sam = 0;
   tseg2 = 6;
   tseg1 = 7;
   *adr = (sam<<7) | (tseg2<<4) | tseg1;     /*BTR 1*/

   k = vme16base + 0x6011;  /* OCR */
   adr = (unsigned char *) k;
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
   usleep(1);
 
   k = vme16base + 0x6001;  /* COMMAND REGISTER */
   adr = (unsigned char *) k;
   *adr = 0;  /* Reset Request low */
   usleep(100000);
}
reset_ctrl2_can()
 { unsigned char *adr,stat;
   unsigned int k,sjw,brp,sam,tseg2,tseg1;
   unsigned int octp1,octn1,ocpol1,octp0,octn0,ocpol0,ocmode1,ocmode0;

   k = vme16base + 0x6001; /* CONTROL REGISTER */
   adr = (unsigned char *) k;
   *adr = 0x01;  /*RESET HIGH*/
   usleep(1);
   
   k = vme16base + 0x6009; /* ACCEPTANCE CODE REGISTER */
   adr = (unsigned char *) k;
   *adr = 0;  
   usleep(1);
   
   k = vme16base + 0x600b; /* ACCEPTANCE MASK */
   adr = (unsigned char *) k;
   *adr = 0xff;  
   usleep(1);
   
   k = vme16base + 0x600d; /* BTR0 */
   adr = (unsigned char *) k;
   sjw = 1;
   brp = 1;  
   *adr = (sjw<<6) | brp;  
   usleep(1);

   k = vme16base + 0x600f; /* BTR1 */
   adr = (unsigned char *) k;
   sam = 0;
   tseg2 = 6;
   tseg1 = 7;
   *adr = (sam<<7) | (tseg2<<4) | tseg1;     /*BTR 1*/
   usleep(1);

   k = vme16base + 0x6011;  /* OCR */
   adr = (unsigned char *) k;
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
   usleep(1);
 
   k = vme16base + 0x6001;  /* COMMAND REGISTER */
   adr = (unsigned char *) k;
   *adr = 0;  /* Reset Request low */
   usleep(1);
   
}
can_adc_init(id,action,version)
 int id;
 unsigned char action;
 unsigned char version[];
  { long status;
    long idrd;
    unsigned char dlc, rtr;
    unsigned char data[8];
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
    can_tx(id, 2, 0 , data);

/* Wait for the adc acknowledgement  */
    status = can_rx(&idrd, &dlc, &rtr, version);  
/* Wait for the transition indicator  */
    status = can_rx(&idrd, &dlc, &rtr, data);
/* If Status equals 0 command was processed -> return 0
	   Otherwise there was a problem -> return -1   */
   adr = (unsigned char *)(vme16base + 0x6005);
   stat = *adr;
   lwait = kjawait(10); /* 2 usleep(0); */
/*   rintf(" status at adc_init exit %x\n",stat);  */
/*   printf(" data[0]=%d\n",data[0]);  */
   cberr=0;
   if(data[0] !=1) 
     { /* printf(" canbus setup failure 0 - exit\n"); */
       cberr=1;;
     }
   if(data[1] != 101)  
     { /* printf(" canbus setup failure 101 - exit\n"); */
       cberr=1;;
     }
}

can_tx(id,dlc,rtr,data)
int id;
unsigned char dlc,rtr;
unsigned char data[8];
 { int i;
   unsigned int k,l;
   unsigned char *adr,value,status;
   // printf("can_tx id=%d  dlc=%d rtr=%d data=%d %d %d %d %d %d %d %d\n",
   //    id,dlc,rtr,data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
/* check we are clear to put data in the xmit buffer */
   adr = (unsigned char *)(vme16base + 0x6005);
	lwait = kjawait(10); /*3 usleep(0); */
busy:
   status = *adr;
/*  printf("TX status=%x\n",status); */
   if( (status&0xc) == 0) goto busy;
   adr = (unsigned char *)(vme16base + 0x6015);
	lwait = kjawait(10); /*4 usleep(0); */
   value = (id >> 3);
   *adr = value;
	lwait = kjawait(10); /*5 usleep(0); */
/* printf(" sending adr=%x  *adr= %x\n",(int)adr,value); */
   adr = (unsigned char *)(vme16base + 0x6017);
   value = (id << 5) | ((rtr&1)<<4) | (dlc&0xf);
   *adr = value;
	lwait = kjawait(10); /*6 usleep(0); */
/* printf(" sending adr=%x  *adr= %x\n",(int)adr,value); */
   k = vme16base + 0x6019;
   for(i=0;i<8;i++)
    {adr = (unsigned char *)k;
     *adr = data[i];
	lwait = kjawait(10); /*7 usleep(0); */
/* printf(" sending adr=%x  *adr= %x   data=%x\n",
          (int)adr,*adr,(int)data[i]);  */
     k = k+2;
    }

   adr = (unsigned char *)(vme16base + 0x6003);
   *adr = 1;   /* send it */
	lwait = kjawait(10); /*8 usleep(0); */
   l=0;
loop:
   l++;
   adr = (unsigned char *)(vme16base + 0x6005);
   status = *adr;
   lwait = kjawait(10); /*9 usleep(0); */
/* if(l<10) printf("complete status=%x\n",status); */
   if((status&8) == 0) goto loop;  /* wait for xmit complete */
   return(0);
   }
can_rx(id,dlc,rtr,data)
int *id;
unsigned char *dlc,*rtr;
unsigned char data[8];
 { int i,k,cnt;
   long status;
   unsigned int kk;
   unsigned char dat1,dat2;
   unsigned char *adr;

   adr = (unsigned char *)(vme16base + 0x6005);
   cnt=0;
wait_rx:
   status = *adr;
   usleep(0);
/* printf(" wait_rx status=%x\n",status);  */
   cnt++;
   if(cnt>1500) goto next;
   if((status&1) == 0) goto wait_rx;  /* wait for message available */
next:
   adr = (unsigned char *)(vme16base + 0x6029);
   dat1 = *adr;
   usleep(0);
   adr = (unsigned char *)(vme16base + 0x602b);
   dat2 = *adr;
   usleep(0);
   *id = (dat1<<3) | (dat2>>5);
   *rtr = (dat2>>4) & 1;
   k = (dat2 & 0xf);
   *dlc = k;
   kk = vme16base + 0x602d;
   for(i=0;i<k;i++)
    { adr = (unsigned char *)kk;
      data[i] = *adr;
      usleep(0);
      kk = kk+2;  
    }
   adr = (unsigned char *)(vme16base + 0x6003);
   *adr = 4;  /* release read buffer */
   usleep(0);
   // printf("can_rx id=%d  dlc=%d rtr=%d data=%d %d %d %d %d %d %d %d\n",
   //    *id,*dlc,*rtr,data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
   return(0);
}

/*  Do a single convertion
    The ADC shifts into the mother board epld the formatted pattern 
    to select the card then waits for DELAY
    then performs the conversion and send back the result  */

long adc_convert(id,command,result)
  int id;
  unsigned short command;
  unsigned short *result;
  { long status;
    long idrd;
    unsigned char dlc, rtr;
    unsigned char data[8];
    unsigned char d1, d2;

/* Send the convert command */
    data[0] = CONVERT;
    data[1] = (unsigned char)( (command >> 8) &0x00FF );
    data[2] = (unsigned char)(  command       &0x00FF );
    status  = can_tx(id, 3, 0, data);

    d1 = data[1];
    d2 = data[2];
   if(status != 0) 
     { printf(" convert send error status=%x\n",status);
       return(-1); }
/* Get back the result  */
    status = can_rx(&idrd, &dlc, &rtr, data);
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

long adc_convert2(id,command,result)
  int id;
  unsigned short command;
  unsigned short *result;
  { long status;
    long idrd;
    unsigned char dlc, rtr;
    unsigned char data[8];
    unsigned char d1, d2;

/* Send the convert command */
    data[0] = CONVERT;
    status  = can_tx(id, 1, 0, data);

   if(status != 0) 
     { printf(" convert send error status=%x\n",status);
       return(-1); }
/* Get back the result  */
    status = can_rx(&idrd, &dlc, &rtr, data);
/* If we don't get back a REQUEST command
   If we don't get back the right parameter code
      then error is set to 1  */

   if(status != 0) 
      { printf(" convert back error status=%x\n",status);
        return(-1); }
   if( data[0] != CONVERT ) status=1;
	
/* Format the returned parameter  */
    *result = ((unsigned short)data[1])*256 + ((unsigned short)data[2]);
	
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

long adc_delay(id,delay)
int id;
unsigned short delay;
  {	long status;
	long idrd;
	unsigned char dlc, rtr;
	unsigned char data[8];

	/* Send the command */
	data[0] = DELAY;
	data[1] = (unsigned char)( (delay >> 8) &0x00FF );
	data[2] = (unsigned char)(  delay       &0x00FF );
	status  = can_tx(id, 3, 0, data);
   if(status != 0) 
     { printf(" set delay send error status=%x\n",status);
       return(-1); }

	/* Wait for the acknowledgement 
	   if we don't get ACK then status is set to -1  */
	status = can_rx(&idrd, &dlc, &rtr, data);
   if(status != 0) 
     { printf(" set delay ACK error status=%x\n",status);
       return(-1); }
	if (data[0] != ACK) status = -1;

	/* If the command was written and acknowledged properly, return 0
	   Otherwise return -1 */

	return(status);
}

long adc_get_delay(id,delay)
int id;
unsigned short *delay;
{
	long status;
	long idrd;
	unsigned char dlc, rtr;
	unsigned char data[8];

	/* Send the read command */
	data[0] = REQUEST;
	data[1] = DELAY;
	status  = can_tx(id, 2, 0, data);
   if(status != 0) 
     { printf(" set delay read error status=%x\n",status);
       return(-1); }
	/* Get back the result */
	status = can_rx(&idrd, &dlc, &rtr, data);
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

long adc_dacset(id,dac)
int id;
unsigned char dac;
{
	long status;
	long idrd;
	unsigned char dlc, rtr;
	unsigned char data[8];

	/* Send the command  */
	data[0] = DACSET;
	data[1] = dac;
	status  = can_tx(id, 2, 0, data);
   if(status != 0) 
     { printf(" set set ADC dac send error status=%x\n",status);
       return(-1); }

	/* Wait for the acknowledgement
	   if we don't get ACK then status is set to -1 */
	status = can_rx(&idrd, &dlc, &rtr, data);
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
  unsigned long *adr;
  (unsigned long *)adr = (unsigned long *)(vme24base+0xaa000c);
  k = *adr;
  shift = i-1;
  l = (k>>shift) & 1;
  return(l);
}
latch()
{ int k,l;
  unsigned long *adr;
  (unsigned long *)adr = (unsigned long *)(vme24base+0xaa000c);
  k = *adr;
  l = (k>>15) & 1;
  return(l);
}
timset(c1,f1,c2,f2)
int c1,f1,c2,f2;
{unsigned long *adr;
  (unsigned long *)adr = (unsigned long *)(vme24base+0xaa000a);
 *adr = (unsigned long)f1;               /* latch fine timer 1 value */
  (unsigned long *)adr = (unsigned long *)(vme24base+0xaa0012);
 *adr = (unsigned long)f2;             /* latch fine timer 2 value */
  (unsigned long *)adr = (unsigned long *)(vme24base+0xaa0008);
 *adr = (unsigned long)c1;               /* write coarse timer 1 to latch */
  (unsigned long *)adr = (unsigned long *)(vme24base+0xaa0010);
 *adr = (unsigned long)c2;               /* write coarse timer 1 to latch */
  (unsigned long *)adr = (unsigned long *)(vme24base+0xaa0004);
 *adr = 0;                        /* load fine timers with latched values */
  (unsigned long *)adr = (unsigned long *)(vme24base+0xaa0000);
 *adr = 0;                        /* xfer latched to counters (coarse) */
}
restim()
{unsigned long *adr;
  (unsigned long *)adr = (unsigned long *)(vme24base+0xaa0006);
 *adr = 0;              /* clear fine timer done bits */
  (unsigned long *)adr = (unsigned long *)(vme24base+0xaa0000);
 *adr = 0;              /* reset timers back to latched values */
}
stvme()
{unsigned long *adr;
  (unsigned long *)adr = (unsigned long *)(vme24base+0xaa000e);
 *adr = 0;              /* start the timers from a vme null write */
}
rlatch()
{unsigned long *adr;
  (unsigned long *)adr = (unsigned long *)(vme24base+0xaa0002);
 *adr = 0;              /* reset the latch bit */
}

int kjawait(int k)
 { int i;
   double sum,s,a;
   for(i=0;i<k;i++)
     { a = (double)i;
       s=sin(a);
       sum=sum+a;
     }
   i = (int)sum;
   return(i);
 }
