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
#include  <dpslp.h> // Pour librairie RTCC

/**********************************************************************
 * Definition dedicated to the local function
 **********************************************************************/
void Delay_ms(unsigned int delay);

/**********************************************************************
 * Definition dedicated to the Global variables
 **********************************************************************/
char value[80]="";
I2C_message_t My_I2C_Message;
EventMaskType TASK_Main_event; // Utilisé pour récupérer Evènement en cours

// To contain the date
uint8_t DateTime[6]="\0";

// Maximum Temperature
float tempMax=0;

// Maximum acceleration
IntTo8_t accXMax=0;
IntTo8_t accYMax=0;
IntTo8_t accZMax=0;

// Structure to send NDEF message
data_t data;

/**********************************************************************
 ------------------------------- TASK_Main ----------------------------
 **********************************************************************/

TASK(TASK_Main)
{
    char str[30] = "";
    IntTo8_t address = 0;
    IntTo8_t subAddress = 0;
    IntTo8_t temperatureIntTo8 = 0;
    float temperatureFloat=0;
    uint8_t counterRTCC=0, counterRF_WIP_BUSY=0;
    uint8_t StatusPackage=52;
    boolean isRF_WIP_BUSY = 0;
    uint8_t configurationBytes[24];
    uint8_t errorState;
            
    //subAddress.LongNb = 0;
    //data.type_message= TYPE_TEMP; data.day=30; data.month=3; data.year=16; data.hour=15; data.min=43; data.temp.LongNb=26;
    
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_STATUS_PACKAGE;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress,0);
    StatusPackage = M24LR04E_ReadOneByte(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress);
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_DATE_RTC;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 9);
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_DATE_RTC+1;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 4);
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_DATE_RTC+2;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 16);
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_DATE_RTC+3;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 16);
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_DATE_RTC+4;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 8);
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_DATE_RTC+5;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 0);
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_TEMP_LIMITS;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress,0b11111111);
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_TEMP_LIMITS+1;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress,0b00000000);
    
    // INIT
    subAddress.LongNb=M24LR16_EEPROM_ADDRESS_STATUS_PACKAGE;
    StatusPackage = M24LR04E_ReadOneByte(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress);
    InitSTTS751();
    M24LR04E_Init();

    
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
    
    // Read user configuration from e²p memory
    address.LongNb = M24LR16_EEPROM_LAST_ADDRESS_DATALOGGER + 1;
    errorState = M24LR04E_ReadBuffer(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS, address, 24, configurationBytes);
    if (errorState == E_OK){
        // Status Package
        StatusPackage = configurationBytes[0];
        
        // DateTime
        memcpy(DateTime, configurationBytes+4, 6);
        convertCharArrayToBCD(DateTime, 6);
        
        // Accelerations limits
        accXMax.Nb8_B[1] = configurationBytes[12];
        accXMax.Nb8_B[0] = configurationBytes[13];
        accYMax.Nb8_B[1] = configurationBytes[14];
        accYMax.Nb8_B[0] = configurationBytes[15];
        accZMax.Nb8_B[1] = configurationBytes[16];
        accZMax.Nb8_B[0] = configurationBytes[17];
        
        // Temperature limits
        temperatureIntTo8.Nb8_B[1] = configurationBytes[20];
        temperatureIntTo8.Nb8_B[0] = configurationBytes[21];
        tempMax = ConvertTemperatureSTTS751(temperatureIntTo8);
    }
    
    StartRTCC(DateTime);

    while (1)
    {
        WaitEvent(RTCC_EVENT | M24LR04E_EVENT);
        GetEvent(TASK_Main_ID, &TASK_Main_event);
        
        // RTCC EVENT
        if (TASK_Main_event & RTCC_EVENT)
        {
            ClearEvent(RTCC_EVENT );
            counterRTCC++;
            counterRF_WIP_BUSY++;
            // Reading Temperature
            if (counterRTCC == 60)
            {
                counterRTCC = 0;
                // ROUTINE TEMPERATURE
                ReadTemperatureSTTS751(&temperatureIntTo8);
                temperatureFloat = ConvertTemperatureSTTS751(temperatureIntTo8);
                if (temperatureFloat > tempMax){
                    STTS751SaveNdefMessage(temperatureIntTo8);
                    subAddress.LongNb = 0;
                    M24LR04E_ReadBuffer(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 70, value);
                }
            } 
            // Toggle LED (toutes les 2 sec)
            if (counterRTCC%2 == 1) {
                if (StatusPackage == ColisDown)
                {
                    LedGreen = 0;
                    LedRed = 1;
                 // if(mRtccGetAlarmRpt()==);
                    Delay_ms(100);
                    LedRed = 0;
                } else
                    if (StatusPackage == ColisUP)
                {
                    LedRed = 0;
                    LedGreen = 1;
                    Delay_ms(100);
                    LedGreen = 0;
                }
            }
            // Check if RF_Change
            if (isRF_WIP_BUSY){
                // Enable INT1 interruptions
                INTCON3bits.INT1IE = 1;
                // Read user configuration from e²p memory
                address.LongNb = M24LR16_EEPROM_LAST_ADDRESS_DATALOGGER + 1;
                errorState = M24LR04E_ReadBuffer(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS, address, 24, configurationBytes);
                if (errorState == E_OK){
                    // Check if RF change
                    if (configurationBytes[1] == RF_Change){
                        // Status Package
                        if (StatusPackage == ColisDown && configurationBytes[0] == ColisUP){
                            // RESEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEET!
                        }
                        // Accelerations limits
                        accXMax.Nb8_B[1] = configurationBytes[12];
                        accXMax.Nb8_B[0] = configurationBytes[13];
                        accYMax.Nb8_B[1] = configurationBytes[14];
                        accYMax.Nb8_B[0] = configurationBytes[15];
                        accZMax.Nb8_B[1] = configurationBytes[16];
                        accZMax.Nb8_B[0] = configurationBytes[17];

                        // Temperature limits
                        temperatureIntTo8.Nb8_B[1] = configurationBytes[20];
                        temperatureIntTo8.Nb8_B[0] = configurationBytes[21];
                        tempMax = ConvertTemperatureSTTS751(temperatureIntTo8);
                    }
                }
            }
        } // Fin if RTCC_EVENT
        
        // M24LR04E_EVENT
        if (TASK_Main_event & M24LR04E_EVENT){
            ClearEvent(M24LR04E_EVENT);
            isRF_WIP_BUSY = 1;
            // Disable INT1 interruptions
            INTCON3bits.INT1IE = 0;
            //LedRed = ~LedRed; //~LATDbits.LATD7;
        }

        // Disable timer0
        //T0CONbits.TMR0ON = 0;
        //Configure Sleep Mode
        //SLEEP
        OSCCONbits.IDLEN = 0; // Not in Idle
        WDTCONbits.REGSLP=1; // Regulator Low power
        //Sleep();
    }//Fin while WaitEvent
}


// Convertion of a BCD number in decimal
uint8_t BcdHexToBcdDec(uint8_t Nb)
{
    uint8_t Dec = 0;
    Dec = (Nb & 0xF0) >> 4;
    Dec = 10 * Dec;
    Dec = Dec + (Nb & 0x0F);
    return Dec;
}

// Convertion of a decimal number in BCD
uint8_t convertCharToBCD(uint8_t toConvert)
{
	uint8_t dizaine = 0;
	uint8_t unite = 0;
  
    // On récupère l'unité du nombre par un modulo
    unite = toConvert % 10;
    // On récupère la dizaine du nombre
    dizaine = toConvert /10; 

    return ((dizaine << 4) | (unite));
}

// Convertion of an array of decimal numbers in BCD
void convertCharArrayToBCD(uint8_t *arrayToConvert, uint8_t length)
{
	uint8_t dizaine = 0;
	uint8_t unite = 0;
	uint8_t i;
    
    for (i=0; i<length; i++){
        // On récupère l'unité du nombre par un modulo
        unite = arrayToConvert[i] % 10;
        // On récupère la dizaine du nombre
        dizaine = arrayToConvert[i]/10; 
        
        arrayToConvert[i] = ((dizaine << 4) | (unite));
    }
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
