#include <stdio.h>
#include <math.h>
  ranf(r)
  double *r;
   {int ir;
    ir=rand();
    ir=ir & 0xfffffff;
    *r=(double)ir/268435455.0;
    return(0);
   }
  rannor(a,b)
  double *a,*b;
  {double t,r,r1,r2,r3,r4,aa,bb;
   ranf(&r1);
   t=6.283185*r1;
   ranf(&r2);
   r3=1.0/r2;
   r4=2.0*log(r3);
   r=sqrt(r4);
   aa=r*cos(t);
   bb=r*sin(t);
/*   printf("r=%f t=%f r1=%f r2=%f r3=%f r4=%f\n",r,t,r1,r2,r3,r4); */
   *a=aa;
   *b=bb;
/*   printf("rannor a=%f b=%f\n",aa,bb); */
   return(0);
  }
