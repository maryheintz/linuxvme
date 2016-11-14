#include <stdio.h>
#include <math.h>
#include <strings.h>
      main()
      { int i;
        double angle,d,y,p;
        char xstring[80],ystring[80];
        dotset(0,300.0,0.0,100.0,0.0,0,0);
        y = pow(10.0,-1);
	printf("y=%f\n",y);
        for(i=0;i<270;i++)
         { angle = (double)i;
	   d=2.0*angle/270.0;
	   p = pow(10.0,-d);
           y=100.0*p;
           printf("i=%d  angle=%f  d=%f  p=%f  y=%f\n",i,angle,d,p,y);
           dotacc(0,angle,y);
         }
        dotwrt(0); 
      }
