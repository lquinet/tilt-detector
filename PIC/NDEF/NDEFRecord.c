#include "NDEFRecord.h"


_NdefRecord_t NdefRecord;

void NdefRecordConstructor(void)
{
    /******************* Memory ************************/
    // TLV Block
    // Value 0x03: Block contains an NDEF message
    NdefRecord._TLV_Tag =0x03 ;
    NdefRecord._TLV_Length =0x00;
    
    /******************* NDEF Record ************************/
    // 1 seul record par NDEF msg (MB=1, ME=1); well-known type (TNF=1); pas de record chunk ni d'ID (CF=IL=0)
    NdefRecord._RecordHeader=0xD1;
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
    NdefRecord._TLV_Length = numBytes;
}

void NdefRecordSetStatusByte(uint8_t numBytes)
{
    NdefRecord._StatusByte = numBytes;
}