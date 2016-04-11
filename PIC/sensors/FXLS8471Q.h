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

// <<Portrait/Landscape>>
#define FXLS8471Q_PL_OFF        0 // Portrait/Landscape disabled
#define FXLS8471Q_PL_ON         1 // Portrait/Landscape enabled
// Back/Front orientation definitions (see datasheet p.48)
#define FXLS8471Q_BKFR_0    0x00
#define FXLS8471Q_BKFR_1    0x01
#define FXLS8471Q_BKFR_2    0x02
#define FXLS8471Q_BKFR_3    0x03
// Z-Lockout angle trip (see datasheet p.48)
#define FXLS8471Q_ZLOCK_13  0x00
#define FXLS8471Q_ZLOCK_17  0x01
#define FXLS8471Q_ZLOCK_20  0x02
#define FXLS8471Q_ZLOCK_24  0x03
#define FXLS8471Q_ZLOCK_28  0x04
#define FXLS8471Q_ZLOCK_32  0x05
#define FXLS8471Q_ZLOCK_36  0x06
#define FXLS8471Q_ZLOCK_40  0x07
// Threshold angle
#define FXLS8471Q_THS_15  0x07 // 15°
#define FXLS8471Q_THS_20  0x09 // 20°
#define FXLS8471Q_THS_30  0x0C // 30°
#define FXLS8471Q_THS_35  0x0D // 35°
#define FXLS8471Q_THS_40  0x0F // 40°
#define FXLS8471Q_THS_45  0x10 // 45°
#define FXLS8471Q_THS_55  0x13 // 55°
#define FXLS8471Q_THS_60  0x14 // 60°
#define FXLS8471Q_THS_70  0x17 // 70°
#define FXLS8471Q_THS_75  0x19 // 75°
// Hysteresus settings (see datasheet p.49)
#define FXLS8471Q_HYS_0  0x00
#define FXLS8471Q_HYS_1  0x01
#define FXLS8471Q_HYS_2  0x02
#define FXLS8471Q_HYS_3  0x03
#define FXLS8471Q_HYS_4  0x04
#define FXLS8471Q_HYS_5  0x05
#define FXLS8471Q_HYS_6  0x06
#define FXLS8471Q_HYS_7  0x07
// PL_Status 
#define FXLS8471Q_LAPO_UP       0x00
#define FXLS8471Q_LAPO_DOWN     0x01
#define FXLS8471Q_LAPO_RIGHT    0x02
#define FXLS8471Q_LAPO_LEFT     0x03
#define FXLS8471Q_BAFRO_FRONT   0x00
#define FXLS8471Q_BAFRO_BACK    0x01

// <<Freefall/Motion Detection>>
// Freefall/motion enable/disable in A_FFMT_CFG
#define FXLS8471Q_FFMT_ENABLE   0x01
#define FXLS8471Q_FFMT_DISABLE  0x00
#define FXLS8471Q_FFMT_FREEFALL 0x00
#define FXLS8471Q_FFMT_MOTION   0x01

// <<Tap Detection>>
// Configuration
#define FXLS8471Q_PULSE_SINGLE  0x00
#define FXLS8471Q_PULSE_DOUBLE  0x01
#define FXLS8471Q_PULSE_BOTH    0x02
// Tap event
#define FXLS8471Q_PULSE_SRC_EVENT   0x80
#define FXLS8471Q_PULSE_SRC_Z_AXIS  0x40
#define FXLS8471Q_PULSE_SRC_Y_AXIS  0x20
#define FXLS8471Q_PULSE_SRC_X_AXIS  0x10
#define FXLS8471Q_PULSE_SRC_DPE     0x08
#define FXLS8471Q_PULSE_SRC_Z_POL   0x04
#define FXLS8471Q_PULSE_SRC_Y_POL   0x02
#define FXLS8471Q_PULSE_SRC_X_POL   0x01

// <<Interruptions>>
// Interruption configuration
#define FXLS8471Q_INT_aslp_OFF      0x00
#define FXLS8471Q_INT_aslp_ON       0x01
#define FXLS8471Q_INT_fifo_OFF      0x00
#define FXLS8471Q_INT_fifo_ON       0x01
#define FXLS8471Q_INT_trans_OFF     0x00
#define FXLS8471Q_INT_trans_ON      0x01
#define FXLS8471Q_INT_lndprt_OFF    0x00
#define FXLS8471Q_INT_lndprt_ON     0x01
#define FXLS8471Q_INT_pulse_OFF     0x00
#define FXLS8471Q_INT_pulse_ON      0x01
#define FXLS8471Q_INT_ffmt_OFF      0x00
#define FXLS8471Q_INT_ffmt_ON       0x01
#define FXLS8471Q_INT_avecm_OFF     0x00
#define FXLS8471Q_INT_avecm_ON      0x01
#define FXLS8471Q_INT_drdy_OFF      0x00
#define FXLS8471Q_INT_drdy_ON       0x01
// Interruptions Routing
#define FXLS8471Q_INT_INT2      0x00
#define FXLS8471Q_INT_INT1      0x01
// Interruptions
#define FXLS8471Q_INT_SOURCE_aslp   0x80
#define FXLS8471Q_INT_SOURCE_fifo   0x40
#define FXLS8471Q_INT_SOURCE_trans  0x20
#define FXLS8471Q_INT_SOURCE_lndprt 0x10
#define FXLS8471Q_INT_SOURCE_pulse  0x08
#define FXLS8471Q_INT_SOURCE_ffmt   0x04
#define FXLS8471Q_INT_SOURCE_avecm  0x02
#define FXLS8471Q_INT_SOURCE_drdy   0x01

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
 *  @param   mode FXLS8471Q_FS_X
 *  @return  none
 */
extern void fxls8471q_calibrate(uint8_t mode);
/**
 *  @brief   Configure the accelerometer.
 *  @param   none
 *  @return  none
 */
extern void fxls8471q_configure(void);
/**
 *  @brief   Read the acceleration value in the accelerometer.
 *  @param   x,y,z int16 pointer with the accelerometer value of the corresponding axis
 *  @return  none
 */
extern void fxls8471q_getAcceleration(int16_t *x, int16_t *y, int16_t *z);
/**
 *  @brief   Configure the accelerometer to detect the orientation.
 *  @param   dbnce is determined by the system ODR value and the value of the PL_COUNT register
 *  @return  none
 */
extern void fxls8471q_configureOrientationDetection(uint8_t dbnce);
/**
 *  @brief   Configure the accelerometer to detect the motion.
 *  @param   none
 *  @return  none
 */
extern void fxls8471q_configureMotionDetection(void);
/**
 *  @brief   Configure the freefall detection.
 *  @param   none
 *  @return  none
 */
extern void fxls8471q_configureFreefallDetection(void);
/**
 *  @brief   Configure the accelerometer to detect a double tap.
 *  @param   none
 *  @return  none
 */
extern void fxls8471q_configureTapDetection(void);
/**
 *  @brief   Configure the interruptions that can wake the accelerometer.
 *  @param   FXLS8471Q_INT_X_ON or FXLS8471Q_INT_X_OFF
 *  @return  none
 */
extern void fxls8471q_configureSleepInterrupt(uint8_t trans, uint8_t lndprt, uint8_t pulse, uint8_t ffmt, uint8_t avecm);
/**
 *  @brief   Configure the interruptions in the accelerometer.
 *  @param   FXLS8471Q_INT_X_ON or FXLS8471Q_INT_X_OFF
 *  @return  none
 */
extern void fxls8471q_configureInterrupt(uint8_t aslp, uint8_t fifo, uint8_t trans, uint8_t lndprt, uint8_t pulse, uint8_t ffmt, uint8_t avecm, uint8_t drdy);
/**
 *  @brief   Configure the pin of interruptions in the accelerometer.
 *  @param   FXLS8471Q_INT_INT2 or FXLS8471Q_INT_INT1
 *  @return  none
 */
extern void fxls8471q_configureRoutingInterrupt(uint8_t aslp, uint8_t fifo, uint8_t trans, uint8_t lndprt, uint8_t pulse, uint8_t ffmt, uint8_t avecm, uint8_t drdy);
/**
 *  @brief   Check the source of the interruption.
 *  @param   none
 *  @return  none
 */
extern void fxls8471q_checkSourceInterrupt(void);
/**
 *  @brief   Print the register value.
 *  @param   regAddr Register to be displayed
 *  @return  none
 */
extern void fxls8471q_debug(uint8_t regAddr);



#endif	/* FXLS8471Q_H */

