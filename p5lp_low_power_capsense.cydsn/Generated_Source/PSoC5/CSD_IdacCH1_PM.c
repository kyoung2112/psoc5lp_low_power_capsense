/*******************************************************************************
* File Name: CSD_IdacCH1.c
* Version 2.0
*
* Description:
*  This file provides the power management source code to API for the
*  IDAC8.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/


#include "CSD_IdacCH1.h"

static CSD_IdacCH1_backupStruct CSD_IdacCH1_backup;


/*******************************************************************************
* Function Name: CSD_IdacCH1_SaveConfig
********************************************************************************
* Summary:
*  Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void CSD_IdacCH1_SaveConfig(void) 
{
    if (!((CSD_IdacCH1_CR1 & CSD_IdacCH1_SRC_MASK) == CSD_IdacCH1_SRC_UDB))
    {
        CSD_IdacCH1_backup.data_value = CSD_IdacCH1_Data;
    }
}


/*******************************************************************************
* Function Name: CSD_IdacCH1_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void CSD_IdacCH1_RestoreConfig(void) 
{
    if (!((CSD_IdacCH1_CR1 & CSD_IdacCH1_SRC_MASK) == CSD_IdacCH1_SRC_UDB))
    {
        if((CSD_IdacCH1_Strobe & CSD_IdacCH1_STRB_MASK) == CSD_IdacCH1_STRB_EN)
        {
            CSD_IdacCH1_Strobe &= (uint8)(~CSD_IdacCH1_STRB_MASK);
            CSD_IdacCH1_Data = CSD_IdacCH1_backup.data_value;
            CSD_IdacCH1_Strobe |= CSD_IdacCH1_STRB_EN;
        }
        else
        {
            CSD_IdacCH1_Data = CSD_IdacCH1_backup.data_value;
        }
    }
}


/*******************************************************************************
* Function Name: CSD_IdacCH1_Sleep
********************************************************************************
* Summary:
*  Stop and Save the user configuration
*
* Parameters:
*  void:
*
* Return:
*  void
*
* Global variables:
*  CSD_IdacCH1_backup.enableState: Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void CSD_IdacCH1_Sleep(void) 
{
    if(CSD_IdacCH1_ACT_PWR_EN == (CSD_IdacCH1_PWRMGR & CSD_IdacCH1_ACT_PWR_EN))
    {
        /* IDAC8 is enabled */
        CSD_IdacCH1_backup.enableState = 1u;
    }
    else
    {
        /* IDAC8 is disabled */
        CSD_IdacCH1_backup.enableState = 0u;
    }

    CSD_IdacCH1_Stop();
    CSD_IdacCH1_SaveConfig();
}


/*******************************************************************************
* Function Name: CSD_IdacCH1_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*  
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  CSD_IdacCH1_backup.enableState: Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void CSD_IdacCH1_Wakeup(void) 
{
    CSD_IdacCH1_RestoreConfig();
    
    if(CSD_IdacCH1_backup.enableState == 1u)
    {
        /* Enable IDAC8's operation */
        CSD_IdacCH1_Enable();
        
        /* Set the data register */
        CSD_IdacCH1_SetValue(CSD_IdacCH1_Data);
    } /* Do nothing if IDAC8 was disabled before */    
}


/* [] END OF FILE */
