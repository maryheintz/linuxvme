/*****************************************************************
  rwt.c  -- allows user to read and write to any buffer.  Only one 
 	    buffer at a time.  Menu explains how to use it.
  Coded by A.J. Tolland (ajt@hep.uchicago.edu)
  Last Modified 3/20/1998
*****************************************************************/


#include <stdio.h>

menu();

main()
{ 
   short *adr;
   int choice;
   long temp;
   unsigned short data;

   printf("Input Address of Target Register in Hex: ");
   scanf("%x", &temp);
   adr = (short*) temp;

   while((choice=menu()) != 5) {
      switch(choice) {
         case 1: {data = *adr; printf("%x\n", data); break;}
         case 2: {printf("Entry: "); scanf("%x", &temp); 
 		  *adr = temp; break;}
         case 3: {printf("Address: "); scanf("%x", &temp); 
                  adr=(short *) temp; break;}
         case 4: {printf("Present address: %x\n", (int *) adr); break;} 
     }
   }
}

int menu() {
   int choice;
   printf("1. Read\n2. Write\n3. Change address\n4. Present Address\n5. Quit\n");
   printf("Enter command: ");
   scanf("%d", &choice);
   return(choice);
}
