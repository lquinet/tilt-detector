/* 
 * File:   NDEFMessage.h
 * Author: Denis
 *
 * Created on 12 June 2015, 11:19
 */

#ifndef NDEFMESSAGE_H
#define	NDEFMESSAGE_H

/* @LOIC:
 ** @name NdefMessageAddTextRecord
 * 
 * @param text: pointer to the string to store in the e²p
 * @param encoding: pointer to the constant string which contains the encoding of the text area ("en" for example)
 */
void NdefMessageAddTextRecord(char *text, const rom char *encoding);

#endif	/* NDEFMESSAGE_H */

