#include <stdio.h>
#include "ugpib.h"		/* user GPIB application include file	*/

unsigned char result[100000];	/* measurement from voltmeter (ASCII) */

main()
{       FILE *fp;
        fp = fopen("scope.ps","w");        
        fprintf(fp,"       \n");
	/* Initialize the interface */
	ibonl(1);

	/* Send IFC to clear the GPIB */
	ibsic();

	/* Set REN remote enable */
	ibsre(1);

	/* 
	 * send the UNL=0x3f, UNT=0x5f, followed by the interface
	 * controller listen address=0x20 and the scope talk address=0x41.
	 */
        ibcmd("\x3f\x5f\x20\x41", 4);

	/* Read the measurement from the scope (it is still addressed
	 * to talk and the interface is still addressed to listen).
	 */
	ibrd(result, 100000);
	result[ibcnt] = 0;		/* make it a null-terminated string */
/*        printf("ibrd return ibcnt=%d\n",ibcnt);

        printf("first=%c%c%c%c%c%c%c%c%c%c\n",
	  result[0],result[1],result[2],result[3],result[4],
          result[5],result[6],result[7],result[8],result[9]);
        printf("last=%c%c%c%c%c%c%c%c%c%c\n",
	  result[ibcnt-9],result[ibcnt-8],result[ibcnt-7],result[ibcnt-6],
	  result[ibcnt-5],result[ibcnt-4],result[ibcnt-3],result[ibcnt-2],
	  result[ibcnt-1],result[ibcnt]);  */
	/* Terminate GPIB activity by clearing the voltmeter and the GPIB */
        fprintf(fp,"%s",result);
	fclose(fp);
	ibcmd("\x14", 1);
	ibsic();

	/* Disable the interface */
	ibonl(0);
}
