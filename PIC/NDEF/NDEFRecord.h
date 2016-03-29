/* 
 * File:   NDEFRecord.h
 * Author: Denis
 *
 * Created on 12 June 2015, 11:25
 */

#ifndef NDEFRECORD_H
#define	NDEFRECORD_H

#include "user.h"

#define TNF_EMPTY 0x0
#define TNF_WELL_KNOWN 0x01
#define TNF_MIME_MEDIA 0x02
#define TNF_ABSOLUTE_URI 0x03
#define TNF_EXTERNAL_TYPE 0x04
#define TNF_UNKNOWN 0x05
#define TNF_UNCHANGED 0x06
#define TNF_RESERVED 0x07
  

void NdefRecordConstructor(void);
////    NdefRecord(const NdefRecord& rhs);
////   // ~NdefRecord();
////    NdefRecord & operator=(const NdefRecord& rhs);
////
////    int getEncodedSize();
////    void encode(byte *data, bool firstRecord, bool lastRecord);
////
////    unsigned int getTypeLength();
////    int getPayloadLength();
////    unsigned int getIdLength();
////
////    byte getTnf();
////    void getType(byte *type);
////    void getPayload(byte *payload);
////    void getId(byte *id);
////
////    // convenience methods
////    String getType();
////    String getId();
////
void NdefRecordSetTnf(uint8_t tnf);
void NdefRecordSetType(uint8_t type, uint8_t numBytes);
void NdefRecordSetPayloadAccValue(float AccX, float AccY, float AccZ);
void NdefRecordSetPayloadLengh(uint8_t numBytes);
void NdefRecordSetTLV_Length(uint8_t numBytes);
void NdefRecordSetStatusByte(uint8_t numBytes);

/**************** _NdefRecord_t: Structure of an NDEF record **********************************************************
 * @LOIC: 
 * CC = Capabylity Container (unused here! send at the initialization of the device!)
 * _CC_C0: CC byte 0: NDEF magic number (tag which contains NDEF message)
 * _CC_C1: CC byte 1: VNo (Version NDEF utilisé; 4.0 ici)
 * _CC_C2: CC byte 2: Tag Memory Size (Taille de la zone de données = TMS*8)
 * _CC_C3: CC byte 3: Read/Write Access (nibble 0 = accès en lecture; nibble 1 = accès en écriture)
 * 
 * TLV Block
 * In order to store an NDEF Message on the e²p, the message needs to be encapsulated inside a TLV Block.
 * _TLV_Tag:    Tag Field: uses a single-byte to identify the type of TLV block
 *              Value 0x03: Block contains an NDEF message
 * _TLV_Length: Length Field: contains the size (in bytes) of the value field (one byte format or 3 bytes format); 
                               (The value field is the field which contains data)
 * _Teminator:  Terminator TLV: it is THE LAST TLV block in the data area, and consist of a single byte: 0x0FE,
 *                              indicates that the remaining memory is empty
 * 
 * NDEF Record 
 * _RecordHeader:   Header: 1er octet du NDEF record (bits MB, ME, etc.) 
 * _typeLength:     specifies the length in octets of the TYPE field
 * _payloadLength:  specifies the length in octets of the payload (payloadLength is a single byte if SR flag set in the header)
 * _type:           identifier describing the type of the payload (Here Type = 'T': encoding of the payload in text)
 * 
 * Inside the payload (for a Text Record Content Syntax)
 * _StatusByte: indicates the encoding of the text ( UTF-8 or UTF16) and the length of the IANA language code
 * 
 * @rmq1: utilise des short records (payload length = 1 octet)
 * @rmq2: n'utilise pas d'ID -> le type URI n'est pas utilisé
 **********************************************************************/
typedef struct Struct_NdefRecord {
  //  uint8_t _CC_C0;         //magicNumber
  //  uint8_t _CC_C1;         
  //  uint8_t _CC_C2;         
  //  uint8_t _CC_C3;         
    uint8_t _TLV_Tag;       
    uint8_t _TLV_Length;    
    uint8_t _RecordHeader; 
    uint8_t _typeLength;
    uint8_t _payloadLength;
    uint8_t _type[1];
    uint8_t _StatusByte;    
  //  uint8_t _LanguageCode;
    uint8_t _payload[200];
    uint8_t _Teminator[2];  // 2 bytes to use strcat

} _NdefRecord_t, *_NdefRecord_tRef;

// variable utilisée dans les fonctions de NDEFMessage.c
extern _NdefRecord_t NdefRecord;

#endif	/* NDEFRECORD_H */

