#include <stdio.h>
#include <math.h>
unsigned int base = 0xf0ee0000;
main()
{ int mode,i;
  unsigned short val;
restart:
   printf(" select desired test - loop\n");
   printf("  1 = toggle can_clk\n");
   printf("  2 = toggle can_execute\n");
   printf("  3 = toggle clk\n");
   printf("  4 = write to shift register\n");
   printf("  5 = read shift register\n");
   printf("  6 = shift test\n");
   fscanf(stdin,"%d",&mode);
   if(mode == 1)
    {loop1:
       sleep(1);
       can_clkl();
       sleep(1);
       can_clkh();
       goto loop1;
    }
   if(mode == 2)
    {loop2:
      sleep(1);
      can_exh();
      sleep(1);
      can_exl();
      goto loop2;
    }
   if(mode == 3)
    {loop3:
       can_int_clk();
       sleep(1);
       goto loop3;
    }
   if(mode == 4)
    {  printf("enter hex value to write\n");
       fscanf(stdin,"%x",&i);
       val = (unsigned short) i;
       can_shft_wr(val);
    }
   if(mode == 5)
    {  can_shft_rd(&val);
       printf(" value read=%x\n",(int)val);
    }
   if(mode == 6)
    {  printf("enter hex value to write\n");
       fscanf(stdin,"%x",&i);
       val = (unsigned short) i;
       can_shft_wr(val);
       for(i=0;i<16;i++)
        { can_shft_rd(&val);
          printf(" value read=%x\n",(int)val);
          can_int_clk();
        }
    }
goto restart;
}
can_clkl()
{ unsigned short *adr;
  adr = (unsigned short *)( base);
  *adr = 0;
}
can_clkh()
{ unsigned short *adr;
  adr = (unsigned short *)( base | 0x2);
  *adr = 0;
}
can_exl()
{ unsigned short *adr;
  adr = (unsigned short *)( base | 0x4);
  *adr = 0;
}
can_exh()
{ unsigned short *adr;
  adr = (unsigned short *)( base | 0x6);
  *adr = 0;
}
can_int_clk()
{ unsigned short *adr;
  adr = (unsigned short *)( base | 0x8);
  *adr = 0;
}
can_shft_wr(val)
unsigned short val;
  { unsigned short *adr;
    adr = (unsigned short *)(base | 0xa);
    *adr = val;
  }
can_shft_rd(val)
unsigned short *val;
  { unsigned short *adr;
    adr = (unsigned short *)(base | 0xa);
    *val = *adr;
  }
