/* 
 * File:   NDEFMessage.h
 * Author: Denis
 *
 * Created on 12 June 2015, 11:19
 */

#ifndef NDEFMESSAGE_H
#define	NDEFMESSAGE_H

#include "../user.h"

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

void NdefMessageAddTextRecord(char *text, const rom char *encoding);
void DataCat(uint8_t *payload, char *text, uint8_t length);
void BuildMessage(char *payload, NDEFPayload_t data);
void FXLS8471QSaveNdefMessage(IntTo8_t Xacc, IntTo8_t Yacc, IntTo8_t Zacc, uint8_t Acc_event);
void STTS751SaveNdefMessage(IntTo8_t temp);

#endif	/* NDEFMESSAGE_H */

