/**********************************************************************/
/*                                                                   */
/* File name: TASK_Main.c                                             */
/*                                                                    */
/* Since:     2004-Sept-20                                            */
/*                                                                    */
/* Version:   PICos18 v3.00                                           */
/*            Copyright (C) 2003, 2004, 2005 Pragmatec.               */
/*                                                                    */
/* Author:    Designed by Pragmatec S.A.R.L.        www.pragmatec.net */
/*            MONTAGNE Xavier [XM]      xavier.montagne@pragmatec.net */
/*                                                                    */
/* Purpose:   First task of the tutorial.                             */
/*                                                                    */
/* Distribution: This file is part of PICos18.                        */
/*            PICos18 is free software; you can redistribute it       */
/*            and/or modify it under the terms of the GNU General     */
/*            Public License as published by the Free Software        */
/*            Foundation; either version 2, or (at your option)       */
/*            any later version.                                      */
/*                                                                    */
/*            PICos18 is distributed in the hope that it will be      */
/*            useful, but WITHOUT ANY WARRANTY; without even the      */
/*            implied warranty of MERCHANTABILITY or FITNESS FOR A    */
/*            PARTICULAR PURPOSE.  See the GNU General Public         */
/*            License for more details.                               */
/*                                                                    */
/*            You should have received a copy of the GNU General      */
/*            Public License along with gpsim; see the file           */
/*            COPYING.txt. If not, write to the Free Software         */
/*            Foundation, 59 Temple Place - Suite 330,                */
/*            Boston, MA 02111-1307, USA.                             */
/*                                                                    */
/*          > A special exception to the GPL can be applied should    */
/*            you wish to distribute a combined work that includes    */
/*            PICos18, without being obliged to provide the source    */
/*            code for any proprietary components.                    */
/*                                                                    */
/* History:                                                           */
/* 2004/09/20 [XM]  Create this file.                                 */
/*                                                                    */
/**********************************************************************/
#include <stdlib.h>
#include <string.h>
#include "define.h"
#include "user.h"
#include "drivers/drv_i2c.h"
#include "drivers/drv_rs.h"
#include "sensors/FXLS8471Q.h"


/**********************************************************************
 * Definition dedicated to the local functions.
 **********************************************************************/

/**********************************************************************
 ------------------------------- TASK_Main ----------------------------
 **********************************************************************/

TASK(TASK_Main)
{
    #ifdef DEBUG
	Printf("\r\nTiltDetector initialisation ...\r\n");
	#endif
    
    fxls8471q_init();
    
    while(1){
        ;
    }
    
}
/* End of File : TASK_Main.c */
