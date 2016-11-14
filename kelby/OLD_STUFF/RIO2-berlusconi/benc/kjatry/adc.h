// adc.h: deeclarations for ADC control

#ifndef _ADC_H
#define _ADC_H

// Structures

struct ADCblock {
  unsigned short b0;        /* base+0 */
  unsigned short b2;        /* base+2 */
  unsigned short b4;        /* base+4 */
  unsigned short b6;        /* base+6 */
  unsigned short b8;        /* base+8 */
  unsigned short ba;        /* base+a */
  unsigned short bc;        /* base+c */
  unsigned short be;        /* base+e */
  unsigned short rd0;       /* base+10 */
  unsigned short rd1;       /* base+12 */
  unsigned short rd2;       /* base+14 */
  unsigned short b16;       /* base+16 */
  unsigned short all;       /* base+18 */
  unsigned short b1a;       /* base+1a */
  unsigned short b1c;       /* base+1c */
  unsigned short delay;     /* base+1e */
  unsigned short sample;    /* base+20 */
  unsigned short status;    /* base+22 */
  unsigned short reset;     /* base+24 */
  unsigned short trig;      /* base+26 */
};

// Function Prototypes

void fermi_setup();
void fermi_reset();
void fermi_read();

// Global Variables

ADCblock *adc;
int fdata[3][40],pk[3],ipk[3];

#endif // _ADC_H not defined
