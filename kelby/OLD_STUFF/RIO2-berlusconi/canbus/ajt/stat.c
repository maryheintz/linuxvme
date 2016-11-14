/*****************************************************
  stat.c  -- Prints out the contents of the control 
             segment.
  Coded by A.J. Tolland  ajt@hep.uchicago.edu
  Last Modified: 3/20/1998
******************************************************/
	
#include <stdio.h>

#define IP 0xffff6001  /* BASE IP ADDRESS */

main() {

   unsigned char *adr; /* register pointer */
   int k, i;

/*Iterates through the first 9 buffers. 
  No one cares about the test buffer */
   for(i=0; i < 18; i +=2) {
     k = IP + i;
     adr = (unsigned char*) k;
     printf("Buffer %d: %x\n", i/2 + 1, *adr);
   }
}