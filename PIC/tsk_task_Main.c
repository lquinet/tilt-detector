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
#include "tsk_task_Main.h"
#include "user.h"
#include "drivers/drv_i2c.h"
#include "drivers/drv_rs.h"
#include "NDEF/NDEFMessage.h"
#include "NDEF/NDEFRecord.h"
#include "Sensors/M24LR04E_R.h"
#include "Sensors/EMC1001.h"
#include "RTCC/MyRTCC.h"
#include "sensors/FXLS8471Q.h"
#include "sensors/FXLS8471Q_registers.h"

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
    uint8_t statusPackage;
    boolean isRF_WIP_BUSY = 0;
    boolean isTempExceeded = 0;
    
    #ifdef DEBUG_M24LR04E_R
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_STATUS_PACKAGE;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress,0);
    statusPackage = M24LR04E_ReadOneByte(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress);
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_DATE_RTC;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 13);
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_DATE_RTC+1;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 4);
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_DATE_RTC+2;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 16);
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_DATE_RTC+3;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 15);
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_DATE_RTC+4;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 0);
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_DATE_RTC+5;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 0);
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_TEMP_LIMITS;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress,0b11111111);
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_TEMP_LIMITS+1;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress,0b00000000);
    
    subAddress.LongNb = 0;
    M24LR04E_ReadBuffer(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 70, value);
    
    #endif

    // Init peripherals
    InitSTTS751();
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

    // Copy Temperature limits
    tempMax = configBytes.tempMax;
    
    // Start RTCC
    StartRTCC(configBytes.DateTime);

    // DEBUG
    /*
    Xacc.LongNb = 0xA051;
    Yacc.LongNb = 0xA052;
    Zacc.LongNb = 0xA053;
    Acc_event = 0x01;
    FXLS8471QSaveNdefMessage( Xacc,  Yacc,  Zacc,  Acc_event);
    subAddress.LongNb = 0;
    M24LR04E_ReadBuffer(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 70, value);
    */
    
    /*
    //strcpypgm2ram(str, "Aest 1: est-ce que ca marche??");
    M24LR04E_SaveNdefMessage(data, "en", &My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS);

    strcpypgm2ram(str, "Best 2: Oui ca a marcheeee!");
    //M24LR04E_SaveNdefMessage(str, "en", &My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
    
    
    M24LR04E_ReadBuffer(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 70, value);
    */
    
    /*
    ReadTemperatureSTTS751(&data);
    data.temp.LongNb=0b1111111110000000;
    tempMax = ConvertTemperatureSTTS751(data.temp);
    */
    
    /*
    // Read Date and time from e²p memory
    address.LongNb = M24LR16_EEPROM_ADDRESS_DATE_RTC;
    M24LR04E_ReadBuffer(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS, address,6,DateTime);
    // Convert the array DateTime in BCD
    convertchartoBCD(DateTime, 6);
    */
    
    // END DEBUG
    
    
    while (1)
    {
        WaitEvent(RTCC_EVENT | M24LR04E_EVENT| ACCEL_EVENT);
        GetEvent(TASK_Main_ID, &TASK_Main_event);
        
        // RTCC EVENT
        if (TASK_Main_event & RTCC_EVENT)
        {
            ClearEvent(RTCC_EVENT );
            // Toggle LED (toutes les 2 sec)
            if (counterRTCC%2 == 0) {
                if (statusPackage == ColisDown)
                {
                    LedGreen = 0;
                    LedRed = 1;
                    Delay_ms(100);
                    LedRed = 0;
                } else
                    if (statusPackage == ColisUP)
                {
                    LedRed = 0;
                    LedGreen = 1;
                    Delay_ms(100);
                    LedGreen = 0;
                }
            }
            
            // Reading Temperature every 60 sec
            if (counterRTCC%60 == 0){
                // ROUTINE TEMPERATURE
                ReadTemperatureSTTS751(&temperatureIntTo8);
                temperatureFloat = ConvertTemperatureSTTS751(temperatureIntTo8);
                if (isTempExceeded == 0){
                    if (temperatureFloat > tempMax){
                        isTempExceeded = 1;
                        STTS751SaveNdefMessage(temperatureIntTo8);
                        #ifdef DEBUG_M24LR04E_R
                        subAddress.LongNb = 0;
                        M24LR04E_ReadBuffer(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 70, value);
                        #endif
                    }
                }
                else {
                    if (temperatureFloat < tempMax){
                        isTempExceeded = 0;
                        STTS751SaveNdefMessage(temperatureIntTo8);
                        #ifdef DEBUG_M24LR04E_R
                        subAddress.LongNb = 0;
                        M24LR04E_ReadBuffer(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 70, value);
                        #endif
                    }
                }
                
            } 
            
            // Write DateTime to e²p every 10 min
            if (counterRTCC == 600) {
                counterRTCC = 1;
                writeDateTimeToConfigurationByte(); // Pour éviter que le transporteur ait retiré la pile pendant le trajet
            }
            
            // Check if RF_Change
            if (isRF_WIP_BUSY){
                // disable isRF_WIP_BUSY
                isRF_WIP_BUSY = 0;
                // Enable INT1 interruptions
                INTCON3bits.INT1IE = 1;
                
                // Read user configuration from e²p memory
                M24LR04E_ReadConfigurationBytes(&configBytes);
                
                // Check if RF change (change config or reset)
                if (configBytes.RF_Change == RF_Change_WithoutReset || configBytes.RF_Change == RF_Change_Reset){
                    // Status Package
                    if (configBytes.RF_Change == RF_Change_Reset){
                        // clear RF_CHANGE byte in e²p
                        subAddress.LongNb = M24LR16_EEPROM_ADDRESS_RF_CHANGE;
                        M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress,0);

                        // RESET
                        Reset();
                    }
                    if (configBytes.RF_Change == RF_Change_WithoutReset){
                        // clear RF_CHANGE byte in e²p
                        subAddress.LongNb = M24LR16_EEPROM_ADDRESS_RF_CHANGE;
                        M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress,0);

                        // Copy accelerations limits
                        XaccMax.LongNb = configBytes.XaccMax.LongNb;
                        YaccMax.LongNb = configBytes.YaccMax.LongNb;
                        ZaccMax.LongNb = configBytes.ZaccMax.LongNb;

                        // Copy Temperature limits
                        tempMax = configBytes.tempMax;
                    }
                }
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

        //Configure Sleep Mode
        OSCCONbits.IDLEN = 0; // Not in Idle
        WDTCONbits.REGSLP=1; // Regulator Low power
        Sleep();
    }//Fin while WaitEvent

}

/**********************************************************************
 * Generic routine to create a delay of many milliseconds.
 *
 * @param  delay     IN  time to wait in ms
 * @return void
 **********************************************************************/
void Delay_ms(unsigned int delay)
{
    CancelAlarm(ALARM_TASK_Main_Delay);
    SetRelAlarm(ALARM_TASK_Main_Delay, delay, 0);
    WaitEvent(DELAY_EVENT);
    GetEvent(TASK_Main_ID, &TASK_Main_event);
    if (TASK_Main_event & DELAY_EVENT)
        ClearEvent(DELAY_EVENT);
    return;
}    


/* End of File : TASK_Main.c */
