/************************************************************************
Title:    EMC1001 Library
Authors:  Julien Delvaux & Loic Quinet
Software: MPLAB X v3.26
Hardware: PIC18F46J50 family
License:  GNU General Public License 3
Usage:    see Doxygen manual

LICENSE:
	Copyright (C) 2016 Julien Delvaux & Loic Quinet

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
 *  @defgroup pic-EMC1001 EMC1001 Library
 *  @code #include <EMC1001.h> @endcode
 * 
 *  @brief
 *
 *
 *  @author Julien Delvaux <delvaux.ju@gmail.com>
 *  @author Loic Quinet <lquinet1@hotmail.com>
 */

#ifndef EMC1001_H
#define	EMC1001_H
#include "user.h"
#include "drivers/drv_i2c.h"
#include "../Sensors/M24LR04E_R.h"

/************************************************************************/
/* Functions prototype                                                  */
/************************************************************************/
/**
 *  @brief   Initialise the EMC1001 in standby and ALERT disable.
 *  @param   none
 *  @return  none
 */
extern void emc1001_init(void);
/**
 *  @brief   Read the temperature from EMC1001.
 *  @param   IntTo8_t pointer to the return value
 *  @return  none
 */
extern void emc1001_readTemperature(IntTo8_t *temperature);
/**
 *  @brief   Convert the temperature given into a float.
 *  @param   IntTo8_t with the value to convert.
 *  @return  float
 */
extern float emc1001_convertTemperature(IntTo8_t temp);

#endif	/* EMC1001_H */

