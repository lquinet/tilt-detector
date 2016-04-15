/**********************************************************************/
/*                                                                    */
/* File name: define.h                                                */
/*                                                                    */
/* Since:     2004-Sept-20                                            */
/*                                                                    */
/* Version:   PICos18 v3.00                                           */
/*            Copyright (C) 2003, 2004, 2005 Pragmatec.               */
/*                                                                    */
/* Author:    Designed by Pragmatec S.A.R.L.        www.pragmatec.net */
/*            MONTAGNE Xavier [XM]      xavier.montagne@pragmatec.net */
/*            NIELSEN  Peter  [PN]                   pnielsen@tuug.fi */
/*            KUSHNIR  Sergey [SK]                     icd2usb@tut.by */
/*                                                                    */
/* Purpose:   Specify all the specific definitions of the project.    */
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
/* 2004/11/06 [RZR] Original idea of RZR.                             */
/* 2007/01/01 [PN]  Added magic formula dedicated to clock frequency. */
/* 2007/11/14 [SK]  Let the compiler computes the formula.            */
/*                                                                    */
/**********************************************************************/

#ifndef _DEFINE_H
#define _DEFINE_H


#include "device.h"

#define DEBUG

/***********************************************************************
 * ------------------------ Timer settings -----------------------------
 **********************************************************************/
/* LMAGIC : Tmr0.lt = 65536 - (CPU_FREQUENCY_HZ/4/1000)               */
#define OSCILLATOR_FREQ  8                                        // MHz
#define TMR0_PRESET      (65536-((unsigned int)(OSCILLATOR_FREQ*1000)/4))

/***********************************************************************
 * ----------------------------- Events --------------------------------
 **********************************************************************/
//DRV_I2C
#define I2C_NEW_MSG         0x02
#define I2C_QUEUE_EMPTY     0x20
#define BUSERROR_EVENT      0x08
#define IDLE_EVENT          0x10
#define TIMEOUT_EVENT       0x08

//TASK Main
#define DELAY_EVENT       	0x02
#define RTCC_EVENT          0x22
#define M24LR04E_EVENT      0x04
//#define TEST_EVENT       	0x05
#define ACCEL_EVENT       	0x05


//DRV_RS
#define RS_NEW_MSG          0x10
#define RS_RCV_MSG          0x20
#define RS_QUEUE_EMPTY      0x10
#define RS_QUEUE_FULL       0x20

/***********************************************************************
 * ----------------------------- Task ID -------------------------------
 **********************************************************************/
#define TASK_Main_ID        1
#define RS_DRV_ID           2
#define	I2C_DRV_ID          3

/***********************************************************************
 * ----------------------------- Task ALARM ---------------------------
 * ID de l'alarme programmée.
 * Index du tableau Alarm_list dans le fichier tascdesc.c.
 **********************************************************************/
#define ALARM_TASK_Main_Delay 	0
#define I2C_TIMEOUT_ALARM       2


/***********************************************************************
 * ----------------------------- Task PRIO ---------------------------
 **********************************************************************/

#define TASK_Main_PRIO      3
#define I2C_DRV_PRIO        4
#define RDV_RS_PRIO         6

#endif /* _DEFINE_H */

/* End of File : define.h */
