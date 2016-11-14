/*
NICAN.H

This is the C language header file for the NI-CAN Application Programming
Interface (API).  For complete information on the host data types
and functions declared in this file, refer to the NI-CAN Programmer
Reference Manual.

This header file supports Win32 operating systems (Windows 95 and 
Windows NT).
*/

/**********************************************************************/

#ifndef __INC_nican
#define __INC_nican

/**********************************************************************/

/* Start of defines used for support of various compilers.  */
   
   /* Needed for C++ to C (DLL) calls  */
#ifdef __cplusplus
   extern "C" {
#endif

   /* Used for function prototypes */
#ifdef _NCDEF_NOT_DLL_
#define  _NCFUNC_
#elif defined(_NCDEF_MAKE_WIN3_)
#define  _NCFUNC_                   __export FAR PASCAL
#elif defined(_NCDEF_CALL_WIN3_)
#define  _NCFUNC_                   _far _pascal
#else
#define  _NCFUNC_                   __stdcall
#endif

   /* Used to ensure proper alignment of structure members */
#pragma  pack(2)

#ifdef _CVI_
#pragma  EnableLibraryRuntimeChecking
#endif

/***********************************************************************
                     H O S T   D A T A   T Y P E S
***********************************************************************/

typedef  char                       NCTYPE_INT8;
typedef  short                      NCTYPE_INT16;
typedef  long                       NCTYPE_INT32;
typedef  unsigned char              NCTYPE_UINT8;
typedef  unsigned short             NCTYPE_UINT16;
typedef  unsigned long              NCTYPE_UINT32;

   /* This two-part declaration is required for compilers which do not
   provide support for native 64-bit integers.  */
typedef  struct {
   NCTYPE_UINT32                       LowPart;
   NCTYPE_UINT32                       HighPart;
} NCTYPE_UINT64;

   /* Boolean value is encoded as an unsigned 8-bit integer in which
   bit 0 is used to indicate true or false, and the upper 7 bits
   are always zero.  */
typedef  unsigned char              NCTYPE_BOOL;

   /* ASCII string is encoded like all C language strings: as an array
   of characters with zero (the NULL character) used to indicate
   end-of-string.  This is known as an ASCIIZ string. */
typedef  char *                     NCTYPE_STRING;

   /* Pointer to a variable of any type */
typedef  void *                     NCTYPE_ANY_P;

typedef  NCTYPE_INT32               NCTYPE_STATUS;
typedef  NCTYPE_UINT32              NCTYPE_OBJH;
typedef  NCTYPE_UINT32              NCTYPE_VERSION;
typedef  NCTYPE_UINT32              NCTYPE_DURATION;
typedef  NCTYPE_UINT32              NCTYPE_ATTRID;
typedef  NCTYPE_UINT32              NCTYPE_OPCODE;
typedef  NCTYPE_UINT32              NCTYPE_PROTOCOL;
typedef  NCTYPE_UINT32              NCTYPE_BAUD_RATE;
typedef  NCTYPE_UINT32              NCTYPE_STATE;
typedef  NCTYPE_UINT32              NCTYPE_COMM_TYPE;

   /* This type can be typecasted to/from the Microsoft 
   Win32 type FILETIME (see MSVC 4.x WINBASE.H).  When 
   your host computer powers up, NC_ATTR_ABS_TIME is loaded
   with the system time of the host computer (value obtained
   using Win32 GetSystemTimeAsFileTime).  Thus, the timestamps
   obtained using ncRead can be converted to local time zone
   format using Win32 FileTimeToLocalFileTime, then can be
   converted to SYSTEMTIME (with year, month, day, and so on)
   using Win32 FileTimeToSystemTime.  */
typedef  NCTYPE_UINT64              NCTYPE_ABS_TIME;

#ifndef _NCDEF_NO_CRNOTIF_

   /* This is the prototype for callback function passed to
   ncCreateNotification (cannot be used to declare your callback). */
typedef  NCTYPE_STATE (_NCFUNC_ * NCTYPE_NOTIFY_CALLBACK) (
                                    NCTYPE_OBJH       ObjHandle,
                                    NCTYPE_STATE      State,
                                    NCTYPE_STATUS     Status,
                                    NCTYPE_ANY_P      RefData);
#endif

   /* CAN arbitration ID (used for both standard and extended IDs) */
typedef  NCTYPE_UINT32              NCTYPE_CAN_ARBID;

   /* Type for ncWrite of CAN Network Interface Object */
typedef  struct {
   NCTYPE_CAN_ARBID                    ArbitrationId;
   NCTYPE_BOOL                         IsRemote;
   NCTYPE_UINT8                        DataLength;
   NCTYPE_UINT8                        Data[8];
} NCTYPE_CAN_FRAME;

   /* Type for ncRead of CAN Network Interface Object */
typedef  struct {
   NCTYPE_ABS_TIME                     Timestamp;
   NCTYPE_CAN_ARBID                    ArbitrationId;
   NCTYPE_BOOL                         IsRemote;
   NCTYPE_UINT8                        DataLength;
   NCTYPE_UINT8                        Data[8];
} NCTYPE_CAN_FRAME_TIMED;

   /* Type for ncWrite of CAN Object */
typedef  struct {
   NCTYPE_UINT8                        Data[8];
} NCTYPE_CAN_DATA;

   /* Type for ncRead of CAN Object */
typedef  struct {
   NCTYPE_ABS_TIME                     Timestamp;
   NCTYPE_UINT8                        Data[8];
} NCTYPE_CAN_DATA_TIMED;

   /* Pointers (used for function prototypes) */
typedef  NCTYPE_INT8 *              NCTYPE_INT8_P;
typedef  NCTYPE_INT16 *             NCTYPE_INT16_P;
typedef  NCTYPE_INT32 *             NCTYPE_INT32_P;
typedef  NCTYPE_UINT8 *             NCTYPE_UINT8_P;
typedef  NCTYPE_UINT16 *            NCTYPE_UINT16_P;
typedef  NCTYPE_UINT32 *            NCTYPE_UINT32_P;
typedef  NCTYPE_UINT64 *            NCTYPE_UINT64_P;
typedef  NCTYPE_BOOL *              NCTYPE_BOOL_P;
typedef  NCTYPE_OBJH *              NCTYPE_OBJH_P;
typedef  NCTYPE_VERSION *           NCTYPE_VERSION_P;
typedef  NCTYPE_DURATION *          NCTYPE_DURATION_P;
typedef  NCTYPE_ATTRID *            NCTYPE_ATTRID_P;
typedef  NCTYPE_OPCODE *            NCTYPE_OPCODE_P;
typedef  NCTYPE_PROTOCOL *          NCTYPE_PROTOCOL_P;
typedef  NCTYPE_BAUD_RATE *         NCTYPE_BAUD_RATE_P;
typedef  NCTYPE_STATE *             NCTYPE_STATE_P;
typedef  NCTYPE_STATUS *            NCTYPE_STATUS_P;
typedef  NCTYPE_COMM_TYPE *         NCTYPE_COMM_TYPE_P;
typedef  NCTYPE_ABS_TIME *          NCTYPE_ABS_TIME_P;
typedef  NCTYPE_CAN_ARBID *         NCTYPE_CAN_ARBID_P;
typedef  NCTYPE_CAN_FRAME *         NCTYPE_CAN_FRAME_P;
typedef  NCTYPE_CAN_FRAME_TIMED *   NCTYPE_CAN_FRAME_TIMED_P;
typedef  NCTYPE_CAN_DATA *          NCTYPE_CAN_DATA_P;
typedef  NCTYPE_CAN_DATA_TIMED *    NCTYPE_CAN_DATA_TIMED_P;

   /* Included for backward compatibility with older versions of NI-CAN */
typedef  NCTYPE_UINT32              NCTYPE_BKD_TYPE;
typedef  NCTYPE_UINT32              NCTYPE_BKD_WHEN;
typedef  NCTYPE_BKD_TYPE *          NCTYPE_BKD_TYPE_P;
typedef  NCTYPE_BKD_WHEN *          NCTYPE_BKD_WHEN_P;

/***********************************************************************
                          C O N S T A N T S
***********************************************************************/

   /* NCTYPE_BOOL (true/false values) */
#define  NC_TRUE                    1
#define  NC_FALSE                   0

   /* NCTYPE_VERSION (bit masks to obtain fields) */
#define  NC_MK_VER_MAJOR            0xFF000000     /* major version */
#define  NC_MK_VER_MINOR            0x00FF0000     /* minor version */
#define  NC_MK_VER_SUBMINOR         0x0000FF00     /* subminor version */
#define  NC_MK_VER_BETA             0x000000FF     /* beta version */

   /* NCTYPE_DURATION (values in one millisecond ticks) */
#define  NC_DURATION_NONE           0              /* zero duration */
#define  NC_DURATION_INFINITE       0xFFFFFFFF     /* infinite duration */
#define  NC_DURATION_1MS            1              /* one millisecond */
#define  NC_DURATION_10MS           10
#define  NC_DURATION_100MS          100
#define  NC_DURATION_1SEC           1000           /* one second */
#define  NC_DURATION_10SEC          10000
#define  NC_DURATION_100SEC         100000
#define  NC_DURATION_1MIN           60000          /* one minute */

   /* NCTYPE_PROTOCOL (values for supported NI-CAN protocols) */
#define  NC_PROTOCOL_CAN            1              /* Controller Area Net */
                                    
   /* NCTYPE_BAUD_RATE (values for baud rates) */
#define  NC_BAUD_10K                10000
#define  NC_BAUD_100K               100000
#define  NC_BAUD_125K               125000
#define  NC_BAUD_250K               250000
#define  NC_BAUD_500K               500000
#define  NC_BAUD_1000K              1000000

   /* NCTYPE_STATE (bit masks for states) */
#define  NC_ST_READ_AVAIL           0x00000001
#define  NC_ST_WRITE_SUCCESS        0x00000002
#define  NC_ST_STOPPED              0x00000004
#define  NC_ST_ERROR                0x00000010
#define  NC_ST_WARNING              0x00000020

   /* NCTYPE_STATUS (bit masks for status fields) */
#define  NC_MK_SEVERITY             0xC0000000     /* severity of err/warn */
#define  NC_MK_QUALIFIER            0x3FFF0000     /* err/warn qualifer */
#define  NC_MK_CODE                 0x0000FFFF     /* err/warn code */
                                    
   /* NCTYPE_STATUS (macros to obtain each status field) */
#define  NC_STATSEV(status)         ((status) & NC_MK_SEVERITY)
#define  NC_STATQUAL(status)        ((status) & NC_MK_QUALIFIER)
#define  NC_STATCODE(status)        ((status) & NC_MK_CODE)

   /* NCTYPE_STATUS (values for severity field)
      Given the definitions below, the severity can be easily
      determined by checking to see if the entire status is 
      equal to zero (success), greater than zero (warning), or 
      less than zero (error).
   */
#define  NC_SEV_SUCCESS             0x00000000     /* severity = success */
#define  NC_SEV_WARNING             0x40000000     /* severity = warning */
#define  NC_SEV_ERROR               0x80000000     /* severity = error */
                                    
   /* NCTYPE_STATUS (can be used with entire status to check for success) */
#define  NC_SUCCESS                 0x00000000     /* success */

   /* NCTYPE_STATUS (values for code field)
      This is the primary warning/error code referenced in NI-CAN 
      documentation.  Even though these codes may be used as warnings, 
      they all use the prefix NC_ERR_.
   */
#define  NC_ERR_TIMEOUT             0x00000001     /* timeout */
#define  NC_ERR_DRIVER              0x00000002     /* driver specific err */
#define  NC_ERR_BAD_NAME            0x00000003     /* bad object name */
#define  NC_ERR_BAD_PARAM           0x00000004     /* bad parameter */
#define  NC_ERR_BAD_VALUE           0x00000005     /* bad (attr) value */
#define  NC_ERR_ALREADY_OPEN        0x00000006     /* object already open */
#define  NC_ERR_NOT_STOPPED         0x00000007     /* can't set if started */
#define  NC_ERR_OVERFLOW            0x00000008     /* data lost on overflow */
#define  NC_ERR_OLD_DATA            0x00000009     /* data was already read */
#define  NC_ERR_NOT_SUPPORTED       0x0000000A     /* not supported */
#define  NC_ERR_CAN_BUS_OFF         0x0000000B     /* CAN comm err/warn (1.3) */
#define  NC_ERR_CAN_COMM            0x0000000B     /* New name for above (1.3) */
#define  NC_ERR_CAN_XCVR            0x0000000C     /* Low spd xcvr err (1.3) */

   /* NCTYPE_STATUS (values for qualifer field)
   */
         /* For NC_ERR_CAN_BUS_OFF, the qualifier field indicates the most 
            most recent communications error detected on the CAN bus.  */
#define  NC_QUAL_CAN_STUFF          0x00010000     /* stuff error */
#define  NC_QUAL_CAN_FORM           0x00020000     /* form error */
#define  NC_QUAL_CAN_ACK            0x00030000     /* ack error */
#define  NC_QUAL_CAN_BIT1           0x00040000     /* bit 1 error */
#define  NC_QUAL_CAN_BIT0           0x00050000     /* bit 0 error */
#define  NC_QUAL_CAN_CRC            0x00060000     /* CRC checksum error */
         /* For NC_ERR_TIMEOUT, the qualifier indicates the location
            where the timeout occurred.  */
#define  NC_QUAL_TIMO_FUNCTION      0x00000000     /* wait/notif timeout */
#define  NC_QUAL_TIMO_WATCHDOG      0x00010000     /* watchdog timeout */
#define  NC_QUAL_TIMO_SCHEDULE      0x00050000     /* periods can't be met */
         /* For NC_ERR_OVERFLOW, the qualifier indicates the location
            where the overflow occurred.  */
#define  NC_QUAL_OVFL_WRITE         0x00000000     /* write q (ncWrite) */
#define  NC_QUAL_OVFL_READ          0x00010000     /* read q (background) */
#define  NC_QUAL_OVFL_CHIP          0x00020000     /* CAN comm chip */
#define  NC_QUAL_OVFL_RX_Q          0x00030000     /* receive q (in ISR), (1.3) */
         /* For NC_ERR_BAD_VALUE, NC_ERR_ALREADY_OPEN, 
            NC_ERR_NOT_STOPPED, and NC_ERR_OLD_DATA, 
            the qualifier is zero (none).  */
#define  NC_QUAL_NONE               0x00000000     /* no qualifier */
         /* For NC_ERR_BAD_NAME, the qualifier indicates the position of
            the invalid name in the hierarhcy.  For example, one
            indicates an invalid Network Interface Object name.  Zero
            indicates a basic syntax error (i.e. single colon). */
         /* For NC_ERR_BAD_PARAM, the qualifier indicates which parameter
            is invalid.  For example, if ncGetAttribute is called with an
            invalid AttrId, the qualifier is two (second param invalid). */
         /* For NC_ERR_DRIVER, the qualifier is implementation specific,
            and can be reported to National Instruments Technical Support
            for more information. */

   /* NCTYPE_ATTRID values
      For every attribute ID, its type and permissions are listed 
      in the comment.
   */
         /* NCTYPE_PROTOCOL, Get */
#define  NC_ATTR_PROTOCOL           0x80000001
         /* NCTYPE_VERSION, Get */
#define  NC_ATTR_PROTOCOL_VERSION   0x80000002
         /* NCTYPE_VERSION, Get */
#define  NC_ATTR_SOFTWARE_VERSION   0x80000003
         /* NCTYPE_BOOL, Config */
#define  NC_ATTR_START_ON_OPEN      0x80000006
         /* NCTYPE_BAUD_RATE, Config
            Note that in addition to standard baud rates like 125000,
            this attribute also allows you to program non-standard
            or otherwise uncommon baud rates.  If bit 31 (0x80000000)
            is set, the low 16 bits of this attribute are programmed
            directly into the bit timing registers of the CAN 
            communications controller.  The low byte is programmed as
            BTR0 of the Intel 82527 chip (8MHz clock), and the high byte 
            as BTR1, resulting in the following bit map:
               15  14  13  12  11  10   9   8   7   6   5   4   3   2   1   0
               sam (tseg2 - 1) (  tseg1 - 1   ) (sjw-1) (     presc - 1     )
            For example, baud rate 0x80001C03 programs the CAN communications
            controller for 125000 baud (same baud rate 125000 decimal).
            For more information, refer to the reference manual for
            any CAN communications controller chip.  */
#define  NC_ATTR_BAUD_RATE          0x80000007
         /* NCTYPE_ABS_TIME, Get/Set */
#define  NC_ATTR_ABS_TIME           0x80000008
         /* NCTYPE_STATE, Get */
#define  NC_ATTR_STATE              0x80000009
         /* NCTYPE_STATUS, Get */
#define  NC_ATTR_STATUS             0x8000000A
         /* NCTYPE_DURATION, Config */
#define  NC_ATTR_BKD_PERIOD         0x8000000F
#define  NC_ATTR_PERIOD             0x8000000F
         /* NCTYPE_BOOL, Config   VER 1.2: Now ignored... always enabled */
#define  NC_ATTR_TIMESTAMPING       0x80000010
         /* NCTYPE_UINT32, Get */
#define  NC_ATTR_READ_PENDING       0x80000011
         /* NCTYPE_UINT32, Get */
#define  NC_ATTR_WRITE_PENDING      0x80000012
         /* NCTYPE_UINT32, Config */
#define  NC_ATTR_READ_Q_LEN         0x80000013
         /* NCTYPE_UINT32, Config */
#define  NC_ATTR_WRITE_Q_LEN        0x80000014
         /* NCTYPE_BOOL, Config */
#define  NC_ATTR_BKD_CHANGES_ONLY   0x80000015
#define  NC_ATTR_CHANGES_ONLY       0x80000015
#define  NC_ATTR_RX_CHANGES_ONLY    0x80000015
         /* NCTYPE_COMM_TYPE, Config */
#define  NC_ATTR_COMM_TYPE          0x80000016
         /* NCTYPE_CAN_ARBID, Config */
#define  NC_ATTR_CAN_COMP_STD       0x80010001
         /* NCTYPE_UINT32, Config */
#define  NC_ATTR_CAN_MASK_STD       0x80010002
         /* NCTYPE_CAN_ARBID, Config */
#define  NC_ATTR_CAN_COMP_XTD       0x80010003
         /* NCTYPE_UINT32, Config */
#define  NC_ATTR_CAN_MASK_XTD       0x80010004
         /* NCTYPE_BOOL, Config */
#define  NC_ATTR_BKD_CAN_RESPONSE   0x80010006
#define  NC_ATTR_CAN_RESPONSE       0x80010006
#define NC_ATTR_CAN_TX_RESPONSE    0x80010006
         /* NCTYPE_UINT32, Config */
#define  NC_ATTR_CAN_DATA_SIZE      0x80010007
#define  NC_ATTR_CAN_DATA_LENGTH   0x80010007
         /* NCTYPE_BOOL, Config (1.3) */
#define  NC_ATTR_LOG_COMM_ERRS      0x8001000A
         /* NCTYPE_RX_Q_LEN, Config (Net Intf Objs only), (1.3) 
         This is the max number of frames for the receive queue used by
         the CAN ISR to offload frames as quickly as possible.  This 
         queue is located between the CAN chip and the card's
         shared memory read queue for the Net Intf (accessed by ncRead).  */
#define  NC_ATTR_RX_Q_LEN           0xA0000004
 
   /* NCTYPE_OPCODE values */
#define  NC_OP_START                0x80000001  /* start comm (no Param) */
#define  NC_OP_STOP                 0x80000002  /* stop comm (no Param) */
#define  NC_OP_RESET                0x80000003  /* reset obj (no Param) */
#define  NC_OP_NET_SYNC             0x800000FF  /* temporary */

   /* NCTYPE_COMM_TYPE values */
#define  NC_CAN_COMM_RX_UNSOL       0x00000000  /* rx unsolicited data */
#define  NC_CAN_COMM_TX_BY_CALL     0x00000001  /* tx data by call */
#define  NC_CAN_COMM_RX_PERIODIC    0x00000002  /* rx periodic using remote */
#define  NC_CAN_COMM_TX_PERIODIC    0x00000003  /* tx data periodically */
#define  NC_CAN_COMM_RX_BY_CALL     0x00000004  /* rx by call using remote */
#define  NC_CAN_COMM_TX_RESP_ONLY   0x00000005  /* tx by response only */
#define  NC_CAN_COMM_TX_WAVEFORM    0x00000006  /* tx periodic "waveform" */

   /* NCTYPE_CAN_ARBID (bit masks) */
#define  NC_FL_CAN_ARBID_XTD        0x20000000  /* extended=1, standard=0 */
   /* NCTYPE_CAN_ARBID (special values) */
#define  NC_CAN_ARBID_NONE          0xCFFFFFFF

   /* Special values for CAN mask attributes (NC_ATTR_CAN_MASK_STD/XTD) */
#define  NC_CAN_MASK_STD_MUSTMATCH  0x000007FF
#define  NC_CAN_MASK_XTD_MUSTMATCH  0x1FFFFFFF
#define  NC_CAN_MASK_STD_DONTCARE   0x00000000
#define  NC_CAN_MASK_XTD_DONTCARE   0x00000000

   /* Values for the IsRemote field of CAN frames (1.3). */
#define  NC_FRMTYPE_DATA            0
#define  NC_FRMTYPE_REMOTE          1
#define  NC_FRMTYPE_COMM_ERR        2

   /* Included for backward compatibility with older versions of NI-CAN */
         /* NCTYPE_UINT32, Config */
#define  NC_ATTR_BKD_READ_SIZE      0x8000000B
         /* NCTYPE_UINT32, Config */
#define  NC_ATTR_BKD_WRITE_SIZE     0x8000000C
         /* NCTYPE_BKD_TYPE, Config */
#define  NC_ATTR_BKD_TYPE           0x8000000D
         /* NCTYPE_BKD_WHEN, Config */
#define  NC_ATTR_BKD_WHEN_USED      0x8000000E
         /* NCTYPE_UINT16, Config */
#define  NC_ATTR_CAN_BIT_TIMINGS    0x80010005
      /* NCTYPE_BKD_TYPE values */
#define  NC_BKD_TYPE_PEER2PEER      0x00000001
#define  NC_BKD_TYPE_REQUEST        0x00000002
#define  NC_BKD_TYPE_RESPONSE       0x00000003
      /* NCTYPE_BKD_WHEN values */
#define  NC_BKD_WHEN_PERIODIC       0x00000001
#define  NC_BKD_WHEN_UNSOLICITED    0x00000002
      /* Special values for background read/write data 
      sizes (NC_ATTR_BKD_READ_SIZE and NC_ATTR_BKD_WRITE_SIZE). */
#define  NC_BKD_CAN_ZERO_SIZE       0x00008000

/***********************************************************************
                F U N C T I O N   P R O T O T Y P E S
***********************************************************************/

#ifndef _NCDEF_NO_PROTO_

extern NCTYPE_STATUS _NCFUNC_ ncOpenObject(
                           NCTYPE_STRING        ObjName,
                           NCTYPE_OBJH_P        ObjHandlePtr);

extern NCTYPE_STATUS _NCFUNC_ ncCloseObject(
                           NCTYPE_OBJH          ObjHandle);

extern NCTYPE_STATUS _NCFUNC_ ncRead(
                           NCTYPE_OBJH          ObjHandle,
                           NCTYPE_UINT32        DataSize,
                           NCTYPE_ANY_P         DataPtr);

extern NCTYPE_STATUS _NCFUNC_ ncReadMult(
				   NCTYPE_OBJH     objhandleptr,
				   NCTYPE_UINT32   DataSize,
				   NCTYPE_ANY_P    DataPtr,
				   NCTYPE_UINT32_P ActualDataSize);

extern NCTYPE_STATUS _NCFUNC_ ncWrite(
                           NCTYPE_OBJH          ObjHandle,
                           NCTYPE_UINT32        DataSize,
                           NCTYPE_ANY_P         DataPtr);

extern NCTYPE_STATUS _NCFUNC_ ncGetAttribute(
                           NCTYPE_OBJH          ObjHandle,
                           NCTYPE_ATTRID        AttrId,
                           NCTYPE_UINT32        AttrSize,
                           NCTYPE_ANY_P         AttrPtr);

extern NCTYPE_STATUS _NCFUNC_ ncSetAttribute(
                           NCTYPE_OBJH          ObjHandle,
                           NCTYPE_ATTRID        AttrId,
                           NCTYPE_UINT32        AttrSize,
                           NCTYPE_ANY_P         AttrPtr);

extern NCTYPE_STATUS _NCFUNC_ ncAction(
                           NCTYPE_OBJH          ObjHandle,
                           NCTYPE_OPCODE        Opcode,
                           NCTYPE_UINT32        Param);

extern NCTYPE_STATUS _NCFUNC_ ncWaitForState(
                           NCTYPE_OBJH          ObjHandle,
                           NCTYPE_STATE         DesiredState,
                           NCTYPE_DURATION      Timeout,
                           NCTYPE_STATE_P       StatePtr);
                                             
#ifndef _NCDEF_NO_CRNOTIF_

extern NCTYPE_STATUS _NCFUNC_ ncCreateNotification(
                           NCTYPE_OBJH          ObjHandle,
                           NCTYPE_STATE         DesiredState,
                           NCTYPE_DURATION      Timeout,
                           NCTYPE_ANY_P         RefData,
                           NCTYPE_NOTIFY_CALLBACK Callback);
#endif

extern NCTYPE_STATUS _NCFUNC_ ncConfig(
                           NCTYPE_STRING        ObjName,
                           NCTYPE_UINT32        NumAttrs,
                           NCTYPE_ATTRID_P      AttrIdList,
                           NCTYPE_UINT32_P      AttrValueList);

   /* Function to completely reset the CAN card.
   DO NOT use this function while an application is running
   on the CAN card.  (1.3)
      IntfName: Name of the network interface (such as "CAN0").
      Param: Unused (0).
   */
extern NCTYPE_STATUS _NCFUNC_ ncReset(
				   NCTYPE_STRING IntfName,
				   NCTYPE_UINT32 Param);					    



#endif /* not _NCDEF_NO_PROTO_ */

/**********************************************************************/

/* End of defines used for support of various compilers.  */

#pragma  pack()

#ifdef __cplusplus
   }
#endif

/**********************************************************************/

#endif /* __INC_nican */
