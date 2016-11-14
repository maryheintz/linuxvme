#include <stdio.h>
#include <math.h>
#include <string.h>
#include <math.h>
#include "machine.h"
double p[10];

extern double tol;
extern int prin, maxfun;
int npnts;
double chisq1,aguess,tguess;
int prnt;
double x[100],y[100];
double t[100],a[100];
double yfit[100],sigmay[100];
extern double functn(); 
extern double ranf();
extern double chisqr();
extern double praxis();
double dt0;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { FILE *fp;
    double xx,yy,dx,r,rn1,rn2;
    double sig,amp,dt,damp,t0,kmean,ksig;
    char xstring[80],ystring[80];
    int i,k,ev,pass,display,i0,i1,i2,i0b,i1b,i2b;
    double p0bst,p1bst,p2bst,x2bst,p0strt,p1strt,p2strt;
    double ampbst,ampsav,t0bst,t0sav,widbst,widsav;
    double ampmn,ampsg,timmn,timsg,widmn,widsg;
    double x2last,da;
    fp=fopen("gauss.out","w");
    pass=0;
   prnt=0;
   prin=0;
   dt0=0.1;
   amp=5.0;
   damp = 1.5;
loop:
   display=1;
   if(amp<50.0) da=5.0;
   if(amp>=50.0) da=50.0;
   amp=amp+da;
   ev=0;
   hstrst();
   hstset(0,25.0,-25.0,100,1);
   hstlbl(0,"amplitude error");
   hstset(1,5.0,-5.0,100,1);
   hstlbl(1,"time error");
   hstset(2,1.0,-1.0,100,1);
   hstlbl(2,"width error");
   hstset(3,10000.0,0.0,100,1);
   hstlbl(3,"chisqr");
again:
   ev++;
   if(ev>10) display=0;
   if(pass>0) display=1;
   printf("\n starting ev=%d\n",ev);
   fflush(stdout);
/* generate data points */
   if(display==1)
    { dotzro();
      dotset(0,300.0,100.0,2.0*amp,0.0,0,0);
    }
   sig=17.0;
   /* amp = 50.0; */
   dt = 25.0;
   npnts = 16;
   r = ranf();
   t0 = (7.5+r)*25.0;
   gendata(t0,damp,dt,npnts,amp,sig);   
   for(i=0;i<npnts;i++)
     { x[i] = t[i];
       y[i] = a[i];
       if(prnt>1) printf("setup i=%d  x[i]=%f  y[i]=%f   %f\n",i,x[i],y[i],t[i]);
     }
     rannor(&rn1,&rn2);
     p0strt = 1000.0+10.0*rn1;
     rannor(&rn1,&rn2);
     p1strt = t0+2.0*rn1;
     rannor(&rn1,&rn2);
     p2strt = 17.0+1.0*rn1;
     p[0] = aguess;
     p[1]=tguess;
     p[2]=17.0;
     rn2=chisqr();
     if(prnt==1) printf(" amp=%f  t0=%f  width=%f\n",amp,t0,sig);
     if(prnt==1) printf(" start values p = %f  %f  %f  x2=%f\n",p[0],p[1],p[2],rn2);
/*     rn2 = praxis(chisqr,p,3);
     rn2 = chisqr();
     if(rn2<10.0) goto finefit;  */
     /* printf(" recalc praxis x2=%f  %f %f %f\n",rn2,p[0],p[1],p[2]); */
/* ==================================== 1st grid search ========================================== */
/* scan for a better t0 */
     printf(" 1st grid start\n");
     x2bst=rn2;
     t0bst = p[1];
     t0sav = p[1];
     x2last=1.0e10;
     for(i0=0;i0<300;i0++)
       { p[1] = t0sav + (double)(i0-150) * 2.0;
         rn2 = chisqr();
	 /* if(rn2>x2last) break; */
	 /* printf(" t0sav=%f  p[1]=%f  rn2=%f  x2bst=%f\n",t0sav,p[1],rn2,x2bst); */
	 if(rn2<x2bst)
	   { x2bst = rn2;
	     t0bst = p[1];
	     /* printf("best=%f  x2=%f\n",p[0],rn2); */
             if(rn2<10.0) goto finefit;
	   }
          x2last = rn2;
       }
     p[1]=t0bst;
     rn2 = x2bst; 
/* scan for a better amp */
     x2bst=rn2;
     ampbst = p[0];
     ampsav = p[0];
     x2last=1.0e10;
     for(i0=0;i0<200;i0++)
       { p[0] = ampsav + (double)(i0-100) * 0.5;
         rn2 = chisqr();
	 /* if(rn2>x2last) break; */
	 /* printf(" t0sav=%f  p[1]=%f  rn2=%f  x2bst=%f\n",t0sav,p[1],rn2,x2bst); */
	 if(rn2<x2bst)
	   { x2bst = rn2;
	     ampbst = p[0];
             if(rn2<10.0) goto finefit;
	     /* printf("best=%f  x2=%f\n",p[0],rn2); */
	   }
	 x2last = rn2;
       }
     p[0]=ampbst;
     rn2 = x2bst; 
/* scan for a better width */
     x2bst=rn2;
     widbst = p[2];
     widsav = p[2];
     x2last=1.0e10;
     for(i0=0;i0<200;i0++)
       { p[2] = widsav + (double)(i0-100) * 0.2;
         rn2 = chisqr();
	 /* if(rn2>x2last) break; */
	 /* printf(" t0sav=%f  p[1]=%f  rn2=%f  x2bst=%f\n",t0sav,p[1],rn2,x2bst); */
	 if(rn2<x2bst)
	   { x2bst = rn2;
	     widbst = p[2];
            if(rn2<10.0) goto finefit;
	     /* printf("best=%f  x2=%f\n",p[0],rn2); */
	   }
	 x2last = rn2;
       }
     p[2]=widbst;
     rn2 = x2bst; 

//* ==================================== 1st grid search ========================================== */
     printf(" 1st grid repeat start\n");
/* scan for a better t0 */
     x2bst=rn2;
     t0bst = p[1];
     t0sav = p[1];
     x2last=1.0e10;
     for(i0=0;i0<300;i0++)
       { p[1] = t0sav + (double)(i0-150) * 0.2;
         rn2 = chisqr();
	 /* if(rn2>x2last) break; */
	 /* printf(" t0sav=%f  p[1]=%f  rn2=%f  x2bst=%f\n",t0sav,p[1],rn2,x2bst); */
	 if(rn2<x2bst)
	   { x2bst = rn2;
	     t0bst = p[1];
	     /* printf("best=%f  x2=%f\n",p[0],rn2); */
	   }
          x2last = rn2;
       }
     p[1]=t0bst;
     rn2 = x2bst; 
/* scan for a better amp */
     x2bst=rn2;
     ampbst = p[0];
     ampsav = p[0];
     x2last=1.0e10;
     for(i0=0;i0<200;i0++)
       { p[0] = ampsav + (double)(i0-100) * 0.5;
         rn2 = chisqr();
	 /* if(rn2>x2last) break; */
	 /* printf(" t0sav=%f  p[1]=%f  rn2=%f  x2bst=%f\n",t0sav,p[1],rn2,x2bst); */
	 if(rn2<x2bst)
	   { x2bst = rn2;
	     ampbst = p[0];
             if(rn2<10.0) goto finefit;
	     /* printf("best=%f  x2=%f\n",p[0],rn2); */
	   }
	 x2last = rn2;
       }
     p[0]=ampbst;
     rn2 = x2bst; 
/* scan for a better width */
     x2bst=rn2;
     widbst = p[2];
     widsav = p[2];
     x2last=1.0e10;
     for(i0=0;i0<200;i0++)
       { p[2] = widsav + (double)(i0-100) * 0.2;
         rn2 = chisqr();
	 /* if(rn2>x2last) break; */
	 /* printf(" t0sav=%f  p[1]=%f  rn2=%f  x2bst=%f\n",t0sav,p[1],rn2,x2bst); */
	 if(rn2<x2bst)
	   { x2bst = rn2;
	     widbst = p[2];
	     /* printf("best=%f  x2=%f\n",p[0],rn2); */
             if(rn2<10.0) goto finefit;
	   }
	 x2last = rn2;
       }
     p[2]=widbst;
     rn2 = x2bst; 

/* ==================================== 2nd grid search ========================================== */
/* scan for a better t0 */
     printf(" 2nd grid start\n");
     x2bst=rn2;
     t0bst = p[1];
     t0sav = p[1];
     x2last=1.0e10;
     for(i0=0;i0<300;i0++)
       { p[1] = t0sav + (double)(i0-150) * 0.1;
         rn2 = chisqr();
	 /* if(rn2>x2last) break; */
	 /* printf(" t0sav=%f  p[1]=%f  rn2=%f  x2bst=%f\n",t0sav,p[1],rn2,x2bst); */
	 if(rn2<x2bst)
	   { x2bst = rn2;
	     t0bst = p[1];
	     /* printf("best=%f  x2=%f\n",p[0],rn2); */
             if(rn2<10.0) goto finefit;
	   }
          x2last = rn2;
       }
     p[1]=t0bst;
     rn2 = x2bst; 
/* scan for a better amp */
     x2bst=rn2;
     ampbst = p[0];
     ampsav = p[0];
     x2last=1.0e10;
     for(i0=0;i0<200;i0++)
       { p[0] = ampsav + (double)(i0-100) * 0.25;
         rn2 = chisqr();
	 /* if(rn2>x2last) break; */
	 /* printf(" t0sav=%f  p[1]=%f  rn2=%f  x2bst=%f\n",t0sav,p[1],rn2,x2bst); */
	 if(rn2<x2bst)
	   { x2bst = rn2;
	     ampbst = p[0];
	     /* printf("best=%f  x2=%f\n",p[0],rn2); */
             if(rn2<10.0) goto finefit;
	   }
	 x2last = rn2;
       }
     p[0]=ampbst;
     rn2 = x2bst; 
/* scan for a better width */
     x2bst=rn2;
     widbst = p[2];
     widsav = p[2];
     x2last=1.0e10;
     for(i0=0;i0<100;i0++)
       { p[2] = widsav + (double)(i0-50) * 0.1;
         rn2 = chisqr();
	 /* if(rn2>x2last) break; */
	 /* printf(" t0sav=%f  p[1]=%f  rn2=%f  x2bst=%f\n",t0sav,p[1],rn2,x2bst); */
	 if(rn2<x2bst)
	   { x2bst = rn2;
	     widbst = p[2];
	     /* printf("best=%f  x2=%f\n",p[0],rn2); */
             if(rn2<10.0) goto finefit;
	   }
	 x2last = rn2;
       }
     p[2]=widbst;
     rn2 = x2bst; 

/* ==================================== 3rd grid search ========================================== */
finefit:
     printf(" finefit 3rd grid start\n");
/* scan for a better t0 */
     x2bst=rn2;
     t0bst = p[1];
     t0sav = p[1];
     x2last=1.0e10;
     for(i0=0;i0<300;i0++)
       { p[1] = t0sav + (double)(i0-150) * 0.01;
         rn2 = chisqr();
	 /* if(rn2>x2last) break; */
	 /* printf(" t0sav=%f  p[1]=%f  rn2=%f  x2bst=%f\n",t0sav,p[1],rn2,x2bst); */
	 if(rn2<x2bst)
	   { x2bst = rn2;
	     t0bst = p[1];
	     /* printf("best=%f  x2=%f\n",p[0],rn2); */
	   }
          x2last = rn2;
       }
     p[1]=t0bst;
     rn2 = x2bst; 
/* scan for a better amp */
     x2bst=rn2;
     ampbst = p[0];
     ampsav = p[0];
     x2last=1.0e10;
     for(i0=0;i0<100;i0++)
       { p[0] = ampsav + (double)(i0-50) * 0.01;
         rn2 = chisqr();
	 /* if(rn2>x2last) break; */
	 /* printf(" t0sav=%f  p[1]=%f  rn2=%f  x2bst=%f\n",t0sav,p[1],rn2,x2bst); */
	 if(rn2<x2bst)
	   { x2bst = rn2;
	     ampbst = p[0];
	     /* printf("best=%f  x2=%f\n",p[0],rn2); */
	   }
	 x2last = rn2;
       }
     p[0]=ampbst;
     rn2 = x2bst; 
/* scan for a better width */
     x2bst=rn2;
     widbst = p[2];
     widsav = p[2];
     x2last=1.0e10;
     for(i0=0;i0<100;i0++)
       { p[2] = widsav + (double)(i0-50) * 0.01;
         rn2 = chisqr();
	 /* if(rn2>x2last) break; */
	 /* printf(" t0sav=%f  p[1]=%f  rn2=%f  x2bst=%f\n",t0sav,p[1],rn2,x2bst); */
	 if(rn2<x2bst)
	   { x2bst = rn2;
	     widbst = p[2];
	     /* printf("best=%f  x2=%f\n",p[0],rn2); */
	   }
	 x2last = rn2;
       }
     p[2]=widbst;
     rn2 = x2bst; 

     printf("  grid ended\n");

     /* rn2 = praxis(chisqr,p,3);
     rn2 = chisqr(); */
     for(i=0;i<1000;i++)
      {xx = (double)i * 0.4;
       yy = functn(xx);
       if(display==1) dotacc(0,xx,yy);
       /* printf(" fit i=%d  xx=%f  yy=%f\n",i,xx,yy);  */
      }
   for(i=0;i<npnts;i++)
     { xx = t[i];
       yy = a[i];
       if(prnt>1) printf(" data points i=%d  t=%f  a=%f\n",i,xx,yy);
       if(display==1)
         { for(k=-5;k<6;k++)
           { dotacc(0,xx+(double)k,yy);
             dotacc(0,xx,yy+1.0*(double)k);
	   }
	 } 
     }
     fflush(stdout);
     usleep(2000);
   if(display==1) dotwrt(0);
   if(rn2>1000.0) 
     { printf("ev=%d  rn2=%f  p=%f %f %f aguess=%f\n",
         ev,rn2,p[0],p[1],p[2],aguess);
       dotwrt(0);
       prnt=1;
       rn2 = chisqr();
       prnt=0; 
       goto again;
     }
   hstacc(0,amp-p[0],1.0);
   hstacc(1,t0-p[1],1.0);
   hstacc(2,sig-p[2],1.0);
   hstacc(3,rn2,1.0);
   /* printf(" ev=%d rn2=%f\n",ev,rn2); */
   if(ev<1000) goto again;
   /* hstwrt(0);
   hstwrt(1);
   hstwrt(2);
   hstwrt(3); */
   rn2 = chisqr();
   hstmsg(0,&ampmn,&ampsg);
   hstmsg(1,&timmn,&timsg);
   hstmsg(2,&widmn,&widsg);
       if(pass==0) printf("   amp       amp mean sig         time mean sig      width mean sig\n");
   if(pass==0) fprintf(fp,"   amp       amp mean sig         time mean sig      width mean sig\n");
   printf("%6.1f      %6.2f  %5.3f         %6.2f  %5.3f       %6.2f  %5.3f    %10.1f\n",
           amp,ampmn,ampsg,timmn,timsg,widmn,widsg,rn2);
   fprintf(fp,"%6.1f      %6.2f  %5.3f         %6.2f  %5.3f       %6.2f  %5.3f    %10.1f\n",
           amp,ampmn,ampsg,timmn,timsg,widmn,widsg,rn2);
   fflush(fp);
   pass=1;
   if(amp<1000.0) goto loop;
   i=fclose(fp);
   }  /* end of main */
 double chisqr()
   { int i;
     double chisq1,sdy,dy;
        for(i=0;i<npnts;i++) yfit[i] = functn(x[i]);
           chisq1=0.0;
	   for(i=0;i<npnts;i++) 
	     { if(y[i]<5.0) continue;
	       dy = y[i]-yfit[i];
	       sdy = sigmay[i]; 
	       chisq1 = chisq1 + (dy*dy)/(sdy*sdy);
	       if(prnt==1) printf("i=%d  y[i]=%f  yfit=%f  dy=%f  chisq1=%f\n",
	         i,y[i],yfit[i],dy,chisq1);
	     }
	  /* printf(" chisqr=%f  p=%f  %f  %f\n",chisq1,p[0],p[1],p[2]); */
	  return(chisq1);
   }
  double functn(double xx)
    { double yft,t1,t2;
      t1 = xx - p[1];
      t2 = 2.0 * p[2]*p[2];
      yft = p[0]*exp(-(t1*t1)/t2);
      /* printf("yft=%f p=%f %f %f\n",yft,p[0],p[1],p[2]); */
      return(yft);
    }
   gendata(double t0,double damp,double dt,int npnts,double amp,double sig)
     { int i,ii;
       double t1,t2,r1,r2,a1,a2,da,asum,tsum,sum;
       if(prnt>1) printf(" t0=%f\n",t0);
       if(prnt>1) printf(" damp=%f\n",damp);
       if(prnt>1) printf(" dt=%f\n",dt);
       if(prnt>1) printf(" npnts=%d\n",npnts);
       if(prnt>1) printf(" amp=%f\n",amp);
       if(prnt>1) printf(" sig=%f\n",sig);
       fflush(stdout);
       sum=0.0;
       for(i=0;i<npnts;i++)
         { t[i] = (double)i * dt;
	   rannor(&r1,&r2);
	   t1 = (t[i]+r1*dt0)-t0;  /* smear the sample time */
	   t2 = 2.0 * sig*sig;
	   a[i] = amp * exp(-(t1*t1)/t2);
	   a1=a[i];
	   rannor(&r1,&r2);
	   da = damp*r1;
	   a[i] = a[i] + da;
	   ii = (int)a[i];
	   a[i] = (double)ii;
	   a2=a[i];
           /* printf("i=%d  a1=%f  a2=%f  da=%f\n",i,a1,a2,da); */
	   if(prnt>1) printf("gendata i=%d  t[i]=%f  a[i]=%f\n",i,t[i],a[i]);
           fflush(stdout);
	   sigmay[i] = 1.0;
	   if(a[i]>5.0) 
	     { sum=sum+1.0;
	       sigmay[i]=1.0;
	       asum = asum + a[i];
	       tsum = tsum + a[i] * (double)i;
	     }
	 }
	 aguess = asum/1.7;
	 tguess = 25.0*tsum/asum;
	 printf("asum=%f  tsum=%f  aguess=%f  tguess=%f\n",
	    asum,tsum,aguess,tguess);
	 return;
     }
      hstfit()
       {return;}
