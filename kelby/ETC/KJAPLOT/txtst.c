#include <stdio.h>
#include <math.h>
main()
{  on(1);
   off(2);
   off(3);
   on(4);
   off(5);
   blank();
   blank();
   blank();
   blank();
   printf("\n"); 
}
on(int k)
 { blank();
   bgyellow();
   red();
   printf("%d-ON",k);
   bgdefault();  /* restore default background */
   fgdefault();  /* restore default foreground */
 }
off(int k)
 { blank();
   blue(); 
   bgdefault();
   printf("%d-OFF",k);
   bgdefault();  /* restore default background */
   fgdefault();  /* restore default foreground */
 }
red()
 { putchar(27);   /*    esc    */
   putchar(91);   /*     [     */ 
   putchar(51);   /*     3     */ 
   putchar(49);   /*     1     */
   putchar(109);  /*     m     */ 
 }
 black()
 { putchar(27);   /*    esc    */
   putchar(91);   /*     [     */ 
   putchar(51);   /*     3     */ 
   putchar(57);   /*     9     */
   putchar(109);  /*     m     */ 
 }
 blue()
 { putchar(27);   /*    esc    */
   putchar(91);   /*     [     */ 
   putchar(51);   /*     3     */ 
   putchar(52);   /*     4     */
   putchar(109);  /*     m     */ 
 }
 white()
 { putchar(27);   /*    esc    */
   putchar(91);   /*     [     */ 
   putchar(57);   /*     5     */ 
   putchar(52);   /*     4     */
   putchar(109);  /*     m     */ 
 }
 bgyellow()
 { putchar(27);   /*    esc    */
   putchar(91);   /*     [     */ 
   putchar(52);   /*     4     */ 
   putchar(51);   /*     3     */
   putchar(109);  /*     m     */ 
 }
 bgdefault()
 { putchar(27);   /*    esc    */
   putchar(91);   /*     [     */ 
   putchar(52);   /*     4     */ 
   putchar(57);   /*     9     */
   putchar(109);  /*     m     */ 
 }
 fgdefault()
 { putchar(27);   /*    esc    */
   putchar(91);   /*     [     */ 
   putchar(51);   /*     3     */ 
   putchar(57);   /*     9     */
   putchar(109);  /*     m     */ 
 }
blank()
  { bgdefault();
    putchar(32);
  }
