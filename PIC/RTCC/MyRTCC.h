/* 
 * File:   MyRTCC.h
 * Author: Denis
 *
 * Created on 16 June 2015, 08:16
 */

#ifndef MYRTCC_H
#define	MYRTCC_H

#include <rtcc.h>
#include  <dpslp.h> // Pour librairie rtcc.h

extern rtccTimeDate RtccTimeDate;
extern rtccTimeDate Rtcc_read_TimeDate;

void StartRTCC(unsigned char value[]);
void RTCC_configure(unsigned char value[]);
    
#endif	/* MYRTCC_H */

