/* 
 * File:   M24LR04E_R.h
 * Author: Denis
 *
 * Created on 26 May 2015, 10:31
 */


#ifndef M24LR04E_R_H
#define	M24LR04E_R_H
#include "drivers/drv_i2c.h"
#include "../NDEF/NDEFMessage.h"
#include "user.h"

// Configuration bytes structure to store the configuration bytes from the M24LR04E memory
typedef struct {        
    boolean statusPackage;       
    uint8_t RF_Change;    
    boolean Memory_Full; 
    uint8_t DateTime[6];
    IntTo8_t XaccMax;
    IntTo8_t YaccMax;
    IntTo8_t ZaccMax;
    float tempMax;

} _ConfigBytes_t;

// I2C addresses
#define M24LR16_EEPROM_I2C_SLAVE_ADDRESS            0xA6 /* I2C DeviceSelect */

//Memory Organisation for M24LR16-E Datalogger
#define M24LR16_EEPROM_LAST_ADDRESS_DATALOGGER	0x1FE7 // Last adress for datalogger
#define M24LR16_EEPROM_ADDRESS_STATUS_PACKAGE   0x1FE8 // If package is down, this byte is set to 1 in the e²p memory
#define M24LR16_EEPROM_ADDRESS_RF_CHANGE        0x1FE9 // If the android application change a configuration, this byte is set to 1 in the e²p memory
#define M24LR16_EEPROM_ADDRESS_FULL_MEMORY      0x1FEB // If Memory is full, this byte is set to 1 in the e²p memory
#define M24LR16_EEPROM_ADDRESS_DATE_RTC         0x1FEC // First address of 6 bytes reserved to store date for RTC
#define M24LR16_EEPROM_ADDRESS_ACCEL_LIMITS     0x1FF4 // First address of 6 bytes reserved to store acceleration limits for accelerometer
#define M24LR16_EEPROM_ADDRESS_TEMP_LIMITS      0x1FFC // First address of 4 bytes reserved to store Temperature limits for temperature sensor

#define ColisUP 0x00    // Transport OK
#define ColisDown 0x01  // Transport KO

// RF_Change
#define RF_Change_WithoutReset 0x01  // if the Android application change configuration byte without reset
#define RF_Change_Reset 0x02        // if the Android application force reset

// Memory_Full
#define MemoryFull 0x01    // e²prom memory full
#define MemoryNotFull 0x00

/********************** Capability container ****************/
#define CC1 0xE1
#define CC2 0x40
#define CC3 0xFF
#define CC4 0x00

uint8_t M24LR04E_ReadOneByte(I2C_message_t *MemMsg, uint8_t address, IntTo8_t subAddress);
StatusType M24LR04E_ReadBuffer(I2C_message_t *MemMsg,uint8_t address, IntTo8_t subAddress,uint8_t NumByteToRead, void *value);
StatusType M24LR04E_WriteByte(I2C_message_t *MemMsg, uint8_t address, IntTo8_t subAddress, uint8_t data);
StatusType M24LR04E_WriteNBytes(I2C_message_t *MemMsg, uint8_t address, IntTo8_t subAddress, uint8_t *data, uint8_t NbByteToSend);
StatusType M24LR04E_SaveNdefMessage(NDEFPayload_t data, const rom char *encoding, I2C_message_t *MemMsg, uint8_t address);
StatusType M24LR04E_SaveNdefRecord(NDEFPayload_t data, const rom char *encoding, I2C_message_t *MemMsg, uint8_t address);
void M24LR04E_UpdateHeader (I2C_message_t *MemMsg, uint8_t address, IntTo8_t lastSubAddressWrited, uint8_t sizeOfLastRecord);
void M24LR04E_SetTLV_Block (I2C_message_t *MemMsg, uint8_t address, boolean isFirstRecord);
void M24LR04E_ReadConfigurationBytes(_ConfigBytes_t *configBytesStruct);
StatusType M24LR04E_SaveCC(I2C_message_t *MemMsg, uint8_t address);
void WaitEepResponse (uint8_t address);
void writeDateTimeToConfigurationByte (void);

#endif	/* M24LR04E_R_H */


