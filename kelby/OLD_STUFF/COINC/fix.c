
#include <stdio.h>
main()
 { FILE *fp,*fp1,*fp2,*fp3,*fp4;
   int mv,muon,scint,coinc;
   fp = fopen("test.out","r");
   fp1 = fopen("mv.dat","w");
   fp2 = fopen("muon.dat","w");
   fp3 = fopen("scint.dat","w");
   fp4 = fopen("coinc.dat","w");
loop:
   fscanf(fp,"%d %d %d %d",&mv,&muon,&scint,&coinc);
   fprintf(fp1,"%d\n",mv);
   fprintf(fp2,"%d\n",muon);
   fprintf(fp3,"%d\n",scint);
   fprintf(fp4,"%d\n",coinc);
   if(mv<198) goto loop;
  }
