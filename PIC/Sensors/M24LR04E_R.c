#include <string.h>
#include "../Include/pro_man.h"
#include "../user.h"
#include "M24LR04E_R.h"
#include "../NDEF/NDEFMessage.h"
#include "../NDEF/NDEFRecord.h"
#include "../RTCC/MyRTCC.h"
#include "EMC1001.h"

extern I2C_message_t My_I2C_Message;
extern NDEFPayload_t data;
extern _NdefRecord_t NdefRecord;
extern uint8_t statusPackage; // Status of the package (UP or DOWN)


void M24LR04E_Init (void)
{
    IntTo8_t subAddress;
    uint8_t RF_ChangeByte;
    
    // Save the Capabylity Container in M24LR04E
    M24LR04E_SaveCC(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
    
    
    // Read RF_CHANGE byte in e²p
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_RF_CHANGE;
    RF_ChangeByte = M24LR04E_ReadOneByte(&My_I2C_Message , M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress);
    
    if (RF_ChangeByte == RF_Change_Reset){
        // clear RF_CHANGE byte in e²p
        subAddress.LongNb = M24LR16_EEPROM_ADDRESS_RF_CHANGE;
        M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress,0);

        // Set the first TLV block to terminator (0xFE) to erase the user memory
        subAddress.LongNb = 0x04;
        M24LR04E_WriteByte(&My_I2C_Message,M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 0xFE);
    }
    
    // RF_WIP_BUSY in input
    TRISRF_WIP_BUSY = 1;
    
    // Set INT1 on the pin RF_WIP_BUSY (RB1)
    RPINR1 = 4;// INT1 mapped to RP4 (RB1)
    INTCON2bits.INTEDG1 = 1; //Interrupt on rising edge
    INTCON3bits.INT1IE = 1; //Enables the INT1 external interrupt
    INTCON3bits.INT1IP = 0; //Low Priority
    
    //INTCONbits.INT0IE = 1;
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
    unsigned char pData[80]; //attention vérifier taille tableau!!!
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
    
    // Wait previous internal writing of the e²p
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

    // Wait previous internal writing of the e²p
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
 * This function read the configuration bytes in M24LR04E memory and store data in _ConfigBytes_t struct
 * Convert the temperature in float and the dateTime in BCD.
 *
 * @param   configBytesStruct   Pointer to the _ConfigBytes_t struct to store the configuration bytes
 * @return  void
 **********************************************************************/
void M24LR04E_ReadConfigurationBytes(_ConfigBytes_t *configBytesStruct)
{
    IntTo8_t subAddress, temperatureIntTo8;
    uint8_t configurationBytes[24];
    uint8_t errorState;
    
    subAddress.LongNb = M24LR16_EEPROM_LAST_ADDRESS_DATALOGGER + 1;
    errorState = M24LR04E_ReadBuffer(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress, 24, configurationBytes);
    if (errorState == E_OK){
        // Status Package
        configBytesStruct->statusPackage = configurationBytes[0];
        
        //RF_Change
        configBytesStruct->RF_Change = configurationBytes[1];
        
        // DateTime
        memcpy(configBytesStruct->DateTime, configurationBytes+4, 6);
        convertCharArrayToBCD(configBytesStruct->DateTime, 6);
        
        // Accelerations limits
        configBytesStruct->XaccMax.Nb8_B[1] = configurationBytes[12];
        configBytesStruct->XaccMax.Nb8_B[0] = configurationBytes[13];
        configBytesStruct->YaccMax.Nb8_B[1] = configurationBytes[14];
        configBytesStruct->YaccMax.Nb8_B[0] = configurationBytes[15];
        configBytesStruct->ZaccMax.Nb8_B[1] = configurationBytes[16];
        configBytesStruct->ZaccMax.Nb8_B[0] = configurationBytes[17];
        configBytesStruct->Thresold_X_Y_Z = configurationBytes[18];
        
        // Temperature limits
        temperatureIntTo8.Nb8_B[1] = configurationBytes[20];
        temperatureIntTo8.Nb8_B[0] = configurationBytes[21];
        configBytesStruct->tempMax = emc1001_convertTemperature(temperatureIntTo8);
    }
} 

uint8_t M24LR04E_ReadRF_Change(void)
{
    IntTo8_t subAddress;
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_RF_CHANGE;
    return M24LR04E_ReadOneByte(&My_I2C_Message, M24LR16_EEPROM_I2C_SLAVE_ADDRESS, subAddress);
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

    // Wait previous internal writing of the e²p
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
    uint8_t i = 0, NbByteSended = 0;
    
    MemMsg->control = address & 0xFE;
    //  High byte of addr, only used if high bit set
    MemMsg->addr_high = subAddress.Nb8_B[1];
    // First register Adress
    MemMsg->addr_low = subAddress.Nb8_B[0];
    // Cross the _NdefRecord_t structure to send the frame
    MemMsg->ram_data = data;
    // 1 page = 4 bytes! -> subaddress must be a multiple of 4 to write 4 bytes one shot!
    MemMsg->num_bytes = 4 - (subAddress.LongNb % 4);
    // 0 = single byte address, 1= two byte address
    MemMsg->flags.long_addr = 1;
    // 1 = read from external, 0 = write to external
    MemMsg->flags.i2c_read = 0;
    // 1 = SMBbus Enabled, 0 = Disabled
    MemMsg->flags.SMBus = 0;
    // Attention important de reseter le flag d'erreur!!!
    MemMsg->flags.error = 0; 
    
    // Wait previous internal writing of the e²p
    WaitEepResponse(M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
    
    // First page write
    I2C_enqMsg(MemMsg);
    SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
    WaitEvent(I2C_QUEUE_EMPTY);
    ClearEvent(I2C_QUEUE_EMPTY);
    
    NbByteSended += MemMsg->num_bytes;
    subAddress.LongNb += MemMsg->num_bytes;
    MemMsg->addr_high = subAddress.Nb8_B[1];
    MemMsg->addr_low = subAddress.Nb8_B[0];
    MemMsg->ram_data+= MemMsg->num_bytes;
    
    if ((NbByteToSend - NbByteSended) < 4)
        MemMsg->num_bytes = (NbByteToSend - NbByteSended);
    else MemMsg->num_bytes = 4;
    
    for (i = 4 - (subAddress.LongNb % 4); i < NbByteToSend; i += 4)//on ne peut écrire que par paquet de 4 bytes
    {
        // Wait internal writing of the e²p
        WaitEepResponse(M24LR16_EEPROM_I2C_SLAVE_ADDRESS);
        
        // Send the message to the I2C buffer
        I2C_enqMsg(MemMsg);
        SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
        WaitEvent(I2C_QUEUE_EMPTY);
        ClearEvent(I2C_QUEUE_EMPTY);

        NbByteSended += 4;
        if ((NbByteToSend - NbByteSended) < 4)
            MemMsg->num_bytes = (NbByteToSend - NbByteSended);
        subAddress.LongNb += 4;
        MemMsg->addr_high = subAddress.Nb8_B[1];
        MemMsg->addr_low = subAddress.Nb8_B[0];
        MemMsg->ram_data+= 4;
    }
    
    if (MemMsg->flags.error != 0)
        return E_OS_STATE;
    return E_OK;
}

/**********************************************************************
 * Save a new NDEF message that contain the datetime and temperature or acceleration.
 * The NDEFMessage is contained in the _NdefRecord_t structure,and we'll cross the memory 
 * of this structure to send the frame to the e²p.
 * 
 * @param  data         data to store in NDEF message
 * @param  *encoding    The string of the encoding language         
 * @param  MemMsg    	IN  Mandatory I2C structure
 * @param  address    	The slave address of the M24LR04E
 * @return Status       E_OK if the EELC256 has been updated
 *                      E_OS_STATE if the I2C access failed
 **********************************************************************/

StatusType M24LR04E_SaveNdefMessage(NDEFPayload_t data, const rom char *encoding, I2C_message_t *MemMsg, uint8_t address)

{
    /*
     
    static IntTo8_t lastSubAddressWrited = 4; // Last address in the e²prom memory writed to save an NDEF message. Initialy to 4 due to the capability container
    uint8_t i = 0, NbByteToSend = 0, NbByteSended = 0;
    char text[NB_MAX_DATA_BYTES];
    
    // Building of a string from the structure NDEFPayload_t
    BuildMessage(text, data);
    
    // Creation of the NDEF message in NdefRecord structure
    NdefMessageAddTextRecord(text, encoding);
    
    // Nb Bytes To Send = the value of the TLV Length byte + the TLV Length byte +  the TLV Length byte + TLV Tag byte + Terminator TLV
    NbByteToSend = NdefRecord._TLV_Length + 3;
    
    // Test if user e²prom memory is full
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
    
    // Wait previous internal writing of the e²p
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
    
    for (i = 4 - (lastSubAddressWrited.LongNb % 4); i < NbByteToSend; i += 4)//on ne peut écrire que par paquet de 4 bytes
    {
        // Wait internal writing of the e²p
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
     
    */
}

/**********************************************************************
 * Wait that the M24LR04E end its internal memory writing at 
 * the end of a page write
 * 
 * @param   address    	 The slave address of the M24LR04E
 * @return  Status       None
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

/**********************************************************************
 * Write the new DateTime in the configuration bytes of M24LR04E memory.
 * This function is called every 10 minutes by the main task.
 * 
 * @param   None
 * @return  None
 **********************************************************************/
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

/**********************************************************************
 * Set the status of the package at M24LR16_EEPROM_ADDRESS_STATUS_PACKAGE in the M24LR04E
 * configuration byte.
 *         
 * @param  MemMsg    	IN  Mandatory I2C structure
 * @param  address    	The slave address of the M24LR04E
 * @return None
 **********************************************************************/
void SetStatusPackageDown (I2C_message_t *MemMsg, uint8_t address){
    IntTo8_t subAddress;
    statusPackage = ColisDown;
    
    subAddress.LongNb = M24LR16_EEPROM_ADDRESS_STATUS_PACKAGE;
    M24LR04E_WriteByte(MemMsg, address, subAddress, ColisDown);
}