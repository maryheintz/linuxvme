/****************************************************************
 statr.c -- Prints out (iteratively) the contents of the receive
	    registers.
 Code by A.J. Tolland (ajt@hep.uchicago.edu)
 Last modified:  2/30/1998
****************************************************************/

#include <stdio.h>
#define IP 0xffff6029

main() {

   unsigned char *adr;
   int k, i;

   for(i=0; i<20; i +=2) {
     k = IP + i;
     adr = (unsigned char*) k;
     printf("Buffer %d: %x\n", i/2 + 1, *adr);
   }
}