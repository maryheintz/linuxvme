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
   hstset(11,pi,0.0,100,0);
   hstset(12,pi,0.0,100,0);
   hstset(13,pi,0.0,100,0);
   hstset(14,pi,0.0,100,0);
   hstset(15,pi,0.0,100,0);
   hstset(16,pi,0.0,100,0);
   hstset(17,pi,0.0,100,0);
   hstset(18,pi,0.0,100,0);
   hstset(19,pi,0.0,100,0);
   hstset(20,pi,0.0,100,0);
   hstset(21,pi,0.0,100,0);
   hstset(22,pi,0.0,100,0);
   hstset(23,pi,0.0,100,0);
   hstset(24,pi,0.0,100,0);
   hstset(25,pi,0.0,100,0);
   hstset(26,pi,0.0,100,0);
   hstset(27,pi,0.0,100,0);
   hstset(28,pi,0.0,100,0);
   hstset(29,pi,0.0,100,0);
   hstset(30,pi,0.0,100,0);
   for(i=1;i<101;i++)
   { hstbin(1,i,&x,&yy);
     y=45.0*sin(x);
     if(y<0.0) y=0.0;
     hstacc(1,x,y);
     hstacc(4,x,y);
     y=y*10000.0;
     hstacc(7,x,y);
     hstacc(10,x,y);
     hstacc(11,x,y);
     hstacc(12,x,y);
     hstacc(13,x,y);
     hstacc(14,x,y);
     hstacc(15,x,y);
     hstacc(16,x,y);
     hstacc(17,x,y);
     hstacc(18,x,y);
     hstacc(19,x,y);
     hstacc(20,x,y);
     hstacc(21,x,y);
     hstacc(22,x,y);
     hstacc(23,x,y);
     hstacc(24,x,y);
     hstacc(25,x,y);
     hstacc(26,x,y);
     hstacc(27,x,y);
     hstacc(28,x,y);
     hstacc(29,x,y);
     hstacc(30,x,y);
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
    hstwrt(30);
    return(0);
  }
    double hstfit(i,l,x)
    int i,l;
    double x;
    {double y;
     y=90.0*exp(-0.1*x);
     return(y); 
    }  
