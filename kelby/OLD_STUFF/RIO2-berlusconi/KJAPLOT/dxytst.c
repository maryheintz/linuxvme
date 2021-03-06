#include <stdio.h>
#include <strings.h>
#include <math.h>
      main()
      { int i;
        char xstring[80],ystring[80];
        char *text;
        double pi,x,y,dy;
        dxyft_set(1,2,50);
        pi=3.14159;
        sprintf(xstring,"X axis label");
        sprintf(ystring,"Y axis label");
        dxyset(1,pi,0.0,100.0,0.0,0,0);
        dxylbl(1,xstring,ystring); 
        dxyset(2,pi,0.0,10.0,0.0,0,0);
        dxylbl(2,xstring,ystring); 
        dxyset(3,50.0,0.0,100.0,0.0,4,0);
        dxylbl(3,xstring,ystring); 
        dxyset(4,pi,0.0,100.0,0.0,0,0);
        dxylbl(4,xstring,ystring); 
        dxyset(5,pi,0.0,10.0,0.0,0,0);
        dxylbl(5,xstring,ystring); 
        dxyset(6,50.0,5.0,100.0,0.01,1,4);
        dxylbl(6,xstring,ystring); 
        for (i=0;i<50;i++)
         { x=pi*(double)(i+1)/51.0;
           y=90.0*sin(x);
          if(y<=0.0) goto s10;
          dy=sqrt(y);
    /*      y=y+dy*(ranf(0)-0.5);  */
          dxyacc(1,x,y,dy,1);
          dxyacc(4,x,y,dy,2);
   s10:
          y=9.5*fabs(cos(x));
          if(y<=0.0) goto s20;
          dy=sqrt(y)/3.0;
  /*       y=y+dy*(ranf(0)-0.5) */
          dxyacc(2,x,y,dy,4);
          dxyacc(5,x,y,dy,3);
   s20:
          x=(double)i;
          y=90.0*exp(-0.1*x);
          dy=sqrt(y)/10.0;
/*        y=y+dy*(ranf(0)-0.5) */
          dxyacc(3,x,y,dy,5);
          dxyacc(6,x,y,dy,6);
        }
        dxywrt(1); 
        dxywrt(2);
        dxymwr(2,1,2); 
      return(0);
      }
      double dxyfit(int l,int k,double x)
      { double dxyf;
        dxyf=45.0;
        if(k==2) dxyf=90.0*sin(x);
        return(dxyf);
      }
