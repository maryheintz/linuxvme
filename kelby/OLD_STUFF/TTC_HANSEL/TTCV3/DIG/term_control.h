/*****************************************************************************
* file: <term_control.h>       created: 01-July-1991         by: Fido Dittus *
******************************************************************************/
#ifndef _term_control_h_
#define _term_control_h_

/*
 *  Define pointers to the escape sequences associated with certain terminal
 *  capabilities:
 */
    extern char *CL_tcap,                            /* clear screen         */
                *CM_tcap,                            /* cursor motion        */
                *CE_tcap,                            /* clear end-of-line    */
                *CD_tcap,                            /* clear end-of-display */
                *UP_tcap,                            /* up cursor            */
                *SO_tcap,                            /* standout begin       */
                *SE_tcap,                            /* standout end         */
                *HO_tcap,                            /* cursor home          */
                *MD_tcap,                            /* highlight start      */
                *MB_tcap,                            /* blink start          */
                *US_tcap,                            /* underscore start     */
                *UE_tcap;                            /* underscore end       */
/* NOTE:
 * On VT100-style terminals ue = se, and both of these also terminate md & mb.
 */

/*
 *  Number of lines/columns on screen:
 */
    extern short
                screen_lines,                           /* lines on screen   */
                screen_colms;                           /* columns on screen */

/*
 *  Function-prototypes for user-callable routines:
 */
extern void term_init(void); /* INIT: mandatory before any call to term_cntrl*/

extern void term_cntrl(char *);  /* function to write an escape sequence    */

#endif /* _term_control_h_ */
