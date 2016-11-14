#ifndef _SLINK_H_
#define _SLINK_H_

/*
 * slink.h: definitions for AMCC register use
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



/* Outgoing mailbox definitions */

/* Expected values */
#define SLINK_OMB_LCTRL		0x02000000
#define SLINK_OMB_LDERR		0x01000000

/* Return lines */
#define SLINK_OMB_URL_MASK	0x00000F00	/* four bits */
#define SLINK_OMB_URL_SHIFT	8		/* shift value to the left */

/* User data width */
#define SLINK_OMB_UDW_32	0x00000000
#define SLINK_OMB_UDW_16	0x00000008
#define SLINK_OMB_UDW_8		0x00000010
#define SLINK_OMB_UDW_RES	0x00000018	/* Reserved, don't use */
#define SLINK_OMB_UDW_MASK	0x00000018

/* Misc, S-Link signals; UXOFF is handled automatically */
#define SLINK_OMB_URESET	0x00000004
#define SLINK_OMB_UTDO		0x00000002

/* External fifo reset */
#define SLINK_OMB_FRESET	0x00000001




/* Incoming mailbox definitions */

/* Overflow */
#define SLINK_IMB_OVFLW		0x00800000

/* S-Link signals */
#define SLINK_IMB_LDOWN		0x04000000
#define SLINK_IMB_LCTRL		0x02000000
#define SLINK_IMB_LDERR		0x01000000


#endif /* _SLINK_H_ */
