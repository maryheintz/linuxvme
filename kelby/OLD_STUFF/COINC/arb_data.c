#include <stdio.h>
#include <time.h>
#include "vme.h"

time_t usec;
unsigned long vmebase,badr;
FILE *fp,*fp1,*fp2,*fp3,*fp4;
main()
 { unsigned long *adr,k,i;
   unsigned short *sadr,klow,khigh,m1,m2;
   unsigned long muon,scint,coinc;
   int dac,scaler,mv,kk,ldac,abcd;
   double eff,hz,time;
   fp = fopen("arb_data.out","w");
   fp1 = fopen("mv.dat","w");
   fp2 = fopen("muon.dat","w");
   fp3 = fopen("scint.dat","w");
   fp4 = fopen("coinc.dat","w");
   kk = 0xfc;
   badr = (kk<<16);
   vmebase = vme_a24_base(); 
   printf("vmebase=0x%x\n",vmebase);
/* set to single tower mode */   
   adr = (unsigned long *)(vmebase + badr + 0x44);
   *adr=0;
/* disable gate */
   adr = (unsigned long *)(vmebase + badr + 0x30);
   *adr = 0;
/* enable coincidence board triggers */
   adr = (unsigned long *)(vmebase + badr + 0x08);
   *adr = 0;
   for(mv=15;mv<400;mv=mv+5)  
/*   for(mv=14;mv<200;mv=mv+2)  */
     { dac = mv*(4096.0/1240.0);
/*       printf(" mv=%d    dac=%d\n",mv,dac); */
       for(abcd=0;abcd<4;abcd++)
        { for(ldac=0;ldac<24;ldac++)
           {  setdac(abcd,ldac,dac);
           }
	}
       /* disable arbmuon gate */
       sadr = (unsigned short *)(vmebase + 0x00de100c);
       *sadr = 0;
       /* clear arbmuon scalers */
       sadr = (unsigned short *)(vmebase + 0x00de1000);
       *sadr = 0;
       /* enable arbmuon scalers */ 
       sadr = (unsigned short *)(vmebase + 0x00de100c);
       *sadr = 1;
/*       sleep(900);  */
       sleep(300);
       time=300.0;
       /* disable arbmuon gate */
       sadr = (unsigned short *)(vmebase + 0x00de100c);
       *sadr = 0;
       /* read out the muon scaler */
       sadr = (unsigned short *)(vmebase + 0x00de1008);
       *sadr = 0;  /* set half to zero */
       sadr = (unsigned short *)(vmebase + 0x00de1002);
       klow = *sadr;
       sadr = (unsigned short *)(vmebase + 0x00de1008);
       *sadr = 1;  /* set half to one */
       sadr = (unsigned short *)(vmebase + 0x00de1002);
       khigh = *sadr; 
       muon = (khigh<<16) + klow;
       m1 = klow;
       m2=khigh;
       
       /* read scintillator scaler */
       sadr = (unsigned short *)(vmebase + 0x00de1008);
       *sadr = 0;  /* set half to zero */
       sadr = (unsigned short *)(vmebase + 0x00de1004);
       klow = *sadr;
       sadr = (unsigned short *)(vmebase + 0x00de1008);
       *sadr = 1;  /* set half to one */
       sadr = (unsigned short *)(vmebase + 0x00de1004);
       khigh = *sadr; 
       scint = (khigh<<16) + klow;

/*     read out the coincidence scaler */
       sadr = (unsigned short *)(vmebase + 0x00de1008);
       *sadr = 0;  /* set half to zero */
       sadr = (unsigned short *)(vmebase + 0x00de1006);
       klow = *sadr;
       sadr = (unsigned short *)(vmebase + 0x00de1008);
       *sadr = 1;  /* set half to one */
       sadr = (unsigned short *)(vmebase + 0x00de1006);
       khigh = *sadr;	 
       coinc = (khigh<<16) + klow;
       eff = (double)coinc/(double)scint;
       hz = (double)muon/time;
       printf(" mv=%d   muon=%d  scint=%d   coinc=%d  eff=%f  hz=%f              %x %x %x\n",
            mv,muon,scint,coinc,eff,hz,m2,m1,muon);
       fprintf(fp," mv=%d   muon=%d  scint=%d   coinc=%d\n",mv,muon,scint,coinc);
       fprintf(fp1,"%d\n",mv);
       fprintf(fp2,"%d\n",muon);
       fprintf(fp3,"%d\n",scint);
       fprintf(fp4,"%d\n",coinc);
    } 
  }

 setdac(abcd,ldac,dac)
 int abcd,ldac,dac;
  { unsigned long k1,k2,*adr;
/*  abcd 0=A  1=C  2=B  3=D  */
    k1 = (abcd<<12) | dac;
    adr = (unsigned long *)(vmebase + badr + 0x14);
    *adr = k1;      /* latch dac value and abcd select */
       /*  usleep(usec); */
    k2 = (1<<ldac);
    adr = (unsigned long *)(vmebase + badr + 0x18);
    *adr = k2;  /* strobe dac line */
       /*  usleep(usec); */
  }
  

  sw(int i)
  { int k,l,shift;
    unsigned short *adr;
    adr = (unsigned short *)(vmebase+0x00aa000c);
    k = *adr;
    usleep(usec);
    shift = i-1;
    l = (k>>shift) & 1;
    return(l);
    } 
     
