#include <stdio.h>
#include <math.h>
main()
{ 
   ttcvi_map();
   reset_can(0);   /* set can reset bit on via TTC */
   sleep(1);
}

