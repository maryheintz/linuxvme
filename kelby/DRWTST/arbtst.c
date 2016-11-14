
#include <stdio.h>
#include <math.h>
#include "vme_base.h"
#include "cberr.h"
int id,ttcadr;
unsigned short *reg,vreg;
main()
{ unsigned long status;
  unsigned short serial,sent,back;
  int i;
  
  status = vme_a24_base();

  reg = (unsigned short *)(vme24base + 0xde0000);
  serial = *reg;
  printf(" arb serial number=%x %x %x\n",serial,vme24base,status);

  reg = (unsigned short *)(vme24base + 0xde0008);
  sent = 0xaaaa;
  *reg = sent;
  usleep(1);
  reg = (unsigned short *)(vme24base + 0xde000a);
  back = *reg;
  printf(" sent=%x   back=%x\n",sent,back);

  reg = (unsigned short *)(vme24base + 0xde0008);
  sent = 0x5555;
  *reg = sent;
  reg = (unsigned short *)(vme24base + 0xde000a);
  back = *reg;
  printf(" sent=%x   back=%x\n",sent,back);
  
  for(i=0;i<10;i++)
    { reg = (unsigned short *)(vme24base + 0xde0008);
      sent = i;
      *reg = sent;
      usleep(1);
      reg = (unsigned short *)(vme24base + 0xde000a);
      back = *reg;
      printf(" sent=%x   back=%x\n",sent,back);
   }
loop:
  arbstart();
  usleep(1000);
  arbreset();
  usleep(1000);
  goto loop;
}

arbreset()
  { reg = (unsigned short *)(vme24base + 0xde0006);
    *reg = 0;
  }

arbstart()
  { reg = (unsigned short *)(vme24base + 0xde0004);
    *reg = 0;
  }

arblatch()
  { reg = (unsigned short *)(vme24base + 0xde0002);
    vreg = *reg;
  }
  






