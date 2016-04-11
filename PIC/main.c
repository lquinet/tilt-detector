/**********************************************************************/
/*                                                                    */
/* File name: main.c                                                  */
/*                                                                    */
/* Since:     2004-Sept-20                                            */
/*                                                                    */
/* Version:   PICos18 v3.00                                           */
/*            Copyright (C) 2003, 2004, 2005 Pragmatec.               */
/*                                                                    */
/* Author:    Designed by Pragmatec S.A.R.L.        www.pragmatec.net */
/*            MONTAGNE Xavier [XM]      xavier.montagne@pragmatec.net */
/*                                                                    */
/* Purpose:   Custom initialization and kernel start.                 */
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

// CONFIG1L
#pragma config WDTEN = OFF      // Watchdog Timer (Disabled - Controlled by SWDTEN bit)
#pragma config PLLDIV = 1       // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset (Enabled)
#pragma config XINST = OFF      // Extended Instruction Set (Disabled)

// CONFIG1H
#pragma config CPUDIV = OSC1    // CPU System Clock Postscaler (No CPU system clock divide)
#pragma config CP0 = OFF        // Code Protect (Program memory is not code-protected)

// CONFIG2L
#pragma config OSC = HS         // Oscillator (HS, USB-HS)
#pragma config T1DIG = ON       // T1OSCEN Enforcement (Secondary Oscillator clock source may be selected)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator (High-power operation)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor (Enabled)
#pragma config IESO = OFF        // Internal External Oscillator Switch Over Mode (Enabled)

// CONFIG2H
#pragma config WDTPS = 32768    // Watchdog Postscaler (1:32768)

// CONFIG3L
#pragma config DSWDTOSC = INTOSCREF// DSWDT Clock Select (DSWDT uses INTRC)
#pragma config RTCOSC = T1OSCREF// RTCC Clock Select (RTCC uses T1OSC/T1CKI)
#pragma config DSBOREN = OFF     // Deep Sleep BOR (Enabled)
#pragma config DSWDTEN = OFF     // Deep Sleep Watchdog Timer (Enabled)
#pragma config DSWDTPS = G2     // Deep Sleep Watchdog Postscaler (1:2,147,483,648 (25.7 days))

// CONFIG3H
#pragma config IOL1WAY = ON     // IOLOCK One-Way Set Enable bit (The IOLOCK bit (PPSCON<0>) can be set once)
#pragma config MSSP7B_EN = MSK7 // MSSP address masking (7 Bit address masking mode)

// CONFIG4L
#pragma config WPFP = PAGE_63   // Write/Erase Protect Page Start/End Location (Write Protect Program Flash Page 63)
#pragma config WPEND = PAGE_WPFP// Write/Erase Protect Region Select (valid when WPDIS = 0) (Page WPFP<5:0> through Configuration Words erase/write protected)
#pragma config WPCFG = OFF      // Write/Erase Protect Configuration Region (Configuration Words page not erase/write-protected)

// CONFIG4H
#pragma config WPDIS = OFF      // Write Protect Disable bit (WPFP<5:0>/WPEND region ignored)

#include "define.h"
#include "user.h"

/**********************************************************************
 * Definition dedicated to the local functions.
 **********************************************************************/
#define DEFAULT_MODE       0

/**********************************************************************
 * Function prototypes.
 **********************************************************************/
void Init(void);

void PIN_MANAGER_Initialize(void);

extern union Timers Tmr0;

AppModeType SelectedMode;

/**********************************************************************
 * -------------------------- main function ---------------------------
 *
 * Setup the different alarms and start the kernel.
 *
 **********************************************************************/
void main(void)
{
    STKPTR = 0;
    SelectedMode = DEFAULT_MODE;
    Init();

    while (1)
    {
        StartOS(SelectedMode);
    }
}

/**********************************************************************
 * Clear all RAM memory and set PORTB to output mode.
 *
 * @return void
 **********************************************************************/
void Init(void)
{
    FSR0H = 0;
    FSR0L = 0;
    /* User setting : actual PIC frequency */
    Tmr0.lt = TMR0_PRESET;
    /* Timer OFF - Enabled by Kernel */
    T0CON = 0x08;
    TMR0H = Tmr0.bt[1];
    TMR0L = Tmr0.bt[0];

    //*****************************************
    //************Init IO's *********************
    //*****************************************

    PIN_MANAGER_Initialize();

}

void PIN_MANAGER_Initialize(void)
{

    //Set all analog input as digital input.
    ANCON0=0xFF ; //Enable AN0-AN7 in Digital mode
    ANCON1=0x7F ; //Enable AN8-AN15 in Digital mode

    // I/O PORTS
    TRISLedGreen=0;LedGreen=0;
    TRISLedRed=0;LedRed=0;
    
    //*****************************************
    //************* Intrerruption *************
    //*****************************************
    IPR1 = 0;  IPR2 = 0;   IPR3 = 0;//Toutes les interruptions en low priority
    //	RCON		//RESET CONTROL REGISTER
    RCONbits.IPEN = 1; //RESET CONTROL REGISTER

    //	INTCON
    INTCONbits.GIE = 1; //Global Interrupt Enable bit
    INTCONbits.PEIE = 1; //Peripheral Interrupt Enable bit
    INTCONbits.INT0IE=1;
}


/**********************************************************************
 * Hook routine called just before entering in kernel.
 *
 * @param error      IN The new error stored in buffer
 * @return error     Error level
 **********************************************************************/
void StartupHook(void)
{
}

/**********************************************************************
 * Hook routine called just after leaving the kernel.
 *
 * @param error      IN The last error detected by OS
 * @return void
 **********************************************************************/
void ShutdownHook(StatusType error)
{
}

/**********************************************************************
 * Store a new error in a global buffer keeping a track of the 
 * application history.
 *
 * @param error      IN The new error stored in buffer
 * @return void
 **********************************************************************/
void ErrorHook(StatusType error)
{
}

/**********************************************************************
 * Hook routine called just before entering in a task.
 *
 * @return void
 **********************************************************************/
void PreTaskHook(void)
{
}

/**********************************************************************
 * Hook routine called just after leaving a task.
 *
 * @return void
 **********************************************************************/
void PostTaskHook(void)
{
}

/* End of File : main.c */
