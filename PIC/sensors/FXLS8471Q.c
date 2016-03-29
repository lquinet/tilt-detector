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

/************************************************************************/
/* Private functions                                                    */
/************************************************************************/
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

/* @brief Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @return none
 */
void fxls8471q_switchMode(uint8_t mode){
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG1, FXLS8471Q_CTRL_REG1_active_BIT, FXLS8471Q_CTRL_REG1_active_LENGTH, mode);
}

/* @brief Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @return none
 */
void fxls8471q_setFullScaleRange(uint8_t range){
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_XYZ_DATA_CFG0, FXLS8471Q_XYZ_fs_BIT, FXLS8471Q_XYZ_fs_LENGTH, range);
}

/* @brief Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @return none
 */
void fxls8471q_setASPLRate(uint8_t rate){
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG1, FXLS8471Q_CTRL_REG1_ASPL_BIT, FXLS8471Q_CTRL_REG1_ASPL_LENGTH, rate);
}

/* @brief Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @return none
 */
void fxls8471q_setODR(uint8_t odr){
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG1, FXLS8471Q_CTRL_REG1_ODR_BIT, FXLS8471Q_CTRL_REG1_ODR_LENGTH, odr);
}
/* @brief Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @return none
 */
void fxls8471q_setMods(uint8_t powerMode){
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG2, FXLS8471Q_CTRL_REG2_mods_BIT, FXLS8471Q_CTRL_REG2_mods_LENGTH, powerMode);
}

/* @brief Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @return none
 */
void fxls8471q_setSleep(uint8_t sleep){
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG2, FXLS8471Q_CTRL_REG2_slpe_BIT, FXLS8471Q_CTRL_REG2_slpe_LENGTH, sleep);
}

/* @brief Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @return none
 */
void fxls8471q_setSMods(uint8_t powerMode){
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG2, FXLS8471Q_CTRL_REG2_smods_BIT, FXLS8471Q_CTRL_REG2_smods_LENGTH, powerMode);
}

/************************************************************************/
/* Public functions                                                    */
/************************************************************************/

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
        fxls8471q_setFullScaleRange(FXLS8471Q_FS_2); //range of +-2G
        fxls8471q_setASPLRate(FXLS8471Q_ASPL_1_56);  //set auto-wake sample frequency
        fxls8471q_setODR(FXLS8471Q_ODR_800);  //
        fxls8471q_setMods(FXLS8471Q_PM_HR);
        fxls8471q_setSMods(FXLS8471Q_PM_LNLP);
        fxls8471q_setSleep(FXLS8471Q_SLEEP_ON);
        
        #ifdef DEBUG_FXLS8471Q
        Printf("FXLS8471Q initialisation finished !\r\n");
        #endif
    }
    else{
        #ifdef DEBUG_FXLS8471Q
        Printf("FXLS8471Q error !\r\n");
        #endif
    }
    

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

/*************************************************************************
Function: fxls8471q_debug()
Purpose:  none
Input:    none
Returns:  none
**************************************************************************/
void fxls8471q_debug(uint8_t regAddr)
{
    uint8_t buffer[10];
    uint8_t buffer2[10];
    uint8_t valReg[1];
  
    I2C_readRegister(FXLS8471Q_ADDRESS, regAddr, 1, &valReg[0]);
    itoa(regAddr, buffer);
    itoa(valReg[0], buffer2);
    Printf("Reg[%s]:%s\n",buffer,buffer2);

}