/*******************************************************************************
* File Name: CSD_Pins.h
* Version 3.40
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CAPSENSE_CSD_Pins_CSD_H)
#define CY_CAPSENSE_CSD_Pins_CSD_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "CSD.h"


/***************************************
*        Function Prototypes
***************************************/

void CSD_SetAllSensorsDriveMode(uint8 mode) ;
void CSD_SetAllCmodsDriveMode(uint8 mode) ;
#if (CSD_CURRENT_SOURCE == CSD_EXTERNAL_RB)
    void CSD_SetAllRbsDriveMode(uint8 mode) ;    
#endif  /* (CSD_CURRENT_SOURCE == CSD_EXTERNAL_RB) */   


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define CSD_DM_ALG_HIZ         (PIN_DM_ALG_HIZ)
#define CSD_DM_DIG_HIZ         (PIN_DM_DIG_HIZ)
#define CSD_DM_RES_UP          (PIN_DM_RES_UP)
#define CSD_DM_RES_DWN         (PIN_DM_RES_DWN)
#define CSD_DM_OD_LO           (PIN_DM_OD_LO)
#define CSD_DM_OD_HI           (PIN_DM_OD_HI)
#define CSD_DM_STRONG          (PIN_DM_STRONG)
#define CSD_DM_RES_UPDWN       (PIN_DM_RES_UPDWN)

/* PC registers defines for sensors */
#define CSD_PortCH0__Button0__BTN  CSD_PortCH0__Button0__BTN__PC
#define CSD_PortCH1__Button1__BTN  CSD_PortCH1__Button1__BTN__PC
/* For Cmods*/
#define CSD_CmodCH0_Cmod_CH0       CSD_CmodCH0__Cmod_CH0__PC
#define CSD_CmodCH1_Cmod_CH1       CSD_CmodCH1__Cmod_CH1__PC


#endif /* (CY_CAPSENSE_CSD_Pins_CSD_H) */


/* [] END OF FILE */
