#include <stdio.h>
#include <math.h>
#include <strings.h>
 int array[1000],i,out[1000];
      main()
      { FILE *fp;
        for(i=0;i<1000;i++) array[i]=i;
        for(i=0;i<1000;i++) out[i]=0;
        fp=fopen("disc.file","w");
        fwrite(&array[0],4,1000,fp);
        i=fclose(fp);
        fp=fopen("disc.file","r");
        fread(&out[0],4,1000,fp);
        for(i=0;i<1000;i++) printf("i=%d array=%d out=%d\n",i,array[i],out[i]);
      }
