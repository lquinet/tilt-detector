/**********************************************************************/
/*                                                                    */
/* File name: int.c                                                   */
/*                                                                    */
/* Since:     2004-Sept-20                                            */
/*                                                                    */
/* Version:   PICos18 v3.00                                           */
/*            Copyright (C) 2003, 2004, 2005 Pragmatec.               */
/*                                                                    */
/* Author:    Designed by Pragmatec S.A.R.L.        www.pragmatec.net */
/*            MONTAGNE Xavier [XM]      xavier.montagne@pragmatec.net */
/*            KUSHNIR  Sergey [KS]                     icd2usb@tut.by */
/*                                                                    */
/* Purpose:   Interrupt vector location.                              */
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
/* 2004/12/01 [XM]  Create this file.                                 */
/* 2005/10/29 [XM]  Removed the "save" statement for InterruptL.      */
/*                  Added "nosave" statement for InterruptH.          */
/* 2007/10/30 [KS]  Removed the InterruptVectorL statement for C18    */
/*                  error reasons.Replaced by SAVE(RESTORE)_TASK_CTX. */
/*                                                                    */
/**********************************************************************/
#include "define.h"
#include "drivers/drv_i2c.h"
#include "drivers/drv_rs.h"
#include "user.h"


/**********************************************************************
 * Function you want to call when an IT occurs.
 **********************************************************************/
extern unsigned char stack_low, stack_high;
extern void AddOneTick(void);
void InterruptVectorL(void);
void InterruptVectorH(void);

// I2C
extern void I2C_INT(void);
// UART
extern void RS_RX_INT(void);
extern void RS_TX_INT(void);

/**********************************************************************
 * General interrupt vector. Do not modify.
 **********************************************************************/
#pragma code IT_vector_low=0x18

void Interrupt_low_vec(void)
{
    _asm goto InterruptVectorL _endasm
}
#pragma code

#pragma code IT_vector_high=0x08

void Interrupt_high_vec(void)
{
    _asm goto InterruptVectorH _endasm
}
#pragma code

/**********************************************************************
 * General ISR router. Complete the function core with the if or switch
 * case you need to jump to the function dedicated to the occuring IT.
 * .tmpdata and MATH_DATA are saved automaticaly with C18 v3.
 **********************************************************************/
#pragma	code _INTERRUPT_VECTORL = 0x003000

void InterruptVectorL(void)
{
    SAVE_TASK_CTX(stack_low, stack_high);
    EnterISR();
    if (INTCONbits.TMR0IF == 1)
    {
        AddOneTick();
    }

    //******************************************************
    /*Here is the next interrupts you want to manage */
    //************************************************

    if ((PIR2bits.BCLIF == 1) || // Check bus collision(bit3)
        (PIR1bits.SSPIF == 1)) // Check I2C interrupt	(bit3)
    {
        I2C_INT();
    }
    if ((PIR1bits.RCIF)&(PIE1bits.RCIE))
        RS_RX_INT();
    if ((PIR1bits.TXIF)&(PIE1bits.TXIE))
        RS_TX_INT();

    LeaveISR();
    RESTORE_TASK_CTX;
}
#pragma	code

/* BE CARREFULL : ONLY BSR, WREG AND STATUS REGISTERS ARE SAVED  */
/* DO NOT CALL ANY FUNCTION AND USE PLEASE VERY SIMPLE CODE LIKE */
/* VARIABLE OR FLAG SETTINGS. CHECK THE ASM CODE PRODUCED BY C18 */
/* IN THE LST FILE.                                              */
#pragma	code _INTERRUPT_VECTORH = 0x2000//0x003500
#pragma interrupt InterruptVectorH nosave=FSR0, TBLPTRL, TBLPTRH, TBLPTRU, TABLAT, PCLATH, PCLATU, PROD, section(".tmpdata"), section("MATH_DATA")

void InterruptVectorH(void)
{
    if (INTCONbits.INT0IF == 1)
    {
        INTCONbits.INT0IF = 0;
    }
}
#pragma	code

extern void _startup(void);
#pragma code _RESET_INTERRUPT_VECTOR =0x2100// 0x003600

void _reset(void)
{
    _asm goto _startup _endasm
}
#pragma code


/* End of file : int.c */
