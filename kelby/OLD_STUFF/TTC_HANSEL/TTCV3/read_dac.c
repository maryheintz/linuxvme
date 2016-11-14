#include <stdio.h>
#include <time.h>
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{  unsigned short dac; 
   int i;
loop:
   i = adc_convert(&dac);
   printf("i=%d  dac= %x %d\n",i,dac,dac);
   if(sw(1) == 0) goto loop;
}

adc_convert(k)
unsigned short *k;
 { unsigned short kk,*reg;
   int stat;
/* ----- digitize ----- */
   (unsigned short *)reg = (unsigned short *)0xdede0018;   /* set convert low */
   *reg=0;
   (unsigned short *)reg = (unsigned short *)0xdede000c;   /* set convert high */
   usleep(100);
   *reg=0;
/* ----- read out data ----- */
   (unsigned short *)reg = (unsigned short *)0xdede000e;
   usleep(100); 
   kk = *reg;
   *k = kk& 0xfff;
   stat=(kk>>15);
/*   printf("kk=%x  %d       stat=%x\n",kk,kk,stat);  */ 
   return(stat);
}
