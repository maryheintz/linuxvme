

#include <stdio.h>
#include <math.h>
#include <time.h>
char string[100];
main()
  { double vpulse,volt,db,dbstp;
    int k;
    gpib_reset();
    sendmessage(3, "*R X");
    sendmessage(3, "E? X");
    getmessage(3,string);
    printf("error=%x\n",string[0]);
/*  initialize the iotech controller - programable attenuators */
    sendmessage(3, "D4 X" );  /* lf+eoi terminate */
    sendmessage(3, "P1 X" );  /* select master and execute */
    sendmessage(3, "F3 X" );  /* select hex */
   dbstp = 0.1;
   vpulse=5.0;
   k = 0;
   for (db=0.0; db<64.01; db=db+dbstp)  /* start l loop */
     {db_set(vpulse,db,&volt);
      if(db>=8.0) dbstp=0.2;
      if(db>=16.0) dbstp=0.4;
      if(db>=32.0) dbstp=0.8;
      if(db>=48.0) dbstp=1.6;
      k++;
      printf("k=%d db=%f volt=%f\n",k,db,volt);
      usleep((time_t)10);
     }
}  /* end of main */

db_set(vpulse,db,volt)
double vpulse,db,*volt;
  { double dbset,afac;
    unsigned long lout;
    char string[80];
    dbset=0.0;
    lout=0;
    if(db>64)
      { dbset=dbset+64.0;
        lout=lout+0x4000;
        db=db-64.0;}
    if(db>32)
      { dbset=dbset+32.0;
        lout=lout+0x2000;
        db=db-32.0;}
    if(db>16)
      { dbset=dbset+16.0;
        lout=lout+0x1000;
        db=db-16.0;}
    if(db>8)
      { dbset=dbset+8.0;
        lout=lout+0x800;
        db=db-8.0;}
    if(db>4)
      { dbset=dbset+4.0;
        lout=lout+0x400;
        db=db-4.0;}
    if(db>2)
      { dbset=dbset+2.0;
        lout=lout+0x200;
        db=db-2.0;}
    if(db>1)
      { dbset=dbset+1.0;
        lout=lout+0x100;
        db=db-1.0;}
    if(db>0.8)
      { dbset=dbset+0.8;
        lout=lout+0x8;
        db=db-0.8;}
    if(db>0.4)
      { dbset=dbset+0.4;
        lout=lout+0x4;
        db=db-0.4;}
    if(db>0.2)
      { dbset=dbset+0.2;
        lout=lout+0x2;
        db=db-0.2;}
    if(db>0.1)
      { dbset=dbset+0.1;
        lout=lout+0x1;
        db=db-0.1;}
    afac=-dbset/20.0;
    *volt=vpulse*pow(10.0,afac);
    sprintf(string,"C%x X",lout);
    sendmessage(3, string );
    return(0);
 }

