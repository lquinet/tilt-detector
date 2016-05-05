/* 
 * File:   MyRTCC.h
 * Author: Denis
 *
 * Created on 16 June 2015, 08:16
 */

#ifndef MYRTCC_H
#define	MYRTCC_H

//#define DEBUG_RTCC

#include "../user.h"
#include <rtcc.h>
#include  <dpslp.h> // Pour librairie rtcc.h

extern rtccTimeDate RtccTimeDate;
extern rtccTimeDate Rtcc_read_TimeDate;

void StartRTCC(unsigned char value[]);
void RTCC_configure(unsigned char value[]);
// Convertion of a BCD number in decimal
uint8_t BcdHexToBcdDec(uint8_t Nb);
// Convertion of a decimal number in BCD
uint8_t convertCharToBCD(uint8_t toConvert);
// Convertion of an array of decimal numbers in BCD
void convertCharArrayToBCD(uint8_t *arrayToConvert, uint8_t length);

#endif	/* MYRTCC_H */

