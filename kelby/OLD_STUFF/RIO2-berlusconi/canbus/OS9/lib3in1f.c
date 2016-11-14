#include <stdio.h>
#include <math.h>
unsigned int canf_base = 0xf0dd0000;

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

     reset_can()     
/*-----------------------------------------------*/
/* send reset signal to integrator card          */
/*-----------------------------------------------*/
     { SendLongCommand(0,1,0xa4,0); }

     set_swop1(i)     
/*-----------------------------------------------*/
/* set reference voltage switch op1              */
/*-----------------------------------------------*/
     int i;
     { SendLongCommand(0,1,0xa8,i); }

     set_swln1(i)     
/*-----------------------------------------------*/
/* set reference voltage switch ln1              */
/*-----------------------------------------------*/
     int i;
     { SendLongCommand(0,1,0xac,i); }

     set_swop2(i)     
/*-----------------------------------------------*/
/* set reference voltage switch op2              */
/*-----------------------------------------------*/
     int i;
     { SendLongCommand(0,1,0xb0,i); }

     set_swln2(i)     
/*-----------------------------------------------*/
/* set reference voltage switch ln2              */
/*-----------------------------------------------*/
     int i;
     { SendLongCommand(0,1,0xb4,i); }

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
/* fake can module originated functions          */
/*-----------------------------------------------*/
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
  { can_set(0x90,0); }

can_multi_low()
  { can_set(0x90,0xff); }

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
     if(sw(8) == 1) printf(" subadr=%x  data=%x\n",i1,i2); 
   }

can_set(subadr,data)
int subadr,data;
  { unsigned short i16;
    int i;
    i16 = (subadr<<8) | data;
    if(sw(8) == 1) printf(" can_set i16=%x\n",(int)i16);
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
  adr = (unsigned short *)( canf_base);
  *adr = 0;
}

can_clkh()
{ unsigned short *adr;
  adr = (unsigned short *)( canf_base | 0x2);
  *adr = 0;
}

can_exl()
{ unsigned short *adr;
  adr = (unsigned short *)( canf_base | 0x4);
  *adr = 0;
}

can_exh()
{ unsigned short *adr;
  adr = (unsigned short *)( canf_base | 0x6);
  *adr = 0;
}

can_int_clk()
{ unsigned short *adr;
  adr = (unsigned short *)( canf_base | 0x8);
  *adr = 0;
}

can_shft_wr(val)
unsigned short val;
  { unsigned short *adr;
    adr = (unsigned short *)(canf_base | 0xa);
    if(sw(8) == 1) printf("can_shft_wr  val=%x\n",val);
    *adr = ~val;
  }

can_shft_rd(val)
unsigned short *val;
  { unsigned short *adr;
    int i;
    for(i=0;i<16;i++)
      { can_int_clk();
	can_clkh();
	can_clkl();
      }
    adr = (unsigned short *)(canf_base | 0xa);
    *val = ~*adr;
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
