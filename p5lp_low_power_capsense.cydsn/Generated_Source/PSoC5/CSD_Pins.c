/*******************************************************************************
* File Name: CSD_Pins.c
* Version 3.40
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "CSD_Pins.h"


/*******************************************************************************
* Function Name: CSD_SetAllSensorsDriveMode
********************************************************************************
*
* Summary:
*  Sets the drive mode for the all pins used by capacitive sensors within 
*  CapSense component.
* 
* Parameters:  
*  mode: Desired drive mode.
*
* Return: 
*  None
*
*******************************************************************************/
void CSD_SetAllSensorsDriveMode(uint8 mode) 
{
    /* Set pins drive mode */
    CyPins_SetPinDriveMode(CSD_PortCH0__Button0__BTN, mode);
    CyPins_SetPinDriveMode(CSD_PortCH1__Button1__BTN, mode);

}



/*******************************************************************************
* Function Name: CSD_SetAllCmodsDriveMode
********************************************************************************
*
* Summary:
*  Sets the drive mode for the all pins used by Cmod capacitors within CapSense 
*  component.
* 
* Parameters:  
*  mode: Desired drive mode.
*
* Return: 
*  None
*
*******************************************************************************/
void CSD_SetAllCmodsDriveMode(uint8 mode) 
{
   /* Set pins drive mode */
    CyPins_SetPinDriveMode(CSD_CmodCH0_Cmod_CH0, mode);
    CyPins_SetPinDriveMode(CSD_CmodCH1_Cmod_CH1, mode);

}


#if (CSD_CURRENT_SOURCE == CSD_EXTERNAL_RB)
    /*******************************************************************************
    * Function Name: CSD_SetAllRbsDriveMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the drive mode for the all pins used by bleed resistors (Rb) within 
    *  CapSense component. Only available when Current Source is external resistor.
    * 
    * Parameters:  
    *  mode: Desired drive mode.
    *
    * Return: 
    *  None
    *
    *******************************************************************************/
    void CSD_SetAllRbsDriveMode(uint8 mode) 
    {
        /* Set pins drive mode */
      
    }
#endif  /* (CSD_CURRENT_SOURCE == CSD_EXTERNAL_RB) */


/* [] END OF FILE */
