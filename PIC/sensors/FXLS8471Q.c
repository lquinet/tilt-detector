/*************************************************************************
	
	MPU9150 library by Julien Delvaux

*************************************************************************/

#include "FXLS8471Q.h"
#include "FXLS8471Q_registers.h"
#include "../drivers/drv_i2c.h"
#include "../drivers/drv_rs.h"

/************************************************************************/
/* Constants and macros                                                 */
/************************************************************************/

/************************************************************************/
/* Prototypes of functions                                              */
/************************************************************************/
void I2C_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
uint8_t I2C_readByte(uint8_t devAddr, uint8_t regAddr);
void I2C_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t nBytes, uint8_t data[]); 

/*************************************************************************
Function: fxls8471q_init()
Purpose:  none
Input:    none
Returns:  none
**************************************************************************/
void fxls8471q_init(void){

    //1. Check if the device is connected
    uint8_t whoAmI=I2C_readByte(FXLS8471Q_ADDRESS, FXLS8471Q_WHO_AM_I);
    
    if(whoAmI==0x6A){
        #ifdef DEBUG_FXLS8471Q
        Printf("FXLS8471Q connected !\r\n");
        #endif

        fxls8471q_switchMode(FXLS8471Q_MODE_STANDBY); //switch to standby mode
    }
    else{
        #ifdef DEBUG_FXLS8471Q
        Printf("FXLS8471Q error !\r\n");
        #endif
    }
    

}

/*************************************************************************
Function: fxls8471q_switchMode()
Purpose:  none
Input:    none
Returns:  none
**************************************************************************/
void fxls8471q_switchMode(uint8_t mode){
    uint8_t valueRegister = I2C_readByte(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG1);

    switch(mode){
        case FXLS8471Q_MODE_STANDBY:
            valueRegister = valueRegister&~(0x01);
            break;
        case FXLS8471Q_MODE_WAKE:
            valueRegister = valueRegister|(0x01);
            break;
        case FXLS8471Q_MODE_SLEEP:
            break;    
        default:
            break;
    }
    
    I2C_writeByte(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG1, valueRegister);
}


/************************************************************************/
/* Private functions                                                    */
/************************************************************************/

void I2C_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data)
{
    I2C_message_t MemMsg;

    
    MemMsg.control = devAddr &0xFE;
    //  High byte of addr, only used if high bit set
    MemMsg.addr_high = 0;
    // First register Adress
    MemMsg.addr_low = regAddr;
    // The bit setting of flags.ptr_type
    MemMsg.ram_data = &data;
    // Must be less than 255
    MemMsg.num_bytes = 1;
    // 0 = single byte address, 1= two byte address
    MemMsg.flags.long_addr = 0;
    // 1 = read from external, 0 = write to external
    MemMsg.flags.i2c_read = 1;
    // 1 = SMBbus Enabled, 0 = Disabled
    MemMsg.flags.SMBus = 0;
    MemMsg.flags.error = 0; 
    MemMsg.retry_counter = 10;

    I2C_enqMsg(&MemMsg);
    SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
    WaitEvent(I2C_QUEUE_EMPTY);
    ClearEvent(I2C_QUEUE_EMPTY);

}

uint8_t I2C_readByte(uint8_t devAddr, uint8_t regAddr)
{
    I2C_message_t MemMsg;
    unsigned char pData[1];

    MemMsg.control = devAddr | 0x01;
    //  High byte of addr, only used if high bit set
    MemMsg.addr_high = 0;
    // First register Adress
    MemMsg.addr_low = regAddr;
    // The bit setting of flags.ptr_type
    MemMsg.ram_data = pData;
    // Must be less than 255
    MemMsg.num_bytes = 1;
    // 0 = single byte address, 1= two byte address
    MemMsg.flags.long_addr = 0;
    // 1 = read from external, 0 = write to external
    MemMsg.flags.i2c_read = 1;
    // 1 = SMBbus Enabled, 0 = Disabled
    MemMsg.flags.SMBus = 0;
    MemMsg.next = 0;
    MemMsg.flags.error = 0; //Attention important de le reseter!!!

    I2C_enqMsg(&MemMsg);
    SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
    WaitEvent(I2C_QUEUE_EMPTY);
    ClearEvent(I2C_QUEUE_EMPTY);
    
    return pData[0];

}

void I2C_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t nBytes, uint8_t data[])
{
    I2C_message_t MemMsg;

    MemMsg.control = devAddr | 0x01;
    //  High byte of addr, only used if high bit set
    MemMsg.addr_high = 0;
    // First register Adress
    MemMsg.addr_low = regAddr;
    // The bit setting of flags.ptr_type
    MemMsg.ram_data = data;
    // Must be less than 255
    MemMsg.num_bytes = nBytes; //sizeof (pData);
    // 0 = single byte address, 1= two byte address
    MemMsg.flags.long_addr = 0;
    // 1 = read from external, 0 = write to external
    MemMsg.flags.i2c_read = 1;
    // 1 = SMBbus Enabled, 0 = Disabled
    MemMsg.flags.SMBus = 0;
    MemMsg.next = 0;
    MemMsg.flags.error = 0; //Attention important de le reseter!!!

    I2C_enqMsg(&MemMsg);
    SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
    WaitEvent(I2C_QUEUE_EMPTY);
    ClearEvent(I2C_QUEUE_EMPTY);


}