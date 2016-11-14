#include <stdio.h>
    int abcd[96] ={0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2,
                   0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2,
                   0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2,
                   0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2,
                   0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2,
                   0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2};

    int ldac[96] ={0,1,1,1,1,0,0,0,2,3,3,3,3,2,2,2,
                   4,5,5,5,5,4,4,4,6,7,7,7,7,6,6,6,
                   8,9,9,9,9,8,8,8,10,11,11,11,11,10,10,10,
                   12,13,13,13,13,12,12,12,14,15,15,15,15,14,14,14,
                   16,17,17,17,17,16,16,16,18,19,19,19,19,18,18,18,
                   20,21,21,21,21,20,20,20,22,23,23,23,23,22,22,22};
main()
 { 
   int k,dac;
/*  set all dacs to zero */
   printf(" enter channel\n");
   fscanf(stdin,"%D",&k);
loop: 
   printf("enter dac value\n");
   fscanf(stdin,"%d",&dac);
   setdac(k,dac);
  if(sw(8) == 0) goto loop;
 } 

 setdac(k,dac)
 int k,dac;
  { unsigned long k1,k2,*adr;
/*  abcd 0=A  1=C  2=B  3=D  */
    k1 = (abcd[k]<<12) | dac;
    adr = (unsigned long *)0x80ff0014;
    *adr = k1;      /* latch dac value and abcd select */
    k2 = (1<<ldac[k]);
    adr = (unsigned long *)0x80ff0018;
    *adr = k2;  /* strobe dac line */
  }

   get_scaler(k,ovfl,count)
   int k,*ovfl,*count;
    { unsigned long *adr,value;
      adr = (unsigned long *)0x80ff0010;
      *adr = k;
      adr = (unsigned long *)0x80ff0004;
      value = *adr;
      *count = value & 0x7ffff;
      *ovfl = (value>>19) & 1;
    }

