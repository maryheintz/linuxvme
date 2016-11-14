#include <stdio.h>
typedef union {
	unsigned long ldata;
	struct {
		unsigned int topbit   :  1;   /* bit31 head flag */
		unsigned int parity   :  1;   /* bit30 parity bit */
		unsigned int data1    :  10;  /* bit(29:20) adc value1 */
		unsigned int data2    :  10;  /* bit(19:10) adc value2 */
		unsigned int data3    :  10;  /* bit(9:0) adc value3 */
		} bits;
	} slink_formatted_data;
unsigned int kk[22]={0xe02c87ea,0x0801b873,0x4801b477,0x4821b873,
                     0x4791ac74,0x0861c878,0x0861c878,0x0861b876,
		     0x47b1b473,0x47f1ac74,0xe028f7ea,0x4731e874,
		     0x0781f48a,0x06d1d476,0x48e25c92,0x5b26d1b9,
		     0x1fe809f3,0x0ebbd0f8,0x08f24c96,0x47820882,
                     0x533005e2,0xe02c87ea};

  main()
  {unsigned long word;
   int i;
   slink_formatted_data sd;
    i=0;
    word = kk[i];
     printf("i=%d   header word = %x\n",i,word);
    for(i=1;i<10;i++)
      { word = kk[i];
	sd.ldata =  word;
        printf("i=%d  data word= %4i %4i %4x %4x %4x",i,
	                              sd.bits.topbit,sd.bits.parity,
				      sd.bits.data1,sd.bits.data2,
				      sd.bits.data3);
        printf("   word=%x\n",word);
      }
    i=10;  
    word = kk[i];
     printf(" header word = %x\n",word);
    for(i=11;i<20;i++)
      { word = kk[i];
	sd.ldata =  word;
        printf("i=%d  data word= %4i %4i %4x %4x %4x",i,
	                              sd.bits.topbit,sd.bits.parity,
				      sd.bits.data1,sd.bits.data2,
				      sd.bits.data3);
        printf("   word=%x\n",i,word);
      }
    i=20;
    word = kk[i];
     printf("i=%d   crc word = %x\n",i,word);
    i=21;
    word = kk[i];
     printf("i=%d   header word = %x\n",i,word);
   }
