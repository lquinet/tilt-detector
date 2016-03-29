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


/************************************************************************/
/* Functions prototype                                                  */
/************************************************************************/
/**
 *  @brief   Power on and prepare for general usage.
 *  This will activate the device and take it out of sleep mode (which must be done
 *  after start-up). This function also sets both the accelerometer and the gyroscope
 *  to their most sensitive settings, namely +/- 2g and +/- 250 degrees/sec, and sets
 *  the clock source to use the X Gyro for reference, which is slightly better than
 *  the default internal clock source.
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
 *  @brief   Power on and prepare for general usage.
 *  This will activate the device and take it out of sleep mode (which must be done
 *  after start-up). This function also sets both the accelerometer and the gyroscope
 *  to their most sensitive settings, namely +/- 2g and +/- 250 degrees/sec, and sets
 *  the clock source to use the X Gyro for reference, which is slightly better than
 *  the default internal clock source.
 *  @param   none
 *  @return  none
 */
extern void fxls8471q_switchMode(uint8_t mode);



#endif	/* FXLS8471Q_H */

