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
/* @brief Write the data into the register
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param data Value to be written in the register
 * @return none
 */
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
/* @brief Read one or multiple register
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param nBytes Number of bytes to read
 * @param data array where data will be stocked
 * @return none
 */
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
/* @brief Write multiple bits in an 8-bit device register.
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

/* @brief Read multiple bits from an 8-bit device register.
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

/* @brief Change the mode of the device (wake or standby)
 * @param mode FXLS8471Q_MODE_X 
 * @return none
 */
void fxls8471q_switchMode(uint8_t mode){
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG1, FXLS8471Q_CTRL_REG1_active_BIT, FXLS8471Q_CTRL_REG1_active_LENGTH, mode);
}

/* @brief Set the range of the accelerometer
 * @param range FXLS8471Q_FS_X
 * @return none
 */
void fxls8471q_setFullScaleRange(uint8_t range){
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_XYZ_DATA_CFG0, FXLS8471Q_XYZ_fs_BIT, FXLS8471Q_XYZ_fs_LENGTH, range);
}

/* @brief Set the auto-wake sample frequency.
 * @param rate FXLS8471Q_ASPL_X
 * @return none
 */
void fxls8471q_setASPLRate(uint8_t rate){
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG1, FXLS8471Q_CTRL_REG1_ASPL_BIT, FXLS8471Q_CTRL_REG1_ASPL_LENGTH, rate);
}

/* @brief Set the system output data rate.
 * @param odr FXLS8471Q_ODR_X
 * @return none
 */
void fxls8471q_setODR(uint8_t odr){
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG1, FXLS8471Q_CTRL_REG1_ODR_BIT, FXLS8471Q_CTRL_REG1_ODR_LENGTH, odr);
}
/* @brief Set the power mode.
 * @param powerMode FXLS8471Q_PM_X
 * @return none
 */
void fxls8471q_setMods(uint8_t powerMode){
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG2, FXLS8471Q_CTRL_REG2_mods_BIT, FXLS8471Q_CTRL_REG2_mods_LENGTH, powerMode);
}

/* @brief Allow the device to enter in sleep mode.
 * @param sleep FXLS8471Q_SLEEP_X
 * @return none
 */
void fxls8471q_setSleep(uint8_t sleep){
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG2, FXLS8471Q_CTRL_REG2_slpe_BIT, FXLS8471Q_CTRL_REG2_slpe_LENGTH, sleep);
}

/* @brief Set the power mode for the sleep mode.
 * @param powerMode FXLS8471Q_PM_X
 * @return none
 */
void fxls8471q_setSMods(uint8_t powerMode){
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG2, FXLS8471Q_CTRL_REG2_smods_BIT, FXLS8471Q_CTRL_REG2_smods_LENGTH, powerMode);
}

/* @brief Manage the portrait landscape interrupt.
 * @param none
 * @return none
 */
void fxls8471q_managePortraitLandscape(void)
{   
    uint8_t newlp=fxls8471q_readBits(FXLS8471Q_ADDRESS, FXLS8471Q_PL_STATUS, FXLS8471Q_PL_STATUS_newlp_BIT, FXLS8471Q_PL_STATUS_newlp_LENGTH);
    if(newlp==1){
        
        uint8_t lapo=fxls8471q_readBits(FXLS8471Q_ADDRESS, FXLS8471Q_PL_STATUS, FXLS8471Q_PL_STATUS_lapo_BIT, FXLS8471Q_PL_STATUS_lapo_LENGTH);
        uint8_t bafro=fxls8471q_readBits(FXLS8471Q_ADDRESS, FXLS8471Q_PL_STATUS, FXLS8471Q_PL_STATUS_bafro_BIT, FXLS8471Q_PL_STATUS_bafro_LENGTH);
        
        switch(bafro){
            case FXLS8471Q_BAFRO_FRONT:
                #ifdef DEBUG_FXLS8471Q
                Printf("Front orientation and ");
                #endif
                break;
            case FXLS8471Q_BAFRO_BACK:
                #ifdef DEBUG_FXLS8471Q
                Printf("Back orientation and ");
                #endif
                break;
            default:
                #ifdef DEBUG_FXLS8471Q
                Printf("Error managePortrait!\r\n");
                #endif
            break;
        }
        
        switch(lapo){
            case FXLS8471Q_LAPO_UP:
                #ifdef DEBUG_FXLS8471Q
                Printf("Portrait up!\r\n");
                #endif
                break;
            case FXLS8471Q_LAPO_DOWN:
                #ifdef DEBUG_FXLS8471Q
                Printf("Portrait down!\r\n");
                #endif
                break;
            case FXLS8471Q_LAPO_RIGHT:
                #ifdef DEBUG_FXLS8471Q
                Printf("Landscape right!\r\n");
                #endif
                break;
            case FXLS8471Q_LAPO_LEFT:
                #ifdef DEBUG_FXLS8471Q
                Printf("Landscape left!\r\n");
                #endif
                break;
            default:
                #ifdef DEBUG_FXLS8471Q
                Printf("Error managePortrait!\r\n");
                #endif
            break;
        }
        
    }
}

/************************************************************************/
/* Public functions                                                    */
/************************************************************************/

/*************************************************************************
Function: fxls8471q_init()
Purpose:  Initialize the FLXS8471Q
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
        fxls8471q_calibrate();
        fxls8471q_setFullScaleRange(FXLS8471Q_FS_2); //range of +-2G
        //fxls8471q_setASPLRate(FXLS8471Q_ASPL_1_56);  //set auto-wake sample frequency
        fxls8471q_setODR(FXLS8471Q_ODR_50);  //
        fxls8471q_setMods(FXLS8471Q_PM_HR);
        //fxls8471q_setSMods(FXLS8471Q_PM_LNLP);
        //fxls8471q_setSleep(FXLS8471Q_SLEEP_ON);
        fxls8471q_configureOrientationDetection();
        fxls8471q_configureInterrupt(FXLS8471Q_INT_aslp_OFF, FXLS8471Q_INT_fifo_OFF, FXLS8471Q_INT_trans_OFF, FXLS8471Q_INT_lndprt_ON, FXLS8471Q_INT_pulse_OFF, FXLS8471Q_INT_ffmt_OFF, FXLS8471Q_INT_avecm_OFF, FXLS8471Q_INT_drdy_OFF);
        fxls8471q_configureRoutingInterrupt(FXLS8471Q_INT_INT1, FXLS8471Q_INT_INT1, FXLS8471Q_INT_INT1, FXLS8471Q_INT_INT1, FXLS8471Q_INT_INT1, FXLS8471Q_INT_INT1, FXLS8471Q_INT_INT1, FXLS8471Q_INT_INT1);
        I2C_writeRegister(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG3, 0x00); // RESET the CTRL_REG3 register
        fxls8471q_switchMode(FXLS8471Q_MODE_WAKE);
        
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
Function: fxls8471q_getAcceleration()
Purpose:  Get values from the accelerometer
Input:    Int16 pointer to x, y, z: values that are returned
Returns:  none
**************************************************************************/
void fxls8471q_getAcceleration(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t rawVal[6];
    I2C_readRegister(FXLS8471Q_ADDRESS, FXLS8471Q_OUT_X_MSB, 6, &rawVal[0]);
    
    *x = (int16_t)(((rawVal[0] << 8) | (rawVal[1]))) / 4 ;
    *y = (int16_t)(((rawVal[2] << 8) | (rawVal[3]))) / 4 ;
    *z = (int16_t)(((rawVal[4] << 8) | (rawVal[5]))) / 4 ;

}

/*************************************************************************
Function: fxls8471q_calibrate()
Purpose:  Calibrate the accelerometer. Must be in flat position
Input:    none
Returns:  none
**************************************************************************/
void fxls8471q_calibrate()
{
    int16_t fxls8471q[3]={0,0,0};
    int8_t offset[3]={0,0,0};
    
    // Configuration 
    fxls8471q_switchMode(FXLS8471Q_MODE_STANDBY); //switch to standby mode
    fxls8471q_setODR(FXLS8471Q_ODR_200); // 200Hz
    fxls8471q_setFullScaleRange(FXLS8471Q_FS_2); //+-2g
    
    // Get values
    fxls8471q_switchMode(FXLS8471Q_MODE_WAKE); //switch to standby mode
    fxls8471q_getAcceleration(&fxls8471q[0], &fxls8471q[1], &fxls8471q[2]);
    
    // Calculate offset correction value for each axis  
    fxls8471q[0]=(fxls8471q[0]/8);
    fxls8471q[1]=(fxls8471q[0]/8);
    fxls8471q[2]=((4096-fxls8471q[2])/8);
    offset[0]=-fxls8471q[0];
    offset[1]=-fxls8471q[1];
    offset[2]=fxls8471q[2];
    
    // Write offset correction values
    fxls8471q_switchMode(FXLS8471Q_MODE_STANDBY);
    I2C_writeRegister(FXLS8471Q_ADDRESS, FXLS8471Q_OFF_X, offset[0]);
    I2C_writeRegister(FXLS8471Q_ADDRESS, FXLS8471Q_OFF_Y, offset[1]);
    I2C_writeRegister(FXLS8471Q_ADDRESS, FXLS8471Q_OFF_Z, offset[2]);
}

/*************************************************************************
Function: fxls8471q_configureOrientationDetection()
Purpose:  Configure the accelerometer to detect the orientation
Input:    none
Returns:  none
**************************************************************************/
void fxls8471q_configureOrientationDetection(void)
{
    fxls8471q_switchMode(FXLS8471Q_MODE_STANDBY);
    // Enable the orientation detection
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_PL_CFG, FXLS8471Q_PL_CFG_plen_BIT, FXLS8471Q_PL_CFG_plen_LENGTH, FXLS8471Q_PL_ON);
    // Set the Back/Front Angle trip points
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_PL_BF_ZCOMP, FXLS8471Q_PL_BF_ZCOMP_bkdr_BIT, FXLS8471Q_PL_BF_ZCOMP, FXLS8471Q_BKFR_2);
    // Set the Z-Lockout angle trip point
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_PL_BF_ZCOMP, FXLS8471Q_PL_BF_ZCOMP_zlock_BIT, FXLS8471Q_PL_BF_ZCOMP_zlock_LENGTH, FXLS8471Q_ZLOCK_28);
    // Set the Trip Threshold Angle 
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_PL_THS_REG, FXLS8471Q_PL_THS_REG_plths_BIT, FXLS8471Q_PL_THS_REG_plths_LENGTH, FXLS8471Q_THS_30);
    // Set the Hysteresis Angle 
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_PL_THS_REG, FXLS8471Q_PL_THS_REG_hys_BIT, FXLS8471Q_PL_THS_REG_hys_LENGTH, FXLS8471Q_HYS_3);
    // Set the debounce counter
    fxls8471q_writeBits(FXLS8471Q_ADDRESS, FXLS8471Q_PL_COUNT, FXLS8471Q_PL_COUNT_dbnce_BIT, FXLS8471Q_PL_COUNT_dbnce_LENGTH, FXLS8471Q_ODR_12_5);
}
/*************************************************************************
Function: fxls8471q_configureInterrupt()
Purpose:  Configure the interruptions in the accelerometer
Input:    FXLS8471Q_INT_X_ON or FXLS8471Q_INT_X_OFF
Returns:  none
**************************************************************************/
void fxls8471q_configureInterrupt(uint8_t aslp, uint8_t fifo, uint8_t trans, uint8_t lndprt, uint8_t pulse, uint8_t ffmt, uint8_t avecm, uint8_t drdy){
    uint8_t intRegister =0;
    intRegister |= aslp << FXLS8471Q_CTRL_REG4_aslp_BIT;
    intRegister |= fifo << FXLS8471Q_CTRL_REG4_fifo_BIT;
    intRegister |= trans << FXLS8471Q_CTRL_REG4_trans_BIT;
    intRegister |= lndprt << FXLS8471Q_CTRL_REG4_lndprt_BIT;
    intRegister |= pulse << FXLS8471Q_CTRL_REG4_pulse_BIT;
    intRegister |= ffmt << FXLS8471Q_CTRL_REG4_ffmt_BIT;
    intRegister |= avecm << FXLS8471Q_CTRL_REG4_avecm_BIT;
    intRegister |= drdy << FXLS8471Q_CTRL_REG4_drdy_BIT;
    
    I2C_writeRegister(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG4, intRegister);
}

/*************************************************************************
Function: fxls8471q_configureRoutingInterrupt()
Purpose:  Configure the pin of interruptions in the accelerometer
Input:    FXLS8471Q_INT_INT2 or FXLS8471Q_INT_INT1
Returns:  none
**************************************************************************/
void fxls8471q_configureRoutingInterrupt(uint8_t aslp, uint8_t fifo, uint8_t trans, uint8_t lndprt, uint8_t pulse, uint8_t ffmt, uint8_t avecm, uint8_t drdy){
    uint8_t intRegister =0;
    intRegister |= aslp << FXLS8471Q_CTRL_REG5_aslp_BIT;
    intRegister |= fifo << FXLS8471Q_CTRL_REG5_fifo_BIT;
    intRegister |= trans << FXLS8471Q_CTRL_REG5_trans_BIT;
    intRegister |= lndprt << FXLS8471Q_CTRL_REG5_lndprt_BIT;
    intRegister |= pulse << FXLS8471Q_CTRL_REG5_pulse_BIT;
    intRegister |= ffmt << FXLS8471Q_CTRL_REG5_ffmt_BIT;
    intRegister |= avecm << FXLS8471Q_CTRL_REG5_avecm_BIT;
    intRegister |= drdy << FXLS8471Q_CTRL_REG5_drdy_BIT;
    
    I2C_writeRegister(FXLS8471Q_ADDRESS, FXLS8471Q_CTRL_REG5, intRegister);
}
/*************************************************************************
Function: fxls8471q_checkSourceInterrupt()
Purpose:  Check the source of the interruption
Input:    none
Returns:  none
**************************************************************************/
void fxls8471q_checkSourceInterrupt(void)
{
    uint8_t sourceI[1]={0};
    I2C_readRegister(FXLS8471Q_ADDRESS, FXLS8471Q_INT_SOURCE, 1, &sourceI[0]);
    
    if(sourceI[0] & FXLS8471Q_INT_SOURCE_lndprt){
        fxls8471q_managePortraitLandscape();
    }
}
/*************************************************************************
Function: fxls8471q_debug()
Purpose:  Print the value of the register
Input:    uint8_t with the address of the register
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