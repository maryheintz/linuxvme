#include <stdio.h>
#include <math.h>
main()
{ 
   int i;
   erasm();
   mova(100,100);
   drwr(500,0);
   drwr(0,500);
   drwr(-500,0);
   drwr(0,-500);
   drwr(500,500);
   mova(800,800);
   for(i=0;i<12;i++)
    { /* mova(800+i*100,800); */
      digm(i,0); }
   plotmx();
   return(0); 
}
