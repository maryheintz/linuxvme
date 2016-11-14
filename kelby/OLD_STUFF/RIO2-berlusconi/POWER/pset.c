#include <stdio.h>
#include <math.h>
#include <time.h>
char string[800];
int mask;
double window_h[15] = {5.4,-4.8,15.2,  5.4,-4.8,15.2,  5.4,-4.8,15.2,
                       5.4,4.8,15.2,  5.4,-4.8,15.2};
double window_l[15] = {4.8,-5.2,14.8,  4.8,-5.2,14.8,  4.8,-5.2,14.8,  
                       4.8,-5.2,14.8,  4.8,-5.2,14.8};  
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{ 
   double volts[16];
   int i,gpib_adr,dval,ipass,ii;
   ipass = 0;
   gpib_reset();
   printf("gpib_reset return\n");
   sleep(0);
   
   printf("enter drawer number 1-5 or 6 for all\n");
   fscanf(stdin,"%d",&dval);
   printf("dval=%d\n",dval);
   mask = 0;
   if(dval == 1) mask = 0x8;
   if(dval == 2) mask = 0x10;
   if(dval == 3) mask = 0x20;
   if(dval == 4) mask = 0x40;
   if(dval == 5) mask = 0x80;  
   if(dval == 6) mask = 0xf8;
   sprintf(string,"D%d X",mask);

   gpib_adr=2;
   sendmessage(gpib_adr,string);
/*   sleep(10);
   sendmessage(gpib_adr,"D? X");
   getmessage(gpib_adr, &string[0] );
   printf("D back=%s\n",string);  */

loop:
   gpib_adr=2;
   for(i=0;i<8;i++)
     { go_get(gpib_adr,i+1,&volts[i]);
/*       printf("i=%d  gpib_adr=%d  volts=%f\n",i,gpib_adr,volts[i]);  */
       if(i == 2) volts[i] = 3.0*volts[i];
       if(i == 5) volts[i] = 3.0*volts[i];
       if(i == 8) volts[i] = 3.0*volts[i];
       
     }

   gpib_adr=3;
   for(i=0;i<8;i++)
     { go_get(gpib_adr,i+1,&volts[i+8]);
/*       printf("i=%d  gpib_adr=%d  volts=%f\n",i,gpib_adr,volts[i]);  */
       if(i == 0) volts[i+8] = 3.0*volts[i+8];
       if(i == 3) volts[i+8] = 3.0*volts[i+8];
       if(i == 6) volts[i+8] = 3.0*volts[i+8];
     }
   for(i=0;i<15;i++) 
     {if(i != 3 && i != 6 && i != 9 && i != 12)
        printf(" %4.1f",volts[i]);
      else
        printf("   %4.1f",volts[i]);
     }	
    printf("\n");
    fflush(stdout);
    sleep(10);
    goto loop;
}

    go_get(gpib_adr,channel,v)
      int gpib_adr,channel;
      double *v;
      {double volts;
       int i;
          sendmessage(gpib_adr,"A0 X");
          usleep((time_t)100);
	  
	  sprintf(string,"C%d X",channel);  
          sendmessage(gpib_adr,string);
          usleep((time_t)100);
  
          sendmessage(gpib_adr,"R3,3,3,3,3,3,3,3 X");
          usleep((time_t)100);

          sendmessage(gpib_adr,"N1 X");
          usleep((time_t)100);

          sendmessage(gpib_adr,"G0 X");
          usleep((time_t)100);
  
          sendmessage(gpib_adr,"Q0 X");
          usleep((time_t)100);

          sendmessage(gpib_adr,"I15 X");
          usleep((time_t)100);

          sendmessage(gpib_adr,"T6 X");   /* trigger voltage reading */
          usleep((time_t)100);
          getmessage(gpib_adr, &string[0] );

/*          printf("readback %d = %s\n",i,string);  */  
          sscanf(string,"%le",&volts);
/*	  printf("i=%d  volts=%f\n",i,volts);   */
          *v = volts;  
	    
}
