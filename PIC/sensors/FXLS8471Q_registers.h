/************************************************************************
Title:    FXLS8471Q Registers
Author:   Julien Delvaux
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

#ifndef FXLS8471Q_REGISTERS_H
#define	FXLS8471Q_REGISTERS_H

// Device ID & Device Address
#define FXLS8471Q_ADDRESS       0x1E
#define FXLS8471Q_DEVICE_ID     0x6A

// Output Data Register
#define FXLS8471Q_OUT_X_MSB 0x01
#define FXLS8471Q_OUT_X_LSB 0x02
#define FXLS8471Q_OUT_Y_MSB 0x03
#define FXLS8471Q_OUT_Y_LSB 0x04
#define FXLS8471Q_OUT_Z_MSB 0x05
#define FXLS8471Q_OUT_Z_LSB 0x06

// Who Am I
#define FXLS8471Q_WHO_AM_I      0x0D  
#define FXLS8471Q_WHO_AM_I_BIT        7
#define FXLS8471Q_WHO_AM_I_LENGTH     8

// CTRL_REG1 configuration
#define FXLS8471Q_CTRL_REG1     0x2A   
#define FXLS8471Q_CTRL_REG1_active_BIT     0 
#define FXLS8471Q_CTRL_REG1_active_LENGTH  1
#define FXLS8471Q_CTRL_REG1_ASPL_BIT     7 
#define FXLS8471Q_CTRL_REG1_ASPL_LENGTH  2 
#define FXLS8471Q_CTRL_REG1_ODR_BIT      5 
#define FXLS8471Q_CTRL_REG1_ODR_LENGTH   3 

// CTRL_REG2 configuration
#define FXLS8471Q_CTRL_REG2     0x2B
#define FXLS8471Q_CTRL_REG2_mods_BIT     1
#define FXLS8471Q_CTRL_REG2_mods_LENGTH  2
#define FXLS8471Q_CTRL_REG2_slpe_BIT     2
#define FXLS8471Q_CTRL_REG2_slpe_LENGTH  1
#define FXLS8471Q_CTRL_REG2_smods_BIT    4
#define FXLS8471Q_CTRL_REG2_smods_LENGTH 2

// Sensor data configuration
#define FXLS8471Q_XYZ_DATA_CFG0 0x0E   
#define FXLS8471Q_XYZ_fs_BIT    0x0
#define FXLS8471Q_XYZ_fs_LENGTH 2

// Offset correction
#define FXLS8471Q_OFF_X 0x2F
#define FXLS8471Q_OFF_Y 0x30
#define FXLS8471Q_OFF_Z 0x31

#define FXLS8471Q_CTRL_REG3     0x2C
#define FXLS8471Q_CTRL_REG4     0x2D
#define FXLS8471Q_CTRL_REG5     0x2E

#endif	/* FXLS8471Q_REGISTERS_H */

