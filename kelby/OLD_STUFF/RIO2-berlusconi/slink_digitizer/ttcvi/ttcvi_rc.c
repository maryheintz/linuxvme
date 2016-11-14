/* file: <ttcvi_rc.c>             created: 13-April 1998   S. Silverstein     *
*                                                                             *
* Main LynxOS TTCVI Control Process                                           *
* ---------------------------------                                           *
*                                                                             *
*******************************************************************************/

#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <strings.h>
#include <ioctl.h>
#include <resource.h>
#include <sys/sched.h>
#include <dir.h>
#include "std_func.h"
#include "ttcvi.h"

char *machine_name(void);

long input_long(void);

void zero_statistics(void);

#define KB_STRING_SIZE 256              /* Size of 'char' array below       */

char  keyboard_string[KB_STRING_SIZE];  /* String to receive keyboard input */

int             keyboard_active;

long equip_no;
long prog_id;
long status;
long ev_status;
long nbunits;
long event_no;
int producer_pid;
typedef char str16[16];
str16 units[8];

int *myrec;

int dumint;

#define BUFFER_SIZE 256
int k, inh, outh, size;
char buffer[BUFFER_SIZE+1];
char o_string[BUFFER_SIZE+1];

int tvalue;
int hist_id;

/* Dummy routines for now */
void daq_get_run_status(long *run_status){}

int map_vme(void);
void wipe_screen(void);


/*===========================================================================*/
int check_directory(void)
/*===========================================================================*/
/* This routine checks that the current directory is dev or bin. 
 * More precisely, the lat 3 characters of the pathname are checked.
 */
{
   char pathname[MAXPATHLEN];
   int nch;
   extern char *getwd(char *);

   getwd(pathname);
   nch = strlen(pathname);
   if(nch < 3) return 0;
   nch -=3;
   if(strncmp(&pathname[nch],"dev",3)==0) {
      printf("This is the DEVELOPMENT run control.\n");
      sleep(1);
      return 1;
      }
   if(strncmp(&pathname[nch],"bin",3) == 0) {
      printf("This is the PRODUCTION run control.\n");
      sleep(1);
      return 2;
      }
   printf("Please run from the dev or bin directory !!\n");
   exit(0); 
   return (0);
}

/*===========================================================================*/
void ensure_vme_mapped(void)
/*===========================================================================*/
/* Map VME address space into a shared memory segment.  Note that map_vme()
 * does not mind if it is called several times so there is no need to check
 * here if we have called it already (map_vme() does that itself).
 */
 
{
   if (map_vme()) {
      printf("Error mapping VME....");
   }
}

/*===========================================================================*/
void type_return_to_continue(void) {
/*===========================================================================*/
/* In various places, we print a message and ask the user to type a carriage
 * return to make the program carry on to the next bit.  This stops the message
 * from whizzing off of the top of the screen.  This is the procedure which
 * displays this message and waits for the carriage return.
 */
   printf("\n Type Return to continue\n");
   input_long();
}


/*===========================================================================*/
long input_long(void)
/*===========================================================================*/
{

   size = read(0,buffer,BUFFER_SIZE);
   return ((size > 0) ? atol(buffer) : 9999);
}
/*===========================================================================*/

/*===========================================================================*/
int print_version_action(void)
/*===========================================================================*/
{

    printf("\n Stockholm TTCvi Run Control for LynxOS, Version 1.0\n");
    input_long();
    return 1;
}
/*===========================================================================*/

/*===========================================================================*/
int quit_menu_action (void)
/*===========================================================================*/
{
   printf("Stockholm TTCvi Run Control - normal exit.\n");

   return 0;
}
/*===========================================================================*/

/*===========================================================================*/
char input_char(void)
/*===========================================================================*/
{
   size=read(0,buffer,BUFFER_SIZE);
   return ((size > 0) ? (buffer[0]) : '\n');
}

/*===========================================================================*/

/*           ***** START OF TTCVI_RC MAIN ***** */

/*===========================================================================*/
void main(int argc, char**argv)
/*===========================================================================*/
{

  char key_command;

  printf("Stockholm TTCvi Run Control System starting...\n");

  check_directory();
  
/*
 * Map to VME
 */

    ensure_vme_mapped();

/*
 * Initialise vme.
 */  

    setup_ttcvi_mapping();
    
    if (!ttcvi_present()){
    	printf("TTCVI card is not in the system!");
	exit(0);
	}

/*
 * Enter main menu
 */

    ttcrx_base = TTCVI_DIG_BASEADDR;
    testwd1 = TTCVI_TESTWD1;
    testwd2 = TTCVI_TESTWD2;
    testwd3 = TTCVI_TESTWD3;

    key_command = ' ';

    while ((key_command != 'z') & (key_command != 'Z')){
 
     wipe_screen();
	
     key_command = ' ';
    
     printf("Stockholm TTCvi Run Control System \n \n");
     printf("Actions Menu:\n \n");
     printf(" A .... View CSR1 and change parameters \n");
     printf(" B .... View CSR2 \n");
     printf(" C .... Set number of samples \n");
     printf(" D .... Set length of pipeline \n");
     printf(" E .... Set 24-bit test word   \n");
     printf(" F .... Set digitizer run mode \n");
     printf(" G .... Send L1A (only works if VME option enabled) \n");
     printf(" H .... Send short VME broadcast command \n");
     printf(" I .... Modify TTCrx address (currently %X) \n",ttcrx_base);
     printf(" J .... Initialize TTCrx control register \n");
     printf(" K .... Set the Phase Value \n");
  
     printf("\n Z .... Exit run control\n \n");

     key_command = input_char();

     if ((key_command == 'a')|(key_command == 'A')) ttcvi_init_l1a();
     else if ((key_command == 'b')|(key_command == 'B')) ttcvi_view_csr2();
     else if ((key_command == 'c')|(key_command == 'C')) ttcvi_set_samples();
     else if ((key_command == 'd')|(key_command == 'D')) ttcvi_set_pipelen();
     else if ((key_command == 'e')|(key_command == 'E')) ttcvi_set_testwd();
     else if ((key_command == 'f')|(key_command == 'F')) ttcvi_set_dig_runmd();
     else if ((key_command == 'g')|(key_command == 'G')) ttcvi_send_l1a();
     else if ((key_command == 'h')|(key_command == 'H')) ttcvi_short_vme();
     else if ((key_command == 'i')|(key_command == 'I')) ttcvi_change_add();
     else if ((key_command == 'j')|(key_command == 'J')) ttcvi_init_register();
     else if ((key_command == 'k')|(key_command == 'K')) ttcvi_set_phase();
    
    }
  exit(0);
}
/*===========================================================================*/

