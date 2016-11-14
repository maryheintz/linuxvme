#include <stdio.h>#include <math.h>char string[100000];char xstring[80],ystring[80];unsigned char gpib_scope = 1;double sbin[5000];main() { FILE *fp;   int sw1,nev,l,i;   int last,bin,sign,value,max,mbin,n,nwds,ierr,events;   double wd0,wd400;      gpib_reset();      /*    read scope ------------------------------- */      sendmessage(gpib_scope,"DAT:WID 2");      sendmessage(gpib_scope,"DAT:ENC ASCI");      sendmessage(gpib_scope,"DATA:SOURCE CH1");      fp=fopen("scint.data","w");      wd0=0.0;      wd400=0.0;      events=0;evloop:      dxyzro();      dxyset(0,500.0,0.0,50000.0,-50000.0,0,0);      sendmessage(gpib_scope,"ACQ:STATE RUN");      tsleep(0x80000001);      sendmessage(gpib_scope,"ACQ:STATE STOP");      sendmessage(gpib_scope,"CURVE?");      getmessage(gpib_scope,&string[0]);      i=0;      last=0;      bin=0;      sbin[bin]=0.0;      sign=1;next:        /*  printf("i=%d string[i]=%c  %x\n",i,string[i],string[i]); */          if(string[i] != ',' && string[i] != 0xa)            {value=0;            if(string[i] == '0') value=0;            if(string[i] == '1') value=1;            if(string[i] == '2') value=2;            if(string[i] == '3') value=3;            if(string[i] == '4') value=4;            if(string[i] == '5') value=5;            if(string[i] == '6') value=6;            if(string[i] == '7') value=7;            if(string[i] == '8') value=8;            if(string[i] == '9') value=9;            if(string[i] == '-') sign=-1;            if(value>=0 && value<=9) last = last*10 + value;           }           if(string[i] == ',')              {sbin[bin]=sbin[bin]+(double)last;               if(sign == -1) sbin[bin] = -sbin[bin];               if(sw(2) == 1) printf(" bin=%d  sbin=%f %c%c%c%c%c%c%c%c%c%c\n",                 bin,sbin[bin],string[i-9],string[i-8],string[i-7],                 string[i-6],string[i-5],string[i-4],string[i-3],                 string[i-2],string[i-1],string[i]);               dxyacc(0,(double)bin,(double)sbin[bin],0.0,0);               last=0;               sign=1.0;               bin++;               sbin[bin]=0.0;              }            if(string[i] == 0xa)              {sbin[bin]=sbin[bin]+(double)last;               if(sign == -1) sbin[bin] = -sbin[bin];               if(sw(2) == 1) printf(" bin=%d  sbin=%f %c%c%c%c%c%c%c%c%c%c\n",                 bin,sbin[bin],string[i-9],string[i-8],string[i-7],                 string[i-6],string[i-5],string[i-4],string[i-3],                 string[i-2],string[i-1],string[i]);             /*  printf(" bin=%d  sbin=%f\n",bin,sbin[bin]); */               last=0;               sign=1.0;               bin++;               if(sw(1) == 1) dxywrt(0);               if(wd0 != sbin[0] || wd400 != sbin[400])                { wd0 = sbin[0];                  wd400 = sbin[400];                  nwds = fwrite(&sbin[0],8,500,fp);                  ierr = ferror(fp);                  if(ierr != 0) printf("fwrite error=%d  nwds=%d\n",ierr,nwds);                }               events++;               if(sw(3) == 1) printf("events=%d\n",events);               if(sw(8) == 0 && events<5000 ) goto evloop;               ierr=fclose(fp);               exit(0);              }            i++;           goto next;}      double dxyfit(l,k,x)      int l,k;      double x;      { double dxyf;        dxyf=0.0;        return(dxyf);      }