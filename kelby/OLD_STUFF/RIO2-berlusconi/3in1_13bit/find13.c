#include <stdio.h>
#include <math.h>
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{ 
   int card,zone,sector,l,wait,i;
   unsigned short cadr,sval,dval;
    printf(" before map_3in1\n");
    map_3in1();
    printf(" after map_3in1\n");
    zone = 1;
    sector = 1;
    printf("argc=%d\n",argc);
    if(argc != 3)
     { zone = 0;
       sector = 0;
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
      wrt_shift(13,sval);
      read_shift(&dval);
      if(sval != (dval&0x1fff) ) goto fail;
      sval = (unsigned short)0xaaa;
      wrt_shift(13,sval);
      read_shift(&dval);
      if(sval != (dval&0x1fff) ) goto fail;
      printf(" card=%d  OK\n",i);
      continue;
fail:
      continue;
    }
  printf(" wait\n");
  i=getchar();
  goto start;
 }
