#include <stdio.h>
#include <math.h>
#include <time.h>
int id,ttcadr;
main()
{ 
   int tube,i,k,board;
   char serial[7] = {"PS1008\0"};
   unsigned short shrt;
   long status;
   int card;
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
   can_init(board,serial);  
   printf("returned from can_init\n");
   printf("enter card\n");
   fscanf(stdin,"%d",&card);
   set_tube(ttcadr,card);
strt:
   SendLongCommand(ttcadr,1,0xf5,0x55);
   usleep(1000);
   status=can_3in1set(id,0xd400);   /* fetch from 3in1 */
   usleep(1000);
   status=can_3in1get(id,&shrt);    /* and load can output reg */
   usleep(1000);
   printf(" sent 0x1555  returned %x\n",shrt);
   goto strt;   
}
