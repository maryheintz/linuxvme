// Commands and State codes to handle the ADC prototype

#define VOID	0
#define INIT	1
#define ERASE	2
#define WRITE	3
#define READ	4
#define TIMER   5
#define NPMT	6
#define MAXSCAN 7
#define PMTLIST 8
#define DACSET  9
#define DELAY   10

#define CANSET	11
#define CANGET	12
#define REQUEST 14
#define CONVERT 15
#define TRIGGER 16
#define START	17
#define STOP	18

#define RESET	50
#define ACK		51

#define GO_FB	101
#define GO_ISP	102
#define ERASE_SUCCESS 103
#define ERASE_FAILURE 104
#define WRITE_SUCCESS 105
#define WRITE_FAILURE 106

// CANbus initialization
long init(int adress, char *branchname);

// ADC communication protocol functions declarations
//		In System  Programability Functions
long adc_init(char action, char version[8]);
long adc_erase(void);
long adc_blankcheck(unsigned int *adr);
long adc_writemem(int adr, char data);
long adc_readmem(unsigned int adr, char *data);
//		Readout Interface Control Functions
long adc_rate(unsigned short timer);
long adc_npmt(char npmt);
long adc_maxscans(unsigned short maxscans);
long adc_pmtlist(char pos, char zone, char sector, char card);
long adc_dacset(char dac);
long adc_delay(unsigned short delay);
long adc_3in1set(unsigned short dat);
long adc_3in1get(unsigned short *dat);
long adc_convert(char zone, char sector, char card, unsigned short *result);
long adc_trigger(char npmt, unsigned short result[48]);
long adc_start(void);
long adc_auto(char npmt, unsigned short result[48]);
long adc_stop(void);
long adc_get_rate(unsigned short *timer);
long adc_get_npmt(char *npmt);
long adc_get_maxscans(unsigned short *maxscans);
long adc_get_pmtlist(char pos, char *zone, char *sector, char *card);
long adc_get_delay(unsigned short *delay);