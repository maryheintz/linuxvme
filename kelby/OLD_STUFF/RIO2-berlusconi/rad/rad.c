#include <stdio.h>
#include <strings.h>
#include <math.h>
#include "dxy.h"
      main()
      { int i;
        double dose[21]={0.297,0.509,1.005,1.503,2.000,2.510,3.007,3.504,
	                 4.002,4.512,5.009,5.507,6.004,6.501,7.053,7.500,
			 8.010,8.509,9.006,9.504,10.000};
        double current[21]={32.94,32.94,32.92,33.01,32.91,32.92,33.15,
	                    33.16,33.19,33.24,33.52,33.88,34.50,35.16,
			    36.02,36.97,38.11,39.57,41.44,43.51,46.00};
        double rmin[24] ={0.0,501.5,1002.9,1504.9,2001.3,2503.7,3000.2,
	                  3501.7,4003.1,4504.6,5001.1,5502.5,6004.0,
			  6500.5,7001.9,7503.4,8004.9,8501.3,9002.8,
			  9504.2,10000.7,10502.2,11003.7,11465.0};
	double rcurrent[24]={40.19,39.60,39.34,39.38,39.12,38.66,37.89,
	                     37.75,37.61,37.40,37.35,37.21,37.51,37.41,
			     37.35,37.30,37.84,37.16,37.11,37.10,37.00,
			     36.95,36.84,36.82};
        double hmin[22] ={0.0,500.1,1000.0,1500.2,2000.1,2500.0,3000.1,
	                  3501.1,4000.1,4500.1,5000.1,5500.0,6000.2,
			  6500.0,7000.0,7500.0,8000.0,8500.0,9000.0,
			  9500.0,10000.0,10500.0};
	double hcurrent[22]={36.94,35.64,34.74,34.21,34.03,33.54,
	                     33.43,33.20,32.97,32.92,32.92,32.84,
			     32.79,32.45,32.60,32.75,32.55,32.45,
			     32.55,32.80,32.65,31.85};
        char xstring[80],ystring[80];
        char *text;
        double x,y,dy; 
        xyft.nplts[1]=0;
        xyft.ivcts[1]=0;
        sprintf(xstring,"Dose [KRad]");
        sprintf(ystring,"Current [ma]");
        dxyset(0,10.0,0.0,50.0,0.0,0,0);
        dxylbl(0,xstring,ystring); 
        for (i=0;i<21;i++)
         { x=dose[i];
           y=current[i];
	   dy=0.0;
          dxyacc(0,x,y,dy,1);
        }
        sprintf(xstring,"time [weeks] at room temp");
        sprintf(ystring,"Current [ma]");
        dxyset(1,1.0,0.0,50.0,0.0,0,0);
        dxylbl(1,xstring,ystring); 
        for (i=0;i<24;i++)
         { x=rmin[i]/(1440.0 * 7.0);
           y=rcurrent[i];
	   dy=0.0;
          printf(" x=%f  y=%f\n",x,y);
          dxyacc(1,x,y,dy,1);
        }
        sprintf(xstring,"time [weeks] at 100 deg C");
        sprintf(ystring,"Current [ma]");
        dxyset(2,1.0,0.0,50.0,0.0,0,0);
        dxylbl(2,xstring,ystring); 
        for (i=0;i<22;i++)
         { x=hmin[i]/(1440.0 * 7.0);
           y=hcurrent[i];
	   dy=0.0;
          dxyacc(2,x,y,dy,1);
        }
        dxywrt(0);
	dxywrt(1); 
	dxywrt(2); 
        dxymwr(3,0,2);
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
