#include <stdio.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>
time_t usec;
unsigned int basef = 0xdedd0000;
long can_3in1set(int,unsigned short);
long can_3in1get(int,unsigned short *);

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

     tp_high(int ttcadr)     
/*-----------------------------------------------*/
/* set tp high                                   */
/*-----------------------------------------------*/
      { SendLongCommand(ttcadr,1,0xc0,0x00); }

     tp_low(int ttcadr)     
/*-----------------------------------------------*/
/* set tp low                                    */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xc0,0x01); }
 
     tp_high_bc()     
/*-----------------------------------------------*/
/* set tp high with broadcast command            */
/*-----------------------------------------------*/
     { BroadcastShortCommand(0xc4); }
 

     tp_low_bc()     
/*-----------------------------------------------*/
/* set tp low with broadcast command             */
/*-----------------------------------------------*/
     { BroadcastShortCommand(0xc0); }
 

     set_tube(ttcadr,tube)     
/*-----------------------------------------------*/
/* set the tube field                            */
/*-----------------------------------------------*/
     int ttcadr,tube;
     { SendLongCommand(ttcadr,1,0xc4,tube); }

     multi_high(int ttcadr)     
/*-----------------------------------------------*/
/* set multi_sel high                            */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xc8,0); }

     multi_low(int ttcadr)     
/*-----------------------------------------------*/
/* set multi_sel low                             */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xc8,1); }

     rxw_high(int ttcadr)     
/*-----------------------------------------------*/
/* set rxw high                                  */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xcc,1); }

     rxw_low(int ttcadr)     
/*-----------------------------------------------*/
/* set rxw low                                   */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xcc,0); }

     back_high(int ttcadr)     
/*-----------------------------------------------*/
/* set back_load high                            */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xd0,1); }

     back_low(int ttcadr)     
/*-----------------------------------------------*/
/* set back_load low                             */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xd0,0); }

     load_can(int ttcadr)     
/*-----------------------------------------------*/
/* load the CAN output register                  */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xd4,0); }

     reset_state(int ttcadr)     
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

     intg_rd_high(int ttcadr)     
/*-----------------------------------------------*/
/* set intg_rd high                              */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xe0,1); }

     intg_rd_low(int ttcadr)     
/*-----------------------------------------------*/
/* set intg_rd low                              */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xe0,0); }

     itr_high(int ttcadr)     
/*-----------------------------------------------*/
/* set itr high                              */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xe4,1); }

     itr_low(int ttcadr)     
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

     mse_high(int ttcadr)     
/*-----------------------------------------------*/
/* set mse high                              */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xec,1); }

     mse_low(int ttcadr)     
/*-----------------------------------------------*/
/* set mse low                              */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xec,0); }

     small_cap_enable(int ttcadr)     
/*-----------------------------------------------*/
/* enable small capacitor calibration            */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xf0,1); }

     small_cap_disable(int ttcadr)     
/*-----------------------------------------------*/
/* disable small capacitor calibration           */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xf0,0); }


     large_cap_enable(int ttcadr)     
/*-----------------------------------------------*/
/* enable large capacitor calibration            */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xf4,1); }

     large_cap_disable(int ttcadr)     
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

     trig_enable(int ttcadr)     
/*-----------------------------------------------*/
/* enable trigger output                         */
/*-----------------------------------------------*/
     { SendLongCommand(ttcadr,1,0xfc,1); }

     trig_disable(int ttcadr)     
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
can_tp_high(int id)
  { long status;
    status = can_3in1set(id,0xc001);
    if(status != 0) printf("CANbus error can_tp_high\n");
  }

can_tp_low(int id)
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

can_multi_high(int id)
  { long status;
    status = can_3in1set(id,0xc800);
    if(status != 0) printf("CANbus error can_multi_high\n");
  }

can_multi_low(int id)
  { long status;
    status = can_3in1set(id,0xc8ff);
    if(status != 0) printf("CANbus error can_multi_low\n");
  }

can_rxw_high(int id)
  { long status;
    status = can_3in1set(id,0xcc01);
    if(status != 0) printf("CANbus error can_rxw_high\n");
  }

can_rxw_low(int id)
  { long status;
    status = can_3in1set(id,0xcc00);
    if(status != 0) printf("CANbus error can_rxw_low\n");
  }

can_bkld_high(int id)
  { long status;
    status = can_3in1set(id,0xd001);
    if(status != 0) printf("CANbus error can_bkld_high\n");
  }

can_bkld_low(int id)
  { long status;
    status = can_3in1set(id,0xd000);
    if(status != 0) printf("CANbus error can_bkld_low\n");
  }

can_load_out(int id)
  { long status;
    status = can_3in1set(id,0xd400);
    if(status != 0) printf("CANbus error can_load_out\n");
  }

can_intg_rd_high(int id)
  { can_send_3in1(id,0,1); }

can_intg_rd_low(int id)
  { can_send_3in1(id,0,0); }

can_itr_high(int id)
  { can_send_3in1(id,1,1); }

can_itr_low(int id)
  { can_send_3in1(id,1,0); }

can_mse_high(int id)
  { can_send_3in1(id,3,1); }

can_mse_low(int id)
  { can_send_3in1(id,3,0); }

can_intg_gain(id,i)
int id,i;
   { can_send_3in1(id,2,i); }

can_dac(id,i)
int id,i;
   { can_send_3in1(id,6,i); }

can_small_cap_enable(int id)
   { can_send_3in1(id,4,1); }

can_small_cap_disable(int id)
   { can_send_3in1(id,4,0); }

can_large_cap_enable(int id)
   { can_send_3in1(id,5,1); }

can_large_cap_disable(int id)
   { can_send_3in1(id,5,0); }

can_trig_enable(int id)
   { can_send_3in1(id,7,1); }

can_trig_disable(int id)
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

can_reset(int ttcadr)
  {}
    
can_init(int ttcadr,int id)
{  }

reset_ctrl_can()
 { }
can_adc_init(id,action,version)
 int id;
 unsigned char action;
 unsigned char version[];
  { }

long can_3in1set(id,dat)
int id;
unsigned short dat;
  { unsigned int subadr,data;
    subadr = (dat>>8);
    data = dat & 0xff;
    can_set(subadr,data);
    return(0);
  }
can_set(subadr,data)
int subadr,data;
  { unsigned short i16;
    int i;
    i16 = (subadr<<8) | data;
/*    if(sw(8) == 1) printf(" can_set i16=%x\n",(int)i16);  */
    can_shft_wr(i16);
    for(i=0;i<16;i++)
      { can_clkh();
        can_clkl();
        can_int_clk();
      }
    can_exh();
    can_exl();
  }   

can_clkl()
{ unsigned short *adr;
  adr = (unsigned short *)( basef);
  *adr = 0;
}

can_clkh()
{ unsigned short *adr;
  adr = (unsigned short *)( basef | 0x2);
  *adr = 0;
}

can_exl()
{ unsigned short *adr;
  adr = (unsigned short *)( basef | 0x4);
  *adr = 0;
}

can_exh()
{ unsigned short *adr;
  adr = (unsigned short *)( basef | 0x6);
  *adr = 0;
}

can_int_clk()
{ unsigned short *adr;
  adr = (unsigned short *)( basef | 0x8);
  *adr = 0;
}

can_shft_wr(val)
unsigned short val;
  { unsigned short *adr;
    adr = (unsigned short *)(basef | 0xa);
    *adr = ~val;
  }

long can_3in1get(id,dat)
int id;
unsigned short *dat;
  { unsigned short *adr;
    int i;
    usec = 1;
    for(i=0;i<16;i++)
      { can_int_clk();
	can_clkh();
	can_clkl();
      }
    adr = (unsigned short *)(basef | 0xa);
    *dat = ~*adr;
    return(0);
  }
/*-----------------------------------------------*/
/* test module functions                         */
/*-----------------------------------------------*/
sw(i)
int i;
{ int k,l,shift;
  unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xdeaa000c;
  k = *adr;
  shift = i-1;
  l = (k>>shift) & 1;
  return(l);
}
latch()
{ int k,l;
  unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xdeaa000c;
  k = *adr;
  l = (k>>15) & 1;
  return(l);
}
setime(c1,f1,c2,f2)
int c1,f1,c2,f2;
{unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xdeaa000a;
 *adr = (unsigned short)f1;               /* latch fine timer 1 value */
  (unsigned short *)adr = (unsigned short *)0xdeaa0012;
 *adr = (unsigned short)f2;             /* latch fine timer 2 value */
  (unsigned short *)adr = (unsigned short *)0xdeaa0008;
 *adr = (unsigned short)c1;               /* write coarse timer 1 to latch */
  (unsigned short *)adr = (unsigned short *)0xdeaa0010;
 *adr = (unsigned short)c2;               /* write coarse timer 1 to latch */
  (unsigned short *)adr = (unsigned short *)0xdeaa0004;
 *adr = 0;                        /* load fine timers with latched values */
  (unsigned short *)adr = (unsigned short *)0xdeaa0000;
 *adr = 0;                        /* xfer latched to counters (coarse) */
}
restim()
{unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xdeaa0006;
 *adr = 0;              /* clear fine timer done bits */
  (unsigned short *)adr = (unsigned short *)0xdeaa0000;
 *adr = 0;              /* reset timers back to latched values */
}
stvme()
{unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xdeaa000e;
 *adr = 0;              /* start the timers from a vme null write */
}
rlatch()
{unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xdeaa0002;
 *adr = 0;              /* reset the latch bit */
}