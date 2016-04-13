/* 
 * File:   M24LR04E_R.h
 * Author: Denis
 *
 * Created on 26 May 2015, 10:31
 */


#ifndef M24LR04E_R_H
#define	M24LR04E_R_H
#include "drivers/drv_i2c.h"
#include "user.h"

// Data format to store in an NDEF record
typedef struct {     
    uint8_t day;       
    uint8_t month;    
    uint8_t year; 
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t unused;
    uint8_t type_message;
    IntTo8_t Xacc;
    IntTo8_t Yacc;
    IntTo8_t Zacc;
    uint8_t Acc_event;
    IntTo8_t temp;
} data_t;

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

/*
// I2C address for M24LR16-E Datalogger
#define M24LR04 0x53
#define M24LR16_EEPROM_ADDRESS_SYSTEM		0xAE //I2C DeviceSelect
#define M24LR16_EEPROM_CONFIGURATION_BYTE       0x2323
#define WIP_BUSY_MODE_MASK 0x08
#define RF_BUSY_MODE_MASK 0xF7
*/

// Memory_Full
#define MemoryFull 0x01    // e²prom memory full
#define MemoryNotFull 0x00

// RF_Change
#define RF_Change_WithoutReset 0x01  // if the Android application change configuration byte without reset
#define RF_Change_Reset 0x02        // if the Android application force reset

/********************** Capability container ****************/
#define CC1 0xE1
#define CC2 0x40
#define CC3 0xFF
#define CC4 0x00

/*********************** PAYLOAD ****************************/
// Type de message (acceleromètre ou temperature)
#define TYPE_ACCEL  1
#define TYPE_TEMP   2

// Nombre de bytes de data pour l'envoyer dans NDEF record
#define NB_MAX_DATA_BYTES   16

// Nombre de bytes dans le payload quand acceleromtètre
#define NB_DATA_BYTES_ACCEL   15

// Nombre de bytes dans le payload quand capteur de température
#define NB_DATA_BYTES_TEMP   10

// Index du type de message dans le payload
#define INDEX_MSG_TYPE  7

ErrorStatus User_ReadNDEFMessage(uint8_t *PayloadLength);

uint8_t M24LR04E_ReadOneByte(I2C_message_t *MemMsg, uint8_t address, IntTo8_t subAddress);
StatusType M24LR04E_ReadBuffer(I2C_message_t *MemMsg,uint8_t address, IntTo8_t subAddress,uint8_t NumByteToRead, void *value);
ErrorStatus User_CheckNDEFMessage(void);
ErrorStatus User_GetPayloadLength(uint8_t *PayloadLength);
void ToUpperCase(uint8_t NbCar, void *StringToConvert);
ErrorStatus User_GetNDEFMessage(uint8_t PayloadLength, uint8_t *NDEFmessage);
StatusType M24LR04E_WriteByte(I2C_message_t *MemMsg, uint8_t address, IntTo8_t subAddress, uint8_t data);
StatusType M24LR04E_WriteNBytes(I2C_message_t *MemMsg, uint8_t address, IntTo8_t subAddress, uint8_t *data, uint8_t NbByteToSend);


/** @LOIC:
 **  @name M24LR04E_SaveNdefMessage
 * The M24LR04E_SaveNdefMessage function save THE ONLY NDEFMessage in the e²p in I²C by page write (4 bytes). 
 * There is only one NDEFMessage stored in the e²p, and we overwrite it when we save a new message.
 * The NDEFMessage is contained in the _NdefRecord_t structure,and we'll cross the memory of this structure to send the frame to the e²p.
 *
 * @param MemMsg: pointer to the I2C_message_t structure
 * @param adress: adress of the e²p
 * @param subadress: adress of the pointer in the e²p memory
 * @return E_OK if the memory has been updated
 *         E_OS_STATE if the I2C access failed
 */
StatusType M24LR04E_SaveNdefMessage(data_t data, const rom char *encoding, I2C_message_t *MemMsg, uint8_t address);
void FXLS8471QSaveNdefMessage(IntTo8_t Xacc, IntTo8_t Yacc, IntTo8_t Zacc, uint8_t Acc_event);
void STTS751SaveNdefMessage(IntTo8_t temp);
StatusType M24LR04E_SaveCC(I2C_message_t *MemMsg, uint8_t address);
void BuildMessage(char *text, data_t data);
void WaitEepResponse (uint8_t address);
void writeDateTimeToConfigurationByte (void);

#endif	/* M24LR04E_R_H */


