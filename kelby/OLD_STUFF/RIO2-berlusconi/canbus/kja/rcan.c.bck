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
   k =*adr = 0x01;
   printf("control reg=%x\n",k);
   
   k = 0xdffe600b; /* ACCEPTANCE MASK */
   adr = (unsigned char*) k;
   k = *adr = 0xff;
   printf("acceptance mask=%x\n",k);
   
   k = 0xdffe6009; /* ACCEPTANCE CODE REGISTER */
   adr = (unsigned char*) k;
   k =*adr;
   printf("acceptance code reg=%x\n",k);
   
   k = 0xdffe600d; /* BTR0 */
   adr = (unsigned char*) k;
   k = *adr;  
   printf("BTR0=%x\n",k);

   k = 0xdffe600f; /* BTR1 */
   adr = (unsigned char*) k;
   k =*adr;  /*BTR 1*/
   printf("btr1=%x\n",k);

   k = 0xdffe6011;  /* OCR */
   adr = (unsigned char*) k;
   k = *adr;
   printf("OCR=%x\n",k);
 
}

