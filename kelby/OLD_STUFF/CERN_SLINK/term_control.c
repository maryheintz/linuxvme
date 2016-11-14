/*****************************************************************************
* file: <term_control.c>      created: 01-July-1991          by: Fido Dittus *
*                                                                            *
* These routines were taken from the example in "Using the Termcap Library"  *
* in the OS9 C-COMPILER USER'S MANUAL                                        *
*                                                                            *
******************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#if OSK
#include <sgstat.h>
#include <termcap.h>
#endif

#ifdef __LYNX
int tgetent(char *, char *);
int tgetnum(char *);
char *tgetstr(char *, char **);
int tputs(char *, int, int(*)(char));
#endif /* LYNXOS */

#include "term_control.h"

#define TCAPSLEN 4096

char tcapbuf[TCAPSLEN];            /* buffer for extracted termcap strings   */
                                   /* must remain intact for entire program  */

char tcbuf[8*TCAPSLEN];            /* buffer for tgetent, must remain intact */
                                   /* for all tgetstr, tgetflag, and tgetnum */

extern char *getenv(const char *);


/* define capabilities declared external in <termcap.h> */

   char PC_;                                   /* pad character              */
   char *BC;                                   /* backspace character string */
   char *UP;                                   /* up cursor                  */
   short ospeed;                               /* terminal speed             */


/* define capabilities declared external in "term_control.h" */

   char *CL_tcap,                              /* clear screen               */
        *CM_tcap,                              /* cursor motion              */
        *CE_tcap,                              /* clear end-of-line          */
        *CD_tcap,                              /* clear end-of-display       */
        *UP_tcap,                              /* up cursor                  */
        *SO_tcap,                              /* standout begin             */
        *SE_tcap,                              /* standout end               */
        *HO_tcap,                              /* cursor home                */
        *MD_tcap,                              /* highlight start            */
        *MB_tcap,                              /* blink start                */
        *US_tcap,                              /* underscore start           */
        *UE_tcap;                              /* underscore end             */

   short
        screen_lines,                          /* lines on screen            */
        screen_colms;                          /* columns on screen          */

/*===========================================================================*/
/*===========================================================================*/
   int tputc(char c)                      /* function to write one character */
/*===========================================================================*/
{
   return write(1, &c, 1);
}
/*===========================================================================*/
/*===========================================================================*/
   void term_cntrl(str)       /* function to write a terminal control string */
   char *str;
/*===========================================================================*/
{
   tputs(str, 1, tputc);
}
/*===========================================================================*/
/*===========================================================================*/
   void term_init() /* INITIALIZATION mandatory before any call to term_cntrl*/
/*===========================================================================*/
{
   register char *term_type,
                 *temp;
   auto char *ptr;

   if ((term_type = getenv("TERM")) == NULL) {
       fprintf(stderr, "Warning: No Terminal Type defined\n");
       strcpy(term_type, "dumb");
   }

   if (tgetent(tcbuf, term_type) <= 0) {
       fprintf(stderr, "Warning: Unknown Terminal Type: '%s'\n", term_type);
       strcpy(term_type, "dumb");
   }

   ptr = tcapbuf;

/* get stuff we are interested in */

   if ( (temp = (char *)tgetstr("PC", &ptr)) ) PC_ = *temp;
   HO_tcap = (char *)tgetstr("ho", &ptr);
   if(HO_tcap == NULL)
      fprintf(stderr, "Warning: no HO capability\n");
   CE_tcap = (char *)tgetstr("ce", &ptr);
   if(CE_tcap == NULL)
      fprintf(stderr, "Warning: no CE capability\n");
   CL_tcap = (char *)tgetstr("cl", &ptr);
   if(CL_tcap == NULL)
      fprintf(stderr, "Warning: no CL capability\n");
   CM_tcap = (char *)tgetstr("cm", &ptr);
   if(CM_tcap == NULL)
      fprintf(stderr, "Warning: no CM capability\n");
   CD_tcap = (char *)tgetstr("cd", &ptr);
   if(CD_tcap == NULL)
      fprintf(stderr, "Warning: no CD capability\n");
   UP_tcap = UP = (char *)tgetstr("up", &ptr);
   if(UP_tcap == NULL)
      fprintf(stderr, "Warning: no UP capability\n");
   SO_tcap = (char *)tgetstr("so", &ptr);
   if(SO_tcap == NULL)
      fprintf(stderr, "Warning: no SO capability\n");
   SE_tcap = (char *)tgetstr("se", &ptr);
   if(SE_tcap == NULL)
      fprintf(stderr, "Warning: no SE capability\n");
   MD_tcap = (char *)tgetstr("md", &ptr);
   if(MD_tcap == NULL)
      fprintf(stderr, "Warning: no MD capability\n");
   US_tcap = (char *)tgetstr("us", &ptr);
   if(US_tcap == NULL)
      fprintf(stderr, "Warning: no US capability\n");
   UE_tcap = (char *)tgetstr("ue", &ptr);
   if(UE_tcap == NULL)
      fprintf(stderr, "Warning: no UE capability\n");

   screen_lines = tgetnum("li");
   screen_colms = tgetnum("co");

#if ZAPPED
   if (!(HO_tcap && CE_tcap && CL_tcap && CM_tcap && CD_tcap && UP_tcap &&
	 SO_tcap && SE_tcap && MD_tcap && MB_tcap && US_tcap && UE_tcap) ) {
       fprintf(stderr, "Incomplete termcap entry\n");
   }
#endif
}
/*===========================================================================*/

#ifdef TEST
main()
	{
	term_init();
}
#endif
