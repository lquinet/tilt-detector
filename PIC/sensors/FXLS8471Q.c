/*************************************************************************
	
	FXLS8471Q library by Julien Delvaux

*************************************************************************/

#include "FXLS8471Q.h"
#include "FXLS8471Q_registers.h"
#include <stdlib.h>
#include "../drivers/drv_i2c.h"
#include "../drivers/drv_rs.h"

/************************************************************************/
/* Constants and macros                                                 */
/************************************************************************/
/*    uint8_t buffer[10];
    itoa(mask, buffer);
    Printf("%s\n",buffer);*/
/************************************************************************/
/* Prototypes of functions                                              */
/************************************************************************/
void I2C_writeRegister(uint8_t devAddr, uint8_t regAddr, uint8_t data);
void I2C_readRegister(uint8_t devAddr, uint8_t regAddr, uint8_t nBytes, uint8_t data[]); 
void fxls8471q_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
uint8_t fxls8471q_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length);

/*************************************************************************
Function: fxls8471q_init()
Purpose:  none
Input:    none
Returns:  none
**************************************************************************/
void fxls8471q_init(void){

    //0. RST -> 0
    FXLS8471Q_RST_TRIS = 0;
    FXLS8471Q_RST_PORT = 0;
    
    //1. Check if the device is connected
    if(fxls8471q_testConnection()==1){
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
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG1, FXLS8471Q_CTRL_REG1_active_BIT, FXLS8471Q_CTRL_REG1_active_LENGTH,mode);
}


/*************************************************************************
Function: fxls8471q_testConnection()
Purpose:  Test the connection the the FXLS8471Q
Input:    none
Returns:  True if connection is valid, false otherwise
**************************************************************************/
uint8_t fxls8471q_testConnection(void)
{
    uint8_t whoAmI=fxls8471q_readBits(FXLS8471Q_ADDRESS, FXLS8471Q_WHO_AM_I, FXLS8471Q_WHO_AM_I_BIT, FXLS8471Q_WHO_AM_I_LENGTH);

    return whoAmI == FXLS8471Q_DEVICE_ID;
}

/************************************************************************/
/* Private functions                                                    */
/************************************************************************/

/** Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return none
 */
void fxls8471q_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data){
	
    uint8_t read[1];
    uint8_t mask=0;
	I2C_readRegister(devAddr, regAddr, 1, &read[0]);
	
	mask = ((1 << length) - 1) << (bitStart - length + 1);
	data <<= (bitStart - length + 1); // shift data into correct position
	data &= mask; // zero all non-important bits in data
	read[0] &= ~(mask); // zero all important bits in existing byte
	read[0] |= data; // combine data with existing byte
	
	I2C_writeRegister(devAddr, regAddr, read[0]);
}

/** Read multiple bits from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @return Status of read operation (true = success)
 */
uint8_t fxls8471q_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length) {
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    
    uint8_t read[1];
    uint8_t mask=0;

	I2C_readRegister(devAddr, regAddr, 1, &read[0]);
    
    mask = ((1 << length) - 1) << (bitStart - length + 1);
    read[0] &= mask;
    read[0] >>= (bitStart - length + 1);

    return read[0];
}

void I2C_writeRegister(uint8_t devAddr, uint8_t regAddr, uint8_t data)
{
    I2C_message_t MemMsg;

    
    MemMsg.control = (devAddr << 1) &0xFE;
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

void I2C_readRegister(uint8_t devAddr, uint8_t regAddr, uint8_t nBytes, uint8_t data[])
{
    I2C_message_t MemMsg;

    MemMsg.control = (devAddr << 1) | 0x01;
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