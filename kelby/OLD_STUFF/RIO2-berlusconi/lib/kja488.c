#include <string.h>
#define SRQI (1<<12)
gpib_reset()
    { ibonl(1);
      ibsic(); 
      ibsre(1);
     }
getmessage(device,buf)
 int device;
 unsigned char buf[];
 { int count;
   char my_address = 0;
   char UNL = 0x3f;
   char UNT = 0x5f;
   char MLA = 0x20;
   char MTA = 0x40;
   char cbuf[10];
   /* 
    * send the UNL=0x3f, UNT=0x5f, followed by the interface
    * controller listen address=0x20 and the scope talk address=0x41.
   */
   cbuf[0] = UNL;
   cbuf[1] = UNT;
   cbuf[2] = MLA | my_address;
   cbuf[3] = MTA | (char)device;
   ibcmd(cbuf, 4);
   
   /* Read the measurement from the scope (it is still addressed
    * to talk and the interface is still addressed to listen).
   */
/*   ibwait(SRQI);  */  
   ibrd(buf,count);
   count = gpib_count();  /* get buffer count used */
   return(count);
 }
sendmessage(device,buf)
 int device;
 unsigned char buf[];
 { int count;
   char my_address = 0;
   char UNL = 0x3f;
   char UNT = 0x5f;
   char LLO = 0x11;
   char DCL = 0x14;
   char MLA = 0x20;
   char MTA = 0x40;
   char cbuf[5];
   /* 
    * send the UNL=0x3f, UNT=0x5f, followed by the interface
    * controller listen address=0x20 and the scope talk address=0x41.
   */
   cbuf[0] = LLO;
   cbuf[1] = MLA | (char)device;
   cbuf[2] = DCL;
   cbuf[3] = MTA | my_address;
   ibcmd(cbuf, 4);    
  
   /* Read the measurement from the scope (it is still addressed
    * to talk and the interface is still addressed to listen).
   */
   count = strlen(buf);
   ibwrt(buf,count);
 }
gpib_close()
  { ibcmd("\x14", 1);
    ibsic();
    /* Disable the interface */
    ibonl(0);
  }
