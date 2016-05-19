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
#include "NDEF/NDEFMessage.h"
#include "NDEF/NDEFRecord.h"
#include "Sensors/M24LR04E_R.h"
#include "Sensors/EMC1001.h"
#include "RTCC/MyRTCC.h"
#include "sensors/FXLS8471Q.h"

extern boolean isMemoryFull; // TRUE if e²p memory is full

/**********************************************************************
 * Definition dedicated to the Global variables
 **********************************************************************/
I2C_message_t My_I2C_Message;
EventMaskType TASK_Main_event; // Utilisé pour récupérer Evènement en cours

// To contain the date
uint8_t DateTime[6]="\0";

// Maximum Temperature
float tempMax=0;

// Maximum acceleration
IntTo8_t XaccMax=0;
IntTo8_t YaccMax=0;
IntTo8_t ZaccMax=0;

// Structure to send NDEF message
NDEFPayload_t data;

// Status of the package (UP or DOWN)
uint8_t statusPackage;


#ifdef DEBUG_M24LR04E_R
char value[80]="";
#endif

/**********************************************************************
 ------------------------------- TASK_Main ----------------------------
 **********************************************************************/

TASK(TASK_Main)
{
    _ConfigBytes_t configBytes;
    IntTo8_t subAddress = 0;
    IntTo8_t temperatureIntTo8;
    float temperatureFloat=0;
    uint16_t counterRTCC=1;
    boolean isRF_WIP_BUSY = 0;
    boolean isTempExceeded = 0;
    uint8_t RF_ChangeByte;
    uint8_t Thresold_X_Y_Z = 0x10;

    // Init peripherals
    emc1001_init();
    M24LR04E_Init();
	fxls8471q_init();

    // Read user configuration from e²p memory
    M24LR04E_ReadConfigurationBytes(&configBytes);
    
    // Copy Status Package
    statusPackage = configBytes.statusPackage;
    
    // Copy accelerations limits
    XaccMax.LongNb = configBytes.XaccMax.LongNb;
    YaccMax.LongNb = configBytes.YaccMax.LongNb;
    ZaccMax.LongNb = configBytes.ZaccMax.LongNb;
    Thresold_X_Y_Z = configBytes.Thresold_X_Y_Z;
    fxls8471q_switchMode(FXLS8471Q_MODE_STANDBY); //switch to standby mode
    fxls8471q_configureMotionDetection(Thresold_X_Y_Z);
    fxls8471q_switchMode(FXLS8471Q_MODE_WAKE);
    
    // Copy Temperature limits
    tempMax = configBytes.tempMax;

    // Start RTCC
    StartRTCC(configBytes.DateTime);
      
    while (1)
    {
        WaitEvent(RTCC_EVENT | M24LR04E_EVENT| ACCEL_EVENT |TIMER1_EVENT);
        GetEvent(TASK_Main_ID, &TASK_Main_event);
           
        
        // RTCC EVENT
        if (TASK_Main_event & RTCC_EVENT)
        {
            ClearEvent(RTCC_EVENT );
            // Toggle LED (toutes les 2 sec)
            if (counterRTCC%2 == 0) {
                if (!isMemoryFull){
                    if (statusPackage == ColisDown)
                    {
                        LedRed = 1;
                        TMR1H=0xF9;
                        TMR1L=0xA4;
                        T1CONbits.TMR1ON=1; // Enables Timer1
                        //TRISCbits.TRISC2 = 0;
                        //PORTCbits.RC2 = 1;
                    } else
                        if (statusPackage == ColisUP)
                    {
                        LedGreen = 1;
                        TMR1H=0xF9;
                        TMR1L=0xA4;
                        T1CONbits.TMR1ON=1; // Enables Timer1*/
                    }
                }
                else { // E²p memory is full!!! The led blink in orange
                    LedGreen = 1;
                    LedRed = 1;
                    TMR1H=0xF9;
                    TMR1L=0xA4;
                    T1CONbits.TMR1ON=0x01; // Enables Timer1
                }
            }
            
            // Check if RF_Change every 20 sec to avoid collision with RF
            if (counterRTCC%20 == 0){
                // Check if RF_Change
                if (isRF_WIP_BUSY){
                    
                    // disable isRF_WIP_BUSY
                    isRF_WIP_BUSY = 0;
                    // Enable INT1 interruptions
                    INTCON3bits.INT1IE = 1;
                    INTCON3bits.INT1IF = 0;
                    
                    // Wait RF communication stop
                    while(!RF_WIP_BUSY);

                    // Read RF_Change byte in e²p
                    RF_ChangeByte = M24LR04E_ReadRF_Change();

                    if(RF_ChangeByte == RF_Change_WithoutReset || RF_ChangeByte == RF_Change_Reset){
                        // Read user configuration from e²p memory
                        M24LR04E_ReadConfigurationBytes(&configBytes);

                        // Check if RF change (change config or reset)
                        if (configBytes.RF_Change == RF_Change_Reset){
                            // RESET
                            Reset();
                        }
                        else {
                            // clear RF_CHANGE byte in e²p
                            subAddress.LongNb = M24LR16_EEPROM_ADDRESS_RF_CHANGE;
                            M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress,0);

                            // Copy accelerations limits
                            XaccMax.LongNb = configBytes.XaccMax.LongNb;
                            YaccMax.LongNb = configBytes.YaccMax.LongNb;
                            ZaccMax.LongNb = configBytes.ZaccMax.LongNb;
                            Thresold_X_Y_Z = configBytes.Thresold_X_Y_Z;
                            fxls8471q_switchMode(FXLS8471Q_MODE_STANDBY); //switch to standby mode
                            fxls8471q_configureMotionDetection(Thresold_X_Y_Z);
                            fxls8471q_switchMode(FXLS8471Q_MODE_WAKE);
                            // Copy Temperature limits
                            tempMax = configBytes.tempMax;
                        }
                    }
                }
            }
            
            // Reading Temperature every 60 sec
            if (counterRTCC%60 == 0){
                // ROUTINE TEMPERATURE
                emc1001_readTemperature(&temperatureIntTo8);
                temperatureFloat = emc1001_convertTemperature(temperatureIntTo8);
                if (isTempExceeded == 0){
                    if (temperatureFloat > tempMax){
                        isTempExceeded = 1;
                        EMC1001SaveNdefMessage(temperatureIntTo8);
                        SetStatusPackageDown (&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
                    }
                }
                else if (temperatureFloat < tempMax){
                        isTempExceeded = 0;
                        EMC1001SaveNdefMessage(temperatureIntTo8);
                    }
                }
                
            
            
            // Write DateTime to e²p every 10 min
            if (counterRTCC == 600) {
                counterRTCC = 1;
                writeDateTimeToConfigurationByte(); // Pour éviter que le transporteur ait retiré la pile pendant le trajet
            }
                        
            counterRTCC++;
        } // Fin if RTCC_EVENT
        
        // M24LR04E_EVENT
        if (TASK_Main_event & M24LR04E_EVENT){
            ClearEvent(M24LR04E_EVENT);
            // Disable INT1 interruptions
            INTCON3bits.INT1IE = 0;
            isRF_WIP_BUSY = 1;
        }
		 // ACCEL_EVENT
        if (TASK_Main_event & ACCEL_EVENT){
			ClearEvent(ACCEL_EVENT);
			fxls8471q_checkSourceInterrupt();
        }
        // TIMER1_EVENT
        if (TASK_Main_event & TIMER1_EVENT){
            ClearEvent(TIMER1_EVENT );
            //PORTCbits.RC2 = 0;
        }
        
        //Configure Sleep Mode
        OSCCONbits.IDLEN = 0; // Not in Idle
        WDTCONbits.REGSLP=1; // Regulator Low power
        Sleep();
        
    }//Fin while WaitEvent

}

/* End of File : TASK_Main.c */
