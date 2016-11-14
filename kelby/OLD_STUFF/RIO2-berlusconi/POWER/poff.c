#include <stdio.h>
#include <math.h>
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{ 
   int gpib_adr;

   gpib_reset();
   sleep(1);
   gpib_adr=2;
   sendmessage(gpib_adr,"D0 X");
}
