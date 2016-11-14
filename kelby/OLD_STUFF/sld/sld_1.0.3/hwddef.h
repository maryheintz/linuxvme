#ifndef __HWDDEF_H__
#define __HWDDEF_H__


/*
 * hwddef.h: export of HWD interface to ddf.c
 * Copyright (C) 2000 by J.E. van Grootheest
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include "sldpriv.h"

#define HWD_VENDOR	(PCI_VENDOR_ID_CERN)
#define HWD_DEVICE	(0x0011)


void hwd_powerUp( sldpriv_t *sldData );
void hwd_powerDown( sldpriv_t *sldData );
int hwd_startDevice( sldpriv_t *sldData );
int hwd_stopDevice( sldpriv_t *sldData );
int hwd_ioctl( sldpriv_t *sldData, unsigned int cmd, unsigned long arg );
int hwd_isInterrupt( sldpriv_t *sldData );
void hwd_disableInterrupt( sldpriv_t *sldData );
void hwd_enableInterrupt( sldpriv_t *sldData );
int hwd_handleInterrupt( sldpriv_t *sldData );
int hwd_timerTask( sldpriv_t *sldData );


#endif
