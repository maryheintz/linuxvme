/* Intel 82527 CAN structure */

#define CTRL		0x00
#define STATUS      0x01
#define CPU         0x02
#define HSRead      0x04
#define GMstd       0x06
#define GMext       0x08
#define Mask15      0x0C
#define Message1    0x10
#define CLKOUT      0x1F
#define Message2    0X20
#define BCR         0X2F
#define Message3	0X30
#define BTR0        0X3F
#define Message4    0X40
#define BTR1        0X4F
#define Message5    0X50
#define IR          0X5F
#define Message6    0X60
#define Message7    0X70
#define Message8    0X80
#define Message9    0X90
#define P1CONF      0X9F
#define Message10   0XA0
#define P2CONF      0XAF
#define Message11   0XB0
#define P1IN        0XBF
#define Message12   0XC0
#define P2IN        0XCF
#define Message13   0XD0
#define P1OUT       0XDF
#define Message14   0XE0
#define P2OUT       0XEF
#define Message15   0XF0
#define SerRST      0XFF
 
#define INTVEC      0x101
 
