/* Integrator Readout Interface Include File 	*/
/* 18 March 1998		                */
/* designed for: VIPC616 in A24			*/
/*	   with: 2 TIP816-10 CAN Controllers	*/
/* OS9                    			*/
/* Georges Blanchot       			*/

/* Basic VME access */

#define VME_ERROR -1
#define VME_SUCCESS 0
#define MAX_PAG 32
#define MAP_VME_BASE 0x50002C00

/* VIC Branch VME mapping routines */

int Get_Free_Page(offset);
int VME_Map( PhysAddress, offset);


short vme_send8(adress, data);
short vme_read8(adress, data);

/* INITIALIZE THE CAN BUS			*/
/* CAN_base = Carrier base + IP offset base 	*/
/* rbtr0    = BTR0 register contents	    	*/
/* rbtr1    = BTR1 register contents        	*/
/* rgm0     = GMstd global mask part I 	    	*/
/* rgm1     = GMstd+0x01 global mask part I	*/
/* rcbr     = BCR register contents	   	*/

short ADC_CAN_Init(CAN_base, rbtr0, rbtr1, rgm0, rgm1, rbcr);

/* RESET ALL THE MESSAGE BUFFERS		*/

short ADC_CAN_Rstbuf(CAN_base);

/* CONFIGURE A MESSAGE BUFFER			*/
/* index = message numebr ( 1 to 15 )		*/
/* id    = 11 bits identifier			*/
/* dlc   = 4 bits data length code ( 0 to 8 )	*/
/* dir   = 0 (tx) or 1 (rx)			*/ 

short ADC_CAN_Setbuf(CAN_base, index, id, dlc, dir);

/* SEND A MESSAGE */
/* index = message number ( 1 to 15 )		*/
/* data  = pointer to an array of 8 chars	*/

short ADC_CAN_send(CAN_base, index, data);

/* READ A MESSAGE */
/* index = message number ( 1 to 15 )		*/
/* data  = pointer to an array of 8 chars	*/

short ADC_CAN_read(CAN_base, index, data);

/* TEST THE RXOK FLAG */
/* IF RXOK = 0 RETURNS 0 */
/* IF RXOK = 1 RETURNS 1 */

short ADC_CAN_GetRX(CAN_base);

/* RESET THE RXOK FLAG */

short ADC_CAN_RstRX(CAN_base);

/* POLL NEWDATA BIT */

short ADC_CAN_GetNewDat(CAN_base, index);

/* ADC LOW LEVEL COMMAND */
/* SET PARAMETERS        */

short ADC_Set(CAN_base, ADC_base, param, value1, value2, value3);

/* GET PARAMETERS        */

short ADC_Get(CAN_base, ADC_base, param, param2, value1, value2, value3);

/* SET CARD POS */

short ADC_Cardpos(CAN_base, ADC_base, Pos, Card);

/* CARD SELECT */

short ADC_Cardsel(CAN_base, ADC_base, Pos);

/* DAC SETTING */

short ADC_DACSet(CAN_base, ADC_base, dacvalue);

/* TRIGGER RATE SETTING */

short ADC_TrigSet(CAN_base, ADC_base, preset);

/* NUMBER OF PMTS IN THE LIST */

short ADC_npmt(CAN_base, ADC_base, npmt);

/* MAX NUMBER OF SCANS */

short ADC_Maxscan(CAN_base, ADC_base, maxscans);

/* START AUTO SCAN */

short ADC_Start(CAN_base, ADC_base);

/* STOP AUTO SCAN */

short ADC_Stop(CAN_base, ADC_base);

/* SINGLE CONVERTION */

short ADC_Convert(CAN_base, ADC_base, card);

/* EXTERNAL TRIGGER */

short Ext_Trigger(CAN_base, ADC_base);

