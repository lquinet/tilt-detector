#include "NDEFMessage.h"
#include "NDEFRecord.h"
#include <string.h>

void NdefMessageAddTextRecord(char *text, const rom char *encoding)
{
    char string[50];
    // @LOIC: Met le payload à 0
    memset(NdefRecord._payload,0,sizeof(NdefRecord._payload));
    // @LOIC: initialisation de tous les champs fixes
    NdefRecordConstructor();
    // @LOIC: Met le nombre de place que prends le texte 
    // On doit ajouter le nb de caractères de l'encodage + 1 car il faut aussi mettre le status byte
    NdefRecordSetPayloadLengh(strlen(text)+strlenpgm(encoding)+1);
    // TLV length = payload length + 4 car on doit ajouter le cabability container CC1, CC2, CC3, CC4
    NdefRecordSetTLV_Length(NdefRecord._payloadLength+4);
    // Satus byte = encoding length car UTF-8
    NdefRecordSetStatusByte(strlenpgm(encoding));
 
 // @LOIC:
 // Structure d'un payload de TNF "NFC Forum well-known type" et de type text:
 // Payload = Status byte + ISO/IANA language code + actual 
 // Voir NFC Forum "Text Record Type Definition"
    strcatpgm2ram( NdefRecord._payload, encoding);
    strcat( NdefRecord._payload, text);
    strcat( NdefRecord._payload,NdefRecord._Teminator); // @LOIC: Met le terminator TLV 2 fois??
    strcpy(string, NdefRecord._payload);
}