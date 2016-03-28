/**********************************************************************/
/*                                                                    */
/* File name: drv_i2c.h                                               */
/*                                                                    */
/* Since:     2004-Aug-10                                             */
/*                                                                    */
/* Version:   PICos18 v2.00                                           */
/*            Copyright (C) 2003, 2004 Pragmatec.                     */
/*            I2C driver v1.04                                        */
/*                                                                    */
/* Author:    DEVINE, Dan [DD] (ddevine@nwi-online.com)               */
/*            ROZIER Bertrand [RZR] bertrand.rozier@pragmatec.net     */
/*                                                                    */
/* Purpose:   I2C communications task, allowing other client          */
/*            tasks to share operation of the PIC18 MSSP module.      */
/*                                                                    */
/* Distribution: This file is part of PICos18.                        */
/*            PICos18 is free software; you can redistribute it       */
/*            and/or modify it under the terms of the GNU General     */
/*            Public License as published by the Free Software        */
/*            Foundation; either version 2, or (at your option)       */
/*            any later version.                                      */
/*                                                                    */
/*            PICos18 is distributed in the hope that it will be      */
/*            useful, but WITHOUT ANY WARRANTY; without even the      */
/*            implied warranty of MERCHANTABILITY or FITNESS FOR A    */
/*            PARTICULAR PURPOSE.  See the GNU General Public         */
/*            License for more details.                               */
/*                                                                    */
/*            You should have received a copy of the GNU General      */
/*            Public License along with gpsim; see the file           */
/*            COPYING.txt. If not, write to the Free Software         */
/*            Foundation, 59 Temple Place - Suite 330,                */
/*            Boston, MA 02111-1307, USA.                             */
/*                                                                    */
/*          > A special exception to the GPL can be applied should    */
/*            you wish to distribute a combined work that includes    */
/*            PICos18, without being obliged to provide the source    */
/*            code for any proprietary components.                    */
/*                                                                    */
/* History:                                                           */
/*   2004/09/10  [DD] Create this file.                               */
/*   2004/11/23  [RZR] Update for PICos18 V2 & Add TimeOut            */
/*                                                                    */
/**********************************************************************/


#ifndef _I2C_MGR_H_		//	Begin include sandwich
#define _I2C_MGR_H_

#include "define.h"

#ifndef NULL
#define NULL 0
#endif

/**********************************************************************
 *
 *	I2C bus states
 *
 *********************************************************************/
#define BUS_IDLE                           0	
#define BUS_START                          1
#define BUS_RESTART                        2
#define BUS_STOP                           3
#define BUS_SEND_ACK_NACK                  4
#define BUS_GEN_CALL                       5
#define BUS_READ_DATA                      6
#define BUS_CTRL_READ                      7
#define BUS_CTRL_WRITE                     8
#define BUS_WRITE_ADDR_L                   9
#define BUS_WRITE_ADDR_H                  10
#define BUS_WRITE_DATA                    11


/**********************************************************************
 *
 *	I2C Manager error codes
 *
 *********************************************************************/
#define ERR_I2C_SUCCESS                    1
#define ERR_I2C_NOSLAVE                    2
#define ERR_I2C_NACK_ADDR                  4
#define ERR_I2C_NACK_DATA                  8
#define ERR_I2C_TIMEOUT                   16

/**********************************************************************
 * 	Pre-declare Flag bits for inclusion in the generic packet type
 * 	below.
 * 	
 * 	These flags allow the client task to determine the status
 * 	of their request or to set special modifiers to be used
 * 	with the data object to be sent/received.
 *********************************************************************/
typedef	struct flag
	{
		// Flags to be set by the client routine;
		// controls transmission/reception
		unsigned long_addr  :1;	//	0 = single byte address, 1= two byte address
		unsigned i2c_read   :1;	//	1 = read from external, 0 = write to external
		unsigned SMBus      :1;	//	1 = SMBbus Enabled, 0 = Disabled
		unsigned extra2     :1;	//	Not used
		
		// Flags to be set by i2c routines; 
		// giving status to client
		unsigned error      :1;	//	Packet error, slave not responding
		unsigned busy       :1;	//	Packet is still enqueued, 0=done		
		unsigned read_phase :1;	//	Read phase of data xfer
		unsigned a7         :1;
	} flag_t;


/**********************************************************************
 *	Data message type for use by client routines.
 *	
 *	Clients will create a data packet with appropriate
 *	values for the below fields.  Client will then
 *	enqueue the message object in the linked list of
 *	I2C operations to be performed.
 *	
 *	Todo:  Find a better way of allowing both pointers
 *	to RAM and pointers to ROM without having to include
 *	two blank fields.  Sorry, this is the best I could
 *	figure right now...
 *********************************************************************/	
typedef struct _I2C_message
	{
		unsigned char control;      //	Standard control code.
		unsigned char addr_high;    //	High byte of addr, only used if high bit set
		unsigned char addr_low;     //	Low byte of addr, used with all messages
		ram unsigned char *ram_data;//	the bit setting of flags.ptr_type
		unsigned char num_bytes;    //	Must be less than 255
		unsigned error		:5;     //	0:SUCCESS, otherwise error code
		unsigned retry_counter 	:3; //	Nb of retry allowed
		TaskType CallerID;          //	ID of task to notify
		struct _I2C_message *next;  //	Pointer to next message
		flag_t   flags;             //	Bit flags for control of operation
	} I2C_message_t, *I2C_message_tRef;



/**********************************************************************
 *
 *	Enqueue a client packet object into the I2C task queue.
 *
 *	Once placed in queue, client must not modify the data
 *	otherwise unpredictable results.  To safely change the object,
 *	dequeue, modify, re-enqueue.
 *
 *	Returns 1 if successfull, 0 if message could not be enqueued
 *********************************************************************/

unsigned char I2C_enqMsg(I2C_message_tRef toEnqueue);


#endif	//	end include sandwich


/* End of File : drv_i2c.h  */
