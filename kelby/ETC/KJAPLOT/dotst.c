#include <stdio.h>
#include <math.h>
#include <strings.h>
      main()
      { int i;
        double x,y,r,fac;
        char xstring[80],ystring[80];
        dotset(0,5.0,-5.0,5.0,-5.0,0,0);
        dotset(1,5.0,-5.0,5.0,-5.0,0,0);
        dotset(2,5.0,-5.0,5.0,-5.0,0,0);
        dotset(3,5.0,-5.0,5.0,-5.0,0,0);
        dotset(4,5.0,-5.0,5.0,-5.0,0,0);
        sprintf(xstring,"BIG rannor plot X");
        sprintf(ystring,"BIG rannor plot Y");
        dotlbl(0,xstring,ystring);
        dotlbl(1,xstring,ystring);
        dotlbl(2,xstring,ystring);
        dotlbl(3,xstring,ystring);
        dotlbl(4,xstring,ystring);
        dotset(5,5.0,-5.0,5.0,-5.0,0,0);
        dotset(6,5.0,-5.0,5.0,-5.0,0,0);
        dotset(7,5.0,-5.0,5.0,-5.0,0,0);
        dotset(8,5.0,-5.0,5.0,-5.0,0,0);
        dotset(9,5.0,-5.0,5.0,-5.0,0,0);
        sprintf(xstring,"BIG X line");
        sprintf(ystring,"BIG Y line");
        dotlbl(5,xstring,ystring);
        dotlbl(6,xstring,ystring);
        dotlbl(7,xstring,ystring);
        dotlbl(8,xstring,ystring);
        dotlbl(9,xstring,ystring);
        dotssg(1);   /* print mean and sigmas */
        fac=10.0/9999.0;
        for(i=0;i<10000;i++)
         { x=-5.0+(double)i*fac;
           y=0.0;
           dotacc(5,x,y);
           dotacc(5,x,x);
           dotacc(6,x,y);
           dotacc(7,x,y);
           dotacc(8,x,y);
           dotacc(9,x,y);
           y=x;
           x=0.0;
           dotacc(5,x,y);
           dotacc(6,x,y);
           dotacc(7,x,y);
           dotacc(8,x,y);
           dotacc(9,x,y);
          }
        for(i=0;i<10000;i++)
         { rannor(&x,&y);
/*           printf(" calling dotacc x=%f y=%f\n",x,y); */
           dotacc(0,x,y); 
           dotacc(1,x,x); 
           dotacc(2,y,y); 
           dotacc(3,x,y); 
           dotacc(4,x,y); 
         }
        dotwrt(0); 
        dotwrt(1); 
        dotwrt(2); 
        dotwrt(3); 
        dotwrt(4); 
        dotwrt(5); 
        dotwrt(6); 
        dotwrt(7); 
        dotwrt(8); 
        dotwrt(9);  
        dotmwr(4,3,6);
      }
