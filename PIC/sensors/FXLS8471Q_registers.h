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

// Interrupt source register
#define FXLS8471Q_INT_SOURCE 0x0C

// Portrait/Landscape Detection
#define FXLS8471Q_PL_STATUS     0x10
#define FXLS8471Q_PL_STATUS_lapo_BIT        2
#define FXLS8471Q_PL_STATUS_lapo_LENGTH     2
#define FXLS8471Q_PL_STATUS_bafro_BIT       0
#define FXLS8471Q_PL_STATUS_bafro_LENGTH    1
#define FXLS8471Q_PL_STATUS_newlp_BIT       7
#define FXLS8471Q_PL_STATUS_newlp_LENGTH    1
#define FXLS8471Q_PL_CFG        0x11
#define FXLS8471Q_PL_CFG_plen_BIT           6
#define FXLS8471Q_PL_CFG_plen_LENGTH        1
#define FXLS8471Q_PL_COUNT      0x12 // This register sets the debounce count for the orientation state transition.
#define FXLS8471Q_PL_COUNT_dbnce_BIT        7
#define FXLS8471Q_PL_COUNT_dbnce_LENGTH     8
#define FXLS8471Q_PL_BF_ZCOMP   0x13 // Back/Front and Z-tilt angle compensation register
#define FXLS8471Q_PL_BF_ZCOMP_bkdr_BIT      7
#define FXLS8471Q_PL_BF_ZCOMP_bkdr_LENGTH   2
#define FXLS8471Q_PL_BF_ZCOMP_zlock_BIT     2
#define FXLS8471Q_PL_BF_ZCOMP_zlock_LENGTH  3
#define FXLS8471Q_PL_THS_REG    0x14 // Portrait to landscape trip threshold registers
#define FXLS8471Q_PL_THS_REG_plths_BIT      7
#define FXLS8471Q_PL_THS_REG_plths_LENGTH   5
#define FXLS8471Q_PL_THS_REG_hys_BIT        2
#define FXLS8471Q_PL_THS_REG_hys_LENGTH     3

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

// CTRL_REG4 -- Interrupt enable register
#define FXLS8471Q_CTRL_REG4     0x2D 
#define FXLS8471Q_CTRL_REG4_aslp_BIT    7
#define FXLS8471Q_CTRL_REG4_fifo_BIT    6
#define FXLS8471Q_CTRL_REG4_trans_BIT   5
#define FXLS8471Q_CTRL_REG4_lndprt_BIT  4
#define FXLS8471Q_CTRL_REG4_pulse_BIT   3
#define FXLS8471Q_CTRL_REG4_ffmt_BIT    2
#define FXLS8471Q_CTRL_REG4_avecm_BIT   1
#define FXLS8471Q_CTRL_REG4_drdy_BIT    0

// CTRL_REG5 -- Interrupt Routing Configuration Register
#define FXLS8471Q_CTRL_REG5     0x2E
#define FXLS8471Q_CTRL_REG5_aslp_BIT    7
#define FXLS8471Q_CTRL_REG5_fifo_BIT    6
#define FXLS8471Q_CTRL_REG5_trans_BIT   5
#define FXLS8471Q_CTRL_REG5_lndprt_BIT  4
#define FXLS8471Q_CTRL_REG5_pulse_BIT   3
#define FXLS8471Q_CTRL_REG5_ffmt_BIT    2
#define FXLS8471Q_CTRL_REG5_avecm_BIT   1
#define FXLS8471Q_CTRL_REG5_drdy_BIT    0

#endif	/* FXLS8471Q_REGISTERS_H */

