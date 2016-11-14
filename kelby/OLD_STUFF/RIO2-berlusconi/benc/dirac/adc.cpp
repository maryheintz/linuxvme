// adc.cpp: implementation of ADC functions

#include <stdio.h>
#include "auxvme.h"
#include "adc.h"

void fermi_setup() {
  unsigned long am,vmead,offset,size,len;
  am=0x39;
  vmead=0x0;
  offset=0;
  size=0;
  len=400;
  pdparam_master param = {
    1,   // VME Iack (must be 1 for all normal pages)
    0,   // VME Read Prefetch Option
    0,   // VME Write Posting Option
    1,   // VME Swap Option
    0,   // page number return
    0,   // reserved
    0,   // reserved
    0
  };
  adc = (ADCblock*)( find_controller(vmead,len,am,offset,size,&param) );
  if( adc == (ADCblock*)(-1) ) {
    printf("unable to map vme address\n");
    exit(0);
  }
  /* issue fermi reset */
  adc->reset=0;
  /* set to full 255 samples */
  adc->sample=255;
  /* set delay */
  adc->delay=2;
}

void fermi_reset() { 
  /* issue reset */
  adc->reset=0;
}


void fermi_read() {
  int stat,data_ready,busy,lcnt,i,k0,k1,k2,kmax0,kmax1,kmax2;
  int dd0,dd1,dd2;
  unsigned short sd0,sd1,sd2;
  /* wait for a trigger */
  lcnt=0;
 loop:
  lcnt++;
  stat = adc->status;
  data_ready = stat & 4;
  busy = stat & 2;
  if(lcnt > 1000 ) goto error;
  if ( data_ready == 0 ) goto loop;
  /* read the data and find peak channel */
  k0=0;
  kmax0=0;
  k1=0;
  kmax1=0;
  k2=0;
  kmax2=0;
  for (i = 0; i < 40; ++i) {
    sd0 = adc->rd0;
    dd0 = (int)sd0 & 0x3ff;
    fdata[0][i] = dd0;
    if (fdata[0][i] > kmax0) {
      kmax0=fdata[0][i];
      k0=i;
    }
    sd1 = adc->rd1;
    dd1 = (int)sd1 & 0x3ff;
    fdata[1][i] = dd1;
    if (fdata[1][i] > kmax1) {
      kmax1=fdata[1][i];
      k1=i;
    }
    sd2 = adc->rd2;
    dd2 = (int)sd2 & 0x3ff;
    fdata[2][i] = dd2;
    if (fdata[2][i] > kmax2) {
      kmax2=fdata[2][i];
      k2=i;
    }
    // sw4 = sw(4);
    // if(sw4 == 1) printf("i=%d  dd0=%d  dd1=%d  dd2=$d\n",i,dd0,dd1,dd2);
  }
  ipk[0]=k0;
  pk[0]=kmax0;
  ipk[1]=k1;
  pk[1]=kmax1;
  ipk[2]=k2;
  pk[2]=kmax2;
  return;
 error:
  ipk[0]=-1;
  ipk[1]=-1;
  ipk[2]=-1;
}
