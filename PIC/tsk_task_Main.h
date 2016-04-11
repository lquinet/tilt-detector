/* 
 * File:   tsk_task_Main.h
 * Author: lquin
 *
 * Created on 31 mars 2016, 13:54
 */

#ifndef TSK_TASK_MAIN_H
#define	TSK_TASK_MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "user.h"
    
#define ColisUP 0x00    // Transport OK
#define ColisDown 0x01  // Transport KO

// Convertion of a BCD number in decimal
uint8_t BcdHexToBcdDec(uint8_t Nb);

// Convertion of a decimal number in BCD
uint8_t convertCharToBCD(uint8_t toConvert);

// Convertion of an array of decimal numbers in BCD
void convertCharArrayToBCD(uint8_t *arrayToConvert, uint8_t length);

#ifdef	__cplusplus
}
#endif

#endif	/* TSK_TASK_MAIN_H */

