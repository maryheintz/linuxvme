#include <stdio.h>#include <math.h>#include <time.h>int gpib_adr = 1;main(argc,argv,envs)int argc;char *argv[];char *envs[];      { char string[100];        printf("calling gpib_reset\n");        gpib_reset();	sendmessage(gpib_adr,"A2 X");          tsleep(0x80000100);          sendmessage(gpib_adr,"A? X");          tsleep(0x80000100);          getmessage(gpib_adr, &string[0] );          printf("A2 back=%s\n",string);         }