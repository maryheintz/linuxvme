#include <stdio.h>

int gohere();
int gothere();

main() {
  int a;
  scanf("%d", &a);

  switch(a) {
  case 1: {gohere(); break;}
  case 2: {gothere(); break;}
  default: {break;}
  }
}

gohere()
{
   printf("You suck!");
}

gothere() 
{
}

