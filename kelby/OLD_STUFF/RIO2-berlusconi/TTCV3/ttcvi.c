#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <timers.h> 
#include <sys/types.h>
#include <ces/vmelib.h>
time_t usec;
static struct  pdparam_master param = {
    1,   /* VME Iack (must be 1 for all normal pages) */
    0,   /* VME Read Prefetch Option                  */
    0,   /* VME Write Posting Option                  */
    1,   /* VME Swap Option                           */
    0,   /* page number return                        */
    0,   /* reserved                                  */
    0,   /* reserved                                  */
    0
  };

union CSR1 {
  unsigned short full;
  struct {
    unsigned short l1_trig_sel  : 3; /* (r/w) L1a<0-3>, VME if 4, random if 5 */
    unsigned short orb_sig_sel  : 1; /* (r/w) external orbit if 0             */
    unsigned short l1_fifofull  : 1; /* (r) L1A FIFO full if 1                */
    unsigned short l1_fifoempty : 1; /* (r) L1A FIFO empty if 1               */
    unsigned short l1_fiforst   : 1; /* (w) L1A FIFO reset if set to 1        */
    unsigned short vme_trpend   : 1; /* (r) VME transfer pending if 1         */
    unsigned short bcdelay      : 4; /* (r) read BC delay switch value        */
    unsigned short rantrigrt    : 3; /* (r/w) random trigger rate setting     */
    unsigned short unused       : 1; /* Highest bit is here     */
  } bits;
};

struct CSR2 {
  unsigned short rst_bgo : 4; /* (w) Reset B-Go FIFO <3..0> if set to 1       */
  unsigned short rtx_bgo : 4; /* (r/w) Retransmit B-Go FIFO if 0 when empty   */
  unsigned char status;       /* (r) FIFO 3 full/empty..FIFO 0 full/empty     */
};


/* structure of "B-Go" channel controls   */

struct B_Go {
  unsigned short mode;
  unsigned short inhibit_delay;
  unsigned short inhibit_duration;
  unsigned short sw_go;
};
  
/* TTCvi address space    */

struct TTCviBlock {
  unsigned short csr1;             /* $80 */
  unsigned short csr2;             /* $82 */
  unsigned short sw_rst;           /* $84 */
  unsigned short sw_L1A;           /* $86 */
  unsigned long evt_count;          
  unsigned long unused;
  struct B_Go b_go[4];
  unsigned long bgo_param[4];  
  unsigned long long_cycle;     
  unsigned short short_cycle;
};

struct TTCviBlock *ttcvi;
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
		    
/* ----------------------------------------------------------- */
struct timespec intvl,rmdr;
ttcvi_map(int ttcadr)
 { int kja;
   unsigned long length = 400;
   unsigned long adrmodifier = 0x39;
   unsigned long offset = 0;
   unsigned long size = 0;
   ttcvi = (struct TTCviBlock *)(find_controller(0xb00080, length, adrmodifier,
				 offset, size, &param) );
   if(ttcvi == (struct TTCviBlock *)(-1)) 
    { printf("Unable to map VME address\n"); }  

/*   ttcvi = (struct TTCviBlock *)0xdeb00080;
   printf("kja - alt addressing\n");  */

   ttcvi->sw_rst = 0;  /* software reset the TTCvi module */
   usleep(100);
   ttcvi->csr1 = 0x402a;
   usleep(1);
   ttcvi->csr2 = 0xf000;  
   usleep(1);

   printf(" csr1=%x  csr2=%x\n",ttcvi->csr1,ttcvi->csr2);
                       /* issue reset to TTCrx */
   SendLongCommand(ttcadr,0,0x6,0);   
   usleep(2000);  /* wait for reset to complete */
   SendLongCommand(ttcadr,0,0x3,0xa9);   /*  set the v3.0 TTCrx control register */
   usleep(1);
 }
ttcvi_reset()
  {    ttcvi->sw_rst = 0;  /* software reset the TTCvi module */ }

/* ----------------------------------------------------------- */
GetTriggerSelect()
 { return ttcvi->csr1 & 0x7; }

/* ----------------------------------------------------------- */
Read_csr1() 
 { return ttcvi->csr1; }

/* ----------------------------------------------------------- */
Read_csr2()
 { return ttcvi->csr2; }

/* ----------------------------------------------------------- */
SetTriggerSelect(unsigned int tsel)
 { ttcvi->csr1 = (ttcvi->csr1 & 0xfff8) | (tsel & 0x7);
   /* printf("csr1=%x\n",ttcvi->csr1);  */
 }

Setcsr1(unsigned int csrset)
 { ttcvi->csr1 = csrset;
   /* printf("csr1=%x\n",ttcvi->csr1);  */
 }

/* ----------------------------------------------------------- */
SendL1A()
 { /* unsigned short saved = GetTriggerSelect(); */
   /* SetTriggerSelect(4); */ /* VME L1A  */
   ttcvi->sw_L1A = 0;
   /* SetTriggerSelect(saved); */ /* Switch back  */
 }

/* ----------------------------------------------------------- */
GetRandTrigRate()
 { return (ttcvi->csr1 >> 12) & 0x7; }

/* ----------------------------------------------------------- */
SetRandTrigRate(unsigned short rate)
 { ttcvi->csr1 = (ttcvi->csr1 & 0x8fff) | ((rate & 0x7) << 12); }

/* ----------------------------------------------------------- */
GetInhibitDelay(int channel)
 { return ttcvi->b_go[channel].inhibit_delay; }

/* ----------------------------------------------------------- */
SetInhibitDelay(int channel, unsigned short delay)
 { ttcvi->b_go[channel].inhibit_delay = delay & 0xfff; }

/* ----------------------------------------------------------- */
GetInhibitDuration(int channel)
 { return ttcvi->b_go[channel].inhibit_duration; }

/* ----------------------------------------------------------- */
SetInhibitDuration(int channel, unsigned short duration)
 {  ttcvi->b_go[channel].inhibit_duration = duration & 0xff; }

/* ----------------------------------------------------------- */
BroadcastShortCommand(unsigned int command)
 { ttcvi->short_cycle = (unsigned short)command; }

/* ----------------------------------------------------------- */
SendLongCommand(int address,int external,int subadr,int data)
 { unsigned long lwd,i;
   double s;
   lwd = 0x80000000 | (address<<17) | (external<<16) | (subadr<<8) | data;
   for(i=0;i<1000;i++) s=sin(0.1); /* give it time to execute */
/*  intvl.tv_sec=0;
   intvl.tv_nsec=0;
   if(nanosleep(&intvl,&rmdr) == -1)
     {if(errno == EINTR) (void)printf("nanosleep interrupted\n");} */
   ttcvi->long_cycle = lwd;
 }

/* ----------------------------------------------------------- */
GetBMode(int channel)
 { return ttcvi->b_go[channel].mode; }

/* ----------------------------------------------------------- */
SetBMode(int channel, unsigned char mode)
 { ttcvi->b_go[channel].mode = mode & 0xf; }

/* ----------------------------------------------------------- */
SetRetransmit(int channel, int retransmit)
 { ttcvi->csr2 = (ttcvi->csr2 & ~(1u << 8 + channel))
    | (!retransmit << 8 + channel);
 }

/* ----------------------------------------------------------- */
ResetFIFO(int channel)
 { ttcvi->csr2 = (ttcvi->csr2) | (1u << 12 + channel); }

/* ----------------------------------------------------------- */
BPutShort(int channel, int command) 
  { unsigned long kl;
    kl =  (command<<23); 
    ttcvi->bgo_param[channel] = kl;  
  }

/* ----------------------------------------------------------- */
BPutLong(int channel, int address, int external, int subadr, int data) 
 { unsigned long lwd;
   lwd = 0x80000000 | (address<<17) | (external<<16) | (subadr<<8) | data;
   ttcvi->bgo_param[channel] = lwd;
 }

/* ----------------------------------------------------------- */
BGoSignal(int channel)
 { ttcvi->b_go[channel].sw_go = 0; }
 
/* ----------------------------------------------------------- */
SetTimeCoarse(int address,int time)
  { if(time<0 || time>255) 
       { printf(" Des1 time setting error %d\n",time); }
    else
       { SendLongCommand(0,0,2,time); }
  }

/* ----------------------------------------------------------- */
SetTimeDes1(int address,int time)
  { int t;
    if(time<0 || time>239)
       { printf(" Des1 time setting error %d\n",time); }
    else    
       { t = unfold[time];
         SendLongCommand(address,0,0,t);
       }
  }

/* ----------------------------------------------------------- */
SetTimeDes2(int address,int time)
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
 { return 2 * (~(ttcvi->csr1 >> 8) & 0xf); }  */
