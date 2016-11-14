#define IDALLOC 107
#define CANSET 11
#define CANGET 12
#define CONVERT 15
#define DELAY 10
#define DACSET 9
#define REQUEST 14
#define ACK 51
#define GO_FB 101
#define GO_ISP 102
#define INIT 1
#include <stdio.h>
#include <math.h>
int id,ttcadr;
main()
{ 
   int tube,i,k,board;
   char serial[7] = {"PS1001\0"};
   unsigned int i1,i2,itst;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
   long status;
   ttcadr = 0x1234;
/*   printf("enter ttcadr\n");
   fscanf(stdin,"%x",&ttcadr); */
   board = 1;
   id = (board<<6) | 1;
   ttcvi_map(ttcadr);
   reset_ctrl_can();
   printf("calling can_reset\n");
   can_reset(ttcadr);  
   printf("calling can_init\n");
loop:
   can_initxx(board,serial);  
   printf("returned from can_init\n");
   goto loop;
}
can_initxx(int board,unsigned char serial[6] )
{  int i,j,id;
   char version[8];
   unsigned short dat;
   long stat,status;
   unsigned char data[8],datarx[8];
   unsigned char dlc,rtr;
   int idrd,id_adc;
   printf("serial=%s  board=%d\n",serial,board);
   id = (board<<6) | 1;
   data[0] = IDALLOC;
   data[1] = serial[0];
   data[2] = serial[1];
   data[3] = serial[2];
   data[4] = serial[3];
   data[5] = serial[4];
   data[6] = serial[5];
   data[7] = board;
   can_tx(0,8,0,data);
   printf("can_tx completed\n");
/*   status=can_rx(&idrd, &dlc, &rtr, datarx);
   printf("can rx complete\n");
   id_adc = datarx[7];
   printf("id adc = %x\n", id_adc);

   printf("calling can_adc_init GO_FB\n");  
   can_adc_init(id,GO_FB,&version[0]);
   for (i=0;i<7;i++) printf("%c", version[i]);
   printf(" - VERSION %d\n", (int)version[7]);  
   usleep(2);  */
}
