#define CANSET 11
#define CANGET 12
#define ACK 51
#include <stdio.h>
main()
 { unsigned char *adr;
   int k,sjw,brp,sam,tseg2,tseg1;
   int octp1,octn1,ocpol1,octp0,octn0,ocpol0,ocmode1,ocmode0;

   k = 0xdffe6001; /* CONTROL REGISTER */
   adr = (unsigned char*) k;
   *adr = 1;
   *adr = 0;
}

