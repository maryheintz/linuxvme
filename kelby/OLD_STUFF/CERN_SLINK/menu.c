/******************************************************************************
* file: <menu.c>            created: 11-July-1991             by: Fido Dittus *
*                                                                             *
* Revision history:                                                           *
* ----------------                                                            *
* Dani Frei    91/08/20    Added value types 'string_type' and 'text_type'    *
* Dani Frei    91/08/28    Added value type 'float_type'                      *
*                          and fixed bug for 'text_type'.                     *
* L. Tremblet  91/11/14    Fixed text overflow in 'menu_add_item              *
*                                                                             *
******************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "menu.h"

#define NL '\n'
#if OSK
#define LF '\l'
#endif

static char keyboard_string[256];

/*===========================================================================*/
/*===========================================================================*/
   static char input_char(void)               /* Read one line from terminal */
/*===========================================================================*/
{
   int k;

   (void) fflush(stdout);
#if OSK
   k = readln(0,keyboard_string,256);
#else
   k = read(0,keyboard_string,256);
#endif
   return ((k > 0) ? keyboard_string[0] : NL);
}

/*===========================================================================*/
/*===========================================================================*/
   static int copy_line(                    /* Copy string until NL        */
   char *d, char *s, int n)                 /*   maximal n characters        */
/*===========================================================================*/
{
   int i;

   if (n <= 0 ) return 0;
   for (i=0; --n>0 && *s!='\0' && *s!=NL; i++) *d++ = *s++;
   *d = '\0';
   return i+1;
}

/*===========================================================================*/
/*===========================================================================*/
   menu_ptr menu_define(char *title)                    /* Define a new menu */
/*===========================================================================*/
{
   register menu_ptr menu_p;
   register char *blank_title = 
     "                                                                       ";

/* Allocate memory for the menu to create */

   menu_p = (menu_ptr)malloc(sizeof(menu_t));

/* Initially, the new menu is not linked to any other menu, and has no items */

#if ZAPPED
   menu_p->next           = NULL;
   menu_p->prev           = NULL;
#endif
   menu_p->first_item_ptr = NULL;

/* Copy the title string into the new menu_t struct */

   strncpy(menu_p->title, title, MENU_TITLE_SIZE);
   strcat(menu_p->title, blank_title+strlen(title));      /* pad with blanks */

   return menu_p;
}
/*===========================================================================*/
/*===========================================================================*/
   menu_item_p menu_add_item(menu_ptr menu_p, char key, char *text,  /* Add */
   int valid, int (*action)(menu_ptr, menu_item_p), int data)        /* item*/
/*===========================================================================*/
/*
 *   data != 0 means allocate space for item-data 
 *   data == 0 means create an item without data  
 */
{
   register menu_item_p item_p, last_item_p;
   register char *vt_p;
   register char *pad;
   register char *blank_text =  "                                       ";
   register char *dotted_text = ".......................................";

/* Find the currently last item in *menu_p */

   last_item_p = NULL;
   for (item_p=menu_p->first_item_ptr; item_p!=NULL; item_p=item_p->next)
      last_item_p = item_p;

/* Allocate memory for the item to create */

   item_p = (menu_item_p)malloc(sizeof(menu_item_t));
   if (data)
   {  pad = dotted_text;
      item_p->data_p = (item_data_t *)malloc(sizeof(item_data_t));
      item_p->data_p->type = integer_type;
      item_p->data_p->value = 0;
      item_p->data_p->limits[0] = 0;
      item_p->data_p->limits[1] = 0x7FFFFFFF;
      for (vt_p = (char *)(item_p->data_p->valtxt); 
           vt_p < (char *)(item_p->data_p->valtxt)
                         + VALTXT_ARRAY_SIZE*VALTXT_SIZE;
          *vt_p = ' ', vt_p++);
   }else
   {  pad = blank_text;
      item_p->data_p = NULL;
   }

/* Link the new item into the linked list */

   if (last_item_p != NULL)
   {   last_item_p->next = item_p;
       item_p->prev = last_item_p;
   }else
   {   menu_p->first_item_ptr = item_p;
       item_p->prev = NULL;
   }
   item_p->next = NULL;

/* Fill the new menu_item_t struct with data supplied via parameters */

   item_p->key = key;                             /* copy item key           */

   strncpy(item_p->text, text, ITEM_TEXT_SIZE);   /* copy item text          */
   if (strlen(text) < ITEM_TEXT_SIZE)
      strcat(item_p->text, pad+strlen(text));     /* add correct padding     */

   item_p->valid = valid;                         /* initial of on/off switch*/
   item_p->action = action;                       /* ptr to action routine   */

   return item_p;
}
/*===========================================================================*/
/*===========================================================================*/
 void item_get_value(menu_item_p item_p) /* Get new item value from keyboard */
/*===========================================================================*/
{
   register char key;
   register long i, user_value;
   float user_fvalue;
   int n, max;
   char *s;


   if (!item_p->valid)  return;    /* How come? item is not currently valid  */
   if (!item_p->data_p) return;    /* How come? no data associated with item */

   while(TRUE)                 /* insist that the user chooses a legal value */
   {
      switch (item_p->data_p->type) {
      case integer_type :                           /* data is integer_type  */
         printf("\n %s [%d] ? ",item_p->text,       /* prompt with item text */
                        (int)item_p->data_p->value);/* and current value     */
         switch (key=input_char()) {                /* key on first char     */    
						  /* the user accepts      */
	 case NL :                                /* the current value     */
#if OSK
	 case LF :
#endif
            return;
         case '?'  :                                /*          HELP!        */
            printf(" Enter a number in the range [%d ... %d]\n",
              (int)item_p->data_p->limits[0], (int)item_p->data_p->limits[1]);
            break;
         default   :                                /* see if legal value    */
            user_value = atol(keyboard_string);     /* convert input to long */
            if (user_value < item_p->data_p->limits[0]            /* outside */
             || user_value > item_p->data_p->limits[1]            /* limits! */
             || isalpha(key) )
               printf(" Value not within [%d ... %d]! - try again\n",
               (int)item_p->data_p->limits[0], (int)item_p->data_p->limits[1]);
            else
            {  item_p->data_p->value = user_value;                /* O.K. !! */
               return;
            }
            break;
         }                                /* end 'switch (key=input_char())' */
         break;                           /* end 'data is integer_type'      */

      case unsigned_type :                          /* data is unsigned_type */
         printf("\n %s [%d] ? ",item_p->text,       /* prompt with item text */
                        (int)item_p->data_p->value);/* and current value     */
         switch (key=input_char()) {                /* key on first char     */    
						  /* the user accepts      */
	 case NL :                                /* the current value     */
#if OSK
	 case LF :
#endif
            return;
         case '?'  :                                /*          HELP!        */
            printf(" Enter a number in the range [%u ... %u]\n",
                   (unsigned int)item_p->data_p->limits[0],
		   (unsigned int)item_p->data_p->limits[1]);
            break;
         default   :                                /* see if legal value    */
            user_value = atol(keyboard_string);     /* convert input to long */
            if ( (unsigned long)user_value <
		 (unsigned long)item_p->data_p->limits[0]         /* outside */
             || (unsigned long)user_value >
		(unsigned long)item_p->data_p->limits[1]          /* limits! */
             || isalpha(key) )
               printf(" Value not within [%u ... %u]! - try again\n",
                      (unsigned int) item_p->data_p->limits[0],
		      (unsigned int) item_p->data_p->limits[1]);
            else
            {  item_p->data_p->value = user_value;                /* O.K. !! */
               return;
            }
            break;
         }                                /* end 'switch (key=input_char())' */
         break;                           /* end 'data is integer_type'      */

      case enum_type :                              /* data is enumeration   */
         printf("\n  Valid choices are:\n");        /* print legal choices   */
         for (i=0; i<item_p->data_p->limits[0]; i++)
            printf("   %d   %s \n", (int)i, item_p->data_p->valtxt[i]);
         printf(" %s [%s] ? ", item_p->text,        /* prompt with item text */
            item_p->data_p->valtxt[item_p->data_p->value]); /* and current v.*/
         switch (key=input_char()) {                /* key on first char     */    
						  /* the user accepts      */
	 case NL :                                /* the current value     */
#if OSK
	 case LF :
#endif
            return;
         case '?'  :                                /*          HELP!        */
            break;                                  /* print choices again   */
         default   :                                /* see if legal value    */
            user_value = atol(keyboard_string);     /* convert input to long */
            if (user_value <  0 || isalpha(key)                   /* outside */
             || user_value >= item_p->data_p->limits[0])          /* limits! */
               printf(" Invalid choice! - try again\n");
            else                                                  /* O.K. !! */
            {  item_p->data_p->value = user_value;
               return;
            }
            break;
         }                                /* end 'switch (key=input_char())' */
         break;                           /* end data is enumeration         */
      case toggle_type :                  /* data is toggle type             */
         item_p->data_p->value = item_p->data_p->value ? 0 : 1;
         return;                          /* no input needed, just "toggle"  */
         break;                           /* end data is toggle type         */
      case string_type :                  /* data is string type             */
         max = item_p->data_p->value;
         if (max <= 0) max = 32;
         if (max > 256) max = 256;
         printf("\n %s [%s]\n? ",item_p->text,
                item_p->data_p->valtxt[0]);
         switch (key=input_char()) {                /* key on first char     */    
						  /* the user accepts      */
	 case NL :                                /* the current value     */
#if OSK
	 case LF :
#endif
            return;
         case '?'  :                                /*          HELP!        */
            printf("Enter one line of text (up to %d characters)\n", max-1);
            break;
         default   :                                /* see if legal value    */
            copy_line(item_p->data_p->valtxt[0],
                      keyboard_string, max);
            return;
            break;
         }                                /* end 'switch (key=input_char())' */
         break;                           /* end data is string type         */
      case text_type :                    /* data is text type               */
         printf("\n %s :\n[%s]\n?\n",item_p->text,
                item_p->data_p->valtxt[0]);
         switch (key=input_char()) {                /* key on first char     */    
						  /* the user accepts      */
	 case NL :                                /* the current value     */
#if OSK
	 case LF :
#endif
            return;
         case '?'  :                                /*          HELP!        */
            printf("Enter one or more lines of text ");
            printf("(<RETURN> on a new line ends input)\n");
            break;
         default   :                                /* see if legal value    */
            s = item_p->data_p->valtxt[0];
            max = VALTXT_ARRAY_SIZE * VALTXT_SIZE;
            n = copy_line(s, keyboard_string, max);
            max -= n;
	    while (max > 0 && input_char() != NL)
            {
	       s[n-1] = NL;
               s += n;
               n = copy_line(s, keyboard_string, max);
               max -= n;
            }
            return;
            break;
         }                                /* end 'switch (key=input_char())' */
         break;                           /* end data is text type           */
      case float_type :                             /* data is float_type    */
         printf("\n %s [%g] ? ",item_p->text,       /* prompt with item text */
                          item_p->data_p->fvalue);  /* and current value     */
         switch (key=input_char()) {                /* key on first char     */    
						  /* the user accepts      */
	 case NL :                                /* the current value     */
#if OSK
	 case LF :
#endif
            return;
         case '?'  :                                /*          HELP!        */
            printf(" Enter a real number\n");
            break;
         default   :                                /* see if legal value    */
            if (sscanf(keyboard_string, "%f", &user_fvalue) != 1)
               printf(" Not a real number\n");
            else
            {  item_p->data_p->fvalue = user_fvalue;               /* O.K. !! */
               return;
            }
            break;
         }                                /* end 'switch (key=input_char())' */
         break;                           /* end 'data is float_type'        */
      }                                   /* end 'switch(item-data-type)'    */
   }                                      /* end 'while(TRUE)'               */
}
/*===========================================================================*/
/*===========================================================================*/
   void item_display(menu_item_p item_p)    /* Display an item on the screen */
/*===========================================================================*/
{
      int n;
      char txt[VALTXT_SIZE];

      if (!item_p->valid) return;

      if (!item_p->data_p) printf(" %c   %s                                 \n",
                                  item_p->key, item_p->text);
      else
      switch (item_p->data_p->type)
      {  case integer_type :
           printf(" %c   %s", item_p->key, item_p->text);
           printf(" : %-30d\n", (int)item_p->data_p->value);
           break;
         case unsigned_type :
           printf(" %c   %s", item_p->key, item_p->text);
           printf(" : %-30u\n", (int)item_p->data_p->value);
           break;
         case enum_type   :
         case toggle_type :
           printf(" %c   %s", item_p->key, item_p->text);
           printf(" : %-30.30s\n", 
                  item_p->data_p->valtxt[item_p->data_p->value]);
           break;
         case string_type :
           printf(" %c   %s", item_p->key, item_p->text);
           printf(" : %-30.30s\n", 
                  item_p->data_p->valtxt[0]);
           break;
         case text_type :
           printf(" %c   %s", item_p->key, item_p->text);
           n = copy_line(txt, item_p->data_p->valtxt[0], VALTXT_SIZE) - 1;
           if (item_p->data_p->valtxt[0][n] != '\0')
              strncpy(txt + n, " ...", VALTXT_SIZE - n);
           printf(" : %-30.30s\n", txt);
           break;
         case float_type :
           printf(" %c   %s", item_p->key, item_p->text);
           printf(" : %-30g\n", item_p->data_p->fvalue);
           break;
         default :
           printf(" %c   %s", item_p->key, item_p->text);
           printf("*** illegal type!\n");
           break;
      }
}
/*===========================================================================*/
/*===========================================================================*/
   void menu_display(menu_ptr menu_p)        /* Display a menu on the screen */
/*===========================================================================*/
{
   register menu_item_t *item_p;


   term_cntrl(CL_tcap);                          /* Clear screen */
   term_cntrl(HO_tcap);                          /* Cursor home  */
   term_cntrl(SO_tcap);                          /* Standout     */
   term_cntrl(US_tcap);                          /* Underscore   */
   term_cntrl(MD_tcap);                          /* Highlight    */
   printf("\n %s     \n", menu_p->title);
   term_cntrl(SE_tcap);                          /* Normal       */

   term_cntrl(SO_tcap);                          /* Standout     */
   for (item_p=menu_p->first_item_ptr; item_p!=NULL; item_p=item_p->next)
      item_display(item_p);
   term_cntrl(SE_tcap);                          /* Normal       */
}
/*===========================================================================*/
/*===========================================================================*/
   menu_item_p menu_find_item(char key,     /* Find menu item for a given key*/
               menu_ptr menu_p)             /* Returns a pointer to the item */
/*===========================================================================*/
{
   register menu_item_p item_p;
   register char K, k;


/* Make K (k) be the uppercase (lowercase) letter selected by the user */

   if (isupper(K=k=key)) k = tolower(k);
   else                  K = toupper(K);

/* Find the item corresponding to the selected key */

   for (item_p=menu_p->first_item_ptr; item_p!=NULL; item_p=item_p->next)
      if (K==item_p->key || k==item_p->key) return item_p;

   return NULL;
}
/*===========================================================================*/
/*===========================================================================*/
int menu_action(char user_key, menu_ptr menu_p)    /* Act on users selection */
                                            /* Returns 0 to exit expose_menu */
/*===========================================================================*/
{
   register menu_item_p item_p;

   if (user_key == NL) return 1;        /* Cariage Return is a NOP */
#if OSK
   if (user_key == LF) return 1;        /* Line Feed is a NOP      */
#endif

/* Find the item corresponding to the selected key, and take action! */

   if (!(item_p=menu_find_item(user_key, menu_p)) || !item_p->valid)
   {   printf("\nInvalid selection: \"%c\"\n", user_key);
       sleep(1);
       return 1;
   }

   if (item_p->data_p) item_get_value(item_p);

   if (item_p->action) return (*item_p->action)(menu_p, item_p);

   return 1;
}
/*===========================================================================*/
/*===========================================================================*/
   void menu_expose(menu_ptr menu_p)          /* Expose a menu on the screen */
/*===========================================================================*/
{
   do {  menu_display(menu_p);              /* Display the menu              */
         term_cntrl(MD_tcap);               /* Highlight                     */
         printf("\nMake a selection> ");    /* Prompt user to select an item */
	 (void)fflush(stdout);              /* ... since we had no NL above*/
         term_cntrl(SE_tcap);               /* Normal                        */
      }while(menu_action(input_char(), menu_p)); /*_action returns 0 to quit */
}
/*===========================================================================*/
