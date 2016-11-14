#include <stdio.h>
#include <math.h>
int id,ttcadr;
main()
{ 
   int tube,i,k,board;
   unsigned int i1,i2,itst;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
   long status;
   printf(" enter ttcadr in hex\n");
   fscanf(stdin,"%x",&ttcadr);
   printf("ttcadr = %x\n",ttcadr); 
   ttcvi_map(ttcadr);
   id=0;
   can_multi_low(id);
   can_bkld_low(id);
loop:
   can_tube(id,38);
   can_tube(id,40);
   goto loop;
   }


