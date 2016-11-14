#include <stdio.h>

typedef unsigned char uchar;


main() {
  uchar* adr;

  adr = (uchar*) 0xffff6003;
  *adr = 0x04;
}