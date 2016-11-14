/**********************************************/
/* CS CALIBRATION DAQ PROCESS                 */      
/* G BLANCHOT - IFAE - APRIL 1998             */
/**********************************************/

#define LOW_PRIORITY  128
#define HIGH_PRIORITY 2000

#define REG_BTR0	0x81
#define REG_BTR1	0x23
#define REG_BCR		0x4A

#include "stdio.h"
#include "signal.h"
#include "cs.h"
#include "cs_shared_memory.h"
#include "cs_clock.h"
#include "cs_cmod.h"
#include "cs_error.h"
#include "errno.h"
#include "time.h"
#include "process.h"
#include "types.h"
#include "math.h"
#include "I82527.H"

typedef struct
{
  unsigned long CAN_base;
  unsigned long ADC_base;
  unsigned int npmt;
  unsigned int pmt[48];
  unsigned int zone;
  unsigned int sector;
} drawer_struct;


/****************************/
/* STATE MACHINE PROTOTYPES */  
/****************************/

void signal_hdlr(s);      /* Signal Handler */
int (*action)();         /* Action general function */
int act_none();          /* Action specific functions */
int act_conf();
int act_enable();
int act_scan();
int act_switch();
int act_save();
int act_exit();
void declare_transitions(); /* FSM Configuration */

int select_list(mod);

/*  DAQ records version  */
# define DAQ_RECORDER_VERSION 1

/*  DAQ record types  */
# define REC_DAQ_TAG          1
# define REC_PMT_LIST         2
# define REC_DATA             11

int add_header (short id, int sizeof_block);
int add_record (short id, void* block, int sizeof_block);

/************************/ /* STATE MACHINE ARRAYS */ /************************/

int (*defined_act[N_STATES][N_INPUTS])(); /* Actions array */
unsigned long next_states[N_STATES][N_INPUTS];  /* Next States array */

/*************/
/* VARIABLES */
/*************/

unsigned int VIC_ADC=0x00000000;
unsigned int ADC1=0xC0550000;
unsigned int ADC2=0xC0340000;
unsigned int ADC_offset=0xC0;

int pid_trig;
int current_state=ST_INIT;
int input;
int max_scans;
int lgain;

FILE *hw;
FILE *pmtlist;    /* File pointer to pmt table   */
FILE *output;     /* File pointer to output data */

int pmt[100];  /* Table of pmts numbers, 2 modules       */
int adc[100];  /* Table of associated adcs, 2 modules    */
int module[100]; /* Table of associated module */
int scanlist[2][100]; /* Local combined pmt list */
short n_pmt;      /* Number of pmts in the table */
int scanlength;
int readmodule;
unsigned long branch_base[2];

data_struct *conf_data;
daq_data_struct* raw_data;
drawer_struct drawer[2];

int nbranches;
int ndrawers;


/* Header Contents */

unsigned short record_type;
unsigned short record_size;
int   record_trigger;
char  record_fname[80];
int   record_run_time;
int   record_run_date;
unsigned short   record_gain;
int   record_FIC_timer;
clock_t record_clk;

/****************/
/* MAIN ROUTINE */
/****************/

void main()

{

 int status=ERR_OK;
 int status2; 
 int pend;
 int linkstatus;
 unsigned long base; 
 int i;
 
 /**********/

 declare_transitions();             /* Configure the FSM */
 intercept(signal_hdlr); /* Installs the Signals Handler */

 conf_data=(data_struct *)cmod_link (common_data_name, sizeof (data_struct));
 raw_data=(daq_data_struct *)cmod_link (daq_data_name, sizeof (daq_data_struct));
 status=load_daq_conf(conf_data);


 conf_data->triggers=0;
 conf_data->daq_state=current_state;
 conf_data->daq_error=status;
 conf_data->module=1;
 readmodule=conf_data->module;

 /* Load the hardware profile */

 hw = fopen("hw.profile", "r");
 fscanf(hw,"CAN BRANCHES: %d\n", &nbranches);
 for (i=0; i<=nbranches-1; i++) fscanf(hw, "%x\n", &branch_base[i]);
 fclose(hw);

 /* Configure CAN branches */
 /* Bit Timings	Only	   */
 /* But takes into account that there might be 1 or 2 branches */

 for (i=0; i<=nbranches-1; i++)
 {
   vme_send8(branch_base[i], 0x01);	/* Enter initialization mode 	*/
   ADC_CAN_Rstbuf(branch_base[i]);	/* Reset the buffers		*/
   ADC_CAN_Init(branch_base[i], REG_BTR0, REG_BTR1, 0xFF, 0xE0, REG_BCR);	/* setup 500kbps */
 }


while(1) 
   {

     if((current_state!=ST_WAIT)||(conf_data->triggers>=conf_data->max_scans))
	 tsleep(0);                     /* Process waits */
     else 
     {
        setpr(getpid(), HIGH_PRIORITY);
	wait_trigger();
	if (input==0) input=IN_OFFSET+IN_TRIG;
	
     }

     /* Calculates the current action and executes it */
     
     action = defined_act[current_state][input-IN_OFFSET];
     status=action();
     
    /* Transits to the new state */

     current_state=next_states[current_state][input-IN_OFFSET];     

     conf_data->daq_state=current_state;
     conf_data->daq_error=status;

   input=0;

   }
}

/*********************************************************/
 
/*   Signals Handler                                     */
/*********************************************************/

void signal_hdlr(s)
int s;
{

  input=s;
  return;
}


/*********************************************************/
/*   Actions                                             */
/*********************************************************/


int act_none()
{
 return ERR_NOACT;
}


int act_conf()
{
 int i,j,k,flag;
 int status, status2;
 int zone, sector, card, zcard;
 int branch, adcnum, adcadr, size;

 status = ERR_OK;

 /* DATA STORAGE MEMORY ALLOCATION */
 
 conf_data->triggers=0;
 raw_data->current = 0;

 /* RETRIEVE CONFIGURATION */

  status=load_daq_conf(conf_data);
  readmodule=conf_data->module;

 /* RETRIEVE THE PMT LIST*/
 /* New format supported here */
 /* Note that for TB98 ndrawers = nbranches */

 if (status == ERR_OK)
 {
   conf_data->npmt = 0;
   pmtlist = fopen(conf_data->table, "r");
   if (pmtlist == NULL) status = ERR_OPEN;
   if (status == ERR_OK)
   {
     fscanf(pmtlist, "DRAWERS: %d\n", &ndrawers);
     conf_data->ndrawers = ndrawers; /* Number of drawers to scan */

     for (i=0; i<=ndrawers-1; i++)
     {
	fscanf(pmtlist, "BRANCH: %d\n", &branch);
	fscanf(pmtlist, "ADC: %d AT %d\n", &adcnum, &adcadr);
	fscanf(pmtlist, "SIZE: %d\n", &size);

	drawer[i].CAN_base = branch_base[branch];
	drawer[i].ADC_base = adcadr << 10;
	drawer[i].npmt = size;

  	for (j=0; j<=size-1; j++)
        {
	  status2 = fscanf(pmtlist, "%d\t%d\t%d\n", &zone, &sector, &card);
	  zcard = card + (sector<<6) + (zone<<11);
	  card_sel(zcard);
	  send4_3in1(0,0);			/* enable Intg_rd 	*/
	  send4_3in1(1,1);			/* disable itr    	*/
	  send12_3in1(2,0, conf_data->gain);	/* program the gain 	*/
	  /* program the card list */
	  ADC_Cardpos(drawer[i].CAN_base, drawer[i].ADC_base, (char)j, (char)card);
	  drawer[i].pmt[j] = card;
	}
	ADC_npmt(drawer[i].CAN_base, drawer[i].ADC_base, (char)drawer[i].npmt);
 	ADC_Maxscan(drawer[i].CAN_base, drawer[i].ADC_base, (short)conf_data->max_scans);
	ADC_DACSet(drawer[i].CAN_base, drawer[i].ADC_base, (char)50);
	ADC_TrigSet(drawer[i].CAN_base, drawer[i].ADC_base, (short)conf_data->FIC_timer);
	conf_data->npmt += size; /* increment npmt global parameter */
	for(j=2;j<14;j++) ADC_CAN_Setbuf(drawer[i].CAN_base, (char)j, drawer[i].ADC_base + j, 8, 0); 
	drawer[i].zone = zone;
	drawer[i].sector = sector;
     } 
   }
  
  fclose(pmtlist);
  if (status2 == EOF) status = ERR_EOF;

 }


 status2=fclose(pmtlist);
 if (status2!=0) status=ERR_CLOSE;

 return status;
}


int act_enable()
{
 
 short day;
 time_t t;
 int i;
 
 struct 
 {
   unsigned long abstime;
   long version;
   long nmodules;
   long max_scans;
   long gain;
   long FIC_timer;
 } daq_tag;

 install_vmeirq1(drawer[0].CAN_base, 130, 0);
 enable_canirq(drawer[0].CAN_base, (int) (ceil ((double) (drawer[0].npmt)/4) ) );		/* first branch msg 2 produces int on RCV */


 daq_tag.abstime = (unsigned long)time(&t);
 daq_tag.version = (long)DAQ_RECORDER_VERSION;
 daq_tag.nmodules = (long)conf_data->nmodules;
 daq_tag.max_scans = (long)conf_data->max_scans;
 daq_tag.gain = (long)conf_data->gain;
 daq_tag.FIC_timer = (long)conf_data->FIC_timer;

 add_record (REC_DAQ_TAG, &daq_tag, sizeof(daq_tag));

 /* scanlist is filled with length scanlength */
 /* This routine is useless in 1997 TB */
 
 /* This routine has to be upgraded by S. kopikov */
 add_record (REC_PMT_LIST, pmt, sizeof(int)*n_pmt);

 for (i=0; i<ndrawers; i++) ADC_Start(drawer[i].CAN_base, drawer[i].ADC_base);

 return ERR_OK;
}


int act_scan()
{
 int i,j,k,z;
 unsigned short result;
 int status, status2;
 short *tmp_data;
 unsigned char lresult[8];
 int nresult=0;

 status=ERR_OK;
 if ((conf_data->triggers < conf_data->max_scans) &&  add_header (REC_DATA, sizeof (short)*n_pmt))
 {

   tmp_data = (short*)&raw_data->buffer[raw_data->current+sizeof(header_struct)];

   /* Scan one or two drawers */

   for(i=0; i<=ndrawers-1; i++)
   {

	/* retrieve the messages */

	nresult = 0;
    	for (k=1;k<=(int)ceil((double)(drawer[i].npmt)/4);k++)
        {

		/* wait the New data flag */

		while (ADC_CAN_GetNewDat(drawer[i].CAN_base, k+1) == 0) ;

		/* retrieve the message data */

		ADC_CAN_read( drawer[i].CAN_base, (char)(k+1), lresult);

		/* move the retrieved data into raw data memory */
		/* until npmt reached or 8 bytes reached        */

		z=0;		
		while( ((drawer[i].npmt) > nresult) && ( z < 8 ))
		{
			*tmp_data = (short)((short)(lresult[z]*256) + (short)(lresult[z+1]));
			tmp_data++;
			z+=2;
			nresult++;
		}

		/* next data */	
	}

	/* next message */


	/* Reset the buffers */
	ADC_CAN_Rstbuf(drawer[i].CAN_base);

     }

     /* next drawer */

    *tmp_data=(short)conf_data->triggers+1;
     tmp_data++;
    raw_data->current += sizeof (header_struct) + n_pmt*sizeof (short);
    conf_data->triggers++;
    status = ERR_OK;

    /* Activates IRQ again */

    enable_canirq(drawer[0].CAN_base, (int) (ceil ((double) (drawer[0].npmt)/4) ) );	

 }
 
 else
   {
     disable_canirq();
     remove_vmeirq1();
     for(i=0;i<ndrawers;i++) ADC_Stop(drawer[i].CAN_base, drawer[i].ADC_base);
     status = ERR_FULL;
   }

 return status;

}

int act_save()
{
 unsigned int i,j;
 int status, status2;
 short type, size;
 int trigger;
 char o_file[80];
 unsigned short msb, lsb;
 unsigned int rtime;
 
 status=ERR_OK;

 disable_canirq(drawer[0].CAN_base, 2);
 remove_vmeirq1();

 setpr(getpid(), LOW_PRIORITY);
 
 /* CREATE THE OUTPUT FILE */

 if (pmtlist!=NULL)
 {

   output = fopen(conf_data->daq_o_file, "w");

   if (output==NULL) status=ERR_OPEN;

   if ((status==ERR_OK) and (raw_data->current < daq_buffer_size))
       fwrite(raw_data->buffer, 1, (size_t)raw_data->current, output); 


   status2=fclose(output);

   if (status2!=0) status=ERR_CLOSE;

  }


 return status;
}

int act_exit()
{
 int status;
/*
 remove_timer();
*/
 status=cmod_unlink(common_data_name);
 if(status==0) printf("Unlink to conf_data failed.\n");

 status=cmod_unlink(daq_data_name);
 if(status==0) printf("Unlink to raw_data failed.\n");

 exit(0);
}

int act_switch()
{
  int i=0,flag;
  int status, status2;
  status=ERR_OK;


/*  disable_triggers(); */
  readmodule=conf_data->module;

 /* RETRIEVE THE PMT LIST*/

  pmtlist = fopen(conf_data->table,"r");
  if(pmtlist==NULL) status=ERR_OPEN;
  
  if(status==ERR_OK)
  {

/*   db=fopen("debug.txt","w");
   fprintf(db,"readmodule=%d\n", readmodule);
*/
   do
   {    
     status2=fscanf(pmtlist,"%d %d %d\n", &module[i], &pmt[i], &adc[i]);
     flag=module[i];

     if ((module[i]==readmodule)||(module[i]==0))
       {
/*	 fprintf(db,"%d %d %d\n", module[i], pmt[i], adc[i]);
*/         i++; 
       }
   }
   while ((flag!=0)&&(status2!=EOF));
 /*  fprintf(db,"i=%d\n",i);
*/
  }

  n_pmt=i-1;   /* Must be always 28 for Extended Barrels */

  if (status2==EOF) status=ERR_EOF;
/*
  fflush(db);
  fclose(db);
*/ 
  conf_data->npmt=n_pmt;

 /* RETRIEVE THE MAX NUMBER OF SCANS */

 if (status==ERR_OK)
 {
  if (conf_data->max_scans<=0) status=ERR_CONV;
 }
 
 /* CONFIGURE THE 3IN1 CARDS */

 if (status==ERR_OK)
 {
  lgain=conf_data->gain;
  if ((lgain!=1)&&(lgain!=2)&&(lgain!=0)&&(lgain!=4)&&(lgain!=8)&&(lgain!=12))
	status=ERR_GAIN;
 }

 
 if (status==ERR_OK)
 {
    for (i=1;i<=n_pmt;i++)
    {

   
     
     card_sel(pmt[i-1]);      /* Select the card */
     send4_3in1(0,1);         /* Enable Integrator Readout */
     send4_3in1(1,0);         /* Disable Calibration Current */
     send12_3in1(2,0,lgain);  /* Set the gain to 0x0 = 27M5  */
     send12_3in1(6,0,i);      /* Test input */



    }

    status2=fclose(pmtlist);
    if (status2!=0) status=ERR_CLOSE;
 }

  add_record (REC_PMT_LIST, pmt, sizeof(int)*n_pmt);

  /* Configure the ZCIO Timer */
/*
  install_timer1(130,0);
  set_division(conf_data->FIC_timer); 

  enable_triggers(); */

  return 0;
}

int select_list(mod)
int mod;
{

 int i,j;
 j=1;

 for(i=1;i<=n_pmt;i++)
   {
     if(mod==module[i-1])
       {
	 scanlist[0][j-1]=pmt[i-1];
	 scanlist[1][j-1]=adc[i-1];
	 j++;
       } 
   }

  readmodule=conf_data->module;

  return j-1;
    
}


/*********************************************************/
/*   Transitions and actions assignements                */
/*********************************************************/

void declare_transitions()
{

#ifdef CSDEBUG

 printf("Starting to declare transitions.\n");

#endif

 /*** ACTIONS SET ***/

 defined_act[ST_INIT][IN_CONF]=act_conf;
 defined_act[ST_INIT][IN_RUN ]=act_none;
 defined_act[ST_INIT][IN_TRIG]=act_none;
 defined_act[ST_INIT][IN_END ]=act_none;
 defined_act[ST_INIT][IN_RST ]=act_none;
 defined_act[ST_INIT][IN_EXIT]=act_exit;
 defined_act[ST_INIT][IN_SWITCH]=act_none;

 defined_act[ST_CONF][IN_CONF]=act_conf;
 defined_act[ST_CONF][IN_RUN ]=act_enable;
 defined_act[ST_CONF][IN_TRIG]=act_none;
 defined_act[ST_CONF][IN_END ]=act_none;
 defined_act[ST_CONF][IN_RST ]=act_none;
 defined_act[ST_CONF][IN_EXIT]=act_exit;
 defined_act[ST_CONF][IN_SWITCH]=act_none;

 defined_act[ST_WAIT][IN_CONF]=act_none;
 defined_act[ST_WAIT][IN_RUN ]=act_none;
 defined_act[ST_WAIT][IN_TRIG]=act_scan;
 defined_act[ST_WAIT][IN_END ]=act_save;
 defined_act[ST_WAIT][IN_RST ]=act_none;
 defined_act[ST_WAIT][IN_EXIT]=act_exit;
 defined_act[ST_WAIT][IN_SWITCH]=act_switch;

 defined_act[ST_SAVD][IN_CONF]=act_none;
 defined_act[ST_SAVD][IN_RUN ]=act_none;
 defined_act[ST_SAVD][IN_TRIG]=act_none;
 defined_act[ST_SAVD][IN_END ]=act_none;
 defined_act[ST_SAVD][IN_RST ]=act_conf;
 defined_act[ST_SAVD][IN_EXIT]=act_exit;
 defined_act[ST_SAVD][IN_SWITCH]=act_none;

 defined_act[ST_EXIT][IN_CONF]=act_none;
 defined_act[ST_EXIT][IN_RUN ]=act_none;
 defined_act[ST_EXIT][IN_TRIG]=act_none;
 defined_act[ST_EXIT][IN_END ]=act_none;
 defined_act[ST_EXIT][IN_RST ]=act_none;
 defined_act[ST_EXIT][IN_EXIT]=act_none;
 defined_act[ST_EXIT][IN_SWITCH]=act_none;

 /*** NEXT STATES SET ***/

 next_states[ST_INIT][IN_CONF]=ST_CONF;
 next_states[ST_INIT][IN_RUN ]=ST_INIT;
 next_states[ST_INIT][IN_TRIG]=ST_INIT;
 next_states[ST_INIT][IN_END ]=ST_INIT;
 next_states[ST_INIT][IN_RST ]=ST_INIT;
 next_states[ST_INIT][IN_EXIT]=ST_EXIT;
 next_states[ST_INIT][IN_SWITCH]=ST_INIT;

 next_states[ST_CONF][IN_CONF]=ST_CONF;
 next_states[ST_CONF][IN_RUN ]=ST_WAIT;
 next_states[ST_CONF][IN_TRIG]=ST_CONF;
 next_states[ST_CONF][IN_END ]=ST_CONF;
 next_states[ST_CONF][IN_RST ]=ST_CONF;
 next_states[ST_CONF][IN_EXIT]=ST_EXIT;
 next_states[ST_CONF][IN_SWITCH]=ST_CONF;

 next_states[ST_WAIT][IN_CONF]=ST_WAIT;
 next_states[ST_WAIT][IN_RUN ]=ST_WAIT;
 next_states[ST_WAIT][IN_TRIG]=ST_WAIT;
 next_states[ST_WAIT][IN_END ]=ST_SAVD;
 next_states[ST_WAIT][IN_RST ]=ST_WAIT;
 next_states[ST_WAIT][IN_EXIT]=ST_EXIT;
 next_states[ST_WAIT][IN_SWITCH]=ST_WAIT;

 next_states[ST_SAVD][IN_CONF]=ST_SAVD;
 next_states[ST_SAVD][IN_RUN ]=ST_SAVD;
 next_states[ST_SAVD][IN_TRIG]=ST_SAVD;
 next_states[ST_SAVD][IN_END ]=ST_SAVD;
 next_states[ST_SAVD][IN_RST ]=ST_CONF;
 next_states[ST_SAVD][IN_EXIT]=ST_EXIT;
 next_states[ST_SAVD][IN_SWITCH]=ST_SAVD;

 next_states[ST_EXIT][IN_CONF]=ST_EXIT;
 next_states[ST_EXIT][IN_RUN ]=ST_EXIT;
 next_states[ST_EXIT][IN_TRIG]=ST_EXIT;
 next_states[ST_EXIT][IN_END ]=ST_EXIT;
 next_states[ST_EXIT][IN_RST ]=ST_EXIT;
 next_states[ST_EXIT][IN_EXIT]=ST_EXIT;
 next_states[ST_EXIT][IN_SWITCH]=ST_EXIT;

#ifdef CSDEBUG

 printf("Transitions table builded.\n");

#endif

 return;
}

int add_header (short id, int sizeof_block)
{
  char* dest;
  if ((raw_data->current + sizeof_block + sizeof (header_struct)) <
      daq_buffer_size)
  {
    dest = &raw_data->buffer[raw_data->current];
    ((header_struct*)dest)->id = id;
    ((header_struct*)dest)->sizeof_block = sizeof_block +
         sizeof ((header_struct*)dest->triggers) +
         sizeof ((header_struct*)dest->sys_timer);
    ((header_struct*)dest)->triggers = conf_data->triggers;
    ((header_struct*)dest)->sys_timer = clock_get ();
    return 1;
  }
  else return 0;
}

int add_record (short id, void* block, int sizeof_block)
{
  char* dest;
  if ((raw_data->current + sizeof_block + sizeof (header_struct)) <
      daq_buffer_size)
  {
    dest = &raw_data->buffer[raw_data->current];
    ((header_struct*)dest)->id = id;
    ((header_struct*)dest)->sizeof_block = sizeof_block +
         sizeof ((header_struct*)dest->triggers) +
         sizeof ((header_struct*)dest->sys_timer);
    ((header_struct*)dest)->triggers = conf_data->triggers;
    ((header_struct*)dest)->sys_timer = clock_get ();

    dest += sizeof (header_struct);
    memcpy ((void*)dest, block, sizeof_block);
    raw_data->current += sizeof (header_struct) + sizeof_block;

    return 1;
  }
  else return 0;
}
