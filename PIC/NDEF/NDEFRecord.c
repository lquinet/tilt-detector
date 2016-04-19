#include "NDEFRecord.h"
#include "../user.h"


/**********************************************************************
 * Definition dedicated to the global variable
 **********************************************************************/
// variable utilisée dans les fonctions de NDEFMessage.c et M24LR04E_R
_NdefRecord_t NdefRecord;

void NdefRecordConstructor( boolean isFirstRecord)
{
    /******************* Memory ************************/
    // TLV Block
    // Value 0x03: Block contains an NDEF message
    // NdefRecord._TLV_Tag =0x03 ;
    // NdefRecord._TLV_Length =0x00;
    
    /******************* NDEF Record ************************/
    if (isFirstRecord){
        // 1 seul record (MB=1, ME=1); well-known type (TNF=1); pas de record chunk ni d'ID (CF=IL=0)
        NdefRecord._RecordHeader=0xD1;
    }
    else {
        // plusieurs records (MB=1, ME=1); well-known type (TNF=1); pas de record chunk ni d'ID (CF=IL=0)
        NdefRecord._RecordHeader=0x51;
    }
    
    NdefRecord._typeLength = 1;
    NdefRecord._payloadLength = 0;
    NdefRecord._type[0] = 'T'; // Type = 'T': encodage du payload en texte
    NdefRecord._Teminator =0xFE;
    
}

void NdefRecordSetType(uint8_t type, uint8_t numBytes)
{

    NdefRecord._type[0] = type;
    NdefRecord._typeLength = numBytes;
    
}

void NdefRecordSetPayloadLengh(uint8_t numBytes)
{
    NdefRecord._payloadLength = numBytes;
}

void NdefRecordSetTLV_Length(uint8_t numBytes)
{
//    NdefRecord._TLV_Length = numBytes;
}

void NdefRecordSetStatusByte(uint8_t numBytes)
{
    NdefRecord._StatusByte = numBytes;
}

void NdefRecordSetRecordLength(uint8_t numBytes)
{
    NdefRecord._recordLength = numBytes;
}