#include <stdio.h>
main()
 { unsigned char *adr;
   int k;

   k = 0xffff6001; /* CONTROL REGISTER */
   adr = (unsigned char*) k;
   *adr = 1;
   *adr = 0;
}

