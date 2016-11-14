#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <mem.h>

#include "vme_base.h"
int fdata[49][32],ipk[49],pk[49]; 

int ttcadr,id;
int digadr[8] = {0x100e,0x100f,0x176e,0x10bb,
        0x1011,0x1014,0x1015,0x1017};
int dm[8] = {0,0,0,0,0,0,0,0};
int ddac1[8]={175,175,175,175,175,175,175,175};
int ddac2[8] ={175,175,175,175,175,175,175,175};
unsigned int deskew1[8] = {0,0,0,0,0,0,0,0};
unsigned int deskew2[8] = {0,0,0,0,0,0,0,0}; 
unsigned int iclksm[8] = {0,0,0,0,0,0,0,0};
unsigned int crca,crcb,crcgbl;
time_t usec = 1;
int code;
static int nsamp;
unsigned long event=0,nloop=0;
int nerror=0,ndcrc=0,nbnch=0,ngcrc=0,nctrl=0,nsdmu=0;
int dbad[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int icap;
unsigned short *reg,vreg;
double sn0,sn1;
unsigned short wd1f,wd2f;
unsigned long nraw,rcntl[300],rflag[300],raw[300];
FILE *fpcfg,*fperr;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { int tim,i,l,k,kkk,lll,pipe,drawer,dig,kk,tube,rate,errx;
    int dac,alarm,used,free,need,wempty,wfull,rempty,rfull,enable,busy,nn,nx,errors;
    double charge,dacval,capacitor;
    char xstring[80],ystring[80],string[80],err[80];
    double asum,axsum,axxsum,ped,dped,sig2,aval,yc;
    char serial[10] = {"S10117\0"};
    unsigned long status;
    status = vme_a24_base();
    status = vme_a16_base(); 
    
    
    fperr = fopen("digfst.log","w");
    ttcadr = 0x3009;
    printf("ttcadr=%x  ADC=%s\n",ttcadr,serial);
    tube=0;
    kk = 0;
    icap=0;
    nsamp = 6;
    id=1;

    usleep(200000);
    ttcvi_map(ttcadr);

    set_mb_time(ttcadr,1,156);
    set_mb_time(ttcadr,2,104);
    set_mb_time(ttcadr,3,92);
    set_mb_time(ttcadr,4,40);

    set_mb_time(ttcadr,1,156);
    set_mb_time(ttcadr,2,104);
    set_mb_time(ttcadr,3,92);
    set_mb_time(ttcadr,4,40);

    multi_low(ttcadr);
    rxw_low(ttcadr);

    for(k=1;k<49;k++)
      { set_tube(ttcadr,k);
        intg_rd_low(ttcadr);
        itr_low(ttcadr);
        set_intg_gain(ttcadr,0);  /* s1=s2=s3=s4=0 */
        trig_enable(ttcadr);
        large_cap_enable(ttcadr);
        small_cap_disable(ttcadr);
        mse_high(ttcadr);         /* mse=1 */
      }
    multi_high(ttcadr);  /* now set_dac calls effect all tubes */
    if(icap == 0)
    { charge = 600.0;
      capacitor = 100.0;
      large_cap_enable(ttcadr);
      small_cap_disable(ttcadr);
    }
    if(icap == 1)
    { charge = 7.0;
      capacitor = 5.2;
      large_cap_disable(ttcadr);
      small_cap_enable(ttcadr);
    }
    dacval = (charge * 1023.0 ) / (2.0 * 4.096* capacitor);
    if(dacval > 1023.0) dacval = 1023.0;
    dac = (int) dacval;
    charge = ( 2.0 * 4.096 * capacitor *(double)dac ) / 1023.0;
    printf(" charge=%f  dacval=%f  dac=%x(hex)  cap=%f\n",
           charge,dacval,dac,capacitor);
        set_dac(ttcadr,dac);
    SetTimeDes2(ttcadr,216);
           
    printf(" Setting up Digitizers\n");
     for(dig=0;dig<8;dig++) digitizer_setup(dig);  
	
	SendLongCommand(0,1,0xf,8); /* send reset to the interface */
	if(nsamp < 7) SendLongCommand(0,1,0xf,4); /* set 16 buffers in interface */


    SetInhibitDelay(0,1);
    SetInhibitDuration(0,2);
    SetBMode(0,0xa); 
    BPutShort(0,1);   /* reset bunch counter on inh0  */

     rate = 7;
/*     printf(" select random trigger rate\n");
     printf("     8 = source\n");
     printf("     7 = 100KHz\n");
     printf("     6 =  50KHz\n");
     printf("     5 =  25KHz\n");
     printf("     4 =  10KHz\n");
     printf("     3 =   5KHz\n");
     printf("     2 =   1KHz\n");
     printf("     1 = 100Hz\n");
     printf("     0 =   1Hz\n");
     fscanf(stdin,"%d",&rate);  */
       if(rate == 8) Setcsr1(0x7028);
       if(rate == 7) Setcsr1(0x702d);
       if(rate == 6) Setcsr1(0x602d);
       if(rate == 5) Setcsr1(0x502d);
       if(rate == 4) Setcsr1(0x402d);
       if(rate == 3) Setcsr1(0x302d);
       if(rate == 2) Setcsr1(0x202d);
       if(rate == 1) Setcsr1(0x102d);
       if(rate == 0) Setcsr1(0x002d);
/* toss 20 events */
       usleep(5000);
	sidev(1);  /* force plot to screen without asking*/
restrt:
	event=0;
	nerror=0;
	ndcrc=0;
	nbnch=0;
	ngcrc=0;
	nctrl=0;
	nsdmu=0;
 loop:
       event++;
       if(event==1000000000) nloop++;
       if((event%5000) == 0) 
             printf("event=%d  nerror=%d  ndcrc=%d  nbnch=%d  nctrl=%d  ngcrc=%d  nsdmu=%d  %x %x\n",
               event,nerror,ndcrc,nbnch,nctrl,ngcrc,nsdmu,wd1f,wd2f);
       if((event%50000) == 0)
           { fprintf(fperr,"event=%d  nerror=%d  ndcrc=%d  nbnch=%d  nctrl=%d  ngcrc=%d  nsdmu=%d  loop=%d\n",
               event,nerror,ndcrc,nbnch,nctrl,ngcrc,nsdmu,nloop);
             fflush(fperr);
	   }
       pio_status(&status);
//       wempty = (status>>31) & 1;
//       wfull = (status>>30) & 1;
//       rempty = (status>>29) & 1;
//       rfull = (status>>28) & 1;
//       enable = (status>>27) & 1;
//       busy = (status>>26) & 1;
       used = status & 0xfff;
       free = 4096 - used;
       need = 2*((nsamp+3)*16 + 4);
       if(used==0) goto loop;
//       printf(" used=%d  need=%d\n",used,need);
       if(need>used)
         { nx++;
	   printf(" %d  fifo error used=%d  free=%d  need=%d  we=%d wf=%d re=%d rf=%d en=%d busy=%d\n",
	     nx,used,free,need,wempty,wfull,rempty,rfull,enable,busy);
	     errors++;
//	   errx = pio_rdwd();
//         if(errx != 0) evdmp();
	   goto loop;
	 }
 
       nx=0;
       waitkja(10);
       errx = pio_read();
       if(errx != 0) 
         { printf(" errx = 1 abort event\n");
	   evdmp();
	 }
          nx--;
          goto loop;
   }  /* end of main */
       
int pio_read()
    {
	unsigned long word,wordlast,crc1,crc2,h1,h2;
	int n,cntl,flag,err;
	int samples,error,sestr,destr,r_parity,v_parity,
	   mode,gain,bunch,cc,kklast;
	int type_of_word,count,kk,ii,stop,hcnt,crcaa,b1;
	int bnchok,gcrcok,dcrcok,ctrlok,bnch[16];
	int debug=0,erset;
	int topbit,parity,data1,data2,data3;

	cc = 0;
	count =0;
	kklast = 0;
	hcnt=0;
	bnchok=0;
	gcrcok=0;
	dcrcok=0;
	ctrlok=0;
	erset=0;
	n=-1;
        err = pio_rdwd();
	if(err == 1) goto errxit;
//        printf(" after pio_read\n");
loop:   n++;
        cntl = rcntl[n];
	flag = rflag[n];
	word = raw[n]; 
	if(cntl==1) goto ctrl;
//	if(end == 1) goto done;
	if(debug==1) goto loop;
	kk = count/(nsamp+3);
	ii = count-(nsamp+3)*kk;
//	printf(" word=%x  cntl=%d  count=%d  kk=%d  ii=%d\n",word,cntl,count,kk,ii);
	count++;
	if(n>299) return(1);
	wordlast = word;
	if(kklast == 15 && ii == 0) 
	  { if(sw(8) == 0) printf(" dig crc %x\n",word);
	    if(word != 0xffffffff) 
	      { dcrcok++;
	        printf(" dig crc ERROR %x\n",word);
		goto loop;
	      }
	  }
	else if(kklast == 15 && ii == 1) 
	   { if(sw(8) == 0) printf(" final crc %x\n",word);
	     h1 = word & 0xffff;
	     h2 = (word>>16) & 0xffff;
	     if(h1 != h2) printf(" final crc ERROR %x\n",word);
	     goto loop;
	   }
	else if(ii == 0) 
	   { /* parity = (word>>30) & 1;
//	     samples = (word>>26) & 0xf;
//	     error = (word>>25) & 1;
//	     sestr = (word>>24) & 1;
//	     destr = (word>>23) & 1;
//	     r_parity = (word>>22) & 1;
//	     v_parity = (word>>18) & 0xf;
//	     mode = (word>>15) & 3;
//	     gain = (word>>12) & 0x7; */
	     bunch = word & 0xfff;
	     if(count == 1) b1 = bunch;
	     if(count!=1 && bunch!=b1) 
	        { printf("bunch number error  block=%d  count=%d  bunch=%d  b1=%d\n",
	             hcnt,count,bunch,b1);
	          bnchok++;
		}
//	     crc(0,word);
	     hcnt++;
	     bnch[hcnt-1] = bunch;
	     if(hcnt>16)
		{ printf("hcnt error count=%d\n",count);
                  fprintf(fperr,"hcnt error count=%d\n",count);
		  return(1);
		}
//	     if(sw(8) == 0) printf(fperr,"%d header word %x  bunch %d  %d\n",
//		hcnt,word,bunch,count);
	   }
	else if(ii>nsamp+1) 
	   { crc1 = (word>>16);
	     crc2 = word & 0xffff;
//	     if(crcaa != crc1 || crcb != crc2) dbad[hcnt-1]++;
//	     if(sw(8) == 0)  
//	      { printf(fperr,"      tiledmu crc %4x %4x",crc1,crc2);
//	        if(crcaa == crc1 && crcb == crc2) printf("  ok\n");
//	        if(crcaa != crc1 || crcb != crc2) printf("  bad\n");
//	      }
//	     crc(1,word);
	   }
	else
	   {
//	     topbit = (word>>31) & 1;
//	     parity = (word>>30) & 1;
//	     data1 = (word>>20) & 0x3ff;
//	     data2 = (word>>10) & 0x3ff;
//	     data3 = word & 0x3ff;
//             crc(1,word);
//	     crcaa = crca & 0x7fff;
//             if(sw(8) == 0) printf(fperr," data word= %4i %4i %4x %4x %4x    %8x  %4x %4x\n",
//	           topbit,parity,data1,data2,data3,word,crcaa,crcb); 
	     kklast = kk;
	   } 
	if(kk<8)
	   { 
//	     topbit = (word>>31) & 1;
//	     parity = (word>>30) & 1;
//	     data1 = (word>>20) & 0x3ff;
//	     data2 = (word>>10) & 0x3ff;
//	     data3 = word & 0x3ff;
//           fdata[3*kk+1][ii] = data3;
//	     fdata[3*kk+2][ii] = data2;
//	     fdata[3*kk+3][ii] = data1; 
	   }
	else
	   { 
//	     topbit = (word>>31) & 1;
//	     parity = (word>>30) & 1;
//	     data1 = (word>>20) & 0x3ff;
//	     data2 = (word>>10) & 0x3ff;
//	     data3 = word & 0x3ff;
//	     fdata[3*kk+1][ii] = data1;
//	     fdata[3*kk+2][ii] = data2;
//	     fdata[3*kk+3][ii] = data3;
 	   }
	 goto loop;

ctrl:
	 cc++;
	 if(sw(8) == 0) printf("control word = %x  cc=%d\n", word,cc);
         if(cc == 1) 
	   { if(word == 0x51115110) goto loop;
	     ctrlok++;
	     goto erxit;
	   }
  	 if(cc == 2)
	  { if(word != 0xfffffff0) 
	      { ctrlok++;
	        goto erxit;
	      }
	    crc1 = wordlast & 0xffff;
	    crc2 = (wordlast>>16) & 0xffff;
	    if(crc1 != crc2) 
              { gcrcok++;
	        printf("crc error wordlast=%x  crc1=%x  crc2=%x\n",
	          wordlast,crc1,crc2);
	        if(sw(5) == 1) fscanf(stdin,"%d",&stop); 
	      }
	 }
erxit:
	 if(bnchok != 0 || gcrcok != 0 || dcrcok != 0 || ctrlok != 0) 
	   { nerror++;
	     erset = 1;
             if(bnchok != 0) printf("bnch= %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
        	bnch[0],bnch[1],bnch[2],bnch[3],bnch[4],bnch[5],bnch[6],bnch[7],
		bnch[8],bnch[9],bnch[10],bnch[11],bnch[12],bnch[13],bnch[14],bnch[15]);
             if(dcrcok != 0) printf(" dbad= %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                 dbad[0],dbad[1],dbad[2],dbad[3],dbad[4],dbad[5],dbad[6],dbad[7],
                 dbad[8],dbad[8],dbad[10],dbad[11],dbad[12],dbad[13],dbad[14],dbad[15]);
	     if(bnchok != 0) nbnch++;
	     else if(gcrcok != 0) ngcrc++;
	     else if(dcrcok != 0) ndcrc++;
	     else if(ctrlok != 0) nctrl++;
	   }    
	if(cc == 2) 
	   { if(erset==1) 
	      { printf(" error detected\n");
	        return(1);
	      }
	     return(0);
	   }
errxit:
	printf("read problem   count=%d\n",count);
        return(1);
       }
	 
digitizer_setup(int i)
      { int mode, adrttc, dac1, dac2, ick;
        adrttc = digadr[i];
	mode = dm[i];
	dac1 = ddac1[i];
	dac2 = ddac2[i];
        ick = iclksm[i];
	
        SendLongCommand(adrttc,0,0x6,0);  /* reset v3.0 ttcrx chips*/ 
	usleep(200);
        SendLongCommand(adrttc,0,0x3,0xa9);  /* setup ttcrx chip bnch cntr */
	
/* set up digitizer registers */
        SendLongCommand(adrttc,1,0xa,ick); /* clock invert bits etc */
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */
        SendLongCommand(adrttc,1,1,nsamp); /* number of samples = 13 */
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0x2,21); /* set pipeline length=21 */
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0xf,mode);  /* set mode=0 NORMAL */	
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */

	SendLongCommand(adrttc,1,0x8,dac1);  /* set dac1 */	
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0x9,dac2);  /* set dac2 */	
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */
        SendLongCommand(adrttc,1,0xb,0xff);  /* high limit LSB=0xff */    
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0xc,3);     /* high limit MSB=3 */
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */
        SendLongCommand(adrttc,1,0xd,0);     /* low limit LSB=0 */
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0xe,0);     /* low limit MSB=0 */
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */

/* set up digitizer registers again */
        SendLongCommand(adrttc,1,0xa,ick); /* clock invert bits etc */
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */
        SendLongCommand(adrttc,1,1,nsamp); /* number of samples = 13 */
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0x2,21); /* set pipeline length=21 */
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0xf,0);  /* set mode=0 NORMAL */	
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */

	SendLongCommand(adrttc,1,0x8,dac1);  /* set dac1 */	
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0x9,dac2);  /* set dac2 */	
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */
        SendLongCommand(adrttc,1,0xb,0xff);  /* high limit LSB=0xff */    
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0xc,3);     /* high limit MSB=3 */
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */
        SendLongCommand(adrttc,1,0xd,0);     /* low limit LSB=0 */
	usleep(1);
        BroadcastShortCommand(0x40);         /* reset digitizer */
	SendLongCommand(adrttc,1,0xe,0);     /* low limit MSB=0 */
	usleep(1);

        BroadcastShortCommand(0x40);         /* reset digitizer */

        SetTimeDes1(adrttc,deskew1[i]);
        SetTimeDes2(adrttc,deskew2[i]);
	return;
	}

     crc(int ent,unsigned long word)
        { static unsigned int D0[16],D1[16],C0[16],C1[16],NewCRC[16];
	  int i;
	  unsigned long inwd0,inwd1;
	  /* ==================================================
	     ent=0 first entry (header word) initialize
	           then do crc calculation
	     ent=1 normal data - just update crc calculation
          ================================================== */
	  

/* unpack the data word back into the serial streams D0 and in1 sent
   by the digitizer  D0(0) and D1(0) were the first bits recieved */
        
/* sort the 32 bit word back into the 2 serial streams */
      if(ent == 0) 
         { for(i=0;i<16;i++)
	    { C0[i] = 0;
	      C1[i] = 0;
	    }
	 }	  
	  D0[0] = (word>>30) & 1;
	  D0[1] = (word>>28) & 1;
	  D0[2] = (word>>26) & 1;
	  D0[3] = (word>>24) & 1;
	  D0[4] = (word>>22) & 1;
	  D0[5] = (word>>20) & 1;
	  D0[6] = (word>>18) & 1;
	  D0[7] = (word>>16) & 1;
	  D0[8] = (word>>14) & 1;
	  D0[9] = (word>>12) & 1;
	  D0[10] = (word>>10) & 1;
	  D0[11] = (word>>8) & 1;
	  D0[12] = (word>>6) & 1;
	  D0[13] = (word>>4) & 1;
	  D0[14] = (word>>2) & 1;
	  D0[15] = (word>>0) & 1;

	  D1[0] = (word>>31) & 1;
	  D1[1] = (word>>29) & 1;
	  D1[2] = (word>>27) & 1;
	  D1[3] = (word>>25) & 1;
	  D1[4] = (word>>23) & 1;
	  D1[5] = (word>>21) & 1;
	  D1[6] = (word>>19) & 1;
	  D1[7] = (word>>17) & 1;
	  D1[8] = (word>>15) & 1;
	  D1[9] = (word>>13) & 1;
	  D1[10] = (word>>11) & 1;
	  D1[11] = (word>>9) & 1;
	  D1[12] = (word>>7) & 1;
	  D1[13] = (word>>5) & 1;
	  D1[14] = (word>>3) & 1;
	  D1[15] = (word>>1) & 1; 
	  
 /*   printf(" C1= %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
       C1[0],C1[1],C1[2],C1[3],C1[4],C1[5],C1[6],C1[7],
       C1[8],C1[9],C1[10],C1[11],C1[12],C1[13],C1[14],C1[15]);

    printf(" D1= %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
       D1[0],D1[1],D1[2],D1[3],D1[4],D1[5],D1[6],D1[7],
       D1[8],D1[9],D1[10],D1[11],D1[12],D1[13],D1[14],D1[15]);  */

/*	  inwd0 = 0;
	  for(i=0;i<16;i++)
	    {inwd0 = inwd0 | (D0[i]<<i);
	    }
	  inwd1 = 0; 
	  for(i=0;i<16;i++)
	    {inwd1 = inwd1 | (D1[i]<<i);
	    }
	  printf(" word=%x  inwd0=%x  inwd1=%x\n",word,inwd0,inwd1); */
	  
    NewCRC[0] = D0[15] ^ D0[13] ^ D0[12] ^ D0[11] ^ D0[10] ^ D0[9] ^ 
                 D0[8] ^ D0[7] ^ D0[6] ^ D0[5] ^ D0[4] ^ D0[3] ^ 
                 D0[2] ^ D0[1] ^ D0[0] ^ C0[0] ^ C0[1] ^ C0[2] ^ 
                 C0[3] ^ C0[4] ^ C0[5] ^ C0[6] ^ C0[7] ^ C0[8] ^ 
                 C0[9] ^ C0[10] ^ C0[11] ^ C0[12] ^ C0[13] ^ C0[15];
    NewCRC[1] = D0[14] ^ D0[13] ^ D0[12] ^ D0[11] ^ D0[10] ^ D0[9] ^ 
                 D0[8] ^ D0[7] ^ D0[6] ^ D0[5] ^ D0[4] ^ D0[3] ^ 
                 D0[2] ^ D0[1] ^ C0[1] ^ C0[2] ^ C0[3] ^ C0[4] ^ 
                 C0[5] ^ C0[6] ^ C0[7] ^ C0[8] ^ C0[9] ^ C0[10] ^ 
                 C0[11] ^ C0[12] ^ C0[13] ^ C0[14];
    NewCRC[2] = D0[14] ^ D0[1] ^ D0[0] ^ C0[0] ^ C0[1] ^ C0[14];
    NewCRC[3] = D0[15] ^ D0[2] ^ D0[1] ^ C0[1] ^ C0[2] ^ C0[15];
    NewCRC[4] = D0[3] ^ D0[2] ^ C0[2] ^ C0[3];
    NewCRC[5] = D0[4] ^ D0[3] ^ C0[3] ^ C0[4];
    NewCRC[6] = D0[5] ^ D0[4] ^ C0[4] ^ C0[5];
    NewCRC[7] = D0[6] ^ D0[5] ^ C0[5] ^ C0[6];
    NewCRC[8] = D0[7] ^ D0[6] ^ C0[6] ^ C0[7];
    NewCRC[9] = D0[8] ^ D0[7] ^ C0[7] ^ C0[8];
    NewCRC[10] = D0[9] ^ D0[8] ^ C0[8] ^ C0[9];
    NewCRC[11] = D0[10] ^ D0[9] ^ C0[9] ^ C0[10];
    NewCRC[12] = D0[11] ^ D0[10] ^ C0[10] ^ C0[11];
    NewCRC[13] = D0[12] ^ D0[11] ^ C0[11] ^ C0[12];
    NewCRC[14] = D0[13] ^ D0[12] ^ C0[12] ^ C0[13];
    NewCRC[15] = D0[15] ^ D0[14] ^ D0[12] ^ D0[11] ^ D0[10] ^ D0[9] ^ 
                  D0[8] ^ D0[7] ^ D0[6] ^ D0[5] ^ D0[4] ^ D0[3] ^ 
                  D0[2] ^ D0[1] ^ D0[0] ^ C0[0] ^ C0[1] ^ C0[2] ^ 
                  C0[3] ^ C0[4] ^ C0[5] ^ C0[6] ^ C0[7] ^ C0[8] ^ 
                  C0[9] ^ C0[10] ^ C0[11] ^ C0[12] ^ C0[14] ^ C0[15];

    crca = 0;
    for(i=0;i<16;i++) 
      { C0[i] = NewCRC[i];
        crca = crca | (NewCRC[i]<<i);
      }
      
    NewCRC[0] = D1[15] ^ D1[13] ^ D1[12] ^ D1[11] ^ D1[10] ^ D1[9] ^ 
                 D1[8] ^ D1[7] ^ D1[6] ^ D1[5] ^ D1[4] ^ D1[3] ^ 
                 D1[2] ^ D1[1] ^ D1[0] ^ C1[0] ^ C1[1] ^ C1[2] ^ 
                 C1[3] ^ C1[4] ^ C1[5] ^ C1[6] ^ C1[7] ^ C1[8] ^ 
                 C1[9] ^ C1[10] ^ C1[11] ^ C1[12] ^ C1[13] ^ C1[15];
    NewCRC[1] = D1[14] ^ D1[13] ^ D1[12] ^ D1[11] ^ D1[10] ^ D1[9] ^ 
                 D1[8] ^ D1[7] ^ D1[6] ^ D1[5] ^ D1[4] ^ D1[3] ^ 
                 D1[2] ^ D1[1] ^ C1[1] ^ C1[2] ^ C1[3] ^ C1[4] ^ 
                 C1[5] ^ C1[6] ^ C1[7] ^ C1[8] ^ C1[9] ^ C1[10] ^ 
                 C1[11] ^ C1[12] ^ C1[13] ^ C1[14];
    NewCRC[2] = D1[14] ^ D1[1] ^ D1[0] ^ C1[0] ^ C1[1] ^ C1[14];
    NewCRC[3] = D1[15] ^ D1[2] ^ D1[1] ^ C1[1] ^ C1[2] ^ C1[15];
    NewCRC[4] = D1[3] ^ D1[2] ^ C1[2] ^ C1[3];
    NewCRC[5] = D1[4] ^ D1[3] ^ C1[3] ^ C1[4];
    NewCRC[6] = D1[5] ^ D1[4] ^ C1[4] ^ C1[5];
    NewCRC[7] = D1[6] ^ D1[5] ^ C1[5] ^ C1[6];
    NewCRC[8] = D1[7] ^ D1[6] ^ C1[6] ^ C1[7];
    NewCRC[9] = D1[8] ^ D1[7] ^ C1[7] ^ C1[8];
    NewCRC[10] = D1[9] ^ D1[8] ^ C1[8] ^ C1[9];
    NewCRC[11] = D1[10] ^ D1[9] ^ C1[9] ^ C1[10];
    NewCRC[12] = D1[11] ^ D1[10] ^ C1[10] ^ C1[11];
    NewCRC[13] = D1[12] ^ D1[11] ^ C1[11] ^ C1[12];
    NewCRC[14] = D1[13] ^ D1[12] ^ C1[12] ^ C1[13];
    NewCRC[15] = D1[15] ^ D1[14] ^ D1[12] ^ D1[11] ^ D1[10] ^ D1[9] ^ 
                  D1[8] ^ D1[7] ^ D1[6] ^ D1[5] ^ D1[4] ^ D1[3] ^ 
                  D1[2] ^ D1[1] ^ D1[0] ^ C1[0] ^ C1[1] ^ C1[2] ^ 
                  C1[3] ^ C1[4] ^ C1[5] ^ C1[6] ^ C1[7] ^ C1[8] ^ 
                  C1[9] ^ C1[10] ^ C1[11] ^ C1[12] ^ C1[14] ^ C1[15];

    crcb = 0;
    for(i=0;i<16;i++) 
      { C1[i] = NewCRC[i];
        crcb = crcb | (NewCRC[i]<<i);
	/* printf("i=%d  NewCRC=%x  crc1=%x\n",i,NewCRC[i],crc1); */
      }
/*    printf("crca=%x  crcb=%x\n",crca,crcb);  */
  }
    unscramble(unsigned long word)
        { int in0[16],in1[16];
	  unsigned long out0,out1;
	  int i;
/*  unscramble the crc word back into the 2 serial streams */
 	  in0[15] = (word>>30) & 1;
	  in0[14] = (word>>28) & 1;
	  in0[13] = (word>>26) & 1;
	  in0[12] = (word>>24) & 1;
	  in0[11] = (word>>22) & 1;
	  in0[10] = (word>>20) & 1;
	  in0[9] = (word>>18) & 1;
	  in0[8] = (word>>16) & 1;
	  in0[7] = (word>>14) & 1;
	  in0[6] = (word>>12) & 1;
	  in0[5] = (word>>10) & 1;
	  in0[4] = (word>>8) & 1;
	  in0[3] = (word>>6) & 1;
	  in0[2] = (word>>4) & 1;
	  in0[1] = (word>>2) & 1;
	  in0[0] = (word>>0) & 1;

	  in1[15] = (word>>31) & 1;
	  in1[14] = (word>>29) & 1;
	  in1[13] = (word>>27) & 1;
	  in1[12] = (word>>25) & 1;
	  in1[11] = (word>>23) & 1;
	  in1[10] = (word>>21) & 1;
	  in1[9] = (word>>19) & 1;
	  in1[8] = (word>>17) & 1;
	  in1[7] = (word>>15) & 1;
	  in1[6] = (word>>13) & 1;
	  in1[5] = (word>>11) & 1;
	  in1[4] = (word>>9) & 1;
	  in1[3] = (word>>7) & 1;
	  in1[2] = (word>>5) & 1;
	  in1[1] = (word>>3) & 1;
	  in1[0] = (word>>1) & 1; 

/* printf("in0 = %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
 in0[0], in0[1], in0[2], in0[3], in0[4], in0[5], in0[6], in0[7],
 in0[8], in0[9],in0[10],in0[11],in0[12],in0[13],in0[14],in0[15]); 
printf("in1 = %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
 in1[0], in1[1], in1[2], in1[3], in1[4], in1[5], in1[6], in1[7],
 in1[8], in1[9],in1[10],in1[11],in1[12],in1[13],in1[14],in1[15]); */
/* now pack the 2 streams back into 16 bit words */

          out0 = 0;
	  out1 = 0;
	  for(i=0;i<16;i++)
	     { out0 = out0 | (in0[i]<<i);
	       out1 = out1 | (in1[i]<<i);
	     }
	  printf("unscrambled TILEdmu crc= %x %x\n",out0,out1);
  }
ksleep(int i)
  { double s,a;
    a = (double)i * 0.123;
    s = sin(a);
    return(s);
  }
pio_reset()
  { unsigned long *adr,pio_base;
    pio_base = vme24base + 0x570000;
    adr = (unsigned long *)(pio_base + 0x14);
    *adr = 0;
  }

pio_disable()
  { unsigned long *adr,pio_base;
    pio_base = vme24base + 0x570000;
    adr = (unsigned long *)(pio_base + 0x20);
    *adr = 0;
  }

pio_enable()
  { unsigned long *adr,pio_base;
    pio_base = vme24base + 0x570000;
    adr = (unsigned long *)(pio_base + 0x1c);
    *adr = 0;
  }

pio_status(unsigned long* status)
  { unsigned long *adr,pio_base;
    pio_base = vme24base + 0x570000;
    adr = (unsigned long *)(pio_base + 0x24);
    *status = *adr;
  }

int pio_rdwd()
  { unsigned long wd1,wd2,wd,rxcntl1,rxdata1,rxflag1,rxcntl2,rxdata2,rxflag2;
    unsigned long *adr1,*adr2,pio_base;
    int n,scrc,debug=0;
    if(debug==1) printf("\n start of event\n");
    pio_base = vme24base + 0x570000;
    adr2 = (unsigned long *)(pio_base + 0x18);
    n=0;
    nraw = 0;
    wd1f = 0xf0f0;
    scrc=0;
loop1:
    waitkja(10);
    wd1 = *adr2; /* read it */
    rxcntl1 = (wd1>>18) & 1;
    rxdata1 = (wd1>>17) & 1;
    rxflag1 = (wd1>>16) & 1;
    wd1 = wd1 & 0xffff;
//    if(rxcntl1==1) printf(" cntl=%d wd1=%x\n",rxcntl1,wd1);
    if(rxcntl1==0 && rxdata1==0 && rxflag1==1) wd1f = wd1;
    n++;
    if(debug==1) printf(" n=%d  wd1=%x  ctrl1=%x data1=%x flag1=%x  crcgbl=%x\n",
       n,wd1,rxcntl1,rxdata1,rxflag1,crcgbl&0xffff);
    if(n>500) 
      { /* printf(" N = 500 exit\n"); */
        return(1); }
    if(nraw != 0) goto next;
    if((rxcntl1 == 1) && (wd1 == 0x5111)) 
      { nraw = 0;
        goto next;
      }
    goto loop1;
next:
    scrc=1;
    waitkja(10);
    wd2 = *adr2; /* read it */
    rxcntl2 = (wd2>>18) & 1;
    rxdata2 = (wd2>>17) & 1;
    rxflag2 = (wd2>>16) & 1;
    wd2 = wd2 & 0xffff;
//    if(rxcntl2==1) printf(" cntl=%d wd2=%x\n",rxcntl2,wd2);
    if(rxflag2 == 1) wd2f = wd2;
    n++;
    if(debug==1) printf(" n=%d  wd2=%x  ctrl2=%x data2=%x flag2=%x  crcgbl=%x\n",
       n,wd2,rxcntl2,rxdata2,rxflag2,crcgbl&0xffff);
a:    wd = wd1<<16 | wd2;
    raw[nraw] = wd;
    rcntl[nraw] = rxcntl2;
    rflag[nraw] = rxflag2;
    nraw++;
    if(rxcntl2==1 && wd2==0xfff0)
     { // printf(" end of event nraw=%d\n\n",nraw); 
       return(0);
     }
     goto loop1;
  }

arbreset()
  { reg = (unsigned short *)(vme24base+0xde0006);
    *reg = 0;
  }

arbstart()
  { reg = (unsigned short *)(vme24base+0xde0004);
    *reg = 0;
  }

arblatch()
  { reg = (unsigned short *)(vme24base+0xde0002);
    vreg = *reg;
  }
  
onthefly()
  { unsigned long wd,*adr1,*adr2,cntl,data,flag,word,pio_base;
    int n;
    pio_base = vme24base + 0x570000;
    adr1 = (unsigned long *)(pio_base + 0x2c);
    adr2 = (unsigned long *)(pio_base + 0x30);
    n=0;
loop:    
    *adr1 = 0;
    wd = *adr2;
    cntl = (wd>>31) & 1;
    data = (wd>>30) & 1;
    flag = (wd>>29) & 1;
    word = wd & 0xffff;
/*    if(n==0 && data==0) goto loop; */
    printf(" cntl=%x data=%x flag=%x  word=%x      wd=%x\n",cntl,data,flag,word,wd);
    n++;
    if(n<10) goto loop;
  }

      waitkja(knt)
       int knt;
       { int i;
         sn0=0.1234;
	 for(i=0;i<knt;i++)
	   { sn1=sin(sn0);
	     sn0=sin(sn1);
	   }  
       }
       
  evdmp()
    { int i,err;
      fprintf(fperr," event dump nraw=%d\n",nraw);
      for(i=0;i<nraw;i++)
       { fprintf(fperr," i=%d  cntl=%d  flag=%d  data=%x\n",
	 i,rcntl[i],rflag[i],raw[i]);
       }
      err = pio_read_dump();
    }

int pio_read_dump()
    {
	unsigned long word,wordlast,crc1,crc2,h1,h2;
	int n,cntl,flag,err,sw8;
	int samples,error,sestr,destr,r_parity,v_parity,
	   mode,gain,bunch,cc,kklast;
	int type_of_word,count,kk,ii,stop,hcnt,crcaa,b1;
	int bnchok,gcrcok,dcrcok,ctrlok,bnch[16];
	int debug=0;
	int topbit,parity,data1,data2,data3;
        sw8=0;
	cc = 0;
	count =0;
	kklast = 0;
	hcnt=0;
	bnchok=0;
	gcrcok=0;
	dcrcok=0;
	ctrlok=0;
	n=-1;
//        err = pio_rdwd();
//	if(err == 1) goto errxit;
//        printf(" after pio_read\n");
loop:   n++;
        cntl = rcntl[n];
	flag = rflag[n];
	word = raw[n]; 
	if(cntl==1) goto ctrl;
//	if(end == 1) goto done;
	if(debug==1) goto loop;
	kk = count/(nsamp+3);
	ii = count-(nsamp+3)*kk;
//	printf(" word=%x  cntl=%d  count=%d  kk=%d  ii=%d\n",word,cntl,count,kk,ii);
	count++;
	if(n>299) return(1);
	wordlast = word;
	if(kklast == 15 && ii == 0) 
	  { if(sw8 == 0) printf(" dig crc %x\n",word);
	    if(word != 0xffffffff) 
	      { dcrcok++;
	        printf(" dig crc ERROR %x\n",word);
		goto loop;
	      }
	  }
	else if(kklast == 15 && ii == 1) 
	   { if(sw8 == 0) printf(" final crc %x\n",word);
	     h1 = word & 0xffff;
	     h2 = (word>>16) & 0xffff;
	     if(h1 != h2) printf(" final crc ERROR %x\n",word);
	     goto loop;
	   }
	else if(ii == 0) 
	   { parity = (word>>30) & 1;
	     samples = (word>>26) & 0xf;
	     error = (word>>25) & 1;
	     sestr = (word>>24) & 1;
	     destr = (word>>23) & 1;
	     r_parity = (word>>22) & 1;
	     v_parity = (word>>18) & 0xf;
	     mode = (word>>15) & 3;
	     gain = (word>>12) & 0x7;
	     bunch = word & 0xfff;
	     if(count == 1) b1 = bunch;
	     if(count!=1 && bunch!=b1) 
	        { printf("bunch number error  block=%d  count=%d  bunch=%d  b1=%d\n",
	             hcnt,count,bunch,b1);
	          bnchok++;
		}
	     crc(0,word);
	     hcnt++;
	     bnch[hcnt-1] = bunch;
	     if(hcnt>16)
		{ printf("hcnt error count=%d\n",count);
                 // exit(0);
		}
	     if(sw8 == 0) printf("%d header word %x  bunch %d  %d\n",
		hcnt,word,bunch,count);
	   }
	else if(ii>nsamp+1) 
	   { crc1 = (word>>16);
	     crc2 = word & 0xffff;
	     if(crcaa != crc1 || crcb != crc2) dbad[hcnt-1]++;
	     if(sw8 == 0)  
	      { printf("      tiledmu crc %4x %4x",crc1,crc2);
	        if(crcaa == crc1 && crcb == crc2) printf("  ok\n");
	        if(crcaa != crc1 || crcb != crc2) printf("  bad\n");
	      }
	     crc(1,word);
	   }
	else
	   { topbit = (word>>31) & 1;
	     parity = (word>>30) & 1;
	     data1 = (word>>20) & 0x3ff;
	     data2 = (word>>10) & 0x3ff;
	     data3 = word & 0x3ff;
             crc(1,word);
	     crcaa = crca & 0x7fff;
             if(sw8 == 0) printf(" data word= %4i %4i %4x %4x %4x    %8x  %4x %4x\n",
	           topbit,parity,data1,data2,data3,word,crcaa,crcb);
	     kklast = kk;
	   } 
	if(kk<8)
	   { topbit = (word>>31) & 1;
	     parity = (word>>30) & 1;
	     data1 = (word>>20) & 0x3ff;
	     data2 = (word>>10) & 0x3ff;
	     data3 = word & 0x3ff;
             fdata[3*kk+1][ii] = data3;
	     fdata[3*kk+2][ii] = data2;
	     fdata[3*kk+3][ii] = data1;
	   }
	else
	   { topbit = (word>>31) & 1;
	     parity = (word>>30) & 1;
	     data1 = (word>>20) & 0x3ff;
	     data2 = (word>>10) & 0x3ff;
	     data3 = word & 0x3ff;
	     fdata[3*kk+1][ii] = data1;
	     fdata[3*kk+2][ii] = data2;
	     fdata[3*kk+3][ii] = data3;
 	   }
				 
	
	 goto loop;

ctrl:
	 cc++;
	 if(sw8 == 0) printf("control word = %x  cc=%d\n", word,cc);
         if(cc == 1) 
	   { if(word == 0x51115110) goto loop;
	     ctrlok++;
	     goto erxit;
	   }
  	 if(cc == 2)
	  { if(word != 0xfffffff0) 
	      { ctrlok++;
	        goto erxit;
	      }
	    crc1 = wordlast & 0xffff;
	    crc2 = (wordlast>>16) & 0xffff;
	    if(crc1 != crc2) 
              { gcrcok++;
	        printf("crc error wordlast=%x  crc1=%x  crc2=%x\n",
	          wordlast,crc1,crc2);
	        if(sw(5) == 1) fscanf(stdin,"%d",&stop); 
	      }
	 }
erxit:
	 if(bnchok != 0 || gcrcok != 0 || dcrcok != 0 || ctrlok != 0) 
	   { nerror++;
             if(bnchok != 0) printf("bnch= %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
        	bnch[0],bnch[1],bnch[2],bnch[3],bnch[4],bnch[5],bnch[6],bnch[7],
		bnch[8],bnch[9],bnch[10],bnch[11],bnch[12],bnch[13],bnch[14],bnch[15]);
             if(dcrcok != 0) printf(" dbad= %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                 dbad[0],dbad[1],dbad[2],dbad[3],dbad[4],dbad[5],dbad[6],dbad[7],
                 dbad[8],dbad[8],dbad[10],dbad[11],dbad[12],dbad[13],dbad[14],dbad[15]);
	     if(bnchok != 0) nbnch++;
	     else if(gcrcok != 0) ngcrc++;
	     else if(dcrcok != 0) ndcrc++;
	     else if(ctrlok != 0) nctrl++;
	     return;
	   }    
	if(cc ==2) return(0);
errxit:
	printf("read problem   count=%d\n",count);
	fprintf(fperr,"read problem   count=%d\n",count);
        return(1);
       }
	 
