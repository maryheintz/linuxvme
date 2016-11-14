#ifndef TTCVI_H_
#define TTCVI_H_

/***************************************************************************/
/* This file contains parameters and declarations needed to access the     */
/* ttcvi card.                                                             */
/*                                                                         */
/* Author : S Silverstein                                                  */
/* Date   : 11-Apr-1998                                                    */
/* Version: 1.0                                                            */
/***************************************************************************/

/* WARNING - the syntax used for bit definitions (aaa.bb :n) is not portable.
 * Other compilers may generate code with inverted bit ordering which is
 * incompatible with the hardware.
 */ 

#ifdef Lynx 
#define BITS_OK
#endif

#ifndef BITS_OK
#error Bit definition ordering has not been tested on this system.
#endif

#define TTCVI_SHORT_VME_CYCLE_OFFSET    (0xC4)
#define TTCVI_LONGVME_LSW_OFFSET        (0xC2)
#define TTCVI_LONGVME_MSW_OFFSET        (0xC0)
#define TTCVI_B_GO_3_PARAM_LSW_OFFSET   (0xBE)
#define TTCVI_B_GO_3_PARAM_MSW_OFFSET   (0xBC)
#define TTCVI_B_GO_2_PARAM_LSW_OFFSET   (0xBA)
#define TTCVI_B_GO_2_PARAM_MSW_OFFSET   (0xB8)
#define TTCVI_B_GO_1_PARAM_LSW_OFFSET   (0xB6)
#define TTCVI_B_GO_1_PARAM_MSW_OFFSET   (0xB4)
#define TTCVI_B_GO_0_PARAM_LSW_OFFSET   (0xB2)
#define TTCVI_B_GO_0_PARAM_MSW_OFFSET   (0xB0)
#define TTCVI_B_GO_3_SW_GO_OFFSET       (0xAE)
#define TTCVI_INH_3_DURATION_OFFSET     (0xAC)
#define TTCVI_INH_3_DELAY_OFFSET        (0xAA)
#define TTCVI_B_GO_3_MODE_OFFSET        (0xA8)
#define TTCVI_B_GO_2_SW_GO_OFFSET       (0xA6)
#define TTCVI_INH_2_DURATION_OFFSET     (0xA4)
#define TTCVI_INH_2_DELAY_OFFSET        (0xA2)
#define TTCVI_B_GO_2_MODE_OFFSET        (0xA0)
#define TTCVI_B_GO_1_SW_GO_OFFSET       (0x9E)
#define TTCVI_INH_1_DURATION_OFFSET     (0x9C)
#define TTCVI_INH_1_DELAY_OFFSET        (0x9A)
#define TTCVI_B_GO_1_MODE_OFFSET        (0x98)
#define TTCVI_B_GO_0_SW_GO_OFFSET       (0x96)
#define TTCVI_INH_0_DURATION_OFFSET     (0x94)
#define TTCVI_INH_0_DELAY_OFFSET        (0x92)
#define TTCVI_B_GO_0_MODE_OFFSET        (0x90)
#define TTCVI_EVENT_COUNT_LSW_OFFSET    (0x8A)
#define TTCVI_EVENT_COUNT_MSW_OFFSET    (0x88)
#define TTCVI_SW_L1A_OFFSET             (0x86)
#define TTCVI_SW_RESET_OFFSET           (0x84)
#define TTCVI_CSR2_OFFSET               (0x82)
#define TTCVI_CSR1_OFFSET               (0x80)
#define TTCVI_CONFIG_EEPROM_OFFSET      (0x00)

#define TTCVI_START_ADDRESS (0xB00000)
#define TTCVI_ADDRESS_RANGE (0x010000)


#define TTCVI_DIG_BASEADDR (0x0)
#define TTCVI_TESTWD1 (0xAA)
#define TTCVI_TESTWD2 (0x55)
#define TTCVI_TESTWD3 (0xAA)

#define TTCVI_FILENAME_SIZE (16)

/* Digitizer Variables */

#define DIGI_SAMPLES (15)
#define DIGI_PIPELEN (28)
#define DIGI_TESTWORD1 (0xAA)
#define DIGI_TESTWORD2 (0x55)
#define DIGI_TESTWORD3 (0xAA)


unsigned short int ttcvi_dummy_var;

/* Global Variables: */

unsigned int ttcrx_base;
unsigned int testwd1, testwd2, testwd3;


/* Set up type definitions */

typedef union {                     /* Input selection and timing */
      unsigned short int full;
      struct {
         unsigned int unused       : 1;  /*  Highest bit is here */
         unsigned int rantrigrt    : 3;  /* (r/w) random trigger rate setting */
         unsigned int bcdelay      : 4;  /* (r) read BC delay switch value */
         unsigned int vme_trpend   : 1;  /* (r) VME transfer pending if 1 */
         unsigned int l1_fiforst   : 1;  /* (w) L1A FIFO reset if set to 1 */
         unsigned int l1_fifoempty : 1;  /* (r) L1A FIFO empty if 1 */
         unsigned int l1_fifofull  : 1;  /* (r) L1A FIFO full if 1 */
         unsigned int orb_sig_sel  : 1;  /* (r/w) external orbit if 0 */
         unsigned int l1_trig_sel  : 3;  /* (r/w) VME if 4, random if 5,
                                                  L1A<n> for n = 0-3. */
      } bits;
   } ttcvi_csr1;

typedef union {                     /* FIFO status */
      unsigned short int full;
      struct {
         unsigned int rst_bgo_ff3  : 1;  /* (w) if 1. Highest bit is here */
         unsigned int rst_bgo_ff2  : 1;  /* (w) Reset B-Go FIFO 2 if 1 */
         unsigned int rst_bgo_ff1  : 1;  /* (w) Reset B-Go FIFO 1 if 1 */
         unsigned int rst_bgo_ff0  : 1;  /* (w) Reset B-Go FIFO 0 if 1 */
         unsigned int rtx_bgo_ff3  : 1;  /* (r/w) Retransmit B-Go FIFO   */
         unsigned int rtx_bgo_ff2  : 1;  /* (r/w) if set to 0 when EMPTY */
         unsigned int rtx_bgo_ff1  : 1;  /* (r/w)                        */
         unsigned int rtx_bgo_ff0  : 1;  /* (r/w)                        */
         unsigned int bgo_ff3_full : 1;  /* (r) B-Go FIFO 3 FULL if 1  */
         unsigned int bgo_ff3_mt   : 1;  /* (r) B-Go FIFO 3 EMPTY if 1 */
         unsigned int bgo_ff2_full : 1;  /* (r) B-Go FIFO 2 FULL if 1  */
         unsigned int bgo_ff2_mt   : 1;  /* (r) B-Go FIFO 2 EMPTY if 1 */
         unsigned int bgo_ff1_full : 1;  /* (r) B-Go FIFO 1 FULL if 1  */
         unsigned int bgo_ff1_mt   : 1;  /* (r) B-Go FIFO 1 EMPTY if 1 */
         unsigned int bgo_ff0_full : 1;  /* (r) B-Go FIFO 0 FULL if 1  */
         unsigned int bgo_ff0_mt   : 1;  /* (r) B-Go FIFO 0 EMPTY if 1 */
      } bits;
   } ttcvi_csr2;

/* Long format asynchronous cycles: */

typedef union {                    
      unsigned short int full;
      struct {                    /* Most significant word */
         unsigned int topbit      :  1;  /* Top bit...always set to one */
         unsigned int ttcrx_addr  : 14;  /* 14 bit TTCRX address */
         unsigned int access_bit  :  1;  /* 0: internal, 1: external access */
      } bits;
   } ttcvi_longvme_msw;

typedef union {                  /*Least significant word */
      unsigned short int full;
      struct {
         unsigned int subaddr    : 8;  /* 8 bit subaddress  */
         unsigned int data       : 8;  /* 8 bit data */
      } bits;
   } ttcvi_longvme_lsw;

typedef union {                        /* Inhibit registers (r/w) */
      unsigned short int full;
      struct {
         unsigned int unused    :  4;  /* Most significant bits are here */
         unsigned int delay     :  4;  /* Bit number 0 is here */
         unsigned int duration  :  8;  /* Bit number 0 is here */
      } bits;
   } ttcvi_inhibit;

typedef  union {                     /* B-Go mode selection (r/w) */
      unsigned short int full;
      struct {
         unsigned int unused  : 12;  /* Most significant bits are here */
         unsigned int fifo    :  1;  /* 0 Start when FIFO not empty, 1 ignore */
         unsigned int single  :  1;  /* 0 single, 1 repetitive */
         unsigned int sync    :  1;  /* 0 synchronous, 1 asynchronous */
         unsigned int enable  :  1;  /* 0 front panel input enable, 1 disable */
      } bits;
   } ttcvi_bgo_mode;
   
typedef union {                     /* B-Go channel data (w) long format */
      unsigned long int full;
      struct {
         unsigned int topbit    :  1; /* Always set to 1 */
         unsigned int ttcrxadd  : 14; /* 14 bit TTrx address */
         unsigned int access    :  1; /* 0 internal, 1 external */
         unsigned int subadd    :  8; /* 8 bit subaddress */
         unsigned int data      :  8; /* 8 bit data */
      } bits;
   } ttcvi_bgo_data_long;

typedef union {                      /* B-Go channel data (w) short format */
      unsigned long int full;
      struct {
         unsigned int topbit    :  1;  /* Always set to 0 */
         unsigned int command   :  8;  /* 8 bit command */
         unsigned int unused    : 23;  /*  */
      } bits;
   } ttcvi_bgo_data_short;



/* ttcvi routine specifications */                      
void ttcvi_setup_bus_error_handler(void); 
void ttcvi_remove_bus_error_handler(void);
void setup_ttcvi_mapping(void);
long *ttcvi_get_first_address(void);
int ttcvi_present(void);
void ttcvi_init_l1a(void);
void ttcvi_view_csr2(void);
void rrcvi_set_samples(void);
void ttcvi_set_pipelen(void);
void ttcvi_set_testwd(void);
void ttcvi_set_dig_runmd();
void ttcvi_short_vme(void);
void ttcvi_send_l1a(void);
void ttcvi_change_add(void);
void ttcvi_signal_bus_error(void);
void ttcvi_init_register(void);
void init_digitizer(void);
void ttcvi_test_word(int wd1, int wd2, int wd3);
void reset_digitizer(void);

#endif










