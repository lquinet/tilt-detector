#ifndef _USER_H
#define _USER_H

typedef unsigned char boolean;
/*!< Signed integer types  */
typedef   signed char     int8_t;
typedef   signed short    int16_t;
typedef   signed long     int32_t;

/*!< Unsigned integer types  */
typedef unsigned char     uint8_t;
typedef unsigned short    uint16_t;
typedef unsigned long     uint32_t;


typedef union IntTo8
{
 unsigned int  LongNb;
 unsigned char Nb8_B[2];
}IntTo8_t;
union DoubleTo8
{
 float  DoubleNb;
 unsigned char Nb8_B[4];
};
union LongTo8
{
 long  LongNb;
 unsigned char Nb8_B[4];
};

typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

//#define OLDMOTOR
//#define STOP_REGU_AT_START
//#define REGU_STOP
#define CALIF_REGU

//--------------------------------------------------
#define LedGreen                PORTDbits.RD6
#define TRISLedGreen			TRISDbits.TRISD6

#define LedRed                  PORTDbits.RD7
#define TRISLedRed              TRISDbits.TRISD7
//--------------------------------------------------
//#define TEMP_INT                PORTDbits.RD5
//#define TRISTEMP_INT			TRISDbits.TRISD5

#define RF_WIP_BUSY             PORTBbits.RB1
#define TRISRF_WIP_BUSY			TRISBbits.TRISB1



#define  ALARME_TASK_MAIN	1000 //en milliseconde


#define NULL   		0
#define KEY_NULL	0   		

#endif /* _USER_H */
