#include <stdio.h>
#include <math.h>
main()
 { FILE *fp;
   double xr0,yr0,r1,r2,xl0,yl0,x,y,tpi,ang,dang,xscale;
   double eta,theta,dx1,dx2,dy2,eme;
   int i,ix,iy;
   fp = fopen("disp.data","w");
   xr0=2000.0;
   yr0=700.0;
   xl0=2000.0;
   yl0 = yr0 + 1500;
   r1 = 300.0;
   r2=600.0;
   xscale = 0.55;
   tpi = 6.283185;
   erasm();
   for(i=0;i<200;i++)
     { ang=tpi*(double)i/200.0;
       x = xr0 + r1*cos(ang);
       y = yr0 + r1*sin(ang);
       ix = (int)(x*xscale);
       iy = (int)y;
       if(i == 0) mova(ix,iy);
       if(i != 0) drwa(ix,iy);
     }
   for(i=0;i<200;i++)
     { ang=tpi*(double)i/200.0;
       x = xr0 + r2*cos(ang);
       y = yr0 + r2*sin(ang);
       ix = (int)(x*xscale);
       iy = (int)y;
       if(i == 0) mova(ix,iy);
       if(i != 0) drwa(ix,iy);
     }
   dang = tpi/64.0;
   for(i=0;i<64;i++)
     { ang = (double)i * dang;
       x = xr0 + r1*cos(ang);
       y = yr0 + r1*sin(ang);
       ix = (int)(x*xscale);
       iy = (int)y;
       mova(ix,iy);
       x = xr0 + r2*cos(ang);
       y = yr0 + r2*sin(ang);
       ix = (int)(x*xscale);
       iy = (int)y;
       drwa(ix,iy);
     }
/* --------------- up right ------------------ */
   x = xl0 + 5;
   y = yl0 + r1;
   ix = (int)(x*xscale);
   iy = (int)y;
   mova(ix,iy);
   x = x + 1.5*r2;
   ix = (int)(x*xscale);
   drwa(ix,iy);
   y = yl0 + r2;    
   iy = (int)y;
   drwa(ix,iy);
   x = xl0 + 5;
   ix = (int)(x*xscale);
   drwa(ix,iy);
   y = yl0 + r1;
   iy = (int)y;
   drwa(ix,iy);
/* --------------- up left ------------------ */
   x = xl0 - 5;
   y = yl0+r1;
   ix = (int)(x*xscale);
   iy = (int)y;
   mova(ix,iy);
   x = x - 1.5*r2;
   ix = (int)(x*xscale);
   drwa(ix,iy);
   y = yl0 + r2;    
   iy = (int)y;
   drwa(ix,iy);
   x = xl0 - 5;
   ix = (int)(x*xscale);
   drwa(ix,iy);
   y = yl0 + r1;
   iy = (int)y;
   drwa(ix,iy);
/* --------------- down right ------------------ */
   x = xl0 + 5;
   y = yl0 - r1;
   ix = (int)(x*xscale);
   iy = (int)y;
   mova(ix,iy);
   x = x + 1.5*r2;
   ix = (int)(x*xscale);
   drwa(ix,iy);
   y = yl0 - r2;    
   iy = (int)y;
   drwa(ix,iy);
   x = xl0 + 5;
   ix = (int)(x*xscale);
   drwa(ix,iy);
   y = yl0 - r1;
   iy = (int)y;
   drwa(ix,iy);
/* --------------- down left ------------------ */
   x = xl0 - 5;
   y = yl0 - r1;
   ix = (int)(x*xscale);
   iy = (int)y;
   mova(ix,iy);
   x = x - 1.5*r2;
   ix = (int)(x*xscale);
   drwa(ix,iy);
   y = yl0 - r2;    
   iy = (int)y;
   drwa(ix,iy);
   x = xl0 - 5;
   ix = (int)(x*xscale);
   drwa(ix,iy);
   y = yl0 - r1;
   iy = (int)y;
   drwa(ix,iy);
/* ------------------- segments --------------- */
   for(i=1;i<11;i++)
     { eta = 0.17 * (double)i;
       eme = exp(-eta);
       theta = 2.0 * atan(eme);

       /* upper right */
       dx1 = r1 / tan(theta);
       x = xl0 + 5.0 + dx1;
       y = yl0 + r1;
       fprintf(fp,"eta=%f  eme=%f  theta=%f  x=%f  y=%f\n",eta,eme,theta,x,y);
       ix = (int)(x*xscale);
       iy = (int)y;
       mova(ix,iy);
       dx2 = r2 / tan(theta);
       dy2 = r2;
       if(dx2>1.5*r2)
         { dx2 = 1.5*r2;
	   dy2 = dx2*tan(theta);
	 }
       x = xl0 + 5.0 + dx2;
       y = yl0 + dy2;
       ix = (int)(x*xscale);
       iy = (int)y;
       drwa(ix,iy);

       /* lower right */
       x = xl0 + 5.0 + dx1;
       y = yl0 - r1;
       ix = (int)(x*xscale);
       iy = (int)y;
       mova(ix,iy);
       x = xl0 + 5.0 + dx2;
       y = yl0 - dy2;
       ix = (int)(x*xscale);
       iy = (int)y;
       drwa(ix,iy);
       
       /* upper left */
       x = xl0 - 5.0 - dx1;
       y = yl0 + r1;
       ix = (int)(x*xscale);
       iy = (int)y;
       mova(ix,iy);
       x = xl0 - 5.0 - dx2;
       y = yl0 + dy2;
       ix = (int)(x*xscale);
       iy = (int)y;
       drwa(ix,iy);

       /* lower left */
       x = xl0 - 5.0 - dx1;
       y = yl0 - r1;
       ix = (int)(x*xscale);
       iy = (int)y;
       mova(ix,iy);
       x = xl0 - 5.0 - dx2;
       y = yl0 - dy2;
       ix = (int)(x*xscale);
       iy = (int)y;
       drwa(ix,iy);
       
     }

  
   plotmx();
 }

