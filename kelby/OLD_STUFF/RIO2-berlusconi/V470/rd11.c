#include <stdio.h>
#include <math.h>
#include <time.h>
struct {int dac;
        double av;
	double dav;
	double ped[100];
	double dped[100];
	double dummy[1000];
       }dat;
int dummy[2000];
double adc[30000],fdac[30000],dadc[30000],adac[30000];
int rdac[30000];
int ndac;
FILE *fp,*fpascii;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { int i,k,nwds,iset,ipass;
    double avg,davg,fac;
    double s,sx,sy,sxy,sx2,fdc,x,y,dy,del,a,b;
    double xadc,dac,dacoffset = -13398.0;
    fpascii = fopen("adc11.txt","w");
    fp = fopen("adc11.dat","r");
    dotset(0,0.0,27000.0,1000.0,0.0,0,0);
    dotset(1,12.0,2.0,10.0,0.0,0,0);
    dotset(2,0.0,1000.0,1000.0,0.0,0,0);
    dotset(4,1000.0,0.0,5.0,-5.0,0,0);
    dotset(5,256.0,0.0,5.0,0.-5,0,0);
    dotset(6,100.0,0.0,5.0,-5.0,0,0);
    dotset(7,100.0,0.0,0.1,-0.1,0,0);
    dotset(8,1000.0,0.0,0.1,-0.1,0,0);
    s = 0.0;
    sx = 0.0;
    sy = 0.0;
    sxy = 0.0;
    sx2 = 0.0;
    ndac=0;
    iset=0;
loop:
    nwds = fread(&dat.dac,1,810,fp);
/*    printf("nwds=%d\n",nwds);
      printf(" dat.dac=%d\n",dat.dac);  */
    if(dat.av>1022) goto xit;
    dac = (double)(dat.dac-dacoffset);
    if(dac<0) goto loop;
/*    if(iset==0) dacoffset = dat.dac;
    if(dat.av<0.1 && iset==0) goto loop;
    iset=1;  */
    xadc = dac/26.069;
    avg = dat.av;
    davg = dat.dav;
    dotacc(0,dac,avg);
    dotacc(1,xadc,avg);

    adc[ndac]=avg;
    dadc[ndac]=dat.dav;
    fdac[ndac]=dac;
    adac[ndac]=xadc;
    rdac[ndac]=dat.dac;

    s = s + 1.0;
    fdc = dac;
    sx = sx + dac;
    sy = sy + avg;
    sxy = sxy + dac*avg;
    sx2 = sx2 + dac*dac;
    k = dat.dac/100;
    if(k*100 == dat.dac) printf(" dac=%f  avg=%f  davg=%f\n",dac,avg,davg);
    ndac++;
    goto loop;
xit:
   del = s*sx2 - sx*sx;
   if(del < 1.0) printf("error del=%f\n",del);
   a = (sx2*sy - sx*sxy) / del;
   b = (s*sxy - sx*sy) / del;
   printf(" fit results a=%f b=%f adc=a+b*dac\n",a,b);
   for(k=0;k<ndac;k++)
     { y = a + b*fdac[k];
       x = fdac[k];
       dy = adc[k]-y;
      /* printf(" k=%d  x=%f  y=%f  adc=%f  dy=%f\n",k,x,y,adc[k],dy); */
       xadc=fdac[k]/26.069;
       if(y>0.0)
        { dotacc(2,xadc,y);
          dotacc(4,xadc,dy);
          dotacc(6,xadc,dy);
          dotacc(7,xadc,dy/y);
          dotacc(8,xadc,dy/y);
          dotacc(5,xadc,dy);
	}
 /*   fprintf(fpascii,"%f %f %f %f %f %d\n",fdac[k],adc[k],dadc[k],y,adac[k],rdac[k]); */ 
    fprintf(fpascii,"%f %f %f %f %f\n",fdac[k],adc[k],dadc[k],y,adac[k]); 
     }
    dotwrt(0); 
    dotwrt(1); 
    dotwrt(2); 
    dotwrt(4);  
    dotwrt(5); 
    dotwrt(6); 
    dotwrt(7);  
    dotwrt(8);
  }  /* end of main */
