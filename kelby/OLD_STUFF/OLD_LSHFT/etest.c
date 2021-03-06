#include <stdio.h>
#include <math.h>
unsigned int sw_base,vme_base;
double a1,b1,a2,b2,a3,b3;
main()
{ int i,l,k,ibin;
  double E;
  unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
  init_by_array(init, length); 
  vme_base = vme_a24_base();
  sw_base = vme_base + 0xaa0000;
  set_dist();
  hstset(0,2.5,0.0,200,-7);  
  hstset(1,2.5,0.0,200,-7);  
loop:
     pick(&E);
     hstacc(0,E,1.0);
  if(sw(7)==1) 
   { hstwrt(0);
     hstwrt(1);
   }
  goto loop; 
}
sw(i)
int i;
{ int k,l,shift;
  unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)(sw_base + 0x0c);
  k = *adr;
  shift = i-1;
  l = (k>>shift) & 1;
  return(l);
}
set_dist()
 { /* -------------- zone 1  0.02<E<0.12 -------------- */
   b1 = -log(0.07)/0.12;
   a1 = 100000.0;
   printf("a1=%f  b1=%f\n",a1,b1);
          /* y = a1*pow(2.71828183,-b1*E) */
   /* -------------- zone 2  0.12<E<0.7 ------------- */ 
   b2 = (log(70.0)-log(5.0))/0.58;
   a2 = 500.0*pow(2.71828183,0.7*b2);
   printf("a2=%f  b2=%f\n",a2,b2);
          /* y = a2*pow(2.71828183,-b2*E) */
   /* -------------- zone 3  E<0.7 ------------------ */ 
   b3 = log(500.0)/2.8;
   a3 = pow(2.71828183,3.5*b3);
   printf("a3=%f  b3=%f\n",a3,b3);
          /* y = a3*pow(2.71828183,-b3*E) */
   /* -------------- zone 2  0.0<E<0.01------------- */ 
     /* lowest bin y = pow(10,7.0)  */

   return(0);
 }
pick(double *E)
 { double r,test,y,e;
   static double range=2500000.0;
again:
   rmt(&r);
   e = 2.5*r;
   if(e<0.02) 
    { hstacc(1,e,1.0);
      *E=e;
      return(0);
    }
   if(e>=0.02 && e<0.12)
     { rmt(&r);
       test = r * range;
       y = a1*pow(2.71828183,-b1*e);
       if(test>y) goto again;
       hstacc(1,e,1.0);
       *E=e;
       return(0);
     }
   if(e>=0.12 && e<0.7)
     { rmt(&r);
       test = r * range;
       y = a2*pow(2.71828183,-b2*e);
       if(test>y) goto again;
       hstacc(1,e,1.0);
       *E=e;
       return(0);
     }
   if(e>=0.7)
     { rmt(&r);
       test = r * range;
       y = a3*pow(2.71828183,-b3*e);
       if(test>y) goto again;
       hstacc(1,e,1.0);
       *E=e;
       return(0);
     }
 }  
    double hstfit(i,l,x)
    int i,l;
    double x;
    {double y;
     y=90.0*exp(-0.1*x);
     return(y); 
    }  
       
