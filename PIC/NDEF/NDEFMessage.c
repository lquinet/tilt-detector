#include <string.h>
#include "NDEFMessage.h"
#include "NDEFRecord.h"
#include "../Sensors/M24LR04E_R.h"
#include "../RTCC/MyRTCC.h"
#include "../user.h"

extern I2C_message_t My_I2C_Message;
extern NDEFPayload_t data;
extern _NdefRecord_t NdefRecord;
extern boolean isMemoryFull; // TRUE if e²p memory is full

/**********************************************************************
 * Definition dedicated to the global variable
 **********************************************************************/
boolean isMemoryFull = MemoryNotFull;

/*******************************************************************************
 * Build the NDEF message.
 * Create the TLV tag and length, the payload length, the status byte for the payload, the encoding
 * 
 * 
 * @name NdefMessageAddTextRecord
 * 
 * @param payloadArray      pointer to the array to store in the e²p
 * @param isFirstRecord     =1 if the record is the first, =0 else
 * 
 * @return                  None
 * 
 *******************************************************************************/

void NdefMessageAddRecord(uint8_t *payloadArray, boolean isFirstRecord)
{
    // Length of the payloadArray
    uint8_t length; 
    
    // Met le payload à 0
    memset(NdefRecord._payload,0,sizeof(NdefRecord._payload));
    
    // initialisation de tous les champs fixes
    NdefRecordConstructor(isFirstRecord);
    
    // Met le nombre de place que prends le payload
    if (payloadArray[INDEX_MSG_TYPE] == TYPE_TEMP) length = NB_DATA_BYTES_TEMP;
    else length = NB_DATA_BYTES_ACCEL;
    NdefRecordSetPayloadLengh(length);
    
    // Record length = value of the payload length + value of the type length + 3 car on doit ajouter le header, le type length et le payload length
    NdefRecordSetRecordLength(NdefRecord._payloadLength + NdefRecord._typeLength + 3);
 
    memcpy(NdefRecord._payload, payloadArray, length);
    
    NdefRecord._payload[length] = NdefRecord._Teminator;
 }

/**********************************************************************
 * Build the payload (in an array). 
 * If the type of the message is for accelerometer, the payload contain the datetime, 
 * the accelerations in X, Y and Z, and the type of the accelerometer event.
 * If the type of the message is for temperature, the payload contain the datetime and the temperature.
 * 
 * @param  *payloadArray     payload (in an array) to store in NDEF message
 * @param  data         structure which contains the data to store in the payload      
 * @return Status       None
 **********************************************************************/

void BuildMessage(uint8_t *payloadArray, NDEFPayload_t data){
    
    payloadArray[0] = data.day;
    payloadArray[1] = data.month;
    payloadArray[2] = data.year;
    payloadArray[3] = data.hour;
    payloadArray[4] = data.min;
    payloadArray[5] = data.sec;
    payloadArray[6] = 0xFF;
    
    if (data.type_message == TYPE_ACCEL){
        payloadArray[7] = data.type_message;
        payloadArray[8] = data.Xacc.Nb8_B[1];
        payloadArray[9] = data.Xacc.Nb8_B[0];
        payloadArray[10] = data.Yacc.Nb8_B[1];
        payloadArray[11] = data.Yacc.Nb8_B[0];
        payloadArray[12] = data.Zacc.Nb8_B[1];
        payloadArray[13] = data.Zacc.Nb8_B[0];
        payloadArray[14] = data.Acc_event;
    }
    else if (data.type_message == TYPE_TEMP){
        payloadArray[7] = data.type_message;
        payloadArray[8] = data.temp.Nb8_B[1];
        payloadArray[9] = data.temp.Nb8_B[0];  
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
    
    if (!isMemoryFull) M24LR04E_SaveNdefRecord(data, &My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
}

/**********************************************************************
 * Save an NDEF message with the dateTime and the temperature.
 * The function is called when the temperature is greater than the maximum temp.
 * 
 * @param  temp         Temperature in RAW VALUE (16 bits)
 * @return none
 **********************************************************************/

void EMC1001SaveNdefMessage(IntTo8_t temp) 
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
    
    if (!isMemoryFull) M24LR04E_SaveNdefRecord(data, &My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
}

/**********************************************************************
 * Save a new NDEF record that contain the datetime and temperature or acceleration.
 * The NDEF record is contained in the _NdefRecord_t structure,and we'll cross the memory 
 * of this structure to send the frame to the e²p.
 * 
 * @param  data         data to store in NDEF message
 * @param  MemMsg    	IN  Mandatory I2C structure
 * @param  address    	The slave address of the M24LR04E
 * @return Status       E_OK if the EELC256 has been updated
 *                      E_OS_STATE if the I2C access failed
 **********************************************************************/
StatusType M24LR04E_SaveNdefRecord(NDEFPayload_t data, I2C_message_t *MemMsg, uint8_t address)

{
    static IntTo8_t lastSubAddressWrited = 0x08; // Last address in the e²prom memory writed to save an NDEF message. Initialy to 4 due to the capability container
    static uint8_t sizeOfLastRecord = 0;
    static IntTo8_t TLV_Length=0; // 3 Bytes format!
    uint8_t i = 0, NbByteToSend = 0, NbByteSended = 0;
    uint8_t payloadArray[NB_MAX_DATA_BYTES];
    IntTo8_t subAddress;
    
    // Building of a string from the structure NDEFPayload_t
    BuildMessage(payloadArray, data);
    
    if (lastSubAddressWrited.LongNb == 0x08) {
        // Creation of the NDEF message in NdefRecord structure
        // The record header is different if this is the first record or not
        NdefMessageAddRecord(payloadArray, 1); 
        
        M24LR04E_SetTLV_Block (MemMsg, address, 1);
    }
    else {
        NdefMessageAddRecord(payloadArray, 0);
        
        // Update the header of the last record writed
        M24LR04E_UpdateHeader (MemMsg, address, lastSubAddressWrited, sizeOfLastRecord);
        
        // Update the TLV length
        M24LR04E_SetTLV_Block (MemMsg, address, 0);
    }

    // Nb Bytes To Send = record Length + Terminator TLV
    NbByteToSend = NdefRecord._recordLength + 1;
    
    // Test if user e²prom memory is full
    if (lastSubAddressWrited.LongNb + NbByteToSend +1 >= M24LR16_EEPROM_LAST_ADDRESS_DATALOGGER)
    {
        subAddress.LongNb = M24LR16_EEPROM_ADDRESS_FULL_MEMORY;
        
        M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, MemoryFull);
        isMemoryFull = MemoryFull;
        return E_OS_STATE;
    }
    else {
        M24LR04E_WriteNBytes(MemMsg, address, lastSubAddressWrited, (unsigned char *) &NdefRecord, NbByteToSend);
    }
        
    // Retain the last adress writed
    lastSubAddressWrited.LongNb += NbByteToSend - 1;
    // Retain the size of the last record
    sizeOfLastRecord = NdefRecord._recordLength;
}

/**********************************************************************
 * Update the header of the last record to modify the MB and the ME bits.
 * 
 * @param  lastSubAddressWrited     Last adress writed in the M24LR04E memory during the last record
 * @param  sizeOfLastRecord         Size of the last record writed in the M24LR04E memory
 * @param  MemMsg                   IN  Mandatory I2C structure
 * @param  address                  The slave address of the M24LR04E
 * @return None
 **********************************************************************/
void M24LR04E_UpdateHeader (I2C_message_t *MemMsg, uint8_t address, IntTo8_t lastSubAddressWrited, uint8_t sizeOfLastRecord){
    IntTo8_t subAddress;
    static boolean LastRecordIsTheFirst=1;
    
    if (LastRecordIsTheFirst){
        // plusieurs records (MB=1, ME=0); well-known type (TNF=1); pas de record chunk ni d'ID (CF=IL=0)
        subAddress.LongNb = lastSubAddressWrited.LongNb - sizeOfLastRecord;
        M24LR04E_WriteByte(&My_I2C_Message, address, subAddress, 0x92); // 10010010
        
        LastRecordIsTheFirst = 0;
    }
    else {
        // plusieurs records (MB=0, ME=0); well-known type (TNF=1); pas de record chunk ni d'ID (CF=IL=0)
        subAddress.LongNb = lastSubAddressWrited.LongNb - sizeOfLastRecord;
        M24LR04E_WriteByte(&My_I2C_Message, address, subAddress, 0x12); // 00010010
    }
}

/**********************************************************************
 * Set or update the TLV Block (TLV length and TLV tag) at the beginning of the memory for the new record
 * 
 * @param  isFirstRecord            TRUE if the record to write in the memory is the first of the NDEF message
 * @param  MemMsg                   IN  Mandatory I2C structure
 * @param  address                  The slave address of the M24LR04E
 * @return None
 **********************************************************************/
void M24LR04E_SetTLV_Block (I2C_message_t *MemMsg, uint8_t address, boolean isFirstRecord){
    static IntTo8_t TLV_Length=0; // 3 Bytes format!
    uint8_t TLV_Block[3];
    IntTo8_t subAddress;
    
    // Écriture TLV Tag si premier NDEF record
    if (isFirstRecord){
        subAddress.LongNb = 0x04;
        M24LR04E_WriteByte(MemMsg, address, subAddress, TLV_TAG);
    }
    // Writing of the TLV Length
    // TLV_Length = the record Length
    TLV_Length.LongNb += NdefRecord._recordLength;
    
    TLV_Block[0] = 0xFF; // 3 bytes format
    TLV_Block[1] = TLV_Length.Nb8_B[1]; // byte high
    TLV_Block[2] = TLV_Length.Nb8_B[0]; // Byte low

    subAddress.LongNb = 0x05;
    M24LR04E_WriteNBytes(MemMsg, address, subAddress, TLV_Block, 3);
}


/**********************************************************************
 * Save the  Capabylity Container in M24LR04E with the values {0xE1, 0x40, 0xFF, 0x00}
 * CC byte 0: NDEF magic number (tag which contains NDEF message)
 * CC byte 1: VNo (Version NDEF utilisé; 4.0 ici)
 * CC byte 2: Tag Memory Size (Taille de la zone de données = TMS*8)
 * CC byte 3: Read/Write Access (nibble 0 = accès en lecture; nibble 1 = accès en écriture)
 *
 * @param  MemMsg    	 IN  Mandatory I2C structure
 * @return Status         E_OK if the EELC256 has been updated
 *                        E_OS_STATE if the I2C access failed
 **********************************************************************/
StatusType M24LR04E_SaveCC(I2C_message_t *MemMsg, uint8_t address)
{
    // pData[0] = CC1, pData[1] = CC2, pData[2] = CC3, pData[3] = CC4
    unsigned char pData[4] = {CC1, CC2, CC3, CC4};
    IntTo8_t subAddress;
    
    subAddress.LongNb = 0;
    M24LR04E_WriteNBytes(MemMsg, address, subAddress, pData, 4);
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
