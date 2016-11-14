#ifndef TB_SER_H_
#define TB_SER_H_
#define L1a_by_x1 (0x4029)
#define digitizer_clock (0x68)
#define com_open (1);
#define com_close (2);
#define com_map_cards  (3);
#define com_ack_data   (4);
#define com_set_db_delay      (5);
#define com_set_readout_delay   (6);
#define com_set_mb_timer        (7);
#define com_reset_testbench     (8);
#define com_get_event       (9);
#define small_capacitor     (0);
#define big_capacitor       (1);


typedef struct {
	int addr;
	int coarse_delay;
	int fine_delay1;
	int fine_delay2;
	} delay_struct;
typedef struct {
	int zone;
	int sector;
	int timer1;
	int timer2;
	int timer3;
	int timer4;
	int capacitor;
	int DAC;
	int reserved;
	} card3in1_struct;
typedef struct {
	unsigned long base_address;
	unsigned short csr1;
	} ttcvi_struct;
typedef struct {
	int control_reg;
	int coarse_delay;
	int fine_delay1;
	int fine_delay2;
	delay_struct delay[8];
	} ttcrx_struct;
typedef struct {
	int address;
	int sample_num;
	int pipe_len;
	int test_w1;
	int test_w2;
	int test_w3;
	int test_w4;
	int test_w5;
	int test_w6;
	int DAC;
	int clock_set;
	int hi_limit_p1;
	int hi_limit_p2;
	int lo_limit_p1;
	int lo_limit_p2;
	int mode;
	} digitizer_struct;
typedef struct {
	int position;
	unsigned long start_word;
	unsigned long stop_word;
	int dma;
	} slink_struct;
typedef struct {
	card3in1_struct card3in1;
	ttcvi_struct ttcvi;
	ttcrx_struct ttcrx;
	digitizer_struct digitizer;
	slink_struct slink;
	} bench_struct;
typedef struct {
	int ID;
	int serviceID;
	bench_struct configure;
	int message_len;
	char message[16];
	int data_len;
	unsigned long data[160];
	} user_struct;
volatile user_struct user1, user2, def_user;
user_struct* puser;
typedef struct {
	int current_user;
	user_struct* puser;
	int user_num;
	int card_status;
	int gain;
	} testbench_status;
testbench_status test_status;

int get_event();
#endif 
