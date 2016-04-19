#include "NDEFMessage.h"
#include "NDEFRecord.h"
#include "../Sensors/M24LR04E_R.h"
#include "../RTCC/MyRTCC.h"
#include <string.h>
#include "../user.h"

extern I2C_message_t My_I2C_Message;
extern NDEFPayload_t data;
extern _NdefRecord_t NdefRecord;

/*******************************************************************************
 * Build the NDEF message.
 * Create the TLV tag and length, the payload length, the status byte for the payload, the encoding
 * 
 * 
 * @name NdefMessageAddTextRecord
 * 
 * @param payloadArray      pointer to the array to store in the e²p
 * @param encoding          pointer to the constant string which contains the encoding of the payloadArray area ("en" for example)
 * 
 * @return                  None
 * 
 *******************************************************************************/

void NdefMessageAddTextRecord(char *payloadArray, const rom char *encoding, boolean isFirstRecord)
{
    // Length of the payloadArray
    uint8_t length; 
    
    // Met le payload à 0
    memset(NdefRecord._payload,0,sizeof(NdefRecord._payload));
    
    // initialisation de tous les champs fixes
    NdefRecordConstructor(isFirstRecord);
    
    // Met le nombre de place que prends le payload
    // On doit ajouter le nb de caractères de l'encodage + 1 car il faut aussi mettre le status byte
    if (payloadArray[INDEX_MSG_TYPE] == TYPE_TEMP) length = NB_DATA_BYTES_TEMP;
    else length = NB_DATA_BYTES_ACCEL;
    NdefRecordSetPayloadLengh(length + strlenpgm(encoding) +1);
    
    // Record length = payload length + 4 car on doit ajouter le header, le type length (=1), le type et le status byte
    NdefRecordSetRecordLength(NdefRecord._payloadLength+4);
    
    // Satus byte = encoding length car UTF-8
    NdefRecordSetStatusByte(strlenpgm(encoding));
 
    // Structure d'un payload de TNF "NFC Forum well-known type" et de type text:
    // Payload = Status byte + ISO/IANA language code + actual 
    // Voir NFC Forum "Text Record Type Definition"
    strcatpgm2ram( NdefRecord._payload, encoding);
    DataCat(NdefRecord._payload, payloadArray, length);
}

/**********************************************************************
 * Concatenate the data to send by NDEF in the payload, with the terminator at the end
 * Don't use strcat because there are no '\0' at the end of the string (impossible because the data can contain
 * null bytes!)
 * 
 * @param  *data    	 data to store in NDEF record
 * @param  *payload      payload of the NDEF record
 * @param  length        length of the data
 * @return none
 **********************************************************************/

void DataCat(uint8_t *payload, char *data, uint8_t length){
    uint8_t i, payloadlength;
    payloadlength = strlen(payload);
    
    for (i= 0; i< length; i++){
        payload[i + payloadlength] = data[i];
    }
    payload[i + payloadlength] = NdefRecord._Teminator;
}

/**********************************************************************
 * Build the payload (in an array). 
 * If the type of the message is for accelerometer, the payload contain the datetime, 
 * the accelerations in X, Y and Z, and the type of the accelerometer event.
 * If the type of the message is for temperature, the payload contain the datetime and the temperature.
 * 
 * @param  *payload     payload (in an array) to store in NDEF message
 * @param  data         structure which contains the data to store in the payload      
 * @return Status       None
 **********************************************************************/

void BuildMessage(char *payload, NDEFPayload_t data){
    
    if (data.type_message == TYPE_ACCEL){
        payload[0] = data.day;
        payload[1] = data.month;
        payload[2] = data.year;
        payload[3] = data.hour;
        payload[4] = data.min;
        payload[5] = data.sec;
        payload[6] = 0xFF;
        payload[7] = data.type_message;
        payload[8] = data.Xacc.Nb8_B[1];
        payload[9] = data.Xacc.Nb8_B[0];
        payload[10] = data.Yacc.Nb8_B[1];
        payload[11] = data.Yacc.Nb8_B[0];
        payload[12] = data.Zacc.Nb8_B[1];
        payload[13] = data.Zacc.Nb8_B[0];
        payload[14] = data.Acc_event;
    }
    else if (data.type_message == TYPE_TEMP){
        payload[0] = data.day;
        payload[1] = data.month;
        payload[2] = data.year;
        payload[3] = data.hour;
        payload[4] = data.min;
        payload[5] = data.sec;
        payload[6] = 0xFF;
        payload[7] = data.type_message;
        payload[8] = data.temp.Nb8_B[1];
        payload[9] = data.temp.Nb8_B[0];  
    }
}

/**********************************************************************
 * Save an NDEF message with the dateTime, the accelerations in X, Y and Z, and the type of the accelerometer event.
 * The function is called when there is an accelerometer event that is not allowed.
 * 
 * @param  Xacc         accelerations in X in RAW VALUE (16 bits)
 * @param  Yacc         accelerations in Y in RAW VALUE (16 bits)
 * @param  Zacc         accelerations in Z in RAW VALUE (16 bits)
 * @param  Acc_event    Accelerometer event
 *                      0x01	retournement
 *                      0x02	motion +x
 *                      0x03	motion -x
 *                      0x04	motion +y
 *                      0x05	motion -y
 *                      0x06	motion +z
 *                      0x07	motion -z
 * @return none
 **********************************************************************/

void FXLS8471QSaveNdefMessage(IntTo8_t Xacc, IntTo8_t Yacc, IntTo8_t Zacc, uint8_t Acc_event) 
{
    RtccReadTimeDate(&Rtcc_read_TimeDate); //Rtcc_read_TimeDate will have latest time
    
    data.day = BcdHexToBcdDec(Rtcc_read_TimeDate.f.mday);
    data.month = BcdHexToBcdDec(Rtcc_read_TimeDate.f.mon);
    data.year = BcdHexToBcdDec(Rtcc_read_TimeDate.f.year);
    data.hour = BcdHexToBcdDec(Rtcc_read_TimeDate.f.hour);
    data.min = BcdHexToBcdDec(Rtcc_read_TimeDate.f.min);
    data.sec = BcdHexToBcdDec(Rtcc_read_TimeDate.f.sec);
    data.type_message = TYPE_ACCEL;
    data.Xacc.LongNb = Xacc.LongNb;
    data.Yacc.LongNb = Yacc.LongNb;
    data.Zacc.LongNb = Zacc.LongNb;
    data.Acc_event = Acc_event;
    
    M24LR04E_SaveNdefMessage(data, "en", &My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
}

/**********************************************************************
 * Save an NDEF message with the dateTime and the temperature.
 * The function is called when the temperature is greater than the maximum temp.
 * 
 * @param  temp         Temperature in RAW VALUE (16 bits)
 * @return none
 **********************************************************************/

void STTS751SaveNdefMessage(IntTo8_t temp) 
{
    RtccReadTimeDate(&Rtcc_read_TimeDate); //Rtcc_read_TimeDate will have latest time
    
    data.type_message = TYPE_TEMP;
    data.temp.Nb8_B[1] = temp.Nb8_B[1];
    data.temp.Nb8_B[0] = temp.Nb8_B[0];
    data.day = BcdHexToBcdDec(Rtcc_read_TimeDate.f.mday);
    data.month = BcdHexToBcdDec(Rtcc_read_TimeDate.f.mon);
    data.year = BcdHexToBcdDec(Rtcc_read_TimeDate.f.year);
    data.hour = BcdHexToBcdDec(Rtcc_read_TimeDate.f.hour);
    data.min = BcdHexToBcdDec(Rtcc_read_TimeDate.f.min);
    data.sec = BcdHexToBcdDec(Rtcc_read_TimeDate.f.sec);
    
    M24LR04E_SaveNdefRecord(data, "en", &My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
}
