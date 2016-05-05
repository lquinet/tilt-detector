#include "NDEFRecord.h"
#include "../user.h"
#include <string.h>

/**********************************************************************
 * Definition dedicated to the global variable
 **********************************************************************/
// variable utilisée dans les fonctions de NDEFMessage.c et M24LR04E_R
_NdefRecord_t NdefRecord;

void NdefRecordConstructor(boolean isFirstRecord)
{
    /******************* Memory ************************/
    // TLV Block
    // Value 0x03: Block contains an NDEF message
    // NdefRecord._TLV_Tag =0x03 ;
    // NdefRecord._TLV_Length =0x00;
    
    /******************* NDEF Record ************************/
    if (isFirstRecord){
        // 1 seul record (MB=1, ME=1); MIME media type (TNF=0x02); pas de record chunk ni d'ID (CF=IL=0)
        NdefRecord._RecordHeader=0xD2; // 11010010
    }
    else {
        // plusieurs records (MB=0, ME=1); MIME media type (TNF=0x02); pas de record chunk ni d'ID (CF=IL=0)
        NdefRecord._RecordHeader=0x52; // 1010010
    }
    
    NdefRecord._typeLength = TYPE_LENGTH;
    NdefRecord._payloadLength = 0;
    memcpypgm2ram(NdefRecord._type, "application/octet-stream", TYPE_LENGTH);
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


void NdefRecordSetRecordLength(uint8_t numBytes)
{
    NdefRecord._recordLength = numBytes;
}

void NdefRecordSetStatusByte(uint8_t numBytes)
{
    //NdefRecord._StatusByte = numBytes;
}

void NdefRecordSetTLV_Length(uint8_t numBytes)
{
//    NdefRecord._TLV_Length = numBytes;
}