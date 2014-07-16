/*******************************************************************************
* File Name: CSD_AMuxCH0.h
* Version 3.40
*
*  Description:
*    This file contains the constants and function prototypes for the Analog
*    Multiplexer User Module CapSense_CSD_AMux.
*
*   Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CAPSENSE_CSD_AMUX_CSD_AMuxCH0_H)
#define CY_CAPSENSE_CSD_AMUX_CSD_AMuxCH0_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cyfitter_cfg.h"


/***************************************
*        Function Prototypes
***************************************/

void CSD_AMuxCH0_Start(void);
void CSD_AMuxCH0_Init(void);
void CSD_AMuxCH0_Stop(void);
void CSD_AMuxCH0_Select(uint8 channel) CYREENTRANT;
void CSD_AMuxCH0_FastSelect(uint8 channel) CYREENTRANT;
void CSD_AMuxCH0_DisconnectAll(void) CYREENTRANT;
/* The Connect and Disconnect functions are declared elsewhere */
/* void CSD_AMuxCH0_Connect(uint8 channel); */
/* void CSD_AMuxCH0_Disconnect(uint8 channel); */


/***************************************
*     Initial Parameter Constants
***************************************/

#define CSD_AMuxCH0_CHANNELS  (1u + 2u +1u+ 0u)
#define CSD_AMuxCH0_MUXTYPE   (1u)


/***************************************
*             API Constants
***************************************/

#define CSD_AMuxCH0_NULL_CHANNEL   (0xFFu)
#define CSD_AMuxCH0_MUX_SINGLE     (1u)
#define CSD_AMuxCH0_MUX_DIFF       (2u)


/***************************************
*        Conditional Functions
***************************************/

#if (CSD_AMuxCH0_MUXTYPE == CSD_AMuxCH0_MUX_SINGLE)
#define CSD_AMuxCH0_Connect(channel) CSD_AMuxCH0_Set(channel)
#define CSD_AMuxCH0_Disconnect(channel) CSD_AMuxCH0_Unset(channel)
#else
    void CSD_AMuxCH0_Connect(uint8 channel) CYREENTRANT;
    void CSD_AMuxCH0_Disconnect(uint8 channel) CYREENTRANT;
#endif  /* End (CSD_AMuxCH0_MUXTYPE == CSD_AMuxCH0_MUX_SINGLE) */

#endif /* CY_CAPSENSE_CSD_AMUX_CSD_AMuxCH0_H */


/* [] END OF FILE */
