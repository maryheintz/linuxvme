#include <stdio.h>
#include <math.h>
#include <time.h>
#include <ces/vmelib.h>
struct block
       { unsigned short b0;        /* base+0 */
         unsigned short b2;        /* base+2 */
         unsigned short b4;        /* base+4 */
         unsigned short b6;        /* base+6 */
         unsigned short b8;        /* base+8 */
         unsigned short ba;        /* base+a */
         unsigned short bc;        /* base+c */
         unsigned short be;        /* base+e */
         unsigned short rd0;       /* base+10 */
         unsigned short rd1;       /* base+12 */
         unsigned short rd2;       /* base+14 */
         unsigned short b16;       /* base+16 */
         unsigned short all;       /* base+18 */
         unsigned short b1a;       /* base+1a */
         unsigned short b1c;       /* base+1c */
         unsigned short delay;     /* base+1e */
         unsigned short sample;    /* base+20 */
         unsigned short status;    /* base+22 */
         unsigned short reset;     /* base+24 */
         unsigned short trig;      /* base+26 */
       };
struct block *adc;
static struct pdparam_master param=
      { 1,   /* VME Iack (must be 1 for all normal pages) */
        0,   /* VME Read Prefetch Option */
        0,   /* VME Write Posting Option */
        1,   /* VME Swap Option */
        0,   /* page number return */
        0,   /* reserved */
        0,   /* reserved */
        0 };
int fdata[3][256],ipk[3],pk[3];
unsigned short *reg;
unsigned short *reg0,*reg1,*reg2;
unsigned int basef=0xb30000,base_fermi,base_last;
unsigned char gpib_device=1;
int adder[48]={ 4, 4, 4, 4, 4, 7,  /*  1 -  6 */
                7, 7,10,10,10,10,  /*  7 - 12 */
	       15,15,15,15,15,15,  /* 13 - 18 */
               21,21,21,21,21,21,  /* 19 - 24 */
	       28,28,28,28,28,28,  /* 25 - 30 */
	       34,34,34,34,34,34,  /* 31 - 36 */
               42,42,42,42,42,42,  /* 37 - 42 */
	       45,45,45,45,45,45}; /* 43 - 48 */
int finput[48]={0,0,0,0,0,1,
                1,1,2,2,2,2,
		0,0,0,0,0,0,
                1,1,1,1,1,1,
		2,2,2,2,2,2,
		0,0,0,0,0,0,
                1,1,1,1,1,1, 
		2,2,2,2,2,2};
int boff[48]={     0,     0,     0,     0,     0,     0,
                   0,     0,     0,     0,     0,     0,
              0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,
	      0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,
	      0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,
	      0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,
	      0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,
	      0x2000,0x2000,0x2000,0x2000,0x2000,0x2000};
int image[50][150];
int ttcadr,id;
unsigned short *reg,vreg;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { FILE *fp;
    int board,kk;
    long stat;
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    double meanhp,meanhc,meanlp,meanlc;
    double sum,xlsump,xxlsump,xlsumc,xxlsumc;
    double siglp,siglc,pmaxl,cmaxl,tmaxl;
    int nevnt,nevmax,nstrtev,l,nn,i,k,wait;
    int card,okcd[48],nall,ngood,nbad,ntry;
    unsigned int kl,lk,start,trouble;
    unsigned short shrt;
    int dac,time1,time2,time3,time4;
    double fdac,charge,val,ratio;
    int bigain,kpeak,klo,khi,icnt,dstep;
    double s,sx,sy,sxy,sev,sxev,sxxev,el,er,ep;
    double sig,sx2,volt,capacitor,a,b,del,x,y,dely;
    double pedhi,dpedhi,pedlo,dpedlo;
    char xstring[80],ystring[80],string[80],err[80];
    unsigned short cadr,tc,tf;
    int kpeakl,timfl,timf;
    int tm1,tm2,tm3,tm4;
    unsigned short sval,dval;
/* ========= check ARBLOGIC is there for trigger logic ========= */
    reg = (unsigned short *)0xdede0000;
    vreg = *reg;
    printf(" serial=%x\n",vreg);

    reg = (unsigned short *)0xdede0008;
    *reg = 0x1234;
    reg = (unsigned short *)0xdede000a;
    vreg = *reg;
    
    printf(" 0x1234 sent %x read back\n",vreg);
    if(vreg != 0x1234) exit(0);

    board = 1;
    ttcadr = 0;
    id = (board<<6) | 1;
   ttcvi_map(ttcadr);
   reset_ctrl_can();
   printf("calling can_init\n");
   can_reset(ttcadr);  
loop:
    printf("  OFF -----------------------------------------\n");
   for (kl=1;kl<49;kl++)
     {set_tube(ttcadr,kl);
      trig_disable(ttcadr);
     }
   sleep(10);
    printf("  ON ++++++++++++++++++++++++++++++++++++++++++\n");
   for (kl=1;kl<49;kl++)
     {set_tube(ttcadr,kl);
      trig_enable(ttcadr);
     }
   sleep(10);
   goto loop;
}  /* end of main */
