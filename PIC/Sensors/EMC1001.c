/*************************************************************************
	
	EMC1001 library by Loic Quinet & Julien Delvaux

*************************************************************************/

#include "EMC1001.h"
#include "drivers/drv_i2c.h"
#include "../Sensors/M24LR04E_R.h"

/************************************************************************/
/* Constants and macros                                                 */
/************************************************************************/
extern I2C_message_t My_I2C_Message;

#define EMC1001_ADDRESS                     0x90
#define EMC1001_Product_ID                  0xFD
#define EMC1001_Manufacturer_ID             0xFE
#define EMC1001_Revision_number             0xFF

#define EMC1001_Temperature_value_high      0x00
#define EMC1001_Temperature_value_low       0x02
#define EMC1001_Temperature_high_limit_high 0x05
#define EMC1001_Temperature_high_limit_low  0x06
#define EMC1001_Temperature_low_limit_high  0x07
#define EMC1001_Temperature_low_limit_low   0x08

#define EMC1001_Configuration               0x03    //POR 0000 0000
#define EMC1001_Conversion_rate             0x04    //POR 0000 0100
#define EMC1001_Status                      0x01

#define EMC1001_One_shot                    0x0F
#define EMC1001_THERM_limit                 0x20
#define EMC1001_THERM_hysteresis            0x21
#define EMC1001_SMBus_timeout_enable        0x22


/************************************************************************/
/* Private functions                                                    */
/************************************************************************/
/* @brief Write the data into the register
 * @param MemMsg I2C message
 * @param address I2C slave device address
 * @param subAddress Register to write to
 * @param data Value to be written in the register
 * @return StatusType
 */
StatusType EMC1001_writeByte(I2C_message_t *MemMsg, uint8_t address, uint8_t subAddress, uint8_t data)
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

/* @brief Read the data from the register
 * @param MemMsg I2C message
 * @param address I2C slave device address
 * @param subAddress Register to write to
 * @return uint8_t with the value
 */
uint8_t EMC1001_readByte(I2C_message_t *MemMsg, uint8_t address, uint8_t subAddress)
{
    unsigned char pData[1];

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

    return pData[0];
}

/*************************************************************************
Function: emc1001_init()
Purpose:  Initialise the EMC1001 in standby and ALERT disable
Input:    none
Returns:  none
**************************************************************************/
void emc1001_init()
{
    EMC1001_writeByte(&My_I2C_Message, EMC1001_ADDRESS, EMC1001_Configuration, 0xC0);
}

/*************************************************************************
Function: emc1001_readTemperature()
Purpose:  Read the temperature from EMC1001
Input:    IntTo8_t pointer to the return value
Returns:  none
**************************************************************************/
void emc1001_readTemperature(IntTo8_t *temperature)
{   
    // Writing to the one-shot register while in standby mode initiates a conversion and comparison cycle
    EMC1001_writeByte(&My_I2C_Message, EMC1001_ADDRESS, EMC1001_One_shot, 0xFF);
    temperature->Nb8_B[1] = EMC1001_readByte(&My_I2C_Message, EMC1001_ADDRESS, EMC1001_Temperature_value_high);
    temperature->Nb8_B[0] = EMC1001_readByte(&My_I2C_Message, EMC1001_ADDRESS, EMC1001_Temperature_value_low);
}

/*************************************************************************
Function: emc1001_convertTemperature()
Purpose:  Convert the temperature given into a float
Input:    IntTo8_t with the value to convert
Returns:  float
**************************************************************************/

float emc1001_convertTemperature(IntTo8_t temperature)
{
    float tempFloat;
    
    if ((int8_t) temperature.Nb8_B[1] >= 0){
        return (float) ((uint16_t)temperature.Nb8_B[1]<<2 | temperature.Nb8_B[0]>>6)/4;
    }
    else {
        tempFloat = (float) ( (int16_t)(temperature.Nb8_B[1]<<8)/64 | (temperature.Nb8_B[0]>>6) )/4;
        return tempFloat;
    }
}