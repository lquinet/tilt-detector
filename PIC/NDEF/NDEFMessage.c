#include "NDEFMessage.h"
#include "NDEFRecord.h"
#include <string.h>

uint8_t NdefMessage[0xFF];

//void NdefMessageAddRomTextRecord(const rom char *text)
//{
//    NdefMessageAddRomTextRecord(text, "en");
//}

void NdefMessageAddRomTextRecord(const rom char *text, const rom char *encoding)

{

    //DATE:17/06/15 TIME:09:39                      24
    //ACC:
    //Xacc=0813mg                                   11
    //Yacc=1987mg                                   11
    //Zacc=1999mg       39
    //POSITION:DOWN                                 13
    //TEMP:26.5°C                                   11



    uint8_t RTD_TEXT[1] = {0x54}; // TODO this should be a constant or preprocessor
    memset(NdefMessage,0,sizeof(NdefMessage));
    NdefRecordConstructor();

    NdefRecordSetTnf(TNF_WELL_KNOWN);
    
    NdefRecordSetType(RTD_TEXT[0], sizeof (RTD_TEXT));

    // X is a placeholder for encoding length
    // TODO is it more efficient to build w/o string concatenation?
    //strcat(PayloadString,RTD_TEXT);

    NdefMessage[0]=RTD_TEXT[0];
    NdefMessage[1]=(uint8_t)strlenpgm(encoding);
  
    strcatpgm2ram(NdefMessage, encoding);
    strcatpgm2ram(NdefMessage, text);

   
    //PayloadString = "X" + encoding + text;
 
    NdefRecordSetPayloadLengh(strlenpgm(NdefMessage));
    //
    //    addRecord(r);
}

void NdefMessageAddTextRecord(char *text, const rom char *encoding)
{
    // @LOIC: Met toute la trame NDEF msg à 0
    memset(NdefMessage,0,sizeof(NdefMessage));
    // @LOIC: Met le payload à 0
    memset(NdefRecord._payload,0,sizeof(NdefRecord._payload));
    // @LOIC: initialisation de tous les champs fixes
    NdefRecordConstructor();
    // @LOIC: Met le nombre de place que prends le texte 
    // On doit ajouter le nb de caractères de l'encodage + 1 car il faut aussi mettre le status byte
    NdefRecordSetPayloadLengh(strlen(text)+strlenpgm(encoding)+1);
    NdefRecordSetTLV_Length(NdefRecord._payloadLength+4);
    NdefRecordSetStatusByte(strlenpgm(encoding));
    
//    NdefRecord._payload[0]=NdefRecord._RecordHeader;
//    NdefRecord._payload[1]=NdefRecord._typeLength;
//    NdefRecord._payload[2]=NdefRecord._payloadLength;
//    NdefRecord._payload[3]=NdefRecord._type[0];
    //NdefRecord._payload[4]=NdefRecord._StatusByte;
    
 
 // @LOIC:
 // Structure d'un payload de TNF "NFC Forum well-known type" et de type text:
 // Payload = Status byte + ISO/IANA language code + actual 
 // Voir NFC Forum "Text Record Type Definition"
    strcatpgm2ram( NdefRecord._payload, encoding);
    strcat( NdefRecord._payload, text);
    strcat( NdefRecord._payload,NdefRecord._Teminator); // @LOIC: Met le terminator TLV 2 fois??
}