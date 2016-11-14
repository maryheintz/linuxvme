#include <stdio.h>
#include <math.h>
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{ 
   int card,zone,sector,l,wait,i;
   unsigned short cadr,sval,dval;
    map_3in1();
    zone = 1;
    sector = 1;
    printf("argc=%d\n",argc);
    if(argc != 3)
     { zone = 1;
       sector = 1;
     }
    if(argc == 3)
     { sscanf(argv[1],"%d",&zone);
       sscanf(argv[2],"%d",&sector);
       printf("zone %d  sector %d\n",zone,sector);
     }
start:
   for(i=0;i<49;i++)
    { card = i;
      l = (zone<<12) | (sector<<6) | card;
      cadr = (unsigned short)l;
      card_sel(cadr);
      sval = (unsigned short)0x555;
      wrt_shift(12,sval);
      read_shift(&dval);
      if(sval != (dval&0xfff) ) goto fail;
      sval = (unsigned short)0xaaa;
      wrt_shift(12,sval);
      read_shift(&dval);
      if(sval != (dval&0xfff) ) goto fail;
      printf(" card=%d  OK\n",i);
      continue;
fail:
      continue;
    }
  printf(" wait\n");
  i=getchar();
  goto start;
 }