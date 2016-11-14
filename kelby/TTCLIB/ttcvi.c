#include <stdio.h>
#include <math.h>
#include "vme_base.h"
int unfold[240] = {  0, 31, 46, 61, 76, 91,106,121,136,151,
                   166,181,196,211,226,  1, 16, 47, 62, 77,
		    92,107,122,137,152,167,182,197,212,227,
		     2, 17, 32, 63, 78, 93,108,123,138,153,
		   168,183,198,213,228,  3, 18, 33, 48, 79,
		    94,109,124,139,154,169,184,199,214,229,
		     4, 19, 34, 49, 64, 95,110,125,140,155,
		   170,185,200,215,230,  5, 20, 35, 50, 65,
		    80,111,126,141,156,171,186,201,216,231,
		     6, 21, 36, 51, 66, 81, 96,127,142,157,
		   172,187,202,217,232,  7, 22, 37, 52, 67,
		    82, 97,112,143,158,173,188,203,218,233,
		     8, 23, 38, 53, 68, 83, 98,113,128,159,
		   174,189,204,219,234,  9, 24, 39, 54, 69,
		    84, 99,114,129,144,175,190,205,220,235,
		    10, 25, 40, 55, 70, 85,100,115,130,145,
		   160,191,206,221,236, 11, 26, 41, 56, 71,
		    86,101,116,131,146,161,176,207,222,237,
		    12, 27, 42, 57, 72, 87,102,117,132,147,
		   162,177,192,223,238, 13, 28, 43, 58, 73,
		    88,103,118,133,148,163,178,193,208,239,
		    14, 29, 44, 59, 74, 89,104,119,134,149,
		   164,179,194,209,224, 15, 30, 45, 60, 75,
		    90,105,120,135,150,165,180,195,210,225 };

   unsigned int base1 = 0xb00000;
   unsigned int base2 = 0xb00000;

   unsigned short *adr1;
   unsigned long *adr2;
		    
/* ----------------------------------------------------------- */
ttcvi_map(ttcadr)
int ttcadr;
 { adr1 = (unsigned short *) (vme24base + base1 + 0x84);
   *adr1 = 0;  /* software reset the TTCvi module */
   BroadcastShortCommand(0x10);  /* initialize all TTCrx chips */
   usleep(50);
   SendLongCommand(ttcadr,0,0x3,0xa8);  /* set the TTCrx control register */
   adr1 = (unsigned short *) (vme24base + base1 + 0x80);
   *adr1 = 0x4048;  /* set the TTCrx csr1 */
   adr1 = (unsigned short *) (vme24base + base1 + 0x82);
   *adr1 = 0xf000;  /* set the TTCrx csr2 */
 }

Setcsr1(unsigned short csr)
 { adr1 = (unsigned short *) (vme24base + base1 + 0x80);
   *adr1 = csr;  /* set the TTCrx csr1 */
 }

Setcsr2(unsigned short csr)
 { adr1 = (unsigned short *) (vme24base + base1 + 0x82);
   *adr1 = csr;  /* set the TTCrx csr1 */
 }

ttcvi_mapr(ttcadr)
int ttcadr;
 { adr1 = (unsigned short *) (vme24base + base1 + 0x84);
   *adr1 = 0;  /* software reset the TTCvi module */
   SendLongCommand(ttcadr,0,0x6,0);  /* initialize TTCrx chips */
   usleep(500000);
   SendLongCommand(ttcadr,0,0x3,0xa8);  /* set the TTCrx control register */
   adr1 = (unsigned short *) (vme24base + base1 + 0x80);
   *adr1 = 0x4048;  /* set the TTCrx csr1 */
   adr1 = (unsigned short *) (vme24base + base1 + 0x82);
   *adr1 = 0xf000;  /* set the TTCrx csr2 */
 }

ttcvi_reset()
 { adr1 = (unsigned short *) (vme24base + base1 + 0x84);
   *adr1 = 0;  /* software reset the TTCvi module */
 }

/* ----------------------------------------------------------- */
GetTriggerSelect()
 { unsigned int k;
   adr1 = (unsigned short *) (vme24base + base1 + 0x80);
   k = *adr1 & 0x7;
   return k;
 }

/* ----------------------------------------------------------- */
Read_csr1() 
 { unsigned int k;
   adr1 = (unsigned short *) (vme24base + base1 + 0x80);
   k = *adr1;
   return k;
 }

/* ----------------------------------------------------------- */
Read_csr2()
 { unsigned int k;
   adr1 = (unsigned short *) (vme24base + base1 + 0x82);
   k = *adr1;
   return k;
 }

/* ----------------------------------------------------------- */
SetTriggerSelect(tsel)
unsigned int tsel;
 { adr1 = (unsigned short *) (vme24base + base1 + 0x80);
   *adr1 = (*adr1 & 0xfff8) | (tsel & 0x7);
 }

/* ----------------------------------------------------------- */
SendL1A()
 { unsigned long saved = GetTriggerSelect();
   SetTriggerSelect(4); /* VME L1A  */
   adr1 = (unsigned short *) (vme24base + base1 + 0x86);
   *adr1 = 0; /* sw_l1a */
   SetTriggerSelect(saved); /* Switch back  */
 }

/* ----------------------------------------------------------- */
GetRandTrigRate()
 { unsigned int k;
   adr1 = (unsigned short *) (vme24base + base1 + 0x80);
   k = (*adr1 >> 12) & 0x7; 
   return k;
 }

/* ----------------------------------------------------------- */
SetRandTrigRate(rate)
unsigned long rate;
 { adr1 = (unsigned short *) (vme24base + base1 + 0x80);
   *adr1 = (*adr1 & 0x8fff) | ((rate & 0x7) << 12); }

/* ----------------------------------------------------------- */
GetInhibitDelay(channel)
int channel;
 { unsigned long k;
   if(channel == 0)
     { adr1 = (unsigned short *) (vme24base + base1 + 0x92);
       k = *adr1;
     } 
   if(channel == 1)
     { adr1 = (unsigned short *) (vme24base + base1 + 0x9a);
       k = *adr1;
     } 
   if(channel == 2)
     { adr1 = (unsigned short *) (vme24base + base1 + 0xa2);
       k = *adr1;
     } 
   if(channel == 3)
     { adr1 = (unsigned short *) (vme24base + base1 + 0xaa);
       k = *adr1;
     } 
   return k;
 }

/* ----------------------------------------------------------- */
SetInhibitDelay(channel,delay)
int channel;
unsigned short delay;
 { printf(" SetInhibitDelay channel=%d  delay=%d\n",channel,delay);
   if(channel == 0)
     { adr1 = (unsigned short *) (vme24base + base1 + 0x92);
       *adr1 = delay & 0xfff; 
     }
   if(channel == 1)
     { adr1 = (unsigned short *) (vme24base + base1 + 0x9a);
       *adr1 = delay & 0xfff; 
     }
   if(channel == 2)
     { adr1 = (unsigned short *) (vme24base + base1 + 0xa2);
       *adr1 = delay & 0xfff; 
     }
   if(channel == 3)
     { adr1 = (unsigned short *) (vme24base + base1 + 0xaa);
       *adr1 = delay & 0xfff; 
     }
 }
/* ----------------------------------------------------------- */
GetInhibitDuration(channel)
int channel;
 { unsigned short k;
   if(channel == 0)
     { adr1 = (unsigned short *) (vme24base + base1 + 0x94);
       k = *adr1;
     } 
   if(channel == 1)
     { adr1 = (unsigned short *) (vme24base + base1 + 0x9c);
       k = *adr1;
     } 
   if(channel == 2)
     { adr1 = (unsigned short *) (vme24base + base1 + 0xa4);
       k = *adr1;
     } 
   if(channel == 3)
     { adr1 = (unsigned short *) (vme24base + base1 + 0xac);
       k = *adr1;
     } 
   printf(" Channel=%d GetInhibitDuration = %d\n",channel,k);
   return k;
 }
/* ----------------------------------------------------------- */
SetInhibitDuration(channel,duration)
int channel;
unsigned short duration;
 { if(channel == 0)
     { adr1 = (unsigned short *) (vme24base + base1 + 0x94);
       *adr1 = duration & 0xff; 
     }
   if(channel == 1)
     { adr1 = (unsigned short *) (vme24base + base1 + 0x9c);
       *adr1 = duration & 0xff; 
     }
   if(channel == 2)
     { adr1 = (unsigned short *) (vme24base + base1 + 0xa4);
       *adr1 = duration & 0xff; 
     }
   if(channel == 3)
     { adr1 = (unsigned short *) (vme24base + base1 + 0xac);
       *adr1 = duration & 0xff; 
     }
 }
/* ----------------------------------------------------------- */
BroadcastShortCommand(command)
unsigned int command;
  { adr1 = (unsigned short *) (vme24base + base1 + 0xc4);
    *adr1 = (unsigned short)command;
  }
/* ----------------------------------------------------------- */
SendLongCommand(address,external,subadr,data)
int address,external,subadr,data;
 { unsigned long lwd;
   lwd = 0x80000000 | (address<<17) | (external<<16) | (subadr<<8) | data;
   adr2 = (unsigned long *) (vme24base + base2 + 0xc0);
   *adr2 = lwd;
   /*  the 3in1 system needs 2.5us to execute a command  */
   /*   - shift 13bits & execute   */
   usleep(5);
 }
/* ----------------------------------------------------------- */
GetBMode(channel)
int channel;
 { unsigned short k;
   if(channel == 0)
     { adr1 = (unsigned short *) (vme24base + base1 + 0x90);
       k = *adr1;
     } 
   if(channel == 1)
     { adr1 = (unsigned short *) (vme24base + base1 + 0x98);
       k = *adr1;
     } 
   if(channel == 2)
     { adr1 = (unsigned short *) (vme24base + base1 + 0xa0);
       k = *adr1;
     } 
   if(channel == 3)
     { adr1 = (unsigned short *) (vme24base + base1 + 0xa8);
       k = *adr1;
     } 
   return (int)k;
 }
/* ----------------------------------------------------------- */
SetBMode(channel,mode)
int channel;
unsigned char mode;
 { if(channel == 0)
     { adr1 = (unsigned short *) (vme24base + base1 + 0x90);
       *adr1 = mode & 0xf; 
     }
   if(channel == 1)
     { adr1 = (unsigned short *) (vme24base + base1 + 0x98);
       *adr1 = mode & 0xf; 
     }
   if(channel == 2)
     { adr1 = (unsigned short *) (vme24base + base1 + 0xa0);
       *adr1 = mode & 0xf; 
     }
   if(channel == 3)
     { adr1 = (unsigned short *) (vme24base + base1 + 0xa8);
       *adr1 = mode & 0xf; 
     }
 }
/* ----------------------------------------------------------- */
SetRetransmit(channel,retransmit)
int channel,retransmit;
  { adr1 = (unsigned short *) (vme24base + base1 + 0x82);
    *adr1 = (*adr1 & ~(1 << 8 + channel))
    | (!retransmit << 8 + channel);
 }
/* ----------------------------------------------------------- */
ResetFIFO(channel)
int channel;
 { adr1 = (unsigned short *) (vme24base + base1 + 0x82);
   *adr1 = *adr1 | (1 << 12 + channel);
 }
/* ----------------------------------------------------------- */
BPutShort(channel,command)
int channel,command; 
  { unsigned long kl;
    kl =  (command<<23); 
    if(channel == 0)
    { adr2 = (unsigned long *) (vme24base + base2 + 0xb0);
      *adr2 = kl;
    }  
    if(channel == 1)
    { adr2 = (unsigned long *) (vme24base + base2 + 0xb4);
      *adr2 = kl;
    }  
    if(channel == 2)
    { adr2 = (unsigned long *) (vme24base + base2 + 0xb8);
      *adr2 = kl;
    }  
    if(channel == 3)
    { adr2 = (unsigned long *) (vme24base + base2 + 0xbc);
      *adr2 = kl;
    }  
  }

/* ----------------------------------------------------------- */
BPutLong(channel,address,external,subadr,data)
int channel,address,external,subadr,data; 
 { unsigned long lwd;
   lwd = 0x80000000 | (address<<17) | (external<<16) | (subadr<<8) | data;
   if(channel == 0)
    { adr2 = (unsigned long *) (vme24base + base2 + 0xb0);
      *adr2 = lwd;
    }
   if(channel == 1)
    { adr2 = (unsigned long *) (vme24base + base2 + 0xb4);
      *adr2 = lwd;
    }
   if(channel == 2)
    { adr2 = (unsigned long *) (vme24base + base2 + 0xb8);
      *adr2 = lwd;
    }
   if(channel == 3)
    { adr2 = (unsigned long *) (vme24base + base2 + 0xbc);
      *adr2 = lwd;
    }
 }
/* ----------------------------------------------------------- */
BGoSignal(channel)
int channel;
 { if(channel == 0)
    { adr1 = (unsigned short *) (vme24base + base1 + 0x96);
      *adr1 = 0; 
    }
   if(channel == 1)
    { adr1 = (unsigned short *) (vme24base + base1 + 0x9e);
      *adr1 = 0; 
    }
   if(channel == 2)
    { adr1 = (unsigned short *) (vme24base + base1 + 0xa6);
      *adr1 = 0; 
    }
   if(channel == 3)
    { adr1 = (unsigned short *) (vme24base + base1 + 0xae);
      *adr1 = 0; 
    }
 } 
/* ----------------------------------------------------------- */
SetTimeCoarse(address,time)
int address,time;
  { if(time<0 || time>255) 
       { printf(" Des1 time setting error %d\n",time); }
    else
       { SendLongCommand(address,0,2,time); }
  }
/* ----------------------------------------------------------- */
SetTimeDes1(address,time)
int address,time;
  { int t;
    if(time<0 || time>239)
       { printf(" Des1 time setting error %d\n",time); }
    else    
       { t = unfold[time];
         SendLongCommand(address,0,0,t);
       }
  }
/* ----------------------------------------------------------- */
SetTimeDes2(address,time)
int address,time;
  { int t;
    if(time<0 || time>239)
       { printf(" Des2 time setting error %d\n",time); }
    else
       { t = unfold[time];
         SendLongCommand(address,0,1,t);
       }
  }
/* ----------------------------------------------------------- */
/* GetClockDelay()
    { int k;
      adr1 = (unsigned short *) (vme24base + base1 + 0x80);
      k = *adr1;
      kk = 2 * (~(k >> 8) & 0xf);
      return k;
    }  */
