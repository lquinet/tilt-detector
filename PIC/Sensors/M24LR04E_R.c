#include <string.h>
#include "pro_man.h"
#include "user.h"
#include "M24LR04E_R.h"
#include "NDEF/NDEFRecord.h"
//#include "tsk_task_main.h"

/**********************************************************************
 * Definition dedicated to the local functions.
 **********************************************************************/
void WaitEepResponse (void);

/**
 * @brief this function checks if a NDEF message is available in the M24LR04E-R EEPROM
 * @par PayloadLength : the number of byte of the NDEF message
 * @retval SUCCESS : A NDEF message has been found
 * @retval ERROR :  a NDEF message doens't have been found
 */
ErrorStatus User_ReadNDEFMessage(uint8_t *PayloadLength)
{
    //    uint8_t NthAttempt = 0,
    //            NbAttempt = 2;
    //
    //    *PayloadLength = 0;
    //
    //    for (NthAttempt = 0; NthAttempt < NbAttempt; NthAttempt++)
    //    {
    //        //M24LR04E_Init();
    //        // check if a NDEF message is available in the M24LR04 EEPROM
    //        if (User_CheckNDEFMessage() == SUCCESS)
    //        {
    //            User_GetPayloadLength(PayloadLength);
    //            if (PayloadLength != 0x00)
    //            {
    //                (*PayloadLength) -= 2;
    //                memset(NDEFmessage, 0, sizeof (NDEFmessage)); //InitializeBuffer(NDEFmessage, (*PayloadLength) + 10);
    //                User_GetNDEFMessage(*PayloadLength, NDEFmessage);
    //
    //                ToUpperCase(*PayloadLength, NDEFmessage);
    //
    //                return SUCCESS;
    //            }
    //        }
    //
    //    }
    //
    //    return ERROR;
}

ErrorStatus User_CheckNDEFMessage(void)
{
    //    uint8_t OneByte = 0x00;
    //    IntTo8_t ReadAddr = 0x0000;
    //
    //    // check the E1 at address 0
    //    OneByte = M24LR04E_ReadOneByte(&My_I2C_Message, M24LR16_EEPROM_ADDRESS_USER, ReadAddr);
    //    if (OneByte != 0xE1)
    //        return ERROR;
    //
    //    ReadAddr.LongNb = 0x0009;
    //    OneByte = M24LR04E_ReadOneByte(&My_I2C_Message, M24LR16_EEPROM_ADDRESS_USER, ReadAddr);
    //    // text or URL message
    //    if (OneByte != 0x54 /*&& OneByte != 0x55*/)
    //        return ERROR;
    //    return SUCCESS;
}

/**
 * @brief this functions returns the payload length
 * @par Parameters None
 * @retval SUCCESS : the M24LR16E contains a NDEF message
 * @retval ERROR : the M24LR16E donesn't contain a NDEF message
 */
ErrorStatus User_GetPayloadLength(uint8_t *PayloadLength)
{
    //    IntTo8_t ReadAddr = 0x0008;
    //
    //    *PayloadLength = M24LR04E_ReadOneByte(&My_I2C_Message, M24LR16_EEPROM_ADDRESS_USER, ReadAddr);
    //    if (*PayloadLength == 0x00)
    //        return ERROR;
    //
    //    return SUCCESS;
}

/**
 * @brief this functions converts a the characters of a string to upper case
 * @par NbCar : Number of byte of StringToConvert field
 * @par StringToConvert : pointer on the string to convert
 * @retval none
 */
void ToUpperCase(uint8_t NbCar, void *StringToConvert)
{
    //    //uint8_t Buffer[0x10] = "\0";//Vérifier la taille !!!
    //    //memset(Buffer, 0, sizeof (Buffer));
    //    //Buffer[0] = StringToConvert[0];
    //    //Buffer = StringToConvert;
    //    //StringToConvert = strupr(Buffer[0]);
    //    StringToConvert = strupr((char*)StringToConvert);
}

/**
 * @brief this functions returns the payload length
 * @par PayloadLength : the number of byte of the NDEF message
 * @par NDEFmessage : pointer on the NDEF message
 * @retval SUCCESS : the M24LR16E contains a NDEF message
 * @retval ERROR : the M24LR16E donesn't contain a NDEF message
 */
ErrorStatus User_GetNDEFMessage(uint8_t PayloadLength, uint8_t *NDEFmessage)
{
    //    IntTo8_t ReadAddr = 0x000D;
    //
    //    if (PayloadLength == 0x00)
    //        return SUCCESS;
    //
    //    M24LR04E_ReadBuffer(&My_I2C_Message,M24LR16_EEPROM_ADDRESS_USER, ReadAddr, PayloadLength, NDEFmessage);
    //
    //    return SUCCESS;
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
    unsigned char pData[10]; //attention vérifier taille tableau!!!
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

    I2C_enqMsg(MemMsg);
    SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
    WaitEvent(I2C_QUEUE_EMPTY);
    ClearEvent(I2C_QUEUE_EMPTY);

    if (MemMsg->flags.error != 0)
        // return E_OS_STATE;
        return;

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

    I2C_enqMsg(MemMsg);
    SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
    WaitEvent(I2C_QUEUE_EMPTY);
    ClearEvent(I2C_QUEUE_EMPTY);

    if (MemMsg->flags.error != 0)
        return E_OS_STATE;
    return E_OK;
}


/**********************************************************************
 * Update the M24LR04E with the Time structure passed to the function.
 *
 * @param  MemMsg    	 IN  Mandatory I2C structure
 * @param  value       	 IN  The date and time to put into the EELC256
 * @return Status         E_OK if the EELC256 has been updated
 *                        E_OS_STATE if the I2C access failed
 **********************************************************************/
StatusType M24LR04E_SaveNdefMessage(I2C_message_t *MemMsg, uint8_t address, IntTo8_t subAddress)

{
    uint8_t i = 0, NbByteToSend = 0, NbByteSended = 0;

    NbByteToSend = NdefRecord._TLV_Length + 7;
    MemMsg->control = address & 0xFE;
    //  High byte of addr, only used if high bit set
    MemMsg->addr_high = subAddress.Nb8_B[1];
    // First register Adress
    MemMsg->addr_low = subAddress.Nb8_B[0];
    // Cross the _NdefRecord_t structure to send the frame
    MemMsg->ram_data = (unsigned char *) &NdefRecord; //pData;
    // Page write = 4 bytes max.!
    MemMsg->num_bytes = 4;// sizeof(pData);
    // 0 = single byte address, 1= two byte address
    MemMsg->flags.long_addr = 1;
    // 1 = read from external, 0 = write to external
    MemMsg->flags.i2c_read = 0;
    // 1 = SMBbus Enabled, 0 = Disabled
    MemMsg->flags.SMBus = 0;
    // Attention important de reseter le flag d'erreur!!!
    MemMsg->flags.error = 0; 

    for (i = 0; i < NbByteToSend; i += 4)//on ne peut écrire que par paquet de 4 bytes
    {
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
        
        // Wait internal writing of the e²p
        WaitEepResponse();
    }
    if (MemMsg->flags.error != 0)
        return E_OS_STATE;
    return E_OK;
}

/**********************************************************************
 * Wait that the M24LR04E end its internal memory writing at the end of a page write
 *
 * @param  MemMsg    	 IN  Mandatory I2C structure
 * @param  value       	 IN  The date and time to put into the EELC256
 * @return Status         E_OK if the EELC256 has been updated
 *                        E_OS_STATE if the I2C access failed
 **********************************************************************/

void WaitEepResponse (void){
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
            SSPBUF = M24LR16_EEPROM_ADDRESS_USER & 0xFE;
            while (PIR1bits.SSPIF == 0);
            PIR1bits.SSPIF = 0;
        }

    } while (SSPCON2bits.ACKSTAT == 1);
    
    // enable the i2c interrupt enable bit (SSP1IE)
    PIE1  |= 0b00001000;
}