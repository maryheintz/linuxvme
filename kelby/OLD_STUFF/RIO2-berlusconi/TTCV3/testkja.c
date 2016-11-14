#include <stdio.h>
main()
  {short *reg;
   reg = (short*)0xdeb00084;
   *reg = 0;
   sleep(1);
   reg = (short*)0xdeb00080;
loop:
   *reg=0x402a;
   sleep(1);
   *reg = 0x4028;
   sleep(1);
   printf("kelby was here\n");
   goto loop;
}

