#include <stdio.h>

void wipe_screen(void) {
/* Erase the screen and place the cursor at the top left hand corner */
  (void)printf("\x1B[0H\x1B[2J");
  (void)fflush (stdout);
}

void home_cursor(void) {
/* Places the cursor at the top left hand corner of the screen */
  (void)printf("\x1B[0H");
  (void)fflush (stdout);
}

