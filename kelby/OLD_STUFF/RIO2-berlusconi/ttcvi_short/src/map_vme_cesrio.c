/* This module contains the system dependent map_vme() procedure.
 *
 * This version is for the CES RIO processor.
 *
 * On the RIO we can access A24D16 and A24D32 via the same base
 * address. We do not need to create a shared memory section as
 * this is already done by the CES startup procedures.
 */

#include <errno.h>
#include <stdio.h>

/* This is a global variable which will contain the base address of the shared
 * memory mapped to VME. 
 */
char *VME_base_address = NULL;
char *VME_a24d16_base_address = NULL;
char *VME_a24d32_base_address = NULL;
static char *check_base_address = NULL;

/*===========================================================================*/
int map_vme(void)
/*===========================================================================*/
{
/* This procedure creates and maps a shared memory section onto the VME A24 D16
 * address space, and sets the global variable VME_base_address to its base and
 * baseaddr to the base address of the UA1 crate interface which is plugged into
 * the VME crate.
 *
 * If VME_base_address is non-zero, then the procedure assumes that map_vme()
 * has already been called and immeduately returns success, provided that
 * check_base_address has the same value.  If this is not the case,
 * then it assumes that there has been a data corruption problem, so returns an
 * error code in errno - and also tries to printf a message.
 *
 * Return value:
 *	0 - Success
 *	1 - Failure
 *
 *	Note that it does not tamper with errno unless it fails - checking errno
 *      then the reason will be in errno (i.e. so you can call perror).  To
 *      make errno always valid, if you felt so inclined, just put errno = 0 at
 *      the top of the procedure.
 */
  if (check_base_address != NULL) {
    printf("map_vme has already been called, or resource busy") ;
    if (check_base_address != VME_base_address) {
      printf("FATAL ERROR from map_vme(): "
		"VME_base_address has been corrupted prior to this call\n");
      errno = EBUSY;			/* "Resource busy" */
      return 1;
    }
    else return 0;		/* Else map_vme() called already */
  } else {

    VME_base_address = check_base_address = 0xDE000000;
    VME_a24d16_base_address = VME_a24d32_base_address = VME_base_address;
    printf("map_vme has been called successfully \n") ;
  }
  return 0;
}
