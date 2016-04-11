/* 
 * File:   NDEFMessage.h
 * Author: Denis
 *
 * Created on 12 June 2015, 11:19
 */

#ifndef NDEFMESSAGE_H
#define	NDEFMESSAGE_H

#include "../user.h"

/* @LOIC:
 ** @name NdefMessageAddTextRecord
 * 
 * @param text: pointer to the string to store in the e²p
 * @param encoding: pointer to the constant string which contains the encoding of the text area ("en" for example)
 */
void NdefMessageAddTextRecord(char *text, const rom char *encoding);

/**********************************************************************
 * Concatenate the data to send by NDEF in the payload, with the terminator at the end
 * Don't use strcat because there are no '\0' at the end of the string (impossible because the data can contain
 * null bytes!)
 * 
 * @param  *text    	 data to store in NDEF record
 * @param  *payload      payload of the NDEF record
 * @param  address    	 The slave address of the M24LR04E
 * @return none
 **********************************************************************/
void DataCat(uint8_t *payload, char *text, uint8_t length);

#endif	/* NDEFMESSAGE_H */

