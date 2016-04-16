#include <rtcc.h>
#include <reset.h>
#include "MyRTCC.h"

//---Global structures used in deep sleep library---
rtccTimeDate RtccTimeDate,  Rtcc_read_TimeDate,RtccAlrmTimeDate;
rtccTime RtccTime;
//extern rtccTimeDate RtccTimeDate;
void StartRTCC(unsigned char value[])
{
    mRtcc_Clear_Intr_Status_Bit; //clears the RTCC interrupt flag bit
    RTCC_configure(value); //Configure RTCC using library APIs
}

void RTCC_configure(unsigned char value[])
{
    unsigned int i = 0, j = 0;
    RtccInitClock(); //turn on clock source
    RtccWrOn(); //write enable the rtcc registers
    mRtccSetClockOe(0); //desable RTCC output on RTCC output pin
    PIE3bits.RTCCIE=1; // enable RTCC interrupt
   /// mRtccSetInt();
    //Set Date and time using global structures defined in libraries
    // WARNING: time in BCD format!!!
    RtccTimeDate.f.hour = value[3];//0x14; //Set Hour
    RtccTimeDate.f.min = value[4];//0x55; //Set minute
    RtccTimeDate.f.sec = value[5]; //Set second
    RtccTimeDate.f.mday = value[0];//0x16; //Set day
    RtccTimeDate.f.mon = value[1];//0x06; //Se month
    RtccTimeDate.f.year = value[2]; //set year
    RtccTimeDate.f.wday = 0x2; //Set which day of the week for the corrsponding date
    RtccWriteTimeDate(&RtccTimeDate, 0); //write into registers
   // RtccAlrmTimeDate.f.sec = RtccTimeDate.f.sec + 10;
    //RtccWriteAlrmTimeDate(&RtccAlrmTimeDate);
    RtccSetAlarmRpt(RTCC_RPT_SEC,1); // RTC Interrupt every seconds
    RtccSetChimeEnable(1,1); // enable the bit CHIME to have interrupts periodically (not a limit number - the ALRMRPT register rollover when reach 0x00)
    RtccSetAlarmRptCount(10,1); //set alarm repeat count (used to have 10 interruptions but CHIME is enabled so we don't care)
    mRtccOn(); //enable the rtcc
    mRtccAlrmEnable(); //enable the rtcc alarm to wake the device up from (deep) sleep
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

// Functions documentation:

/*********************************************************************
  Function:        BOOL RtccWriteTimeDate(const rtccTimeDate* pTD , BOOL di)
 
  PreCondition:    rtccTimeDate structure fields have to have proper values:
                        *sec:  BCD codification, 00-59 
                        *min:  BCD codification, 00-59
                        *hour: BCD codification, 00-24
                        *wday: BCD codification, 00-06 
                        *mday: BCD codification, 01-31
                        *mon: BCD codification, 01-12
                        *year: BCD codification, 00-99
 
  Input:           pTD - pointer to a rtccTimeDate union to store the current 
                         time and date 
                   di - if interrupts need to be disabled
                   
  Output:          TRUE '1' : If all the values are within range
                   FALSE '0' : If any value is out of above mentioned range.
 
  Side Effects:    None
 
  Overview:        The function updates the user supplied union/structure with
                   the current time and date of the RTCC device.
 
  Note:            The write is successful only if Wr Enable is set.
                   The function will enable the write itself, if needed.
                   Also, the Alarm will be temporarily disabled and the
                   device will be stopped (On set to 0) in order
                   to safely perform the update of the RTC time register.
                   However, the device status will be restored.
                   Usually the disabling of the interrupts is desired, 
                   if the user has to have more precise control over the 
                   actual moment of the time setting.
 ********************************************************************/

/*
void RtccSetChimeEnable(BOOL enable, BOOL dsblAlrm) 

The function enables/disables the chime alarm of the RTCC device.

Input Parameters 
enable      boolean to enable/disable the RTCC chime 

dsblAlrm    if TRUE, the API can temporarily disable the alarm when changing the Chime status

 * */

