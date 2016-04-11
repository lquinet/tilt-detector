#include "NDEFMessage.h"
#include "NDEFRecord.h"
#include "../Sensors/M24LR04E_R.h"
#include <string.h>

void NdefMessageAddTextRecord(char *text, const rom char *encoding)
{
    // Length of the text
    uint8_t length; 
    
    // @LOIC: Met le payload à 0
    memset(NdefRecord._payload,0,sizeof(NdefRecord._payload));
    // @LOIC: initialisation de tous les champs fixes
    NdefRecordConstructor();
    // @LOIC: Met le nombre de place que prends le texte 
    // On doit ajouter le nb de caractères de l'encodage + 1 car il faut aussi mettre le status byte
    if (text[INDEX_MSG_TYPE] == TYPE_TEMP) length = NB_DATA_BYTES_TEMP;
    else length = NB_DATA_BYTES_ACCEL;
    NdefRecordSetPayloadLengh(length + strlenpgm(encoding) +1);
    // TLV length = payload length + 4 car on doit ajouter le cabability container CC1, CC2, CC3, CC4
    NdefRecordSetTLV_Length(NdefRecord._payloadLength+4);
    // Satus byte = encoding length car UTF-8
    NdefRecordSetStatusByte(strlenpgm(encoding));
 
 // @LOIC:
 // Structure d'un payload de TNF "NFC Forum well-known type" et de type text:
 // Payload = Status byte + ISO/IANA language code + actual 
 // Voir NFC Forum "Text Record Type Definition"
    strcatpgm2ram( NdefRecord._payload, encoding);
    DataCat(NdefRecord._payload, text, length);
}

/**********************************************************************
 * Concatenate the data to send by NDEF in the payload, with the terminator at the end
 * Don't use strcat because there are no '\0' at the end of the string (impossible because the data can contain
 * null bytes!)
 * 
 * @param  *text    	 data to store in NDEF record
 * @param  *payload      payload of the NDEF record
 * @param  length        length of the data
 * @return none
 **********************************************************************/

void DataCat(uint8_t *payload, char *text, uint8_t length){
    uint8_t i, payloadlength;
    payloadlength = strlen(payload);
    
    for (i= 0; i< length; i++){
        payload[i + payloadlength] = text[i];
    }
    payload[i + payloadlength] = NdefRecord._Teminator[0];
}
 
