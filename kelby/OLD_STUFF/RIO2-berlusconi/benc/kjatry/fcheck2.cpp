#include <iostream.h>
#include <vector.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <ces/vmelib.h>
/*  #include <kjaplot.h>  */
#include "A3in1Controller.h"
#include "SwitchBox.h"
#include "TTCvi.h"
#include "DigBoard.h"
#include "fcheck.h"

int fdata[3][40];
int ipk[3], pk[3];
double crg[6500],fvl[6500];
time_t usec = 1;
int ktime;

int main() {
  // Set up cards
  A3in1Controller control(1, 1, 0x560000);
  SwitchBox pulser(0xcf0000);
  TTCvi ttc(0xb00000);
  DigBoard *pdig;
  try {
    pdig = new DigBoard(&ttc, 7);
  }
  catch (SLINK_error) {
    cout << "Error initializing S-link." << endl;
    throw;
  }
  DigBoard& dig = *pdig;
  cout << "DigBoard constructed OK" << endl;
  ttc.SetTriggerSelect(1);
  int i; // General loop counter
  bool first; // To mimic a do statement
  bool dacdone, evtdone;

  // Other variables: main() uses way too much data.
  FILE *fp;
  double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
  double meanhp,meanhc,meanlp,meanlc;
  double sum,xhsump,xxhsump,xhsumc,xxhsumc,xlsump,xxlsump,xlsumc,xxlsumc;
  double sighp,sighc,pmaxh,cmaxh,tmaxh;
  double siglp,siglc,pmaxl,cmaxl,tmaxl;
  int nevnt,nevmax,nstrtev,l,nn,k,wait;
  int ipeak,timc,timf,kpk;
  int dac; // ,time1,time2,time3,time4;
  double fdac,charge,val,ratio;
  int bigain,kpeak,klo,khi,icnt,dstep;
  double s,sx,sy,sxy,sev,sxev,sxxev,el,er,ep;
  double sig,sx2,volt,capacitor,a,b,del,x,y,dely;
  double pedhi,dpedhi,pedlo,dpedlo;
  char xstring[80],ystring[80],string[80],err[80];
  unsigned short cadr,tc,tf;
  int kpeakh,timfh,kpeakl,timfl;
  double ah,bh,al,bl;
  
  // Setup
  dig.SetMode(DIG_CALIB);
  dig.SetSamples(15);
  int board, channel;

  // Finding working cards
  vector<int> found;
  for (i = 1; i <= 48; ++i) {
    if (control.TestCard(i)) {
      found.push_back(i);
    }
  }
  int size = found.size();
  // If no cards are found, print a message and exit.
  if (size == 0) {
    cout << "No cards found." << endl;
    return 0;
  }
  // If just one is found, use it.
  int card;
  if (size == 1) {
    card = found[0];
    cout << "Card " << found[0] << " found." << endl;
  } else { // if more than one card is found, give user a choice
    cout << "Cards found:" << endl;
    for (i = 0; i < size; ++i) {
      cout << "  " << found[i] << endl;
    }
    cout << "Choose a card: ";
    cin >> card;
  }
  control.SelectCard(card);
  cout << "OK1" << endl;

  sprintf(string, "fck%d.ps", card);
  sidev(1);
  pfset(string);
  
  fp=fopen("fck.dat","w");   
  
  control.SetDrawerTimer(1, 0);
  control.SetDrawerTimer(2, 0);
  control.SetDrawerTimer(3, 0);
  control.SetDrawerTimer(4, 0);
  control.SelectCard(card);
  control.Send12(2, 0, 0);   // s1=s2=s3=s4=0
  control.Send4(1, 0);   // itr=0
  control.Send4(0, 0);   // ire=0
  control.Send4(3, 0);   // mse=0

  nevmax = 25;
  pmaxh = 0.0;
  cmaxh = 0.0;
  tmaxh = 0.0;
  pmaxl = 0.0;
  cmaxl = 0.0;
  tmaxl = 0.0;

  control.SetTimer1(15);
  control.SetTimer2(0);
  
  // Lots of graphics calls.
  dotset(0,250.0,0.0,1000.0,500.0,0,0);
  sprintf(xstring,"fine time setting");
  sprintf(ystring,"high gain FERMI peak channel amplitude");
  dotlbl(0,xstring,ystring);
  
  dotset(1,250.0,0.0,30.0,20.0,0,0);
  sprintf(xstring,"fine time setting");
  sprintf(ystring,"high gain peak channel number");
  dotlbl(1,xstring,ystring);
  
  dotset(2,250.0,0.0,1000.0,500.0,0,0);
  sprintf(xstring,"fine time setting");
  sprintf(ystring,"low gain FERMI peak channel amplitude");
  dotlbl(2,xstring,ystring);
  
  dotset(3,250.0,0.0,30.0,20.0,0,0);
  sprintf(xstring,"fine time setting");
  sprintf(ystring,"low gain peak channel number");
  dotlbl(3,xstring,ystring);
  
  dotset(4,20.0,0.0,1000.0,0.0,0,0);
  sprintf(xstring,"charge");
  sprintf(ystring,"high gain fermi peak");
  dotlbl(4,xstring,ystring);
  
  dotset(5,800.0,0.0,1000.0,0.0,0,0);
  sprintf(xstring,"charge");
  sprintf(ystring,"low gain fermi peak");
  dotlbl(5,xstring,ystring);
  
  dotset(6,20.0,0.0,5.0,-5.0,0,0);
  sprintf(xstring,"charge");
  sprintf(ystring,"high gain fit y-yfit");
  dotlbl(6,xstring,ystring);  

  dotset(7,800.0,0.0,5.0,-5.0,0,0);
  sprintf(xstring,"charge");
  sprintf(ystring,"low gain fit y-yfit");
  dotlbl(7,xstring,ystring);
  
  dotset(8,20.0,0.0,5.0,-5.0,0,0);
  sprintf(xstring,"charge");
  sprintf(ystring,"high gain sigma");
  dotlbl(8,xstring,ystring);
  
  dotset(9,800.0,0.0,5.0,-5.0,0,0);
  sprintf(xstring,"charge");
  sprintf(ystring,"low gain sigma");
  dotlbl(9,xstring,ystring);
  
  hstset(0,50.0,0.0,50,1);
  hstlbl(0,"logain pedestal");
  
  hstset(1,50.0,0.0,50,1);
  hstlbl(1,"higain pedestal");
  
  hstset(2,2.0,0.0,50,1);
  hstlbl(2,"logain pedestal");
  
  hstset(3,5.0,0.0,50,1);
  hstlbl(3,"higain pedestal");
  
  /* --------------------- determine higain timing -------------------*/
  control.SetCapacitor(small);
  control.Inject(11); // Inject 11 pC
  dig.ReadEvent();
  cout << "OK2" << endl;
  dig.Dump(cout);
  cout << "OK3" << endl;

  i=0;
  for (timf = 0; timf < 256; timf++)  /* start l loop */ {
    tf = (unsigned short)timf;
    control.SetTimer2(tf);
    first = true;
    // loop:
    while (pulser.GetSwitch(8) || first) {
      first = false;
      sum=0.0;
      xhsump=0.0;
      xxhsump=0.0;
      xhsumc=0.0;
      xxhsumc=0.0;
      for (nevnt = 0; nevnt < nevmax; nevnt++) {
	control.Inject(11);
	cout << "OK2" << endl;
	// Read the digitizers
	dig.ReadEvent();
	dig.Dump(cout);
	ipk[1] = ipk[2] = 0;
	for (i = 0; i < 15; ++i) {
	  fdata[1][i] = dig.GetSample(board, channel, lowgain, i);
	  fdata[2][i] = dig.GetSample(board, channel, highgain, i);
	  cout << "OK3" << endl;
	  if (fdata[1][i] > fdata[1][ipk[1]]) ipk[1] = i;
	  if (fdata[2][i] > fdata[2][ipk[2]]) ipk[2] = i;
	}
	pk[1] = fdata[1][ipk[1]];
	pk[2] = fdata[2][ipk[2]];
	/*     display ? */
	if (pulser.GetSwitch(2)) { 
	  asum = 0.0;
	  axsum = 0.0;
	  axxsum = 0.0;
	  for (i = 0; i < 5; i++) {
	    aval = (double)fdata[2][i];
	    asum = asum+1.0;
	    axsum = axsum + aval;
	    axxsum = axxsum + aval*aval;
	  }
	  ped = axsum / asum;
	  dped = 0.0;
	  sig2=axxsum/asum - ped*ped;
	  if(sig2>0.0) dped=sqrt(sig2);
 	  sprintf(string,
		  "hi gain ipk=%d  pk=%d  ped=%f +/- %f",
		  ipk[2],pk[2],ped,dped);
	  evdisp(15, string, 2);
	}
	/* now gets sums for mean calculation */
	sum=sum+1.0;
	kpk=ipk[2]; 
	yc=(double)fdata[2][kpk];
	xhsumc=xhsumc+yc;
	xxhsumc=xxhsumc+yc*yc;
	yc=(double)ipk[2];
	xhsump=xhsump+yc;
	xxhsump=xxhsump+yc*yc;
      } // end of nevnt loop
    } // end of while (pulser.GetSwitch(8)) loop
    /*  now get mean  */
    meanhp=xhsump/sum;
    sighp=0.0;
    sig2=xxhsump/sum-meanhp*meanhp;
    if(sig2>0.0) sighp=sqrt(sig2);
    meanhc=xhsumc/sum;
    sighc=0.0;
    sig2=xxhsumc/sum-meanhc*meanhc;
    if(sig2>0.0) sighc=sqrt(sig2);
    if (pulser.GetSwitch(3)) { 
      printf("\n timer=%d sum=%f\n",timf,sum); 
      printf("       meanhp=%f sighp=%f\n",meanhp,sighp);
      printf("       meanhc=%f sighc=%f\n",meanhc,sighc);
    }

    printf("high gain timer=%d channel=%6.1f peak=%5.1f\n",
	   timf,meanhp,meanhc);
    if (meanhc > pmaxh && timf < 125) {
      pmaxh=meanhc;
      cmaxh=meanhp;
      tmaxh=timf;
    }
    dotacc(0,(double)timf,meanhc);
    dotacc(1,(double)timf,meanhp);
  }  /* end of dac loop */
  /* --------------------- determine logain timing -------------------*/
  control.SetCapacitor(large);
  control.Inject(700);
  i=0;
  for (timf = 0; timf < 256; timf++)  /* start l loop */ {
    tf = (unsigned short)timf;
    control.SetTimer2(tf);
    sum=0.0;
    xlsump=0.0;
    xxlsump=0.0;
    xlsumc=0.0;
    xxlsumc=0.0;
    for(nevnt=0; nevnt < nevmax; nevnt++) {
      control.Inject(700);
      // Read the digitizers
      ipk[1] = ipk[2] = 0;
      for (i = 0; i < 15; ++i) {
	fdata[1][i] = dig.GetSample(board, channel, lowgain, i); // Low gain
	fdata[2][i] = dig.GetSample(board, channel, highgain, i); // High gain
	if (fdata[1][i] > fdata[1][ipk[1]]) ipk[1] = i;
	if (fdata[2][i] > fdata[2][ipk[2]]) ipk[2] = i;
      }
      pk[1] = fdata[1][ipk[1]];
      pk[2] = fdata[2][ipk[2]];
      /*     display ? */
      if(pulser.GetSwitch(1)) {
	asum = 0.0;
	axsum = 0.0;
	axxsum = 0.0;
	for(i = 0; i < 5; i++) {
	  aval = (double)fdata[1][i];
	  asum = asum+1.0;
	  axsum = axsum + aval;
	  axxsum = axxsum + aval*aval;
	}
	ped = axsum / asum;
	dped = 0.0;
	sig2=axxsum/asum - ped*ped;
	if(sig2>0.0) dped=sqrt(sig2);
	sprintf(string,
		"lo gain ipk=%d  pk=%d  ped=%f +/- %f",
		ipk[1],pk[1],ped,dped);
	evdisp(15, string, 1);
      }
      /* now gets sums for mean calculation */
      sum=sum+1.0;
      kpk=ipk[1];
      yc=(double)fdata[1][kpk];
      xlsumc=xlsumc+yc;
      xxlsumc=xxlsumc+yc*yc;
      yc=(double)ipk[1];
      xlsump=xlsump+yc;
      xxlsump=xxlsump+yc*yc;
    } /* end of nevnt loop */
    /*  now get mean  */
    meanlp=xlsump/sum;
    siglp=0.0;
    sig2=xxlsump/sum-meanlp*meanlp;
    if(sig2>0.0) siglp=sqrt(sig2);
    meanlc=xlsumc/sum;
    siglc=0.0;
    sig2=xxlsumc/sum-meanlc*meanlc;
    if(sig2>0.0) siglc=sqrt(sig2);
    if(pulser.GetSwitch(3)) {
      printf("\n timer=%d sum=%f\n",timf,sum); 
      printf("       meanlp=%f siglp=%f\n",meanlp,siglp);
      printf("       meanlc=%f siglc=%f\n",meanlc,siglc);
    }
    
    printf(" low gain timer=%d channel=%6.1f peak=%5.1f\n",
	   timf,meanlp,meanlc);
    if(meanlc>pmaxl && timf<125) {
      pmaxl=meanlc;
      cmaxl=meanlp;
      tmaxl=timf;
    }
    dotacc(2,(double)timf,meanlc);
    dotacc(3,(double)timf,meanlp);
  }  /* end of dac loop */
  /*   show the plots */
  printf("pmaxh=%f cmaxh=%f tmaxh=%f\n",pmaxh,cmaxh,tmaxh);
  printf("pmaxl=%f cmaxl=%f tmaxl=%f\n",pmaxl,cmaxl,tmaxl);
  
  /* ---------------- high gain calibration ----------------------*/
  control.SetCapacitor(small);
  kpeak=(int)(cmaxh+0.5);
  klo=kpeak-2;
  khi=kpeak+2;
  timf=(int)(tmaxh+0.5);
  printf("starting higain  kpeak=%d timf=%d\n",kpeak,timf);
  kpeakh = kpeak;
  timfh = timf;
  tf = (unsigned short)timf;
  control.SetTimer2(tf);
  icnt=0;
  s=0.0;
  sx=0.0;
  sy=0.0;
  sxy=0.0;
  sx2=0.0;
  dstep = 4;
  dacdone = false;
  for (dac = 0; dac < 400 && !dacdone; dac=dac+dstep) {
    volt = 2.0 * 4.096 * (double)dac/1023.0;
    charge = volt * 5.2;
    sev = 0.0;
    sxev = 0.0;
    sxxev = 0.0;
    evtdone = false;
    for (nevnt = 0; nevnt < nevmax && !evtdone; nevnt++) {
      first = true;
      while (pulser.GetSwitch(8) || first) {
	first = false;
	control.Inject(charge);
	// Read the digitizers
	ipk[1] = ipk[2] = 0;
	for (i = 0; i < 15; ++i) {
	  fdata[1][i] = dig.GetSample(board, channel, lowgain, i);
	  fdata[2][i] = dig.GetSample(board, channel, highgain, i);
	  if (fdata[1][i] > fdata[1][ipk[1]]) ipk[1] = i;
	  if (fdata[2][i] > fdata[2][ipk[2]]) ipk[2] = i;
	}
	pk[1] = fdata[1][ipk[1]];
	pk[2] = fdata[2][ipk[2]];
	/*     display ? */
	if (pulser.GetSwitch(2)) {
	  asum = 0.0;
	  axsum = 0.0;
	  axxsum = 0.0;
	  for(i = 10; i < 20; i++) {
	    aval = (double)fdata[2][i];
	    asum = asum+1.0;
	    axsum = axsum + aval;
	    axxsum = axxsum + aval*aval;
	  }
	  ped = axsum / asum;
	  dped = 0.0;
	  sig2=axxsum/asum - ped*ped;
	  if(sig2>0.0) dped=sqrt(sig2);
	  sprintf(string,
		  "lo gain ipk=%d  pk=%d  ped=%f +/- %f",
		  ipk[2],pk[2],ped,dped);
	  evdisp(40, string, 2);
	}
	asum = 0.0;
	axsum = 0.0;
	axxsum = 0.0;
	for(i = 10; i < 20; i++) {
	  aval = (double)fdata[2][i];
	  asum = asum+1.0;
	  axsum = axsum + aval;
	  axxsum = axxsum + aval*aval;
	}
	if(asum < 1.0) printf(" error asum==%f\n",asum);
	ped = axsum / asum;
	if(ped<10.0) printf("error ped=%f axsum=%f asum=%f\n",ped,axsum,asum);
	hstacc(1,ped,1.0);
	sig2=axxsum/asum - ped*ped;
	if(sig2>0.0) dped=sqrt(sig2);
	hstacc(3,dped,1.0);
	kpk=ipk[2];
	if (fdata[2][kpk] > 1020) {
	  dacdone = true;
	  evtdone = true;
	  break;
	}
	val = (double)fdata[2][kpeak] - ped;
	if (val < 0.0) {
	  evtdone = true;
	  break;
	}
	if(ipk[2]>=klo && ipk[2]<=khi && fdata[2][kpeak]<1020) {
	  sev = sev + 1.0;
	  sxev = sxev + val;
	  sxxev = sxxev + val*val;
	}
      } // end of while loop
    } /* end of nevnt loop */
    val=0.0;
    if(sev>1.0) {
      val = sxev / sev;
      printf(" high gain dac=%d charge=%f fmean=%f\n",
	     dac,charge,val);
      dotacc(4,charge,val);
      sig=0.0;
      sig2=sxxev/sev - val*val;
      if(sig2>0.0) sig=sqrt(sig2);
      if(val>0.0) dotacc(8,charge,sig);
    }
    if(val>0.0 && val<1000.0) {
      crg[icnt] = charge;
      fvl[icnt] = val;
      if(charge>0.0) {
	s = s +1.0;
	sx = sx + charge;
	sy = sy + val;
	sxy = sxy + charge*val;
	sx2 = sx2 + charge*charge;}
      icnt++; 
    }
  }  /* end of dac loop */
  del = s*sx2 - sx*sx;
  if(del < 1.0) printf("error del=%f\n",del);
  a = (sx2*sy - sx*sxy) / del;
  b = (s*sxy - sx*sy) / del;
  printf(" higain fit results a=%f b=%f y=a+b*charge\n",a,b);
  ah = a;
  bh = b;
  for (k = 0; k < icnt; k++) {
    x = crg[k];
    y = a + b*x;
    dely = y - fvl[k];
    if(pulser.GetSwitch(3))
      printf("k=%d x=%f  y=%f  fvl[k]=%f  dely=%f\n",
	     k,x,y,fvl[k],dely);
    dotacc(6,x,dely);
  }
  /* ---------------------- low gain calibration --------------------------*/
  control.SetCapacitor(large);
  kpeak=(int)(cmaxl+0.5);
  klo=kpeak-2;
  khi=kpeak+2;
  timf=(int)(tmaxl+0.5);
  printf("starting logain  kpeak=%d timf=%d\n",kpeak,timf);
  kpeakl = kpeak;
  timfl = timf;
  tf = (unsigned short)timf;
  control.SetTimer2(tf);
  icnt=0;
  s=0.0;
  sx=0.0;
  sy=0.0;
  sxy=0.0;
  sx2=0.0;
  dstep = 12;
  dacdone = false;
  for (dac = 0; dac < 1024 && !dacdone; dac=dac+dstep) {
    volt = 2.0 * 4.096 * (double)dac/1023.0;
    charge = volt * 100;
    sev = 0.0;
    sxev = 0.0;
    sxxev = 0.0;
    evtdone = false;
    for (nevnt = 0; nevnt < nevmax && !evtdone; nevnt++) {
      first = true;
      while (pulser.GetSwitch(8) || first) {
	first = false;
	control.Inject(charge);
	// Read the digitizers
	ipk[1] = ipk[2] = 0;
	for (i = 0; i < 15; ++i) {
	  fdata[1][i] = dig.GetSample(board, channel, lowgain, i);
	  fdata[2][i] = dig.GetSample(board, channel, highgain, i);
	  if (fdata[1][i] > fdata[1][ipk[1]]) ipk[1] = i;
	  if (fdata[2][i] > fdata[2][ipk[2]]) ipk[2] = i;
	}
	pk[1] = fdata[1][ipk[1]];
	pk[2] = fdata[2][ipk[2]];
	/*     display ? */
	if (pulser.GetSwitch(1)) {
	  asum = 0.0;
	  axsum = 0.0;
	  axxsum = 0.0;
	  for (i = 0; i < 5; i++) {
	    aval = (double)fdata[1][i];
	    asum = asum+1.0;
	    axsum = axsum + aval;
	    axxsum = axxsum + aval*aval;
	  }
	  ped = axsum / asum;
	  dped = 0.0;
	  sig2=axxsum/asum - ped*ped;
	  if(sig2>0.0) dped=sqrt(sig2);
	  sprintf(string,
		  "lo gain ipk=%d  pk=%d  ped=%f +/- %f",
		  ipk[1],pk[1],ped,dped);
	  evdisp(15, string, 1);
	}
	asum = 0.0;
	axsum = 0.0;
	axxsum = 0.0;
	for (i = 0; i < 5; i++) {
	  aval = (double)fdata[1][i];
	  asum = asum+1.0;
	  axsum = axsum + aval;
	  axxsum = axxsum + aval*aval;
	}
	ped = axsum / asum;
	hstacc(0,ped,1.0);
	sig2=axxsum/asum - ped*ped;
	if(sig2>0.0) dped=sqrt(sig2);
	hstacc(2,dped,1.0);
	kpk=ipk[1];
	if (fdata[1][kpk] > 1020) {
	  dacdone = true;
	  evtdone = true;
	  break;
	}
	val = (double)fdata[1][kpeak] - ped;
	if (val < 0.0) {
	  evtdone = true;
	  break;
	}
	if(ipk[1]>=klo && ipk[1]<=khi && fdata[1][kpeak]<1020) {
	  sev = sev + 1.0;
	  sxev = sxev + val;
	  sxxev = sxxev + val * val;
	}
      } // end of while loop
    } /* end of nevnt loop */
    val=0.0;
    if(sev>0.0) {
      val = sxev / sev;
      printf("  low gain dac=%d charge=%f fmean=%f\n",
	     dac,charge,val);
      dotacc(5,charge,val);
      sig=0.0;
      sig2=sxxev/sev - val*val;
      if(sig2 > 0.0) sig=sqrt(sig2);
      if(val>0.0) dotacc(9,charge,sig);
    }
    if(val > 0.0 && val < 1000.0) {
      crg[icnt] = charge;
      fvl[icnt] = val;
      if(charge>50.0) {
	s = s +1.0;
	sx = sx + charge;
	sy = sy + val;
	sxy = sxy + charge*val;
	sx2 = sx2 + charge*charge;
      }
      icnt++;
    }
  }  /* end of dac loop */
  del = s*sx2 - sx*sx;
  a = (sx2*sy - sx*sxy) / del;
  b = (s*sxy - sx*sy) / del;
  al = a;
  bl = b;
  printf(" logain fit results a=%f b=%f y=a+b*charge\n",a,b);
  for(k=0;k<icnt;k++) {
    x = crg[k];
    y = a + b*x;
    dely = y - fvl[k];
    if (pulser.GetSwitch(3)) {
      printf("k=%d x=%f  y=%f  fvl[k]=%f  dely=%f\n",k,x,y,fvl[k],dely);
    }
    dotacc(7,x,dely);
  }
  fprintf(fp,"%4d %5d %4d %9.2f %6.2f %4.1f %5.2f %5d %4d %8.2f %4.2f %4.1f %5.2f\n",
          card,kpeakh,timfh,ah,bh,pedhi,dpedhi,
	  kpeakl,timfl,al,bl,pedlo,dpedlo);
  ratio = bh/bl;
  printf("card=%d bh=%f  bl=%f  ratio=%f\n",card,bh,bl,ratio);

  sidev(1);
  dotmwr(2,0,1);
  dotmwr(2,2,3);
  dotwrt(4);
  dotwrt(5);
  dotwrt(6);
  dotwrt(7);
  dotwrt(8);
  dotwrt(9);
  hstwrt(0);
  hstwrt(1);
  hstwrt(2);
  hstwrt(3);

}  /* end of main */

void evdisp(int nn, char *string, int kkk) {
  int max,mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,k,ii;
  int idel;
  double dely,y,dy;
  char str[5];
  erasm();
  max = pk[kkk];
  mmax=5000;
  if(max<4000) mmax=4000;
  if(max<3000) mmax=3000;
  if(max<2000) mmax=2000;
  if(max<1000) mmax=1000;
  if(max<800) mmax=800;
  if(max<600) mmax=600;
  if(max<500) mmax=500;
  if(max<400) mmax=400;
  if(max<200) mmax=200;
  if(max<100) mmax=100;
  if(max<50) mmax=50;
  idx = 1600/nn;
  linx = idx*nn;
  ixbas = 100+(2200-linx)/2;
  dy = 2400.0/(double)mmax;
  iybas = 400;
  mova(ixbas,iybas);
  drwr(linx,0);
  mova(ixbas,iybas);
  iylst = 0;
  for(i = 0; i < nn; i++) {
    ii = fdata[kkk][i];
    y = (double)ii;
    y = y*dy;
    iy = (int)y;
    idy = iy-iylst;
    drwr(0,idy);
    drwr(idx,0);
    iylst=iy;
  }
  drwr(0,-iylst);
  
  /*  label horizontal axis */
  idel=10;
  if(nn>100) idel=25;
  if(nn>200) idel=50;
  for(i = 0; i < nn+1; i=i+idel) {
    ix=ixbas+idx*i;
    mova(ix,iybas);
    drwr(0,-30);
    k=i/2;
    if(k*2 == i) {
      sprintf(str,"%3d",i);
      kx = ix-45;
      if(i<100) kx = ix-55;
      if(i<10) kx=ix-75;
      symb(kx,iybas-100,0.0,str);
    }
  }
  /*  label vertical axis  */
  ny=mmax/100;
  if(ny>10) ny=10;
  if(mmax == 200) ny=4;
  if(mmax == 100) ny=4;
  if(mmax == 50) ny=5;
  y=(double)mmax*dy;
  iy = (int)y;
  mova(ixbas,iybas);
  drwr(0,iy);
  for(i=0;i<=ny;i++) {
    ky = i*mmax/ny;
    y = (double)ky * dy;
    iy = (int)y + iybas;
    mova(ixbas,iy);
    drwr(-20,0);
    sprintf(str,"%4d",ky);
    kx = ixbas-150;
    symb(kx,iy-20,0.0,str);
  }
  /*  print label */
  symb(200,3000,0.0,string);
  plotmx();
}
