

#include <stdio.h>
#include <math.h>
#include <time.h>
int sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8;
main()
  { 
    int zone,sector,card;
    int time1,time2,time3,time4;
    map_vme();
    zone = 1;
    sector = 1;
start:
    printf(" input time1 time2 time3 time4\n");
    fscanf(stdin,"%d %d %d %d",&time1,&time2,&time3,&time4);
    printf("time1=%d time2=%d time3=%d time4=%d\n",time1,time2,time3,time4); 
reset:
    set_drawer_timer(zone,sector,1,time1);
    set_drawer_timer(zone,sector,2,time2);
    set_drawer_timer(zone,sector,3,time3);
    set_drawer_timer(zone,sector,4,time4);
loop:
    tp_high();
    tp_low();
    if(sw(2) == 1) goto reset;
    if(sw(1) == 0) goto loop;
    goto start;
  }
sw(i)
  int i;
  {int *adr,k,l,shft;
   adr=(int *)0x80cf0000;
   k=*adr;
   shft=23+i;
   l=(k>>shft)&1; 
   return(l);
  }      
