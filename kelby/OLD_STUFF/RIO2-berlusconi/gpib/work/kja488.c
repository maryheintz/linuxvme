#include <string.h>
#define SRQI (1<<12)
#define my_address 0
#define UNL 0x3f 
#define UNT 0x5f
#define LLO 0x11
#define DCL 0x14
#define SDC 0x4
#define MLA 0x20
#define MTA 0x40
unsigned char cbuf[30];
gpib_reset()
    { ibonl(1);
      ibsre(1);
      ibsic(); 
      ibsre(1);
      cbuf[0] = LLO;
      cbuf[1] = MLA | 0;
      cbuf[2] = SDC;
      cbuf[3] = DCL;
      ibcmd(cbuf,4);
      sleep(1);
     }
getmessage(device,buf)
 unsigned char device;
 unsigned char buf[];
 { int count;
   unsigned int status;
   /* 
    * send the UNL=0x3f, UNT=0x5f, followed by the interface
    * controller listen address=0x20 and the scope talk address=0x41.
   */
   cbuf[0] = UNL;
   cbuf[1] = UNT;
   cbuf[2] = MLA | my_address;
   cbuf[3] = MTA | device;
   ibcmd(cbuf, 4);
   
   /* Read the measurement from the scope (it is still addressed
    * to talk and the interface is still addressed to listen).
   */
/*   ibwait(SRQI);  */
   count=200000;    
   status=ibrd(buf,count);
/*   printf("ibrd status=%x\n",status);  */   
   count = gpib_count();  /* get buffer count used */
/*   printf("buf=%s\n",buf);    */
   return(count);
 }
sendmessage(device,buf)
 unsigned char device;
 unsigned char buf[];
 { int count;
   unsigned int status;
   /* 
    * send the UNL=0x3f, UNT=0x5f, followed by the interface
    * controller listen address=0x20 and the scope talk address=0x41.
   */
   cbuf[0] = UNL;
   cbuf[1] = UNT;
   cbuf[2] = MTA | my_address;
   cbuf[3] = MLA | device;
   ibcmd(cbuf, 4);
  
   /* Read the measurement from the scope (it is still addressed
    * to talk and the interface is still addressed to listen).
   */
   count = strlen(buf);
   status=ibwrt(buf,count);
/*   printf("ibwrt status=%x\n",status);    */ 
 }
gpib_close()
  { ibsic();
    /* Disable the interface */
    ibonl(0);
  }
