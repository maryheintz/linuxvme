#include <stdio.h>
#include <math.h>
main()
{ 
   ttcvi_map();
   reset_can(1);   /* set can reset bit on via TTC */
   sleep(1);
}
