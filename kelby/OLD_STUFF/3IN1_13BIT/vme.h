/* vme.h - vme access
 
   ATLAS Tile Calorimeter Testbeam
 
   May-2003 by solo
*/

/*#ifndef _VME_H
#define _VME_H

#include <stdlib.h>

#ifdef RIOII
unsigned int vme_a24_base() {
  return 0xde000000;
}

unsigned int vme_a16_base() {
  return 0xdffe0000;
}
#endif

#ifdef linux  */
#include "rcc_error/rcc_error.h"
#include "vme_rcc/vme_rcc.h"
unsigned int vme_a24_base() {
  VME_MasterMap_t master_map;
  VME_ErrorCode_t error_code;
  int master_mapping;
  u_int virtual_address;

  master_map.vmebus_address = 0x00000000;
  master_map.window_size =    0x01000000;
  master_map.address_modifier = VME_AM39;
  master_map.options = 0;

  error_code = VME_Open();
  if(error_code==VME_SUCCESS) {
    error_code=VME_MasterMap(&master_map,&master_mapping);
    if(error_code==VME_SUCCESS) {
      error_code=VME_MasterMapVirtualAddress(master_mapping,&virtual_address);
      if(error_code==VME_SUCCESS) {
        return virtual_address;
      }
    }
  }
  VME_Close();
  exit(error_code);
}

unsigned int vme_a16_base() {
  VME_MasterMap_t master_map;
  VME_ErrorCode_t error_code;
  int master_mapping;
  u_int virtual_address;

  master_map.vmebus_address = 0x00000000;
  master_map.window_size =    0x00010000;
  master_map.address_modifier = VME_AM29;
  master_map.options = 0;

  error_code = VME_Open();
  if(error_code==VME_SUCCESS) {
    error_code=VME_MasterMap(&master_map,&master_mapping);
    if(error_code==VME_SUCCESS) {
      error_code=VME_MasterMapVirtualAddress(master_mapping,&virtual_address);
      if(error_code==VME_SUCCESS) {
        return virtual_address;
      }
    }
  }
  VME_Close();
  exit(error_code);
}

/*#endif

#endif */
