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

#define FXLS8471Q_MODE_STANDBY  0
#define FXLS8471Q_MODE_WAKE     1
#define FXLS8471Q_MODE_SLEEP    2


/************************************************************************/
/* Functions prototype                                                  */
/************************************************************************/
extern void fxls8471q_init(void);
extern void fxls8471q_switchMode(uint8_t mode);


#endif	/* FXLS8471Q_H */

