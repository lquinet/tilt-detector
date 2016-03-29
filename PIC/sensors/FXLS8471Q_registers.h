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

#define FXLS8471Q_ADDRESS       0x1E
#define FXLS8471Q_DEVICE_ID     0x6A

#define FXLS8471Q_WHO_AM_I      0x0D  
#define FXLS8471Q_WHO_AM_I_BIT        7
#define FXLS8471Q_WHO_AM_I_LENGTH     8

#define FXLS8471Q_CTRL_REG1     0x2A   
#define FXLS8471Q_CTRL_REG1_active_BIT     0 
#define FXLS8471Q_CTRL_REG1_active_LENGTH  1

#define FXLS8471Q_CTRL_REG2     0x2B
#define FXLS8471Q_CTRL_REG3     0x2C
#define FXLS8471Q_CTRL_REG4     0x2D
#define FXLS8471Q_CTRL_REG5     0x2E

#endif	/* FXLS8471Q_REGISTERS_H */

