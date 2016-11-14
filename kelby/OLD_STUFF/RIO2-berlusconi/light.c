#include <stdio.h>
#include <math.h>
main()
{ 
     hstset(0,25.0,15.0,40,0);
     hstacc(0,22.88,1.0);
     hstacc(0,21.54,1.0);
     hstacc(0,20.16,1.0);
     hstacc(0,20.34,1.0);
     hstacc(0,21.82,1.0);
     hstacc(0,20.93,1.0);
     hstacc(0,20.84,1.0);
     hstacc(0,20.58,1.0);
     hstacc(0,21.23,1.0);
     hstacc(0,18.85,1.0);
     hstacc(0,20.95,1.0);
     hstacc(0,21.19,1.0);
     hstacc(0,20.43,1.0);
     hstacc(0,20.78,1.0);
     hstacc(0,21.05,1.0);
     hstacc(0,20.78,1.0);
     hstacc(0,21.12,1.0);
     hstacc(0,21.31,1.0);
     hstacc(0,17.94,1.0);
     hstacc(0,20.81,1.0);
     hstacc(0,20.16,1.0);
     hstacc(0,19.62,1.0);
     hstacc(0,17.87,1.0);
     hstacc(0,19.83,1.0);
     hstwrt(0);
  }
    double hstfit(i,l,x)
    int i,l;
    double x;
    {double y;
     y=90.0*exp(-0.1*x);
     return(y); 
    }  
