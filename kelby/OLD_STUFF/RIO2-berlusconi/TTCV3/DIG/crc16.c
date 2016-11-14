#include <stdio.h>
#include <math.h>
unsigned int crc0,crc1;
FILE *fp;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { unsigned long word;
    int i;
    fp = fopen("dig.dat","r");
    fscanf(fp,"%x",&word);
    printf(" word=%0.8x\n",word);
    crc(0,word);
    for(i=0;i<5;i++)
      { fscanf(fp,"%x",&word);
        printf(" word=%8.8x\n",word);
        crc(1,word);
      }
    fscanf(fp,"%x",&word);
    printf(" crc word=%8.8x\n",word);
    unscramble(word);
  /*  crc(1,word);  */
  /*  crc(1,0);    */
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

    crc0 = 0;
    for(i=0;i<16;i++) 
      { C0[i] = NewCRC[i];
        crc0 = crc0 | (NewCRC[i]<<i);
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

    crc1 = 0;
    for(i=0;i<16;i++) 
      { C1[i] = NewCRC[i];
        crc1 = crc1 | (NewCRC[i]<<i);
	/* printf("i=%d  NewCRC=%x  crc1=%x\n",i,NewCRC[i],crc1); */
      }
    printf("crc0=%x  crc1=%x\n",crc0,crc1);  
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

/*printf("in0 = %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
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
	  printf("unscramble digitizer crc %x %x\n",out0,out1);
  }
