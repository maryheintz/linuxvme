#include <stdio.h>
#include <math.h>
unsigned int crca,crcb;
FILE *fp;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { unsigned long word;
    int i;
    fp = fopen("dig.dat","r");
    fscanf(fp,"%x",&word);
    crc(0,word);
    printf(" word=%0.8x   offline crca=%x  crcb=%x\n",word,crca,crcb);
    for(i=0;i<5;i++)
      { fscanf(fp,"%x",&word);
        crc(1,word);
         printf(" word=%8.8x   offline crca=%x  crcb=%x\n",word,crca,crcb);
      }
    fscanf(fp,"%x",&word);
    printf(" crc word=%8.8x\n",word);
    unscramble(word);
    crc(1,0);
  }
     crc(int ent,unsigned long word)
        { int in0[16],in1[16];
          static int crc0[16],crc1[16];
	  static int poly[16] = {1,0,1,0, 0,0,0,0, 0,0,0,0, 0,0,0,1};  
	  int outbit0,outbit1;
	  int i,k,data_in0,data_in1,kk;
	  unsigned long inwd0,inwd1;
	  /* ==================================================
	     ent=0 first entry (header word) initialize
	           then do crc calculation
	     ent=1 normal data - just update crc calculation
          ================================================== */
	  

/* unpack the data word back into the serial streams in0 and in1 sent
   by the digitizer  in0(0) and in1(0) were the first bits recieved */
        
/* sort the 32 bit word back into the 2 serial streams */	  
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
	  
	  inwd0 = 0;
	  for(i=0;i<16;i++)
	    {inwd0 = inwd0 | (in0[i]<<i);
	    }
	  inwd1 = 0; 
	  for(i=0;i<16;i++)
	    {inwd1 = inwd1 | (in1[i]<<i);
	    }
	  printf(" word=%x  inwd0=%x  inwd1=%x\n",word,inwd0,inwd1);
	  
/* if this is the first entry  initialize the outbits and 16 bit crc's */	  
	 if(ent == 0)
	    { outbit0=0;
	      outbit1=0;
	      for(i=0;i<16;i++)
	         { crc0[i]=0;
		   crc1[i]=0;
		 }
	   /*      crc0[0]=1;
		 crc1[0]=1;   */
	    }
/* now send the bits one at a time into the crc calculation */

     for(k=0;k<16;k++)
       { 
/* -------------------- serial data stream 1 -------------- */       
         data_in1 = in1[k];
/* shift in 1 bit -- save the high order bit shifted out */
 	 outbit1 = crc1[15];
	 for(i=15;i>0;i--) crc1[i] = crc1[i-1];
	 crc1[0] = data_in1;
/* exclusive or crc1 with the polynomial if outbit is 1 */ 
	 if(outbit1 == 1) 
	   { for(i=15;i>-1;i--)
	      { crc1[i] = crc1[i] ^ poly[i];  /* ^ = xor */
	      }
	   }

/* -------------------- serial data stream 0 -------------- */       
	 data_in0 = in0[k];
/* shift in 1 bit -- save the high order bit shifted out */
  	 outbit0 = crc0[15];    
	 for(i=15;i>0;i--) crc0[i] = crc0[i-1];
	 crc0[0] = data_in0;
	 if(outbit0 == 1) 
 	   { for(i=15;i>-1;i--)
	      { crc0[i] = crc0[i] ^ poly[i];  /* ^ = xor */
	      }
	   }

/* now put the crc0 bits back together as a 16 bit word */
       crca = 0;
       for(i=15;i>-1;i--) 
         { crca = crca | (crc0[i]<<i);
	 } 
	  
/* now put the crc1 bits back together as a 16 bit word */
       crcb = 0;
       for(i=15;i>-1;i--) crcb = crcb | (crc1[i]<<i);
       } 
       printf("crca=%x  crcb=%x\n",crca,crcb); 
       if(crca == 0x1a71 || crca == 0xa816) exit(0);
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

printf("in0 = %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
 in0[0], in0[1], in0[2], in0[3], in0[4], in0[5], in0[6], in0[7],
 in0[8], in0[9],in0[10],in0[11],in0[12],in0[13],in0[14],in0[15]); 
printf("in1 = %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
 in1[0], in1[1], in1[2], in1[3], in1[4], in1[5], in1[6], in1[7],
 in1[8], in1[9],in1[10],in1[11],in1[12],in1[13],in1[14],in1[15]); 
/* now pack the 2 streams back into 16 bit words */

          out0 = 0;
	  out1 = 0;
	  for(i=0;i<16;i++)
	     { out0 = out0 | (in0[i]<<i);
	       out1 = out1 | (in1[i]<<i);
	     }
	  printf("unscramble digitizer crc %x %x\n",out0,out1);
  }
