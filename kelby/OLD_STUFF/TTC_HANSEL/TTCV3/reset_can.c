#include <stdio.h>
#include <math.h>
int ttcadr;
main()
{ 
   ttcadr = 0;
   ttcvi_map(ttcadr);
   reset_can(ttcadr,0);
   usleep(5);
   reset_can(ttcadr,1);
   sleep(1);
   reset_can(ttcadr,0);
   usleep(5);
}
