/******************************************************************************
* file: <menu.h>            created: 11-July-1991             by: Fido Dittus *
*                                                                             *
* Revision history:                                                           *
* ----------------                                                            *
* D. Frei      91/08/20    Added value types 'string_type' and 'text_type'.   *
* D. Frei      91/08/28    Added value type 'float_type'.                     *
* L. Tremblet  91/12/02    Merge menu.h and term_control.h                    *
* F. Pennerath 96/10/23    Adds in item_data optional fields                  *
*                                                                             *
******************************************************************************/

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif

#include "term_control.h"

/* typedef's for menu & menu_item struct's: */

   typedef enum value_type
   {   integer_type,
       unsigned_type,
       enum_type,
       toggle_type,
       string_type,
       text_type,
       float_type
   }valtyp_t;


#define VALTXT_ARRAY_SIZE 32
#define VALTXT_SIZE       32
   typedef struct item_data
   {   valtyp_t  type;
       long      value;
       float     fvalue;
       long      limits[2];
       long      optional[5];
       char      valtxt[VALTXT_ARRAY_SIZE][VALTXT_SIZE];
   }item_data_t;


#define ITEM_TEXT_SIZE 40
typedef struct menu_item * menu_item_p;
typedef struct menu      * menu_ptr;

   typedef struct menu_item
   {   struct menu_item   *prev;
       struct menu_item   *next;
       char               key;
       char               text[ITEM_TEXT_SIZE];
       int                valid;
       int               (*action)(menu_ptr, menu_item_p);
       item_data_t        *data_p;
   }menu_item_t;


#define MENU_TITLE_SIZE (ITEM_TEXT_SIZE + VALTXT_SIZE)
   typedef struct menu  {
#ifdef ZAPPED
       struct menu        *prev;
       struct menu        *next;
#endif
       char               title[MENU_TITLE_SIZE];
       struct menu_item   *first_item_ptr;
   }menu_t;

/* Function prototypes: */

#define menu_init term_init

/* Define a new menu               */
/* Returns address of new menu     */
extern menu_ptr menu_define(char *title);

/* Create & add an item to a menu  */
extern menu_item_p menu_add_item(menu_ptr menu_p, char key, char *text,
  int valid, int (*action)(menu_ptr, menu_item_p), int data);

/* Display a menu on the screen    */
extern void menu_display(menu_ptr menu_p);

/* Find menu item for a given key  */
/* Returns a pointer to the item   */
extern menu_item_p menu_find_item(char key, menu_ptr menu_p);

/* Act on selection made by user   */
/* Returns 0 to exit expose_menu   */
extern int menu_action(char user_key, menu_ptr menu_p);

/* Expose a menu on the screen     */
extern void menu_expose(menu_ptr menu_p);
