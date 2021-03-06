/**************************************************************************/
/* This file contains the code necessary to use the ttcvi board for the   */
/* Tile Calorimeter digitizer test setup.                                 */
/*                                                                        */
/* Author : S Silverstein & A.Rios                                        */
/* Date   : 31-May-1998                                                   */
/*                                                                        */
/**************************************************************************/

#include <types.h>
#include "ttcvi.h"
#include "std_func.h"

#define VOLATILE volatile

extern char *VME_a24d16_base_address;
extern char *VME_a24d32_base_address;

static VOLATILE char *ttcvi_address;
VOLATILE char *ttcvi = 0;

/*===========================================================================*/
void setup_ttcvi_mapping(void) 
/*===========================================================================*/
{

      ttcvi_address = (VOLATILE char *) (TTCVI_START_ADDRESS +
		      (unsigned long)VME_a24d16_base_address);
			
}
/*===========================================================================*/
void ttcvi_init_l1a(void) {
/*===========================================================================*/
/* This procedure reads and displays the contents of CSR1. */

   VOLATILE unsigned short *csr1;

   ttcvi_csr1 local_csr1;


/* assign pointer to base of ttcvi card: */ 
   ttcvi = (VOLATILE char *) ttcvi_address; 

/*
 * Setup the mapping of the different registers of the ttcvi card;
 */ 

    csr1 = (VOLATILE unsigned short *) &ttcvi[TTCVI_CSR1_OFFSET];

     local_csr1.full = *csr1;
    
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

   return;

}

/*===========================================================================*/
void ttcvi_view_csr2(void) {
/*===========================================================================*/
/*
 * This procedure reads and displays the contents of CSR2.
 */

   VOLATILE unsigned short *csr2;

   ttcvi_csr2 local_csr2;

/* assign pointer to base of ttcvi card: */ 
   ttcvi = (VOLATILE char *) ttcvi_address; 

/*
 * Setup the mapping of the different registers of the ttcvi card;
 */ 

    csr2  = (VOLATILE unsigned short *) &ttcvi[TTCVI_CSR2_OFFSET];

    local_csr2.full = *csr2;

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

   return;

}
/*===========================================================================*/
void init_digitizer(void) {
/*===========================================================================*/
/*
 * This procedure initializes the digitizer board
 */

   VOLATILE unsigned short *longvme1, *longvme2;

   ttcvi_longvme_msw local_vme1;
   ttcvi_longvme_lsw local_vme2;
   int k, ttcrxadd;

/* assign pointer to base of ttcvi card: */ 
   ttcvi = (VOLATILE char *) ttcvi_address; 

/*
 * Setup the mapping of the different registers of the ttcvi card;
 */ 

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

   return;

}


