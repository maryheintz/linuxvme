/* reads out tektronics digital scope gpib eps file --> gpib.plot */
#include <stdio.h>
#include <math.h>
char string[1000000];
main()
  { 
    FILE *fp;
    unsigned char gpib_device = 1;
    int k;
/*  -------------------- INITIALIZE GPIB PULSER ---------------------- */
    fp=fopen("scope_plot.ps","w");
    fprintf(fp,"            \n");
    gpib_reset();
    k=0;
    getmessage(gpib_device, &string[0] );
    fprintf(fp,"%s",string); 
    k=fclose(fp);
    gpib_close();
    }
