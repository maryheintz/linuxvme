#include <stdio.h>
#include <strings.h>
#include <math.h>
#include "dxy.h"
      main()
      { int i;
        char xstring[80],ystring[80];
        char *text;
        double pi,x,y,dy; 
        xyft.nplts[1]=2;
        xyft.ivcts[1]=50;
        pi=3.14159;
        sprintf(xstring,"charge [pC]");
        sprintf(ystring,"volts");
        dxyset(1,500.0,0.0,5.0,0.0,0,0);
        dxylbl(1,xstring,ystring); 
          dxyacc(1,50.0,0.52,0.0,1);
          dxyacc(1,100.0,1.08,0.0,1);
          dxyacc(1,150.0,1.68,0.0,1);
          dxyacc(1,200.0,2.24,0.0,1);
          dxyacc(1,250.0,2.82,0.0,1);
          dxyacc(1,300.0,3.38,0.0,1);
          dxyacc(1,325.0,3.66,0.0,1);
          dxyacc(1,350.0,3.90,0.0,1);
          dxyacc(1,375.0,4.04,0.0,1);
        dxywrt(1); 
      return(0);
      }
      double dxyfit(l,k,x)
      int l,k;
      double x;
      { double dxyf;
        dxyf=45.0;
        if(l==2) return(dxyf);
        dxyf=90.0*sin(x);
        return(dxyf);
      }