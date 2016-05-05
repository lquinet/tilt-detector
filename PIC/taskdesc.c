/**********************************************************************/
/*                                                                    */
/* File name: taskdesc.c                                              */
/*                                                                    */
/* Since:     2004-Sept-20                                            */
/*                                                                    */
/* Version:   PICos18 v3.00                                           */
/*            Copyright (C) 2003, 2004, 2005 Pragmatec.               */
/*                                                                    */
/* Author:    Designed by Pragmatec S.A.R.L.        www.pragmatec.net */
/*            MONTAGNE Xavier [XM]      xavier.montagne@pragmatec.net */
/*            KUSHNIR  Sergey [KS]                     icd2usb@tut.by */
/*                                                                    */
/* Purpose:   Kind of OIL file where all the features of the tasks    */
/*            are described.                                          */
/*                                                                    */
/*           rom_desc_tsk foo = {                                     */
/*             0x..,    Priority level [0:15],15 the most significant */
/*             0x....,  Stack save area address                       */
/*             0x....,  Start adress of the task                      */
/*             0x..,    Task state at start [RUN/WAIT/READY/SUSPEND]  */
/*             0x..,    Identification number of the task             */
/*             0x....   Stack size                                    */
/*             };                                                     */
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
/* 2004/09/20 [XM]  Create this file                                  */
/* 2005/10/29 [XM]  Added cast to avoid some C18 v3.00 WARNINGS.      */
/* 2007/10/30 [KS]  Removed the #pragma directive to regroup the stack*/
/*                  in the same area. No more necessary with the new  */
/*                  v3.00 architecture and the C18 multi-bank option. */
/*                                                                    */
/**********************************************************************/

#include "define.h"

/**********************************************************************
 * --------------------- COUNTER & ALARM DEFINITION -------------------
 **********************************************************************/
Counter Counter_list[] = 
  {
   /*******************************************************************
    * -------------------------- First counter ------------------------
    *******************************************************************/
   {
     {
       200,                                /* maxAllowedValue        */
        10,                                /* ticksPerBase           */
       100                                 /* minCycle               */
     },
     0,                                    /* CounterValue           */
     0                                     /* Nbr of Tick for 1 CPT  */
   }
  };

Counter Counter_kernel = 
  {
    {
      65535,                              /* maxAllowedValue        */
          1,                              /* ticksPerBase           */
          0                               /* minCycle               */
    },
    0,                                    /* CounterValue           */
    0                                     /* Nbr of Tick for 1 CPT  */
  };

AlarmObject Alarm_list[] = 
 { 
    /*******************************************************************
    * ----------------- ALARM_TASK_Main_Delay -------------------------
    *******************************************************************/
   {
     OFF,                                  	/* State                   */
     0,                                    	/* AlarmValue              */
     0,                                    	/* Cycle                   */
     &Counter_kernel,                      	/* ptrCounter              */
     TASK_Main_ID,                          /* TaskID2Activate         */
     DELAY_EVENT,                        	/* EventToPost             */
     0                                     	/* CallBack                */
   },
   
   /*******************************************************************
    * ---------------------- Alarm I2C task ---------------------------
    *******************************************************************/
   {
     OFF,                                  /* State                   */
     0,                                    /* AlarmValue              */
     0,                                    /* Cycle                   */
     &Counter_kernel,                      /* ptrCounter              */
     I2C_DRV_ID,                           /* TaskID2Activate         */
     TIMEOUT_EVENT,                        /* EventToPost             */
     0                                     /* CallBack                */
   },
      
 };

#define _ALARMNUMBER_          sizeof(Alarm_list)/sizeof(AlarmObject)
#define _COUNTERNUMBER_        sizeof(Counter_list)/sizeof(Counter)
unsigned char ALARMNUMBER    = _ALARMNUMBER_;
unsigned char COUNTERNUMBER  = _COUNTERNUMBER_;
unsigned long global_counter = 0;

/**********************************************************************
 * --------------------- COUNTER & ALARM DEFINITION -------------------
 **********************************************************************/
Resource Resource_list[] = 
  {
   {
      10,                                /* priority           */
       0,                                /* Task prio          */
       0,                                /* lock               */
   }
  };
  
#define _RESOURCENUMBER_       sizeof(Resource_list)/sizeof(Resource)
unsigned char RESOURCENUMBER = _RESOURCENUMBER_;

/**********************************************************************
 * ----------------------- TASK & STACK DEFINITION --------------------
 **********************************************************************/
#define DEFAULT_STACK_SIZE      256
DeclareTask(TASK_Main);
DeclareTask(I2C_Drv);
DeclareTask(RS_Drv);

volatile unsigned char stackMain[DEFAULT_STACK_SIZE];
volatile unsigned char stack_I2C[DEFAULT_STACK_SIZE];
volatile unsigned char stack_rs[DEFAULT_STACK_SIZE];

/**********************************************************************
 * ---------------------- TASK DESCRIPTOR SECTION ---------------------
 **********************************************************************/
#pragma		romdata		DESC_ROM
const rom unsigned int descromarea;
/**********************************************************************
 * -------------------------- task Main ------------------------------
 **********************************************************************/
rom_desc_tsk rom_desc_task_Main_ID = {
	TASK_Main_PRIO,				/* prioinit from 0 to 15       */
	stackMain,					/* stack address (16 bits)     */
	TASK_Main,					/* start address (24 bits)     */
	READY	,					/* state at init phase         */
	TASK_Main_ID,				/* id_tsk from 0 to 15         */
	sizeof(stackMain)			/* stack size    (16 bits)     */
};

/**********************************************************************
 * ---------------------------  I2C driver ----------------------------
 **********************************************************************/
rom_desc_tsk rom_desc_I2C = {
    I2C_DRV_PRIO,						/* prioinit from 0 to 15       */
    stack_I2C,                      	/* stack address (16 bits)     */
    I2C_Drv,                        	/* start address (16 bits)     */
    READY,								/* state at init phase         */
    I2C_DRV_ID,							/* id_tsk from 0 to 15         */
    sizeof(stack_I2C)					/* ctx address   (16 bits)     */
};
/**********************************************************************
 * ---------------------------  RS task -------------------------------
 **********************************************************************/
rom_desc_tsk rom_desc_task_IO_ID = {
	RDV_RS_PRIO,                       	/* prioinit from 0 to 15       */
	stack_rs,							/* stack address (16 bits)     */
	RS_Drv,								/* start address (24 bits)     */
	READY,								/* state at init phase         */
	RS_DRV_ID,                         	/* id_tsk from 0 to 15         */
	sizeof(stack_rs)               		/* stack size    (16 bits)     */
};
/**********************************************************************
 * --------------------- END TASK DESCRIPTOR SECTION ------------------
 **********************************************************************/
rom_desc_tsk end = {
	0x00,                              /* prioinit from 0 to 15       */
	0x0000,                            /* stack address (16 bits)     */
	0x0000,                            /* start address (24 bits)     */
	0x00,                              /* state at init phase         */
	0x00,                              /* id_tsk from 0 to 15         */
	0x0000                             /* stack size    (16 bits)     */
};

volatile far rom unsigned int * taskdesc_addr = (&(descromarea)+1);
	
/* End of File : taskdesc.c */
