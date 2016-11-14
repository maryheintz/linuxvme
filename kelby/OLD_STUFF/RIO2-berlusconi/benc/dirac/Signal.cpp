// Signal.cpp: Implementation of ADC Signals

#include <vector.h>
#include <math.h>
#include <stdio.h>
/* #include <graphics.h>  */
#include "Signal.h"

// Constructors: Use default constructor for vector<int>

Signal::Signal() { }

int Signal::operator[](int sample) {
  return samples[sample];
}

int Signal::GetPeaksample() {
  return peaksample;
}

int Signal::GetPeak() {
  return samples[peaksample];
}

pedinfo Signal::Pedestal(int first, int last) {
  pedinfo result;
  int count = last - first + 1;
  double sum = 0;
  double sumsq = 0;
  for (int i = first; i <= last; ++i) {
    sum += samples[i];
    sumsq += samples[i] * samples[i];
  }
  result.mean = sum / count;
  result.variance = sqrt(sumsq / count - result.mean * result.mean);
  return result;
}

void Signal::Clear() {
  samples.erase(samples.begin(), samples.end());
}

// Adds sample to end of signal
void Signal::AddSample(int counts) {
  int cursamp = samples.size(); // Size before adding is the element number
  samples.push_back(counts);    // of the sample being added
  if (cursamp == 0) {
    peaksample = 0;
  } else {
    if (counts > samples[peaksample]) peaksample = cursamp;
  }
}

void Signal::Plot(GraphPort *port, const char *title, int pbegin, int pend) {
  // Compute dimensions
  int ns = samples.size();
  double xmax = ns;
  double ymax = 100 * ceil(GetPeak() / 100.0);
  axisinfo Xaxis = { 0, xmax, 0, 1, 5 };
  axisinfo Yaxis = { 0, ymax, 0, roundunder(ymax / 10), 5 };
  Axes axes(port, 16 / xmax, 14 / ymax, 2, 3, Xaxis, Yaxis);

  // Plot
  axes.Draw();
  axes.MoveTo(0, 0);
  for (int i = 0; i < ns; ++i) {
    axes.LineTo(i, samples[i]);
    axes.LineTo(i + 1, samples[i]);
  }
  axes.LineTo(ns, 0); // not needed if graph is boxed.

  // Draw labels, title, and statistics
  axes.DrawTitle(title, 1.2);
  axes.DrawXLabel("Sample Number", 0.8);
  axes.DrawYLabel("ADC Counts", 0.5);
  pedinfo ped = Pedestal(pbegin, pend);
  char pksamp[20];
  sprintf(pksamp, "Peak Sample: %d", GetPeaksample());
  port->PlotString(pksamp, 19, 17, 0, 1);
  char pk[20];
  sprintf(pk, "Peak: %d", GetPeak());
  port->PlotString(pk, 19, 16, 0, 1);
  port->PlotString("Pedestal:", 19, 15, 0, 1);
  char pedstr[20];
  sprintf(pedstr, "  %.3g +/- %.3g", ped.mean, ped.variance);
  port->PlotString(pedstr, 19, 14.2, 0, 1);
}

//  old kja code
//   int nn = samples.size();
//   int idx, idy, ny, linx, ixbas, iybas, ix, iy, iylst, kx, ky, i, k, ii;
//   int idel;
//   double dely,y,dy;
//   char str[5];

//   erasm();
//   int max = GetPeak();
//   int mmax = 5000;
//   if(max<4000) mmax=4000;
//   if(max<3000) mmax=3000;
//   if(max<2000) mmax=2000;
//   if(max<1000) mmax=1000;
//   if(max<800) mmax=800;
//   if(max<600) mmax=600;
//   if(max<500) mmax=500;
//   if(max<400) mmax=400;
//   if(max<200) mmax=200;
//   if(max<100) mmax=100;
//   if(max<50) mmax=50;
//   idx = 1600/nn;
//   linx = idx*nn;
//   ixbas = 100+(2200-linx)/2;
//   dy = 2400.0/(double)mmax;
//   iybas = 400;
//   mova(ixbas,iybas);
//   drwr(linx,0);
//   mova(ixbas,iybas);
//   iylst = 0;
//   for (i = 0; i < nn; ++i) {
//     ii = samples[i];
//     y = (double)ii;
//     y = y*dy;
//     iy = (int)y;
//     idy = iy-iylst;
//     drwr(0,idy);
//     drwr(idx,0);
//     iylst=iy;
//   }
//   drwr(0,-iylst);

//   /*  label horizontal axis */
//   idel=10;
//   if(nn>100) idel=25;
//   if(nn>200) idel=50;
//   for(i = 0; i < nn + 1; i += idel) {
//     ix=ixbas+idx*i;
//     mova(ix,iybas);
//     drwr(0,-30);
//     k=i/2;
//     if(k*2 == i) {
//       sprintf(str,"%3d",i);
//       kx = ix-45;
//       if(i<100) kx = ix-55;
//       if(i<10) kx=ix-75;
//       symb(kx,iybas-100,0.0,str);
//     }
//   }
//   /*  label vertical axis  */
//   ny=mmax/100;
//   if(ny>10) ny=10;
//   if(mmax == 200) ny=4;
//   if(mmax == 100) ny=4;
//   if(mmax == 50) ny=5;
//   y=(double)mmax*dy;
//   iy = (int)y;
//   mova(ixbas,iybas);
//   drwr(0,iy);
//   for (i = 0; i <= ny; ++i) {
//     ky = i*mmax/ny;
//     y = (double)ky * dy;
//     iy = (int)y + iybas;
//     mova(ixbas,iy);
//     drwr(-20,0);
//     sprintf(str,"%4d",ky);
//     kx = ixbas-150;
//     symb(kx,iy-20,0.0,str);
//   }
//   /*  print label */
//   symb(200,3000,0.0,label);
//   plotmx();
// }
  
