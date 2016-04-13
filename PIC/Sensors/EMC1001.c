#include <rtcc.h>
#include "../RTCC/MyRTCC.h"
#include "EMC1001.h"
#include "drivers/drv_i2c.h"
#include "../Sensors/M24LR04E_R.h"
#include "EMC1001.h"
#include "../tsk_task_Main.h"

extern I2C_message_t My_I2C_Message;
extern data_t data;

void InitSTTS751()
{
    // ALERT disabled and Standby mode
    STTS751WriteByte(&My_I2C_Message, STTS751_ADDRESS, Configuration, 0xC0);
    //STTS751WriteByte(&My_I2C_Message, STTS751_ADDRESS, Configuration, 0x00);
}

void ReadTemperatureSTTS751(IntTo8_t *temperature)
{   
    // Writing to the one-shot register while in standby mode initiates a conversion and comparison cycle
    STTS751WriteByte(&My_I2C_Message, STTS751_ADDRESS, One_shot, 0xFF);
    temperature->Nb8_B[1] = STTS751ReadByte(&My_I2C_Message, STTS751_ADDRESS, Temperature_value_high);
    temperature->Nb8_B[0] = STTS751ReadByte(&My_I2C_Message, STTS751_ADDRESS, Temperature_value_low);
}

float ConvertTemperatureSTTS751(IntTo8_t temperature)
{
    if ((int8_t) temperature.Nb8_B[1] >= 0){
        return (float) ((uint16_t)temperature.Nb8_B[1]<<2 | temperature.Nb8_B[0]>>6)/4;
    }
    else {
        return (float) ( (int16_t)(temperature.Nb8_B[1]<<8)/64 | (temperature.Nb8_B[0]>>6) )/4;
    }
}

/**********************************************************************
 *
 *
 * @param  MemMsg    	 IN  Mandatory I2C structure
 * @param
 * @return Status         E_OK if the STTS751 has been updated
 *                        E_OS_STATE if the I2C access failed
 **********************************************************************/
StatusType STTS751WriteByte(I2C_message_t *MemMsg, uint8_t address, uint8_t subAddress, uint8_t data)
{
    unsigned char pData;

    MemMsg->control = address & 0xFE;
    //  High byte of addr, only used if high bit set
    MemMsg->addr_high = 0;
    // First register Adress
    MemMsg->addr_low = subAddress;
    // The bit setting of flags.ptr_type
    MemMsg->ram_data = &pData;
    // Must be less than 255
    MemMsg->num_bytes = sizeof (pData);
    // 0 = single byte address, 1= two byte address
    MemMsg->flags.long_addr = 0;
    // 1 = read from external, 0 = write to external
    MemMsg->flags.i2c_read = 0;
    // 1 = SMBbus Enabled, 0 = Disabled
    MemMsg->flags.SMBus = 1;
    
     MemMsg->flags.error = 0;//Attention important de le reseter!!!

    pData = data;

    I2C_enqMsg(MemMsg);
    SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
    WaitEvent(I2C_QUEUE_EMPTY);
    ClearEvent(I2C_QUEUE_EMPTY);

    if (MemMsg->flags.error != 0)
        return E_OS_STATE;
    return E_OK;
}

/**********************************************************************
 * Read a single byte from subaddress
 *
 * @param  MemMsg    	IN  Mandatory I2C structure
 * @param
 * @return Status         E_OK if the STTS751 has been read
 *                        E_OS_STATE if the I2C access failed
 **********************************************************************/
uint8_t STTS751ReadByte(I2C_message_t *MemMsg, uint8_t address, uint8_t subAddress)
{
    unsigned char pData[1];
    uint8_t temp=0;

    MemMsg->control = address | 0x01;
    //  High byte of addr, only used if high bit set
    MemMsg->addr_high = 0;
    // First register Adress
    MemMsg->addr_low = subAddress;
    // The bit setting of flags.ptr_type
    MemMsg->ram_data = pData;
    // Must be less than 255
    MemMsg->num_bytes = sizeof (pData);
    // 0 = single byte address, 1= two byte address
    MemMsg->flags.long_addr = 0;
    // 1 = read from external, 0 = write to external
    MemMsg->flags.i2c_read = 1;
    // 1 = SMBbus Enabled, 0 = Disabled
    MemMsg->flags.SMBus = 1;
    MemMsg->next = 0;
    MemMsg->flags.error = 0;//Attention important de le reseter!!!

    I2C_enqMsg(MemMsg);
    SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
    WaitEvent(I2C_QUEUE_EMPTY);
    ClearEvent(I2C_QUEUE_EMPTY);

    if (MemMsg->flags.error != 0)
        return E_OS_STATE;
temp=pData[0];
    return temp;
}

void STTS751SaveNdefMessage(IntTo8_t temp) 
{
    RtccReadTimeDate(&Rtcc_read_TimeDate); //Rtcc_read_TimeDate will have latest time
    
    data.type_message = TYPE_TEMP;
    data.temp.Nb8_B[1] = temp.Nb8_B[1];
    data.temp.Nb8_B[0] = temp.Nb8_B[0];
    data.day = BcdHexToBcdDec(Rtcc_read_TimeDate.f.mday);
    data.month = BcdHexToBcdDec(Rtcc_read_TimeDate.f.mon);
    data.year = BcdHexToBcdDec(Rtcc_read_TimeDate.f.year);
    data.hour = BcdHexToBcdDec(Rtcc_read_TimeDate.f.hour);
    data.min = BcdHexToBcdDec(Rtcc_read_TimeDate.f.min);
    data.sec = BcdHexToBcdDec(Rtcc_read_TimeDate.f.sec);
    
    M24LR04E_SaveNdefMessage(data, "en", &My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
}