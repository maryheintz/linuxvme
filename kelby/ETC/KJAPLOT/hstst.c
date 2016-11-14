#include <stdio.h>
#include <math.h>
#include "hst.h"
main()
{ 
   double x,yy,y,pi;
   int i;
   pi=3.14159;
   hstset(1,pi,0.0,100,0);
   hstset(2,pi,0.0,10,1);
   hstset(3,50.0,0.0,20,-4);
   hstset(4,pi,0.0,100,0);
   hstset(5,pi,0.0,10,0);
   hstset(6,50.0,0.0,20,-3);
   hstset(7,pi,0.0,100,0);
   hstset(8,pi,0.0,10,1);
   hstset(9,50.0,0.0,20,-4);
   hstset(10,pi,0.0,100,0);
   for(i=1;i<101;i++)
   { hstbin(1,i,&x,&yy);
     y=45.0*sin(x);
     if(y<0.0) y=0.0;
     hstacc(1,x,y);
     hstacc(4,x,y);
     y=y*10000.0;
     hstacc(7,x,y);
     hstacc(10,x,y);
     if(i>10) goto s20;
     hstbin(2,i,&x,&yy);
     y=9.5*fabs(cos(x));
     if(y<0.0) y=0.0;
     hstacc(2,x,y);
     hstacc(8,x,y);
     hstacc(5,x,y);
 s20:
    if(i<=21) 
     { hstbin(3,i,&x,&yy);
       y=90.0*exp(-0.1*x);
       hstacc(3,x,y);
       hstacc(9,x,y);
       hstacc(6,x,y);}
   }
    hft.nplts[6]=1;
    hft.ivcts[6]=50; 
    hstwrt(6);
    hstwrt(3);
    hstmwr(2,5,6); 
    return(0);
  }
    double hstfit(i,l,x)
    int i,l;
    double x;
    {double y;
     y=90.0*exp(-0.1*x);
     return(y); 
    }  
