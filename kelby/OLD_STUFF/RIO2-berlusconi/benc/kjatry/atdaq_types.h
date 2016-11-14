#ifndef _atdaq_types_h_
#define _atdaq_types_h_ 
/*---------------------------------------------------------------------------*/
/* atdaq_types.h -- Atlas DAQ/FE specific typedef's                          */
/*                                                                           */
/* P. Kapinos  6/27/96                                                       */
/*---------------------------------------------------------------------------*/
typedef unsigned char   byte;    /* 8-bit */
typedef unsigned short  word;    /* 16-bit Word */
typedef unsigned long   dword;   /* 32-bit Double Word */

/* At Ralf's request... T.W. 10/12/97 */
typedef struct {
        dword hi;
        dword lo;
} d_long;

#endif /* _atdaq_types_h_ */
