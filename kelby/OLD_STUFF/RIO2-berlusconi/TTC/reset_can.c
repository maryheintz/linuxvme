#include <stdio.h>
#include <math.h>
int ttcadr;
main()
{ 
   ttcadr = 0;
   ttcvi_map(0);
   reset_ctrl_can();    /* set up the TIP810 canbus controller */
   reset_can(0,0);
   usleep(5);
   reset_can(0,1);
   sleep(1);
   reset_can(0,0);
   usleep(5);     
}
