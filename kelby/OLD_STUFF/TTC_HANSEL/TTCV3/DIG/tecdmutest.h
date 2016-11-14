#ifndef TECDMUTST_H_
#define TECDMUTST_H_

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

typedef union {                     /* 24 bit test word */
      unsigned long int full;
      struct {
         unsigned int unused       : 8; /* Highest bit is here */
	 unsigned int word3        : 8;
	 unsigned int word2        : 8;
	 unsigned int word1        : 8;
	     } bits;
   } digi_testwd;

typedef union {                     /* ADC output in SLINK string */
      unsigned long int full;
      struct {
         unsigned int unused       : 12;  /*  Highest bit is here */
         unsigned int reading      : 20;  /*  TEC DMU output      */
      } bits;
   } digi_datawd;

#endif










