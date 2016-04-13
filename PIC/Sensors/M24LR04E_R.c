#include <string.h>
#include "pro_man.h"
#include "user.h"
#include "M24LR04E_R.h"
#include "NDEF/NDEFRecord.h"
#include "../tsk_task_Main.h"
#include "../RTCC/MyRTCC.h"

extern I2C_message_t My_I2C_Message;
extern data_t data;

/**********************************************************************
 * Definition dedicated to the global variable
 **********************************************************************/
boolean isMemoryFull;


void M24LR04E_Init (void)
{
    IntTo8_t subAddress;
    
    // Save the Capabylity Container in M24LR04E
    M24LR04E_SaveCC(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
    
    // Set the first TLV block to terminator (0xFE) to erase the user memory
    subAddress.LongNb = 4;
    M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 4);
    
    // RF_WIP_BUSY in input
    TRISRF_WIP_BUSY = 1;
    
    // Set INT1 on the pin RF_WIP_BUSY (RB1)
    RPINR1 = 4;// INT1 mapped to RP4 (RB1)
    INTCON2bits.INTEDG1 = 1; //Interrupt on rising edge
    INTCON3bits.INT1IE = 1; //Enables the INT1 external interrupt
    INTCON3bits.INT1IP = 0; //Low Priority
    
    //INTCONbits.INT0IE = 1;
}

void M24LR04E_CompareIfDataLoggerChange (void)
{
    
}


/**
 * @brief this function reads a block of data from the M24LR16E EEPROM .
 * @param[in] pBuffer pointer to the buffer that receives the data read
 * from the EEPROM.
 * @param[in] WriteAddr EEPROM's internal address to read from.
 * @param[in] NumByteToWrite EEPROM's number of bytes to read from the EEPROM.
 * @retval None
 * @par Required preconditions:
 * None
 */

//void M24LR04E_ReadBuffer (uint8_t EE_address, uint16_t ReadAddr, uint8_t NumByteToRead,uint8_t* pBuffer)

StatusType M24LR04E_ReadBuffer(I2C_message_t *MemMsg, uint8_t address, IntTo8_t subAddress, uint8_t NumByteToRead, void *value)
{
    unsigned char pData[80]; //attention v�rifier taille tableau!!!
    unsigned char i, *ptr = (unsigned char *) value;

    MemMsg->control = address | 0x01;
    //  High byte of addr, only used if high bit set
    MemMsg->addr_high = subAddress.Nb8_B[1];
    // First register Adress
    MemMsg->addr_low = subAddress.Nb8_B[0];
    // The bit setting of flags.ptr_type
    MemMsg->ram_data = pData;
    // Must be less than 255
    MemMsg->num_bytes = NumByteToRead;
    // 0 = single byte address, 1= two byte address
    MemMsg->flags.long_addr = 1;
    // 1 = read from external, 0 = write to external
    MemMsg->flags.i2c_read = 1;
    // 1 = SMBbus Enabled, 0 = Disabled
    MemMsg->flags.SMBus = 0;
    MemMsg->next = 0;
    
    // Wait previous internal writing of the e�p
    WaitEepResponse(M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
    
    I2C_enqMsg(MemMsg);
    SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
    WaitEvent(I2C_QUEUE_EMPTY);
    ClearEvent(I2C_QUEUE_EMPTY);

    if (MemMsg->flags.error != 0)
        return E_OS_STATE;

    for (i = 0; i < NumByteToRead; i++)
        *(ptr++) = pData[i];

    return E_OK;
}

/**********************************************************************
 * Read a single byte from subaddress
 *
 * @param  MemMsg    	IN  Mandatory I2C structure
 * @param
 * @return Status         E_OK if the EEprom has been read
 *                        E_OS_STATE if the I2C access failed
 **********************************************************************/
uint8_t M24LR04E_ReadOneByte(I2C_message_t *MemMsg, uint8_t address, IntTo8_t subAddress)
{
    unsigned char pData[1];

    MemMsg->control = address | 0x01;
    //  High byte of addr, only used if high bit set
    MemMsg->addr_high = subAddress.Nb8_B[1];
    // First register Adress
    MemMsg->addr_low = subAddress.Nb8_B[0];
    // The bit setting of flags.ptr_type
    MemMsg->ram_data = pData;
    // Must be less than 255
    MemMsg->num_bytes = sizeof (pData);
    // 0 = single byte address, 1= two byte address
    MemMsg->flags.long_addr = 1;
    // 1 = read from external, 0 = write to external
    MemMsg->flags.i2c_read = 1;
    // 1 = SMBbus Enabled, 0 = Disabled
    MemMsg->flags.SMBus = 0;
    MemMsg->next = 0;

    // Wait previous internal writing of the e�p
    WaitEepResponse(M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
    
    I2C_enqMsg(MemMsg);
    SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
    WaitEvent(I2C_QUEUE_EMPTY);
    ClearEvent(I2C_QUEUE_EMPTY);

    if (MemMsg->flags.error != 0)
        return -1;

    return pData[0];
}


/**********************************************************************
 *
 *
 * @param  MemMsg    	 IN  Mandatory I2C structure
 * @param
 * @return Status         E_OK if the M24LR04E has been updated
 *                        E_OS_STATE if the I2C access failed
 **********************************************************************/
StatusType M24LR04E_WriteByte(I2C_message_t *MemMsg, uint8_t address, IntTo8_t subAddress, uint8_t data)
{
    unsigned char pData;

    MemMsg->control = address & 0xFE;
    //  High byte of addr, only used if high bit set
    MemMsg->addr_high = subAddress.Nb8_B[1];
    // First register Adress
    MemMsg->addr_low = subAddress.Nb8_B[0];
    // The bit setting of flags.ptr_type
    MemMsg->ram_data = &pData;
    // Must be less than 255
    MemMsg->num_bytes = sizeof (pData);
    // 0 = single byte address, 1= two byte address
    MemMsg->flags.long_addr = 1;
    // 1 = read from external, 0 = write to external
    MemMsg->flags.i2c_read = 0;
    // 1 = SMBbus Enabled, 0 = Disabled
    MemMsg->flags.SMBus = 0;
    MemMsg->flags.error = 0; 

    pData = data;

    // Wait previous internal writing of the e�p
    WaitEepResponse(M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
    
    I2C_enqMsg(MemMsg);
    SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
    WaitEvent(I2C_QUEUE_EMPTY);
    ClearEvent(I2C_QUEUE_EMPTY);

    if (MemMsg->flags.error != 0)
        return E_OS_STATE;
    return E_OK;
}

/**********************************************************************
 *
 *
 * @param  MemMsg    	 IN  Mandatory I2C structure
 * @param
 * @return Status         E_OK if the M24LR04E has been updated
 *                        E_OS_STATE if the I2C access failed
 **********************************************************************/
StatusType M24LR04E_WriteNBytes(I2C_message_t *MemMsg, uint8_t address, IntTo8_t subAddress, uint8_t *data, uint8_t NbByteToSend)
{
    unsigned char pData;
    uint8_t i = 0, NbByteSended = 0;
    
    MemMsg->control = address & 0xFE;
    //  High byte of addr, only used if high bit set
    MemMsg->addr_high = subAddress.Nb8_B[1];
    // First register Adress
    MemMsg->addr_low = subAddress.Nb8_B[0];
    // The bit setting of flags.ptr_type
    MemMsg->ram_data = data;
    // Must be less than 255
    MemMsg->num_bytes = NbByteToSend;
    // 0 = single byte address, 1= two byte address
    MemMsg->flags.long_addr = 1;
    // 1 = read from external, 0 = write to external
    MemMsg->flags.i2c_read = 0;
    // 1 = SMBbus Enabled, 0 = Disabled
    MemMsg->flags.SMBus = 0;
    MemMsg->flags.error = 0; 

    
    for (i = 0; i < NbByteToSend; i += 4)//on ne peut �crire que par paquet de 4 bytes
    {
        // Wait previous internal writing of the e�p
        WaitEepResponse(M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
        
        // Send the message to the I2C buffer
        I2C_enqMsg(MemMsg);
        SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
        WaitEvent(I2C_QUEUE_EMPTY);
        ClearEvent(I2C_QUEUE_EMPTY);

        NbByteSended += 4;
        if ((NbByteToSend - NbByteSended) < 4)
            MemMsg->num_bytes = (NbByteToSend - NbByteSended);
        MemMsg->addr_low += 4;
        MemMsg->ram_data+= 4;
    }
    
    if (MemMsg->flags.error != 0)
        return E_OS_STATE;
    return E_OK;
}

/**********************************************************************
 * Update the M24LR04E with the Time structure passed to the function.
 * 
 * @param  *text    	 Text to store in NDEF message
 * @param  *encoding     The string of the encoding language         
 * @param  MemMsg    	 IN  Mandatory I2C structure
 * @param  address    	 The slave address of the M24LR04E
 * @return Status         E_OK if the EELC256 has been updated
 *                        E_OS_STATE if the I2C access failed
 **********************************************************************/
StatusType M24LR04E_SaveNdefMessage(data_t data, const rom char *encoding, I2C_message_t *MemMsg, uint8_t address)

{
    static IntTo8_t lastSubAddressWrited = 4; // Last address in the e�prom memory writed to save an NDEF message. Initialy to 4 due to the capability container
    uint8_t i = 0, NbByteToSend = 0, NbByteSended = 0;
    char text[NB_MAX_DATA_BYTES];
    
    // Building of a string from the structure data_t
    BuildMessage(text, data);
    
    // Creation of the NDEF message in NdefRecord structure
    NdefMessageAddTextRecord(text, encoding);
    
    // Nb Bytes To Send = the value of the TLV Length byte + the TLV Length byte +  the TLV Length byte + TLV Tag byte + Terminator TLV
    NbByteToSend = NdefRecord._TLV_Length + 3;
    
    // Test if user e�prom memory is full
    if (lastSubAddressWrited.LongNb + NbByteToSend +1 >= M24LR16_EEPROM_LAST_ADDRESS_DATALOGGER)
    {
        IntTo8_t subAddress = M24LR16_EEPROM_ADDRESS_FULL_MEMORY;
        
        M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress,MemoryFull);
        isMemoryFull = MemoryFull;
        return E_OS_STATE;
    }
    
    MemMsg->control = address & 0xFE;
    //  High byte of addr, only used if high bit set
    MemMsg->addr_high = lastSubAddressWrited.Nb8_B[1];
    // First register Adress
    MemMsg->addr_low = lastSubAddressWrited.Nb8_B[0];
    // Cross the _NdefRecord_t structure to send the frame
    MemMsg->ram_data = (unsigned char *) &NdefRecord; //pData;
    // 1 page = 4 bytes! -> subaddress must be a multiple of 4 to write 4 bytes one shot!
    MemMsg->num_bytes = 4 - (lastSubAddressWrited.LongNb % 4);
    // 0 = single byte address, 1= two byte address
    MemMsg->flags.long_addr = 1;
    // 1 = read from external, 0 = write to external
    MemMsg->flags.i2c_read = 0;
    // 1 = SMBbus Enabled, 0 = Disabled
    MemMsg->flags.SMBus = 0;
    // Attention important de reseter le flag d'erreur!!!
    MemMsg->flags.error = 0; 
    
    // Wait previous internal writing of the e�p
    WaitEepResponse(M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
    
    // First page write
    I2C_enqMsg(MemMsg);
    SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
    WaitEvent(I2C_QUEUE_EMPTY);
    ClearEvent(I2C_QUEUE_EMPTY);
    
    NbByteSended += MemMsg->num_bytes;
    MemMsg->addr_low += MemMsg->num_bytes;
    MemMsg->ram_data+= MemMsg->num_bytes;
    if ((NbByteToSend - NbByteSended) < 4)
        MemMsg->num_bytes = (NbByteToSend - NbByteSended);
    else MemMsg->num_bytes = 4;
    
    for (i = 4 - (lastSubAddressWrited.LongNb % 4); i < NbByteToSend; i += 4)//on ne peut �crire que par paquet de 4 bytes
    {
        // Wait internal writing of the e�p
        WaitEepResponse(M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
        
        // Send the message to the I2C buffer
        I2C_enqMsg(MemMsg);
        SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
        WaitEvent(I2C_QUEUE_EMPTY);
        ClearEvent(I2C_QUEUE_EMPTY);

        NbByteSended += 4;
        if ((NbByteToSend - NbByteSended) < 4)
            MemMsg->num_bytes = (NbByteToSend - NbByteSended);
        MemMsg->addr_low += 4;
        MemMsg->ram_data+= 4;
    }
    lastSubAddressWrited.LongNb += NbByteToSend - 1;
    if (MemMsg->flags.error != 0)
        return E_OS_STATE;
    return E_OK;
}

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
    
    M24LR04E_SaveNdefMessage(data, "en", &My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
}

/**********************************************************************
 * Save the  Capabylity Container in M24LR04E with the values {0xE1, 0x40, 0xFF, 0x00}
 * CC byte 0: NDEF magic number (tag which contains NDEF message)
 * CC byte 1: VNo (Version NDEF utilis�; 4.0 ici)
 * CC byte 2: Tag Memory Size (Taille de la zone de donn�es = TMS*8)
 * CC byte 3: Read/Write Access (nibble 0 = acc�s en lecture; nibble 1 = acc�s en �criture)
 *
 * @param  MemMsg    	 IN  Mandatory I2C structure
 * @return Status         E_OK if the EELC256 has been updated
 *                        E_OS_STATE if the I2C access failed
 **********************************************************************/
StatusType M24LR04E_SaveCC(I2C_message_t *MemMsg, uint8_t address)

{
    // pData[0] = CC1, pData[1] = CC2, pData[2] = CC3, pData[3] = CC4
    unsigned char pData[4] = {CC1, CC2, CC3, CC4};
        
    MemMsg->control = address & 0xFE;
    //  High byte of addr, only used if high bit set
    MemMsg->addr_high = 0x00;
    // First register Adress
    MemMsg->addr_low = 0x00;
    // The bit setting of flags.ptr_type
    MemMsg->ram_data = pData;
    // Must be less than 255
    MemMsg->num_bytes = sizeof (pData)/sizeof (pData[0]);
    // 0 = single byte address, 1= two byte address
    MemMsg->flags.long_addr = 1;
    // 1 = read from external, 0 = write to external
    MemMsg->flags.i2c_read = 0;
    // 1 = SMBbus Enabled, 0 = Disabled
    MemMsg->flags.SMBus = 0;
    MemMsg->flags.error = 0;

    // Wait previous internal writing of the e�p
    WaitEepResponse(M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
    
    I2C_enqMsg(MemMsg);
    SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
    WaitEvent(I2C_QUEUE_EMPTY);
    ClearEvent(I2C_QUEUE_EMPTY);

    if (MemMsg->flags.error != 0)
        return E_OS_STATE;
    return E_OK;
}


/**********************************************************************
 * Wait that the M24LR04E end its internal memory writing at 
 * the end of a page write
 **********************************************************************/

void WaitEepResponse ( uint8_t address){
    /*
    // Set up the I2C port
	TRISB |= 0b00110000;
	
	SSP1CON1 = 0b00101000;
    SSPADD  = 19;       // Assuming 8MHz speed - 100KHz Clock = FOSC/(4 * (SSPxADD + 1))
	SSPSTAT = 0b10000000;   // 100Khz
	PIR2bits.BCLIF = 0;
    */
    
    // Disable the i2c interrupt enable bit (SSP1IE)
	PIE1  &= (~0b00001000);
    
    do {
        // Start I2C transfert
        SSPCON2bits.SEN = 1;
        while (PIR1bits.SSPIF == 0);
        PIR1bits.SSPIF = 0;
        
        // Send SLA+WR
        if (SSPSTATbits.S) {
            // Launch into sending next byte
            SSPSTATbits.S = 0;
            SSPBUF = address & 0xFE;
            while (PIR1bits.SSPIF == 0);
            PIR1bits.SSPIF = 0;
        }
    } while (SSPCON2bits.ACKSTAT == 1);
    
    // enable the i2c interrupt enable bit (SSP1IE)
    PIE1  |= 0b00001000;
}

void BuildMessage(char *text, data_t data){
    
    if (data.type_message == TYPE_ACCEL){
        text[0] = data.day;
        text[1] = data.month;
        text[2] = data.year;
        text[3] = data.hour;
        text[4] = data.min;
        text[5] = data.sec;
        text[6] = 0xFF;
        text[7] = data.type_message;
        text[8] = data.Xacc.Nb8_B[1];
        text[9] = data.Xacc.Nb8_B[0];
        text[10] = data.Yacc.Nb8_B[1];
        text[11] = data.Yacc.Nb8_B[0];
        text[12] = data.Zacc.Nb8_B[1];
        text[13] = data.Zacc.Nb8_B[0];
        text[14] = data.Acc_event;
    }
    else if (data.type_message == TYPE_TEMP){
        text[0] = data.day;
        text[1] = data.month;
        text[2] = data.year;
        text[3] = data.hour;
        text[4] = data.min;
        text[5] = data.sec;
        text[6] = 0xFF;
        text[7] = data.type_message;
        text[8] = data.temp.Nb8_B[1];
        text[9] = data.temp.Nb8_B[0];  
    }
}

void writeDateTimeToConfigurationByte (void){
    uint8_t dateTimeToWrite[6];
    IntTo8_t subAddress = M24LR16_EEPROM_ADDRESS_DATE_RTC;
    RtccReadTimeDate(&Rtcc_read_TimeDate); //Rtcc_read_TimeDate will have latest time
    
    dateTimeToWrite[0] = BcdHexToBcdDec(Rtcc_read_TimeDate.f.mday);
    dateTimeToWrite[1] = BcdHexToBcdDec(Rtcc_read_TimeDate.f.mon);
    dateTimeToWrite[2] = BcdHexToBcdDec(Rtcc_read_TimeDate.f.year);
    dateTimeToWrite[3] = BcdHexToBcdDec(Rtcc_read_TimeDate.f.hour);
    dateTimeToWrite[4] = BcdHexToBcdDec(Rtcc_read_TimeDate.f.min);
    dateTimeToWrite[5] = BcdHexToBcdDec(Rtcc_read_TimeDate.f.sec);
    
    M24LR04E_WriteNBytes(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, dateTimeToWrite, 6);
}