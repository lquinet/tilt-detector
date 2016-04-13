/* 
 * File:   STTS751.h
 * Author: Denis
 *
 * Created on 26 May 2015, 10:50
 */

#ifndef STTS751_H
#define	STTS751_H
#include "drivers/drv_i2c.h"
#include "user.h"
#include "../Sensors/M24LR04E_R.h"

    // I2C temperature sensor - see table 1 of data sheet.  Resistor selects address.
#define STTS751_ADDRESS 0x90//A revoir en fonction de la valeur de la résistance de pull up

#define Temperature_value_high      0x00
#define Status                      0x01
#define Temperature_value_low       0x02
#define Configuration               0x03    //POR 0000 0000
#define Conversion_rate             0x04    //POR 0000 0100
#define Temperature_high_limit_high 0x05
#define Temperature_high_limit_low  0x06
#define Temperature_low_limit_high  0x07
#define Temperature_low_limit_low   0x08
#define One_shot                    0x0F
#define THERM_limit                 0x20
#define THERM_hysteresis            0x21
#define SMBus_timeout_enable        0x22
#define Product_ID                  0xFD
#define Manufacturer_ID             0xFE
#define Revision_number             0xFF

void InitSTTS751(void);
StatusType STTS751WriteByte(I2C_message_t *MemMsg, uint8_t address, uint8_t subAddress, uint8_t data);
uint8_t STTS751ReadByte(I2C_message_t *MemMsg, uint8_t address, uint8_t subAddress);
void ReadTemperatureSTTS751(IntTo8_t *temperature);
float ConvertTemperatureSTTS751(IntTo8_t temp);

#endif	/* STTS751_H */

