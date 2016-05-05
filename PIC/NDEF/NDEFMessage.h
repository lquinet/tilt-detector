/* 
 * File:   NDEFMessage.h
 * Author: Denis
 *
 * Created on 12 June 2015, 11:19
 */

#ifndef NDEFMESSAGE_H
#define	NDEFMESSAGE_H

#include "../user.h"
#include "../drivers/drv_i2c.h"

/********************** Capability container ****************/
#define CC1 0xE1
#define CC2 0x40
#define CC3 0xFF
#define CC4 0x00

/*********************** TLV BLOCK **************************/
#define TLV_TAG 0x03

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

// Payload structure to store in an NDEF record
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
} NDEFPayload_t;

void NdefMessageAddRecord(uint8_t *payloadArray, boolean isFirstRecord);
void DataCat(uint8_t *payload, char *text, uint8_t length);
void BuildMessage(uint8_t *payloadArray, NDEFPayload_t data);
void FXLS8471QSaveNdefMessage(IntTo8_t Xacc, IntTo8_t Yacc, IntTo8_t Zacc, uint8_t Acc_event);
void EMC1001SaveNdefMessage(IntTo8_t temp);
StatusType M24LR04E_SaveNdefRecord(NDEFPayload_t data, I2C_message_t *MemMsg, uint8_t address);
void M24LR04E_UpdateHeader (I2C_message_t *MemMsg, uint8_t address, IntTo8_t lastSubAddressWrited, uint8_t sizeOfLastRecord);
void M24LR04E_SetTLV_Block (I2C_message_t *MemMsg, uint8_t address, boolean isFirstRecord);
StatusType M24LR04E_SaveCC(I2C_message_t *MemMsg, uint8_t address);

#endif	/* NDEFMESSAGE_H */

