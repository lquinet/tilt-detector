/************************************************************************
Title:    FXLS8471Q Library
Author:   Julien Delvaux
Software: MPLAB X v3.26
Hardware: PIC18F46J50 family
License:  GNU General Public License 3
Usage:    see Doxygen manual

LICENSE:
	Copyright (C) 2015 Julien Delvaux

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3 of the License, or
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

    
************************************************************************/

/** 
 *  @defgroup pic-FXLS8471Q FXLS8471Q Library
 *  @code #include <FXLS8471Q.h> @endcode
 * 
 *  @brief
 *
 *
 *  @author Julien Delvaux <delvaux.ju@gmail.com>
 */

#ifndef FXLS8471Q_H
#define	FXLS8471Q_H

#include "../user.h"

/************************************************************************/
/* Constants and macros                                                 */
/************************************************************************/

// DEBUG option through UART
#define DEBUG_FXLS8471Q 

// PIN MAPPING
#define FXLS8471Q_RST_PORT      PORTBbits.RB2
#define FXLS8471Q_RST_TRIS      TRISBbits.TRISB2
#define FXLS8471Q_INT1_PORT     PORTBbits.RB0
#define FXLS8471Q_INT1_TRIS		TRISBbits.TRISB0
#define FXLS8471Q_INT2_PORT     PORTBbits.RB3//PORTDbits.RD4
#define FXLS8471Q_INT2_TRIS		TRISBbits.TRISB3//TRISDbits.TRISD4

// Modes
#define FXLS8471Q_MODE_STANDBY  0
#define FXLS8471Q_MODE_WAKE     1

// Range of the accelerometer
#define FXLS8471Q_FS_2          0x00 // +- 2g
#define FXLS8471Q_FS_4          0x01 // +- 4g
#define FXLS8471Q_FS_8          0x02 // +- 8g

// Auto-wake sample frequency
#define FXLS8471Q_ASPL_50       0x00 // 50Hz
#define FXLS8471Q_ASPL_12_5     0x01 // 12.5Hz
#define FXLS8471Q_ASPL_6_25     0x02 // 6.25Hz
#define FXLS8471Q_ASPL_1_56     0x03 // 1.56Hz

// System Output Data Rate selection
#define FXLS8471Q_ODR_800       0x00 // 800Hz
#define FXLS8471Q_ODR_400       0x01 // 400Hz
#define FXLS8471Q_ODR_200       0x02 // 200Hz
#define FXLS8471Q_ODR_100       0x03 // 100Hz
#define FXLS8471Q_ODR_50        0x04 // 50Hz
#define FXLS8471Q_ODR_12_5      0x05 // 12.5Hz
#define FXLS8471Q_ODR_6_25      0x06 // 6.25Hz
#define FXLS8471Q_ODR_1_56      0x07 // 1.5625Hz

// Sleep
#define FXLS8471Q_SLEEP_OFF     0
#define FXLS8471Q_SLEEP_ON      1

// Power Mode
#define FXLS8471Q_PM_NORMAL     0x00 // Normal
#define FXLS8471Q_PM_LNLP       0x01 // Low noise, low power
#define FXLS8471Q_PM_HR         0x02 // High resolution
#define FXLS8471Q_PM_LP         0x03 // Low Power


/************************************************************************/
/* Functions prototype                                                  */
/************************************************************************/
/**
 *  @brief   Power on the accelerometer and set it @800Hz +-2g.
 *  @param   none
 *  @return  none
 */
extern void fxls8471q_init(void);
/**
 *  @brief   Verify the I2C connection.
 *  @param   none
 *  @return  True if connection is valid, false otherwise
 */
extern uint8_t fxls8471q_testConnection(void);
/**
 *  @brief   Calibrate the accelerometer and write offset in device register.
 *  @param   none
 *  @return  none
 */
extern void fxls8471q_calibrate(void);
/**
 *  @brief   Read the acceleration value in the accelerometer.
 *  @param   x,y,z int16 pointer with the accelerometer value of the corresponding axis
 *  @return  none
 */
extern void fxls8471q_getAcceleration(int16_t *x, int16_t *y, int16_t *z);
/**
 *  @brief   Print the register value.
 *  @param   regAddr Register to be displayed
 *  @return  none
 */
extern void fxls8471q_debug(uint8_t regAddr);



#endif	/* FXLS8471Q_H */

