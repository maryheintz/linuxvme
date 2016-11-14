#define CANSET 11
#define CANGET 12
#define ACK 51
#include <stdio.h>
#include <time.h>
time_t usec;
main()
  { unsigned int id,stat,crate,subject;
    unsigned char rtr,dlc,trt,data[8];
/*    reset_can();  */
    crate=1;
    subject=6;
    id=(subject<<7) | crate;
    stat = can_tx(id,8,1,data);
    usec=1000;
    usleep(usec);
    stat = can_rx(&id,&dlc,&rtr,&data[0]);
    printf(" id=%x  dlc=%x  rtr=%x\n",id,dlc,rtr);
    printf(" data= %x %x %x %x %x %x %x %x\n",
       data[0],data[1],data[2],data[3],data[04],data[5],data[6],data[7]);
    printf(" data= %d %d %d %d %d %d %d %d\n",
       data[0],data[1],data[2],data[3],data[04],data[5],data[6],data[7]);
       
    }
reset_can()
 { unsigned char *adr;
   unsigned int k;
   int sjw,brp,sam,tseg2,tseg1;
   int octp1,octn1,ocpol1,octp0,octn0,ocpol0,ocmode1,ocmode0;
   
   k = 0xdffe6003; /* COMMAND REGISTER */
   adr = (unsigned char*) k;
   *adr = 0x02;  /*ABORT TRANSMISSIONS */
   

   k = 0xdffe6001; /* CONTROL REGISTER */
   adr = (unsigned char*) k;
   *adr = 0x01;  /*RESET HIGH */
   
   k = 0xdffe600b;  /* ACCEPTANCE MASK */
   adr = (unsigned char*) k;  
   *adr = 0xff;  
   
   k = 0xdffe6009;   /* ACCEPTANCE CODE REGISTER */
   adr = (unsigned char*) k;  
   *adr = 0;     
   
   k = 0xdffe600d; /* BTR0 */
   adr = (unsigned char*) k;
   sjw = 1;
   sjw = 0;
   brp = 0;
   *adr = (sjw<<6) | brp;  

   k = 0xdffe600f; /* BTR1 */
   adr = (unsigned char*) k;
   sam = 0;
   tseg2 = 6;
   tseg1 = 7;
   *adr = (sam<<7) | (tseg2<<4) | tseg1;  
/*   *adr = 0x14;  */

   k = 0xdffe6011;  /* OCR */
   adr = (unsigned char*) k;
   octp1 = 0;
   octn1 = 0;
   ocpol1 = 0;
   octp0 = 1;
   octn0 = 1;
   ocpol0 = 0;
   ocmode1 = 1;
   ocmode0 = 0;
   *adr = (octp1<<7) | (octn1<<6) | (ocpol1<<5) | (octp0<<4)
        | (octn0<<3) | (ocpol0<<2) | (ocmode1<<1) | ocmode0;  
/*   *adr = 0xda;  */
 
   k = 0xdffe6001;  /* COMMAND REGISTER */
   adr = (unsigned char*) k;
   *adr = 0x0;  /* Reset Request low */
}

can_tx(id,dlc,rtr,data)
int id;
char dlc,rtr;
char data[8];
 { int i;
   unsigned int k;
   unsigned char *adr,j;
   adr = (unsigned char *)0xdffe6015;
   j = (id>>3);
   *adr = j;
   printf(" writing %x to 0x15\n",j);
   adr = (unsigned char *)0xdffe6017;
   j = (id << 5) | ((rtr&1)<<4) | (dlc&0xf);
   printf(" writing %x to 0x17\n",j);
   *adr = j;
   k = 0xdffe6019;
   for(i=0;i<dlc;i++)
    {adr = (unsigned char *)k;
     *adr = data[i];
     k = k+2;
    }
   adr = (unsigned char *)0xdffe6003;
   *adr = 0x01;
}
can_rx(id,dlc,rtr,data)
int *id;
char *dlc,*rtr;
char data[8];
 { int i,k,count;
   unsigned int kk;
   unsigned char status,dat1,dat2;
   unsigned char *adr;
   count=0;
   adr = (unsigned char *)0xdffe6005;
wait_rx:
   status = *adr;
   count++;
   if(count>10) exit(0);
   printf("status=%x\n",status);
   if((status&1) == 0) goto wait_rx;
   adr = (unsigned char *)0xdffe6029;
   dat1 = *adr;
   printf(" read 6029 = %x\n",dat1);
   adr = (unsigned char *)0xdffe602b;
   dat2 = *adr;
   printf(" read 602b = %x\n",dat2);
   *id = (dat1<<3) | (dat2>>5);
   *rtr = (dat2>>4) & 1;
   k = (dat2 & 0xf);
   *dlc = k;
   kk = 0xdffe602d;
   for(i=0;i<8;i++)
    { adr = (unsigned char *)kk;
      data[i] = *adr;
      kk = kk+2;
    }
   adr = (unsigned char *)0xdffe6003;
   *adr = 4;  /* release receive buffer */

}

