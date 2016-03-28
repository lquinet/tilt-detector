/**********************************************************************/
/*                                                                    */
/* File name: drv_i2c.c                                               */
/*                                                                    */
/* Since:     2004-Aug-10                                             */
/*                                                                    */
/* Version:   PICos18 v2.00                                           */
/*            Copyright (C) 2003, 2004 Pragmatec.                     */
/*            I2C driver v1.04                                        */
/*                                                                    */
/* Author:    DEVINE, Dan [DD] (ddevine@nwi-online.com) 			  */
/*			  ROZIER Bertrand [RZR] bertrand.rozier@pragmatec.net     */
/*																	  */
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
/*   2004/09/10  [DD]  Create this file.                              */
/*	 2004/11/23	 [RZR] Update for PICos18 V2 & Add TimeOut			  */
/*	 2004/11/23	 [XM]  Added Suspend(Resume)OSInterrupts() calls in   */
/*                     I2C_deqMsg function.                           */
/*                                                                    */
/**********************************************************************/


#include "drivers/drv_i2c.h"
#include "../define.h"

/**********************************************************************
 *      Forward declarations of local functions not for public
 *
 *********************************************************************/


I2C_message_tRef	I2C_deqMsg(void); 

I2C_message_tRef	I2C_list_head;		//	Start of message queue
I2C_message_tRef	I2C_current_message;//	Current message

//#pragma udata bank5=0x850
unsigned char		I2C_list_count;		//	Number of items currently in queue
unsigned char 		I2C_bus_state;		//	Current/next I2C state/action

unsigned char		*p_data;			//	Pointer into message data
unsigned char		I2C_byte_count;		//	Counts from 0 to 
										//	current_message.num_bytes


/**********************************************************************
 *
 *							I2C DRIVER
 *
 *********************************************************************/
TASK(I2C_Drv)
{
	EventMaskType  Mask_event;
	  
	// Set up the I2C port
	TRISB |= 0b00110000;
	
	PIE1  |= 0b00001000;
	PIE2  |= 0b00001000;
	
	SSP1CON1 = 0b00101000;
	//SSPADD= 0x63;           // Assuming 40MHz speed - 100KHz
	//SSPADD= 0x27;         // Assuming 16MHz speed - 100KHz
    SSPADD  = 19;       // Assuming 8MHz speed - 100KHz Clock = FOSC/(4 * (SSPxADD + 1))
    
	SSPSTAT = 0b10000000;   // 100Khz
	PIR2bits.BCLIF = 0;
	
	// Initialize Task/module variables
	I2C_list_count = 0;
	I2C_list_head  = NULL;

	//	Initiate a stop condition to clear bus
	I2C_bus_state   = BUS_STOP;
	SSPCON2bits.PEN = 1;

	while (1)	
	{	// Wait for another task to post 1 or more messages to send
		WaitEvent(I2C_NEW_MSG);
		ClearEvent(0xFF); //Clear all Event
		while (I2C_list_count > 0)
		{
			I2C_current_message = I2C_deqMsg();
			I2C_byte_count = I2C_current_message->num_bytes;
			p_data = I2C_current_message->ram_data;
			// Specific settings for SMBus
			if (I2C_current_message->flags.SMBus == 1)
			{
				// Enable SMBus specific inputs
				SSPSTATbits.CKE = 1; 
			}
			else
			{
				// Disble SMBus specific inputs
				SSPSTATbits.CKE = 0;
			}
			// Set the timeout at 30ms//10ms
			CancelAlarm(I2C_TIMEOUT_ALARM);
			SetRelAlarm(I2C_TIMEOUT_ALARM, 30, 0);
			// Start I2C transfert
			I2C_bus_state = BUS_START;
			SSPCON2bits.SEN = 1;
			// Wait for transfert result
			WaitEvent(TIMEOUT_EVENT | BUSERROR_EVENT | IDLE_EVENT);
			GetEvent(I2C_DRV_ID, &Mask_event);
			// Timeout occured (10 ms)
			if (Mask_event & TIMEOUT_EVENT)
    		{
				ClearEvent(TIMEOUT_EVENT);
				I2C_current_message->error = ERR_I2C_TIMEOUT;
				I2C_current_message->flags.busy = 0;
				I2C_current_message->flags.error = 1;
				SetEvent(I2C_current_message->CallerID,I2C_QUEUE_EMPTY);
			}
			// BUS ERROR occured
			if (Mask_event & BUSERROR_EVENT)
    		{
				ClearEvent(BUSERROR_EVENT);
				// If the automatic retry is used
				if (I2C_current_message->retry_counter != 0)
				{
					I2C_current_message->retry_counter--;
					I2C_current_message->flags.busy = 1;
					I2C_enqMsg(I2C_current_message);
				}
				else 
				{
					I2C_current_message->flags.busy = 0;
					I2C_current_message->flags.error = 1;
					SetEvent(I2C_current_message->CallerID,I2C_QUEUE_EMPTY);
				}
			}
			if (Mask_event & IDLE_EVENT)
   			{
				// Message sent successfully
				ClearEvent(IDLE_EVENT);
				// Disabled TIMEOUT counter
				CancelAlarm(I2C_TIMEOUT_ALARM);
				SetEvent(I2C_current_message->CallerID,I2C_QUEUE_EMPTY);
			}
		}// End of list_count_loop
		Nop();				
	} // End of the infinte loop
}// End of task
 

/**********************************************************************
 *
 *	Enqueue a client packet object into the I2C task queue.
 *
 *	Once placed in queue, client must not modify the data
 *	otherwise unpredictable results. To safely change the object,
 *	dequeue, modify, re-enqueue.
 *
 *	Returns 1 if successfull, 0 if message could not be enqueued
 **********************************************************************/
unsigned char I2C_enqMsg(I2C_message_tRef toEnqueue)
{
  I2C_message_tRef I2C_list_itor;

  if (toEnqueue != NULL)
  {
    SuspendOSInterrupts();
    if (I2C_list_head == NULL)
      I2C_list_head = toEnqueue;
    else
    {
      I2C_list_itor = I2C_list_head;
      while (I2C_list_itor->next != NULL)
        I2C_list_itor = I2C_list_itor->next;
      I2C_list_itor->next = toEnqueue;
    }
    toEnqueue->next     = NULL;
    toEnqueue->CallerID = id_tsk_run;
    I2C_list_count++;
    ResumeOSInterrupts();
    return 1;
  }
  else
    return 0;
}

/**********************************************************************
 *
 *	Dequeue a client message from the I2c task queue.
 *
 *
 *********************************************************************/
I2C_message_tRef I2C_deqMsg(void)
{
  I2C_message_tRef I2C_list_itor;

  SuspendOSInterrupts();
  I2C_list_itor = NULL;
  if (I2C_list_head != NULL)
  {
    I2C_list_itor = I2C_list_head;
    I2C_list_head = I2C_list_head->next;
    I2C_list_count--;
  }
  ResumeOSInterrupts();
  SetEvent(I2C_DRV_ID, I2C_NEW_MSG);
  return I2C_list_itor;
} 


/**********************************************************************
 *
 *			I2C Interrupt Service Routine
 * 
 *			Operates as state machine with case statements
 *			while interrupt handler is large as whole, only
 *			small ammount of code should be executed at any
 *			one time
 *
 *			The I2C_bus_state when entering the switch statement relates
 *			to what event was called for prior to the interrupt occuring.
 *			example:set I2C_bus_state to "stop", enable stop bit and wait
 *			for interrupt to return here in (hopefully) stopped condition
 *			after completion of the event.
 *		
 **********************************************************************/
void I2C_INT (void)
{
  PIR1bits.SSPIF = 0;
 // PIR2bits.BCLIF = 0;// ajout personnel
//  if(STKPTR>PtrMax)
//	while(1);
  switch (I2C_bus_state)
	{
	    // Received interrupt after calling start event
		case (BUS_START): 
		{
			// Got control of bus ?
			if (SSPSTATbits.S) 
			{	// Launch into sending next byte
				SSPSTATbits.S = 0;
				SSPBUF = I2C_current_message->control & 0xFE;
				I2C_bus_state = BUS_CTRL_WRITE;
			}
					
			if (PIR2bits.BCLIF)		//	Failed on bus collision
			{	// FIXME:  Find a better way to keep looping while waiting for
				// the bus to become idle.  Calling for another start will occur
				// according to period TbaudRateGenerator...could have lots of frequent
				// interrupts until bus is clear.
				
				SSPCON2bits.SEN = 1;
				SetEvent(I2C_DRV_ID,BUSERROR_EVENT);
			}					
			break;
		}
			
				
		// Received interrupt after control write
		case (BUS_CTRL_WRITE):
				{
					// Slave is answered
					if (!SSPCON2bits.ACKSTAT)
					{
						if (I2C_current_message->flags.long_addr == 1)
						{
							SSPBUF = I2C_current_message->addr_high;
							I2C_bus_state = BUS_WRITE_ADDR_H;
						}
						else
						{
							SSPBUF = I2C_current_message->addr_low;
							I2C_bus_state = BUS_WRITE_ADDR_L;
						}
					}
					// Slave not responding
					else if (SSPCON2bits.ACKSTAT)
					{
						I2C_current_message->error = ERR_I2C_NOSLAVE;
						I2C_bus_state = BUS_IDLE;
						SetEvent(I2C_DRV_ID,BUSERROR_EVENT);
					}
				break;
				}

				
		// Received interrupt after send high addr
		case (BUS_WRITE_ADDR_H):
				{
					// Slave ACK'd
					if (!SSPCON2bits.ACKSTAT)
					{
						SSPBUF = I2C_current_message->addr_low;
						I2C_bus_state = BUS_WRITE_ADDR_L;
					}
					else
					{
						I2C_bus_state = BUS_IDLE;
						I2C_current_message->error = ERR_I2C_NACK_ADDR;
						SetEvent(I2C_DRV_ID,BUSERROR_EVENT);
					}					
					break;
				}


		// Received interrupt after send low addr
		case (BUS_WRITE_ADDR_L):
				{
					// Slave responded
					if (!SSPCON2bits.ACKSTAT)
					{
						if (I2C_current_message->control & 0x01)
						{	//	This is a read, initiate a restart condition
							I2C_bus_state = BUS_RESTART;
							SSPCON2bits.RSEN = 1;
						}					
						else
						{	//	Next operation is a data write, output first byte
							I2C_bus_state = BUS_WRITE_DATA;
							SSPBUF = *p_data;
							p_data++;
							I2C_byte_count--;
						}
					}
					// Slave did not respond
					else
					{
							I2C_bus_state = BUS_IDLE;
							I2C_current_message->error = ERR_I2C_NACK_ADDR;
							SetEvent(I2C_DRV_ID,BUSERROR_EVENT);
					}
					break;
				}



			
		// Received interrupt after calling for restart
		case (BUS_RESTART):
				{
					SSPBUF = I2C_current_message->control;						
					I2C_bus_state = BUS_CTRL_READ;
					break;
				}


					
		// Received interrupt after calling for control read
		case (BUS_CTRL_READ):
				{
					// Slave responded
					if (!SSPCON2bits.ACKSTAT)
					{
						SSPCON2bits.RCEN = 1;
						I2C_bus_state = BUS_READ_DATA;
					}
					else
					{
						I2C_bus_state = BUS_IDLE;
						SetEvent(I2C_DRV_ID,BUSERROR_EVENT);
					}
					break;
				}


		// Received interrupt after setting RCEN
		case (BUS_READ_DATA):
				{
					*p_data = SSPBUF;
					p_data++;
					I2C_byte_count--;
					
					if (I2C_byte_count == 0)
						SSPCON2bits.ACKDT = 1;
					else
						SSPCON2bits.ACKDT = 0;
						
					I2C_bus_state = BUS_SEND_ACK_NACK;       
		  	 		SSPCON2bits.ACKEN = 1;
					break;
				}
		
		
		// Received interrupt after setting/clearing ACKDT 
		case (BUS_SEND_ACK_NACK):
				{
					if (SSPCON2bits.ACKDT)
					{	// Last was nack, so we're done..
						SSPCON2bits.ACKDT = 0;
						I2C_bus_state = BUS_STOP;
						SSPCON2bits.PEN = 1;
					}
					else
					{	//	Last was ack, so there must be more to transfer 
						SSPCON2bits.RCEN = 1;
						I2C_bus_state = BUS_READ_DATA;
					}
					break;
				}

		
		// Received interrupt after calling for stop
		case (BUS_STOP):
				{
					PIR2bits.BCLIF = 0;
					I2C_bus_state = BUS_IDLE;
					SetEvent(I2C_DRV_ID,IDLE_EVENT);
					break;
				}				
						
		case (BUS_WRITE_DATA):
				{
					//	Check ACKSTAT to determine if slave answered
					if (SSPCON2bits.ACKSTAT)
					{
						I2C_current_message->error = ERR_I2C_NACK_DATA;
						I2C_bus_state = BUS_STOP;
						break;
					}
			
					//	Write a byte of data to the slave
					//	Decide which state is next
					if (I2C_byte_count > 0)
					{
						SSPBUF = *p_data;
						p_data++;
						I2C_byte_count--;
						I2C_bus_state = BUS_WRITE_DATA;
					}
					else
					{
						SSPCON2bits.PEN = 1;
						I2C_bus_state = BUS_STOP;
						I2C_current_message->error = ERR_I2C_SUCCESS;
					}
					break;
				}
						
			default:
					break;
	}	//	End switch
  return;
}


/* End of File : drv_i2c.c  */
