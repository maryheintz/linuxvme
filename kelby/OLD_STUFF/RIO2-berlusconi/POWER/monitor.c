#include <stdio.h>
#include <math.h>
#include <time.h>
char string[800];
int mask;
double window_h[15] = {5.4,-4.8,15.2,  5.4,-4.8,15.2,  5.4,-4.8,15.2,
                       5.4,-4.8,15.2,  5.4,-4.8,15.2};
double window_l[15] = {4.8,-5.2,14.8,  4.8,-5.2,14.8,  4.8,-5.2,14.8,  
                       4.8,-5.2,14.8,  4.8,-5.2,14.8};  
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{ 
   double volts[16];
   int i,gpib_adr,dval,ipass,bad[15],ibad1,ibad2,ibad3,ibad4,ibad5;
   ipass = 0;
   gpib_reset();
/*   printf("gpib_reset return\n");  */
   sleep(1); 
   
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
   { bad[i] = 0;
   /*printf(" i=%d  low=%f  volts=%f  high=%f\n",
        i,window_l[i],volts[i],window_h[i]);  */
    if(volts[i] < window_l[i] || volts[i] > window_h[i]) bad[i] = 1;
/*    printf("i=%d  volt=%f  bad=%d\n",i,volts[i],bad[i]); */
    }
   ibad1 = bad[0]+bad[1]+bad[2];
   ibad2 = bad[3]+bad[4]+bad[5];
   ibad3 = bad[6]+bad[7]+bad[8];
   ibad4 = bad[9]+bad[10]+bad[11];
   ibad5 = bad[12]+bad[13]+bad[14];
   
   if(ibad1>0 && ibad1 <3) alarm(1,ibad1);
   if(ibad2>0 && ibad2 <3) alarm(2,ibad2);
   if(ibad3>0 && ibad3 <3) alarm(3,ibad3);
   if(ibad4>0 && ibad4 <3) alarm(4,ibad4);
   if(ibad5>0 && ibad5 <3) alarm(5,ibad5);

   ipass++;
   printf("\r %d",ipass);
   for(i=0;i<15;i++) 
     if(i != 3 && i != 6 && i != 9 && i != 12)
        printf(" %4.1f",volts[i]);
     else
        printf("   %4.1f",volts[i]);
	
    fflush(stdout);
    sleep(10);
    goto loop;
}
alarm(int i,int ibad)
  { int k;
    printf("\n\a\a\a\a\a alarm i = %d   ibad = %d\n",i,ibad);  
    if(i == 1) k=0x8;
    if(i == 2) k=0x10;
    if(i == 3) k=0x20;
    if(i == 4) k=0x40;
    if(i == 5) k=0x80;
    
   sendmessage(2,"D? X");
   getmessage(2, &string[0] );
   printf("D back=%s\n",string);
   mask = 0;   
   sprintf(string,"D%d X",mask);
   sendmessage(2,string);  
   exit(0);
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
