#ifndef ADCTST_H_
#define ADCTST_H_

/***************************************************************************/
/* This file contains parameters and declarations needed to access the     */
/* slink and ttcvi cards for tests of the ADCs.                            */
/*                                                                         */
/* Author : S Silverstein                                                  */
/* Date   : 11-Apr-1998                                                    */
/* Version: 1.0                                                            */
/***************************************************************************/

/* WARNING - the syntax used for bit definitions (aaa.bb :n) is not portable.
 * Other compilers may generate code with inverted bit ordering which is
 * incompatible with the hardware.
 */ 


/* Set up type definitions */

typedef union {                     /* ADC output in SLINK string */
      unsigned long int full;
      struct {
         unsigned int unused       :  8;  /*  Highest bit is here */
         unsigned int adc_high     : 12;  /*  upper ADC reading */
         unsigned int adc_low      : 12;  /*  lower ADC reading */
      } bits;
   } digi_datawd;

#endif










