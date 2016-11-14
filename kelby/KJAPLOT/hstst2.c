#include <stdio.h
#include <math.h
main(
{
   double x,weight,xhi,xlo,dx
   int i,iplot1,iplot2,nhi,nbin
    xhi=3.14
    xlo=0.0
    nbin=100
    nhi=1
    iplot1=0
    iplot2=1
    hstset(iplot1,xhi,xlo,nbin,nhi)
    hstset(iplot2,xhi,xlo,nbin,nhi)
/*  printf("starting for loop in hstst\n"); fflush(stdout); *
    dx=3.14/100.0
    for(i=0;i<100;i++
       { x=xlo+dx*(double)i
         weight=sin(x)
         if(weight<0.0) weight=-weight
/*  printf("calling hstacc 1\n"); fflush(stdout); *
         hstacc(iplot1,x,weight)
/*  printf("calling hstacc 1\n"); fflush(stdout); *
         hstacc(iplot2,x,weight);
    hstwrt(iplot1)
    hstwrt(iplot2)
   return(0);
}

