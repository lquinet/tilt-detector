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

// I2C address for M24LR16-E Datalogger
#define M24LR04 0x53
#define M24LR16_EEPROM_ADDRESS_USER		0xA6 /* I2C DeviceSelect */
#define M24LR16_EEPROM_ADDRESS_SYSTEM		0xAE /* I2C DeviceSelect */
#define M24LR16_EEPROM_CONFIGURATION_BYTE       0x2323
#define WIP_BUSY_MODE_MASK 0x08
#define RF_BUSY_MODE_MASK 0xF7



ErrorStatus User_ReadNDEFMessage(uint8_t *PayloadLength);

uint8_t M24LR04E_ReadOneByte(I2C_message_t *MemMsg, uint8_t address, IntTo8_t subAddress);
StatusType M24LR04E_ReadBuffer(I2C_message_t *MemMsg,uint8_t address, IntTo8_t subAddress,uint8_t NumByteToRead, void *value);
ErrorStatus User_CheckNDEFMessage(void);
ErrorStatus User_GetPayloadLength(uint8_t *PayloadLength);
void ToUpperCase(uint8_t NbCar, void *StringToConvert);
ErrorStatus User_GetNDEFMessage(uint8_t PayloadLength, uint8_t *NDEFmessage);
StatusType M24LR04E_WriteByte(I2C_message_t *MemMsg, uint8_t address, IntTo8_t subAddress, uint8_t data);

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
StatusType M24LR04E_SaveNdefMessage(I2C_message_t *MemMsg, uint8_t address);
StatusType M24LR04E_SaveCC(I2C_message_t *MemMsg, uint8_t address);
#endif	/* M24LR04E_R_H */


