#include <stdio.h>
#include <math.h>
main()
{ 
   char text[80];
   int i,iy;
   erasm();
   mova(1000,1000);
   drwr(500,0);
   drwr(0,500);
   drwr(-500,0);
   drwr(0,-500);
   drwr(500,500);
   for(i=1;i<15;i++)
   { iy=2500-i*100;
     sprintf(text,"this is font %d",i);
     symps(200,iy,0.0,i,text);
   }
   plotmx(0x30000);
   return(0); 
}

