/**************************************************************************/
/* This file contains the code necessary to use the ttcvi board for the   */
/* Tile Calorimeter digitizer test setup.                                 */
/*                                                                        */
/* Author : S Silverstein                                                 */
/* Date   : 30-Mar-1998                                                   */
/*                                                                        */
/**************************************************************************/

/* define INTERACTIVE */

#include <limits.h>
#include <types.h>
#include <setjmp.h>
#include <signal.h>
#include "ttcvi.h"
#include "std_func.h"
#include "strings.h"

/*
 * The Lynx-supplied default signal handler definition doesn't include
 * a full function prototpe. Redefine this after checking the value.
 */
 
#define MY_SIG_DFL (void(*)(int))0

/* The GNU C compiler version 1.37.1 on OS9 does not always generate the correct
 * code for pointers to volatile data, which we would normally use to access
 * registers on the various DAQ module cards.  However, as the compiler does not
 * optimise multiple references to non-volatile away, we can safely not use the
 * volatile keyword.  However, we will keep the option of enabling it later by
 * making it an empty macro.
 */
#define VOLATILE volatile


/* This is set to the base address of the shared memory segment which maps the
 * VME address space, by the caller.
 */
extern char *VME_base_address;
extern char *VME_a24d16_base_address;
extern char *VME_a24d32_base_address;
VOLATILE char *ttcvi = 0;

unsigned long vic_map_module(unsigned long addr, int crt);

static VOLATILE char *ttcvi_address; 

static char keyboard_string[256];

char routine_name[30];

 /* This is all stuff concerned with the bus trap handler.  Trap on segment
 * violations too as we often get these instead.
 */
static jmp_buf environment;

void type_return_to_continue(void);
void wipe_screen(void);
char input_char(void);
long input_long(void);

/*===========================================================================*/
int getline(char *line, int max){
/*===========================================================================*/
   if (fgets(line, max, stdin) == NULL)
     return 0;
   else
     return strlen(line);

}

/*===========================================================================*/
static void ttcvi_handler(int sig){ 
/*===========================================================================*/
/* This is the trap handler
 */
   longjmp(environment,1);
}


/*===========================================================================*/
void ttcvi_setup_bus_error_handler(void) {
/*===========================================================================*/
/* Declare trap handler
 */
   signal(SIGSEGV,ttcvi_handler);
   signal(SIGBUS,ttcvi_handler);
}

/*===========================================================================*/
void ttcvi_remove_bus_error_handler(void) {
/*===========================================================================*/
/* Remove trap handler - and revert to original system handler instead
 */
   signal(SIGBUS,MY_SIG_DFL);
   signal(SIGSEGV,MY_SIG_DFL);
}

/*===========================================================================*/
void setup_ttcvi_mapping(void) 
/*===========================================================================*/
{

      ttcvi_address = (VOLATILE char *) (TTCVI_START_ADDRESS +
		      (unsigned long)VME_a24d16_base_address);
			
}

/*===========================================================================*/
long *ttcvi_get_first_address(void) {
/*===========================================================================*/
/* This is an access procedure to return the base address of the ttcvi card.
 */
   return (long *) TTCVI_START_ADDRESS;
}

/*===========================================================================*/
void ttcvi_dummy(int integer) {
/*===========================================================================*/
/* This procedure does nothing!  It is called by ttcvi_present with a JPM MID
 * as its parameter so that the reference to the CSR does not get optimized
 * away by the compiler.
*/
}

/*===========================================================================*/
int ttcvi_present(void) {
/*===========================================================================*/
/* This procedure checks if the specified card is present by probing the
 * location corresponding to its base address.
 *
 * Parameters:
 *  card_number - ttcvi card number
 *
 * Returned value:
 *  1 - Card present
 *  0 - Card not present
 *  Any other value - Cannot install trap handler, so could not find out, or
 *                    could not enable memory access to VME address space.
 */
   int return_value;
   unsigned short data;


   if (!setjmp(environment)) {
      ttcvi = (VOLATILE char *) ttcvi_address;
      ttcvi_setup_bus_error_handler();
/* Access Module ID Register this way so that it isn't optimised away  */
      ttcvi_dummy(data = 
          *(unsigned short *)(ttcvi + TTCVI_CONFIG_EEPROM_OFFSET));
      return_value = 1;
   } else return_value = 0;
   ttcvi_remove_bus_error_handler();
   return return_value;
}

/*===========================================================================*/
void ttcvi_init_l1a(void) {
/*===========================================================================*/
/* This procedure reads and displays the contents of CSR1, and writes them
 * back to the same address, as well as resetting the L1A FIFO. This 
 * subroutine can be used as a model for other functions.
 */

   VOLATILE unsigned short *csr1;
   FILE *file_pointer;

   unsigned short int ttcvi_dummy_var_local;

   ttcvi_csr1 local_csr1;

   char key_command;
   long answer;
   int i;

/* If there is a bus trap during any of this, we jump back to here and
 * display a message about it before returning (although it is probably
 * pretty fatal).
 */
   if (setjmp(environment)) {
      printf("Error initialising L1A fifo"); 
      ttcvi_signal_bus_error();
      return;
   }

   ttcvi_setup_bus_error_handler();

/* assign pointer to base of ttcvi card: */ 
   ttcvi = (VOLATILE char *) ttcvi_address; 

/*
 * Setup the mapping of the different registers of the ttcvi card;
 */ 

    ttcvi_dummy_var_local = 0;

    csr1 = (VOLATILE unsigned short *) &ttcvi[TTCVI_CSR1_OFFSET];

    key_command = ' ';

    while ((key_command != 'z') & (key_command != 'Z')){

     local_csr1.full = *csr1;

     wipe_screen();
    
     printf("Current Status of CSR1: \n");  
     printf("Rand Trigger Rate ......... : %X  ", local_csr1.bits.rantrigrt);
      if (local_csr1.bits.rantrigrt == 0) printf("(1 HZ)\n");     
      if (local_csr1.bits.rantrigrt == 1) printf("(100 Hz)\n");     
      if (local_csr1.bits.rantrigrt == 2) printf("(1 KHz)\n");     
      if (local_csr1.bits.rantrigrt == 3) printf("(5 KHz)\n");     
      if (local_csr1.bits.rantrigrt == 4) printf("(10 KHz)\n");     
      if (local_csr1.bits.rantrigrt == 5) printf("(25 KHz)\n");     
      if (local_csr1.bits.rantrigrt == 6) printf("(50 KHz)\n");     
      if (local_csr1.bits.rantrigrt == 7) printf("(100 KHz)\n");     
     printf("BC delay switch value ..... : %X\n", 
                                           ((0xF)^(local_csr1.bits.bcdelay)));
     printf("VME transfer pending ...... : %X\n", local_csr1.bits.vme_trpend);
     printf("L1A FIFO empty ............ : %X\n", local_csr1.bits.l1_fifoempty);
     printf("L1A FIFO full ............. : %X\n", local_csr1.bits.l1_fifofull);
     printf("Orbit signal select ....... : %X  ", local_csr1.bits.orb_sig_sel);
      if (local_csr1.bits.orb_sig_sel == 0) printf("(External)\n");     
      if (local_csr1.bits.orb_sig_sel == 1) printf("(Internal)\n");     
     printf("L1A trigger select ........ : %X  ", local_csr1.bits.l1_trig_sel);
      if (local_csr1.bits.l1_trig_sel == 0) printf("(L1A<0>)\n");     
      if (local_csr1.bits.l1_trig_sel == 1) printf("(L1A<1>)\n");     
      if (local_csr1.bits.l1_trig_sel == 2) printf("(L1A<2>)\n");     
      if (local_csr1.bits.l1_trig_sel == 3) printf("(L1A<3>)\n");     
      if (local_csr1.bits.l1_trig_sel == 4) printf("(VME)\n");     
      if (local_csr1.bits.l1_trig_sel == 5) printf("(Random)\n");     
      if (local_csr1.bits.l1_trig_sel == 6) printf("(Not supported)\n");     
      if (local_csr1.bits.l1_trig_sel == 7) printf("(Not supported)\n");     
    
     printf("\n");

     printf("Actions Menu: \n");
     
     printf("A .... Set random trigger rate \n");
     printf("B .... Toggle orbit signal select \n");
     printf("C .... Set L1A trigger select \n");
     printf("D .... Reset L1A FIFO \n");
     printf("Z .... Return to main menu \n");
    
     local_csr1.bits.l1_fiforst = 0;
    
     printf("Enter command: \n \n");

     key_command = input_char();
    
     if ((key_command == 'A') | (key_command == 'a')) {
       printf("0=1Hz, 1=100 Hz, 2=1K, 3=5K, 4=10K, 5=25K, 6=50K, 7=100K \n");
       printf("Enter new setting (0-7):\n");  
       answer = input_long();
       i = answer;
       local_csr1.bits.rantrigrt = i;
     }
     if ((key_command == 'B') | (key_command == 'b')) {
       if (local_csr1.bits.orb_sig_sel == 1){
         local_csr1.bits.orb_sig_sel = 0;
       }
       else {
         local_csr1.bits.orb_sig_sel = 1;
       }  
     }
     if ((key_command == 'C') | (key_command == 'c')) {
       printf("0=L1A<0>, 1=L1A<1>, 2=L1A<2>, 3=L1A<3>, 4=VME, 5=RANDOM \n");
       printf("Enter new setting (0-5):\n");  
       answer = input_long();
       i = answer;
       local_csr1.bits.l1_trig_sel = i;
     }
     if ((key_command == 'D') | (key_command == 'd')) {
       local_csr1.bits.l1_fiforst = 1;
     }

/* End of routine */

    *csr1 = local_csr1.full;

   }

   ttcvi_remove_bus_error_handler();

   return;

}

/*===========================================================================*/
void ttcvi_view_csr2(void) {
/*===========================================================================*/
/*
 * This procedure reads and displays the contents of CSR2.
 */

   VOLATILE unsigned short *csr2;

   unsigned short int ttcvi_dummy_var_local;
   ttcvi_csr2 local_csr2;

   char key_command;

/* If there is a bus trap during any of this, we jump back to here and
 * display a message about it before returning (although it is probably
 * pretty fatal).
 */
   if (setjmp(environment)) {
      printf("Error reading CSR2"); 
      ttcvi_signal_bus_error();
      return;
   }

   ttcvi_setup_bus_error_handler();

/* assign pointer to base of ttcvi card: */ 
   ttcvi = (VOLATILE char *) ttcvi_address; 

/*
 * Setup the mapping of the different registers of the ttcvi card;
 */ 
    ttcvi_dummy_var_local = 0;

    csr2  = (VOLATILE unsigned short *) &ttcvi[TTCVI_CSR2_OFFSET];

    local_csr2.full = *csr2;

    wipe_screen();
    
    printf("Current Status of CSR2: \n");
    printf("RTX B-GO FIFO0 ............ : %X\n", local_csr2.bits.rtx_bgo_ff0);
    printf("RTX B-GO FIFO1 ............ : %X\n", local_csr2.bits.rtx_bgo_ff1);
    printf("RTX B-GO FIFO2 ............ : %X\n", local_csr2.bits.rtx_bgo_ff2);
    printf("RTX B-GO FIFO3 ............ : %X\n", local_csr2.bits.rtx_bgo_ff3);
    printf("\n");
    printf("B-GO FIFO 0 FULL .......... : %X\n", local_csr2.bits.bgo_ff0_full);
    printf("B-GO FIFO 0 EMPTY ......... : %X\n", local_csr2.bits.bgo_ff0_mt);
    printf("B-GO FIFO 1 FULL .......... : %X\n", local_csr2.bits.bgo_ff1_full);
    printf("B-GO FIFO 1 EMPTY ......... : %X\n", local_csr2.bits.bgo_ff1_mt);
    printf("B-GO FIFO 2 FULL .......... : %X\n", local_csr2.bits.bgo_ff2_full);
    printf("B-GO FIFO 2 EMPTY ......... : %X\n", local_csr2.bits.bgo_ff2_mt);
    printf("B-GO FIFO 3 FULL .......... : %X\n", local_csr2.bits.bgo_ff3_full);
    printf("B-GO FIFO 3 EMPTY ......... : %X\n", local_csr2.bits.bgo_ff3_mt);

    printf("\n");

/* End of csr2 display */

   ttcvi_remove_bus_error_handler();

   type_return_to_continue();

   return;

}

/*===========================================================================*/
void ttcvi_set_phase(void) {
/*===========================================================================*/
/*
 * This procedure sets the phase value
 */

   VOLATILE unsigned short *longvme1, *longvme2;

   unsigned short int ttcvi_dummy_var_local;
   ttcvi_longvme_msw local_vme1;
   ttcvi_longvme_lsw local_vme2;

   long answer;
   int ttcrxadd, phase_value;
  
   char key_command;

/* If there is a bus trap during any of this, we jump back to here and
 * display a message about it before returning (although it is probably
 * pretty fatal).
 */
   if (setjmp(environment)) {
      printf("Error setting sample number");
      ttcvi_signal_bus_error();
      return;
   }

   ttcvi_setup_bus_error_handler();

/* assign pointer to base of ttcvi card: */
   ttcvi = (VOLATILE char *) ttcvi_address;

/*
 * Setup the mapping of the different registers of the ttcvi card;
 */
    ttcvi_dummy_var_local = 0;
    ttcrxadd = ttcrx_base;

    longvme1 = (VOLATILE unsigned short *) &ttcvi[TTCVI_LONGVME_MSW_OFFSET];
    longvme2 = (VOLATILE unsigned short *) &ttcvi[TTCVI_LONGVME_LSW_OFFSET];

    wipe_screen();

    printf("Enter phase value \n");

    answer = input_long();
    phase_value = answer;
 
    local_vme1.bits.topbit = 1;
    local_vme1.bits.ttcrx_addr = ttcrxadd;
    local_vme1.bits.access_bit = 0;
    local_vme2.bits.subaddr = 1;
    local_vme2.bits.data = phase_value;;

    printf("TOP BIT (always 1)  ....... : %X\n", local_vme1.bits.topbit);
    printf("TTCRX address ............. : %X\n", local_vme1.bits.ttcrx_addr);
    printf("Access (0/1 = int/ext) .... : %X\n", local_vme1.bits.access_bit);

    printf("\n");

    printf("Subaddress ................ : %X\n", local_vme2.bits.subaddr);
    printf("Phase value .............. : %X\n", local_vme2.bits.data);

    printf("\n");

    printf("Send command? (y/n) \n");

    key_command = input_char();

    if ((key_command == 'y') | (key_command == 'Y')) {
       *longvme1 = local_vme1.full;
       *longvme2 = local_vme2.full;
    }

/* End of routine */

   ttcvi_remove_bus_error_handler();

   return;

}

/*===========================================================================*/
void ttcvi_set_samples(void) {
/*===========================================================================*/
/*
 * This procedure sets the number of samples on the digitizer board.
 */

   VOLATILE unsigned short *longvme1, *longvme2;

   unsigned short int ttcvi_dummy_var_local;
   ttcvi_longvme_msw local_vme1;
   ttcvi_longvme_lsw local_vme2;
 
   long answer;
   int ttcrxadd, samples;
   
   char key_command;

/* If there is a bus trap during any of this, we jump back to here and
 * display a message about it before returning (although it is probably
 * pretty fatal).
 */
   if (setjmp(environment)) {
      printf("Error setting sample number"); 
      ttcvi_signal_bus_error();
      return;
   }

   ttcvi_setup_bus_error_handler();

/* assign pointer to base of ttcvi card: */ 
   ttcvi = (VOLATILE char *) ttcvi_address; 

/*
 * Setup the mapping of the different registers of the ttcvi card;
 */ 
    ttcvi_dummy_var_local = 0;
    ttcrxadd = ttcrx_base;

    longvme1 = (VOLATILE unsigned short *) &ttcvi[TTCVI_LONGVME_MSW_OFFSET];
    longvme2 = (VOLATILE unsigned short *) &ttcvi[TTCVI_LONGVME_LSW_OFFSET];

    wipe_screen();
    
    printf("Enter desired number of samples: \n");

    answer = input_long();
    samples = answer;

    local_vme1.bits.topbit = 1;
    local_vme1.bits.ttcrx_addr = ttcrxadd;
    local_vme1.bits.access_bit = 1;
    local_vme2.bits.subaddr = 1;
    local_vme2.bits.data = samples;
    
    printf("TOP BIT (always 1)  ....... : %X\n", local_vme1.bits.topbit);
    printf("TTCRX address ............. : %X\n", local_vme1.bits.ttcrx_addr);
    printf("Access (0/1 = int/ext) .... : %X\n", local_vme1.bits.access_bit);
 
    printf("\n");

    printf("Subaddress ................ : %X\n", local_vme2.bits.subaddr);
    printf("# of samples .............. : %X\n", local_vme2.bits.data);

    printf("\n");

    printf("Send command? (y/n) \n");

    key_command = input_char();
    
    if ((key_command == 'y') | (key_command == 'Y')) {
       *longvme1 = local_vme1.full;
       *longvme2 = local_vme2.full;
    }

/* End of routine */

   ttcvi_remove_bus_error_handler();

   return;

}


/*===========================================================================*/
void ttcvi_set_pipelen(void) {
/*===========================================================================*/
/*
 * This procedure sets the pipeline length of the digitizer board.
 */

   VOLATILE unsigned short *longvme1, *longvme2;

   unsigned short int ttcvi_dummy_var_local;
   ttcvi_longvme_msw local_vme1;
   ttcvi_longvme_lsw local_vme2;
 
   long answer;
   int ttcrxadd, pipelen;
   
   char key_command;

/* If there is a bus trap during any of this, we jump back to here and
 * display a message about it before returning (although it is probably
 * pretty fatal).
 */
   if (setjmp(environment)) {
      printf("Error setting pipleline length"); 
      ttcvi_signal_bus_error();
      return;
   }

   ttcvi_setup_bus_error_handler();

/* assign pointer to base of ttcvi card: */ 
   ttcvi = (VOLATILE char *) ttcvi_address; 

/*
 * Setup the mapping of the different registers of the ttcvi card;
 */ 
    ttcvi_dummy_var_local = 0;
    ttcrxadd = ttcrx_base;

    longvme1 = (VOLATILE unsigned short *) &ttcvi[TTCVI_LONGVME_MSW_OFFSET];
    longvme2 = (VOLATILE unsigned short *) &ttcvi[TTCVI_LONGVME_LSW_OFFSET];

    wipe_screen();
    
    printf("Enter desired pipeline length: \n");

    answer = input_long();
    pipelen = answer;

    local_vme1.bits.topbit = 1;
    local_vme1.bits.ttcrx_addr = ttcrxadd;
    local_vme1.bits.access_bit = 1;
    local_vme2.bits.subaddr = 2;
    local_vme2.bits.data = pipelen;
    
    printf("TOP BIT (always 1)  ....... : %X\n", local_vme1.bits.topbit);
    printf("TTCRX address ............. : %X\n", local_vme1.bits.ttcrx_addr);
    printf("Access (0/1 = int/ext) .... : %X\n", local_vme1.bits.access_bit);
 
    printf("\n");

    printf("Subaddress ................ : %X\n", local_vme2.bits.subaddr);
    printf("Pipeline length ........... : %X\n", local_vme2.bits.data);

    printf("\n");

    printf("Send command? (y/n) \n");

    key_command = input_char();
    
    if ((key_command == 'y') | (key_command == 'Y')) {
       *longvme1 = local_vme1.full;
       *longvme2 = local_vme2.full;
    }

/* End of routine */

   ttcvi_remove_bus_error_handler();

   return;

}


/*===========================================================================*/
void ttcvi_set_testwd(void) {
/*===========================================================================*/
/*
 * This procedure sets the test word. NOT DONE YET!
 */

   VOLATILE unsigned short *longvme1, *longvme2;

   unsigned short int ttcvi_dummy_var_local;

   ttcvi_longvme_msw local_vme1;
   ttcvi_longvme_lsw local_vme2;
   int k, ttcrxadd;
   long answer;
   
   char key_command, canswer;

/* If there is a bus trap during any of this, we jump back to here and
 * display a message about it before returning (although it is probably
 * pretty fatal).
 */
   if (setjmp(environment)) {
      printf("Error setting pipleline length"); 
      ttcvi_signal_bus_error();
      return;
   }

   ttcvi_setup_bus_error_handler();

/* assign pointer to base of ttcvi card: */ 
   ttcvi = (VOLATILE char *) ttcvi_address; 

/*
 * Setup the mapping of the different registers of the ttcvi card;
 */ 
    ttcvi_dummy_var_local = 0;
    ttcrxadd = ttcrx_base;

    longvme1 = (VOLATILE unsigned short *) &ttcvi[TTCVI_LONGVME_MSW_OFFSET];
    longvme2 = (VOLATILE unsigned short *) &ttcvi[TTCVI_LONGVME_LSW_OFFSET];

    while ((key_command != 'z') & (key_command !='Z')) {

      wipe_screen();
    
      printf("Test word part 1: %X \n", testwd1);
      printf("          part 2: %X \n", testwd2);
      printf("          part 3: %X \n", testwd3);
      printf("\n");

      printf("Change test word parts (1-3) or send test word (z): \n \n");

      key_command = input_char();

      if (key_command == '1') {
    	 printf("Enter new value for test word 1 (00 to FF)");
         k = getline(keyboard_string,5);
         (void)sscanf(keyboard_string,"%X",&answer);
	 testwd1 = answer;
      }
      else if (key_command == '2') {
         printf("Enter new value for test word 2 (00 to FF)");
         k = getline(keyboard_string,5);
         (void)sscanf(keyboard_string,"%X",&answer);
	 testwd2 = answer;
      }
      else if (key_command == '3') {
         printf("Enter new value for test word 3 (00 to FF)");
         k = getline(keyboard_string,5);
         (void)sscanf(keyboard_string,"%X",&answer);
	 testwd3 = answer; 
      }

    }

    local_vme1.bits.topbit = 1;
    local_vme1.bits.ttcrx_addr = ttcrxadd;
    local_vme1.bits.access_bit = 1;
    local_vme2.bits.subaddr = 3;
    local_vme2.bits.data = testwd1;

    *longvme1 = local_vme1.full;
    *longvme2 = local_vme2.full;

    local_vme2.bits.subaddr = 4;
    local_vme2.bits.data = testwd2;

    *longvme1 = local_vme1.full;
    *longvme2 = local_vme2.full;

    local_vme2.bits.subaddr = 5;
    local_vme2.bits.data = testwd3;

    *longvme1 = local_vme1.full;
    *longvme2 = local_vme2.full;



/* End of routine */

   ttcvi_remove_bus_error_handler();

   return;

}


/*===========================================================================*/
void ttcvi_set_dig_runmd(void) {
/*===========================================================================*/
/*
 * This procedure sets the run mode of the digitizer board.
 */

   VOLATILE unsigned short *longvme1, *longvme2;

   unsigned short int ttcvi_dummy_var_local;
   ttcvi_longvme_msw local_vme1;
   ttcvi_longvme_lsw local_vme2;
 
   int ok;
   int ttcrxadd, mode;
   
   char key_command;

/* If there is a bus trap during any of this, we jump back to here and
 * display a message about it before returning (although it is probably
 * pretty fatal).
 */
   if (setjmp(environment)) {
      printf("Error setting run mode"); 
      ttcvi_signal_bus_error();
      return;
   }

   ttcvi_setup_bus_error_handler();

/* assign pointer to base of ttcvi card: */ 
   ttcvi = (VOLATILE char *) ttcvi_address; 

/*
 * Setup the mapping of the different registers of the ttcvi card;
 */ 
    ttcvi_dummy_var_local = 0;
    ttcrxadd = ttcrx_base;

    longvme1 = (VOLATILE unsigned short *) &ttcvi[TTCVI_LONGVME_MSW_OFFSET];
    longvme2 = (VOLATILE unsigned short *) &ttcvi[TTCVI_LONGVME_LSW_OFFSET];

    ok = 0; 
    
    while (ok == 0) {

      wipe_screen();
    
      printf("Enter desired digitizer running mode (A-C): \n \n");
      printf(" A ... Normal Mode (0) \n");
      printf(" B ... Test Mode (1) \n");
      printf(" C ... Calibration Mode (2) \n");

      key_command = input_char();

      if ((key_command == 'a')|(key_command == 'A')) {
    	  mode = 0;
	  ok = 1;
      }
      else if ((key_command == 'b')|(key_command == 'B')) {
    	  mode = 1;
	  ok = 1;
      }
      else if ((key_command == 'c')|(key_command == 'C')) {
    	  mode = 2;
	  ok = 1;
      }

    }

    local_vme1.bits.topbit = 1;
    local_vme1.bits.ttcrx_addr = ttcrxadd;
    local_vme1.bits.access_bit = 1;
    local_vme2.bits.subaddr = 7;
    local_vme2.bits.data = mode;
    
    printf("TOP BIT (always 1)  ....... : %X\n", local_vme1.bits.topbit);
    printf("TTCRX address ............. : %X\n", local_vme1.bits.ttcrx_addr);
    printf("Access (0/1 = int/ext) .... : %X\n", local_vme1.bits.access_bit);
 
    printf("\n");

    printf("Subaddress ................ : %X\n", local_vme2.bits.subaddr);
    printf("Mode ...................... : %X\n", local_vme2.bits.data);

    printf("\n");

    printf("Send command? (y/n) \n");

    key_command = input_char();
    
    if ((key_command == 'y') | (key_command == 'Y')) {
       *longvme1 = local_vme1.full;
       *longvme2 = local_vme2.full;
    }

/* End of routine */

   ttcvi_remove_bus_error_handler();

   return;

}
/*===========================================================================*/
void ttcvi_short_vme(void) {
/*===========================================================================*/
/*
 * This procedure sends a short vme broadcast command.
 */

   VOLATILE unsigned short *shortvme;

   unsigned short int ttcvi_dummy_var_local;
 
   long answer;
   
   char key_command;

/* If there is a bus trap during any of this, we jump back to here and
 * display a message about it before returning (although it is probably
 * pretty fatal).
 */
   if (setjmp(environment)) {
      printf("Error sending short VME broadcast command"); 
      ttcvi_signal_bus_error();
      return;
   }

   ttcvi_setup_bus_error_handler();

/* assign pointer to base of ttcvi card: */ 
   ttcvi = (VOLATILE char *) ttcvi_address; 

   shortvme = (VOLATILE unsigned short *) &ttcvi[TTCVI_SHORT_VME_CYCLE_OFFSET];

/*
 * Setup the mapping of the different registers of the ttcvi card;
 */ 
    ttcvi_dummy_var_local = 0;
    wipe_screen();
    
    printf("Enter 8-bit command (integer from 0 to 255): \n");

    answer = input_long();
    ttcvi_dummy_var_local = answer;

    printf("\n");

    printf("Command ................... : %X\n", ttcvi_dummy_var_local);

    printf("\n");

    printf("Send command? (y/n) \n");

    key_command = input_char();
    
    if ((key_command == 'y') | (key_command == 'Y')) {
       *shortvme = ttcvi_dummy_var_local;
    }

/* End of routine */

   ttcvi_remove_bus_error_handler();

   return;

}


/*===========================================================================*/
void ttcvi_send_l1a(void) {
/*===========================================================================*/
/*
 * This procedure sends a level-1 accept if the VME trigger is enabled.
 */

   VOLATILE unsigned short *vmetrig;
 
   int dume;
   int ttcrxadd, mode;
   
   char key_command;

/* If there is a bus trap during any of this, we jump back to here and
 * display a message about it before returning (although it is probably
 * pretty fatal).
 */
   if (setjmp(environment)) {
      printf("Error generating L1A"); 
      ttcvi_signal_bus_error();
      return;
   }

   ttcvi_setup_bus_error_handler();

/* assign pointer to base of ttcvi card: */ 
   ttcvi = (VOLATILE char *) ttcvi_address; 

/*
 * Setup the mapping of the SW-L1A register of the ttcvi card;
 */ 

    vmetrig = (VOLATILE unsigned short *) &ttcvi[TTCVI_SW_L1A_OFFSET];

/*
 * Send the trigger...
 */
    dume = 0; 
    
    *vmetrig = dume;

/* End of routine */

   ttcvi_remove_bus_error_handler();

   return;

}

/*===========================================================================*/
void ttcvi_change_add(void) {
/*===========================================================================*/
/*
 * This procedure changes the address of the TTCrx chip to be addressed
 */
   
   char key_command, canswer;
   unsigned int testint;
    
    printf("Enter TTCrx address: \n");

    canswer = input_char();
    testint = atoi(&canswer);
    
    ttcrx_base = testint;

   return;

}

/*===========================================================================*/
void ttcvi_init_register(void) {
/*===========================================================================*/
/*
 * This procedure initializes the TTCrx control register.
 */

   VOLATILE unsigned short *longvme1, *longvme2;

   unsigned short int ttcvi_dummy_var_local;
   ttcvi_longvme_msw local_vme1;
   ttcvi_longvme_lsw local_vme2;
 
   long answer;
   int ttcrxadd, command;
   
   char key_command;

/* If there is a bus trap during any of this, we jump back to here and
 * display a message about it before returning (although it is probably
 * pretty fatal).
 */
   if (setjmp(environment)) {
      printf("Error initializing control register"); 
      ttcvi_signal_bus_error();
      return;
   }

   ttcvi_setup_bus_error_handler();

/* assign pointer to base of ttcvi card: */ 
   ttcvi = (VOLATILE char *) ttcvi_address; 

/*
 * Setup the mapping of the different registers of the ttcvi card;
 */ 
    ttcvi_dummy_var_local = 0;
    ttcrxadd = ttcrx_base;

    longvme1 = (VOLATILE unsigned short *) &ttcvi[TTCVI_LONGVME_MSW_OFFSET];
    longvme2 = (VOLATILE unsigned short *) &ttcvi[TTCVI_LONGVME_LSW_OFFSET];

    wipe_screen();

    command  = 0xA3;

    local_vme1.bits.topbit = 1;
    local_vme1.bits.ttcrx_addr = ttcrxadd;
    local_vme1.bits.access_bit = 0;
    local_vme2.bits.subaddr = 3;
    local_vme2.bits.data = command;
    
    printf("TOP BIT (always 1)  ....... : %X\n", local_vme1.bits.topbit);
    printf("TTCRX address ............. : %X\n", local_vme1.bits.ttcrx_addr);
    printf("Access (0/1 = int/ext) .... : %X\n", local_vme1.bits.access_bit);
 
    printf("\n");

    printf("Subaddress ................ : %X\n", local_vme2.bits.subaddr);
    printf("Pipeline length ........... : %X\n", local_vme2.bits.data);

    printf("\n");

    printf("Send command? (y/n) \n");

    key_command = input_char();
    
    if ((key_command == 'y') | (key_command == 'Y')) {
       *longvme1 = local_vme1.full;
       *longvme2 = local_vme2.full;
    }

/* End of routine */

   ttcvi_remove_bus_error_handler();

   return;

}

/*===========================================================================*/
void init_digitizer(void) {
/*===========================================================================*/
/*
 * This procedure initializes the digitizer board
 */

   VOLATILE unsigned short *longvme1, *longvme2;

   unsigned short int ttcvi_dummy_var_local;

   ttcvi_longvme_msw local_vme1;
   ttcvi_longvme_lsw local_vme2;
   int k, ttcrxadd;
   long answer;
   
   char key_command, canswer;

/* If there is a bus trap during any of this, we jump back to here and
 * display a message about it before returning (although it is probably
 * pretty fatal).
 */
   if (setjmp(environment)) {
      printf("Error setting pipleline length"); 
      ttcvi_signal_bus_error();
      return;
   }

   ttcvi_setup_bus_error_handler();

/* assign pointer to base of ttcvi card: */ 
   ttcvi = (VOLATILE char *) ttcvi_address; 

/*
 * Setup the mapping of the different registers of the ttcvi card;
 */ 
    ttcvi_dummy_var_local = 0;
    ttcrxadd = ttcrx_base;

    longvme1 = (VOLATILE unsigned short *) &ttcvi[TTCVI_LONGVME_MSW_OFFSET];
    longvme2 = (VOLATILE unsigned short *) &ttcvi[TTCVI_LONGVME_LSW_OFFSET];

/* Initialize TTCrx for receiving commands */

    local_vme1.bits.topbit = 1;
    local_vme1.bits.ttcrx_addr = ttcrxadd;
    local_vme1.bits.access_bit = 0;

    *longvme1 = local_vme1.full;

    
    local_vme2.bits.subaddr = 3;
    local_vme2.bits.data = 0xA3;

    *longvme2 = local_vme2.full;

/* Configure MSW */

    local_vme1.bits.topbit = 1;
    local_vme1.bits.ttcrx_addr = ttcrxadd;
    local_vme1.bits.access_bit = 1;

    *longvme1 = local_vme1.full;

/* Set number of samples */

    local_vme2.bits.subaddr = 1;
    local_vme2.bits.data = DIGI_SAMPLES;

    *longvme2 = local_vme2.full;
   
/* Set pipeline length */

    local_vme2.bits.subaddr = 2;
    local_vme2.bits.data = DIGI_PIPELEN;

    *longvme2 = local_vme2.full;
   
/* Set test word (3 bytes) */

    local_vme2.bits.subaddr = 3;
    local_vme2.bits.data = DIGI_TESTWORD1;

    *longvme2 = local_vme2.full;

    local_vme2.bits.subaddr = 4;
    local_vme2.bits.data = DIGI_TESTWORD2;

    *longvme2 = local_vme2.full;

    local_vme2.bits.subaddr = 5;
    local_vme2.bits.data = DIGI_TESTWORD3;

    *longvme2 = local_vme2.full;

/* Set run mode to data taking */

    local_vme2.bits.subaddr = 7;
    local_vme2.bits.data = 0;

    *longvme2 = local_vme2.full;



/* End of routine */

   ttcvi_remove_bus_error_handler();

   return;

}


/*===========================================================================*/
void ttcvi_signal_bus_error(void) {
/*===========================================================================*/
/* This procedure handles bus errors - it is generally from the JPM code when
 * a bus error is detected to produce the necessary error messages.  To prevent
 * the program from going into an infinite loop if there is a bus error in this
 * procedure (an unlikely occurrance I hope!), it cancels the error handler
 * before doing anything else. Although it doesn't do a great deal, it is not
 * called often but takes up quite a few lines because of the conditional
 * compilation, so it is worth putting into a seperate procedure.
 */


   ttcvi_remove_bus_error_handler();
#ifdef INTERACTIVE
   (void)printf("Bus error while accessing TTCVI");
#endif

}



