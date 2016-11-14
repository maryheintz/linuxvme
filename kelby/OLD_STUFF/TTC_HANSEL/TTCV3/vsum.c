#include <stdio.h>
#include <math.h>
main()
{  int i;
   unsigned char chksum;
   unsigned char k[20] =
              {0x10,
	       0x00,
	       0x00,
	       0x00,
	       0x05,
	       0xef,
	       0xdf,
	       0x7b,
	       0x2f,
	       0x71,
	       0xe1,
	       0x03,
	       0x1c,
	       0x00,
	       0x00,
	       0x00,
	       0x00,
	       0x00,
	       0x00,
	       0x00};
   unsigned int sum;
   sum = 0;
   for(i=0;i<20;i++)
     { sum = sum + (unsigned int)k[i];
        printf("i=%d  entry=%x  sum=%x\n",i,(unsigned int)k[i],sum);
     }
   chksum = ~sum + 1;
   chksum = chksum & 0xff;
   printf(" chksum=%x\n",chksum);
   printf("%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
   k[0],k[1],k[2],k[3],k[4],
   k[5],k[6],k[7],k[8],k[9],
   k[10],k[11],k[12],k[13],k[14],
   k[15],k[16],k[17],k[18],k[19],chksum);
    
}
