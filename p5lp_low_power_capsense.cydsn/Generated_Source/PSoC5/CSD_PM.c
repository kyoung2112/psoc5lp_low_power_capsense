/*******************************************************************************
* File Name: CSD_PM.c
* Version 3.40
*
* Description:
*  This file provides Sleep APIs for CapSense CSD Component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "CSD.h"

CSD_BACKUP_STRUCT CSD_backup =
{   
    0x00u, /* enableState; */
    
};


/*******************************************************************************
* Function Name: CSD_SaveConfig
********************************************************************************
*
* Summary:
*  Saves customer configuration of CapSense none-retention registers. Resets 
*  all sensors to an inactive state.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global Variables:
*  CSD_backup - used to save component state before enter sleep 
*  mode and none-retention registers.
*
* Reentrant:
*  Yes.
*
*******************************************************************************/
void CSD_SaveConfig(void) 
{    

    /* Set CONTROL_REG */
    CSD_backup.ctrlReg = CSD_CONTROL_REG;

    /* Clear all sensors */
    CSD_ClearSensors();
    
    /* The pins disable is customer concern: Cmod and Rb */
}


/*******************************************************************************
* Function Name: CSD_Sleep
********************************************************************************
*
* Summary:
*  Disables Active mode power template bits for number of component used within 
*  CapSense. Calls CSD_SaveConfig() function to save customer 
*  configuration of CapSense none-retention registers and resets all sensors 
*  to an inactive state.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global Variables:
*  CSD_backup - used to save component state before enter sleep 
*  mode and none-retention registers.
*
* Reentrant:
*  No
*
*******************************************************************************/
void CSD_Sleep(void) 
{
    /* Check and save enable state */
    if(CSD_IS_CAPSENSE_ENABLE(CSD_CONTROL_REG))
    {
        CSD_backup.enableState = 1u;
        CSD_Stop();
    }
    else
    {
        CSD_backup.enableState = 0u;
    }
    
    CSD_SaveConfig();
}


/*******************************************************************************
* Function Name: CSD_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores CapSense configuration and non-retention register values.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  Must be called only after CSD_SaveConfig() routine. Otherwise 
*  the component configuration will be overwritten with its initial setting.  
*
* Global Variables:
*  CSD_backup - used to save component state before enter sleep 
*  mode and none-retention registers.
*
*******************************************************************************/
void CSD_RestoreConfig(void) 
{       
    /* Set PRS */
    #if (CSD_PRS_OPTIONS == CSD_PRS_8BITS)        
        /* Write FIFO with seed */
        CSD_SEED_COPY_REG = CSD_MEASURE_FULL_RANGE_LOW;               /* F0 register */
    
    #elif (CSD_PRS_OPTIONS == CSD_PRS_16BITS)
        /* Write FIFO with seed */
        CY_SET_REG16(CSD_SEED_COPY_PTR, CSD_MEASURE_FULL_RANGE);      /* F0 register */
                
    #elif (CSD_PRS_OPTIONS == CSD_PRS_16BITS_4X)
        
        /* Write FIFO with seed */
        CSD_SEED_COPY_A__F1_REG = CSD_MEASURE_FULL_RANGE_LOW;         /* F0 register */
        CSD_SEED_COPY_A__F0_REG =CSD_MEASURE_FULL_RANGE_LOW;          /* F1 register */
        
    #else
        /* Do nothing = config without PRS */
    #endif  /* (CSD_PRS_OPTIONS == CSD_PRS_8BITS) */
    
    /* Set the Measure */
    #if (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_FF)
        /* Window PWM  - FF Timer register are retention */
        /* Raw Counter - FF Timer register are retention */
    #else
        
        /* Window PWM */
        CSD_PWM_CH0_PERIOD_LO_REG    = CSD_MEASURE_FULL_RANGE_LOW;        /* F0 register */
        
        /* Raw Counter */
        CSD_RAW_CH0_PERIOD_HI_REG    = CSD_MEASURE_FULL_RANGE_LOW;        /* F1 register */
        CSD_RAW_CH0_PERIOD_LO_REG    = CSD_MEASURE_FULL_RANGE_LOW;        /* F0 register */
    
    #endif  /* (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_FF) */ 
    
    #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
        #if (CSD_IMPLEMENTATION_CH1 == CSD_MEASURE_IMPLEMENTATION_FF)
            /* Window PWM  - FF Timer register are retention */
            /* Raw Counter - FF Timer register are retention */
        #else
            
            /* Window PWM */
            CSD_PWM_CH1_PERIOD_LO_REG    = CSD_MEASURE_FULL_RANGE_LOW;       /* F0 register */
            
            /* Raw Counter */
            CSD_RAW_CH1_PERIOD_HI_REG    = CSD_MEASURE_FULL_RANGE_LOW;       /* F1 register */
            CSD_RAW_CH1_PERIOD_LO_REG    = CSD_MEASURE_FULL_RANGE_LOW;       /* F0 register */
            
        #endif  /* (CSD_IMPLEMENTATION_CH1 == CSD_MEASURE_IMPLEMENTATION_FF) */
    
    #endif  /* (CSD_DESIGN_TYPE == TWO_CHANNELS_DESIGN)*/

    /* Set CONTROL_REG */
    CSD_CONTROL_REG = CSD_backup.ctrlReg;

    /* Enable window generation */
    #if (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN)
        CSD_CONTROL_REG |= CSD_CTRL_WINDOW_EN__CH0;
    #elif (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) 
        CSD_CONTROL_REG |= (CSD_CTRL_WINDOW_EN__CH0 | CSD_CTRL_WINDOW_EN__CH1); 
    #endif  /* CSD_DESIGN_TYPE */
 
    /* The pins enable are customer concern: Cmod and Rb */
 }
 

/*******************************************************************************
* Function Name: CSD_Wakeup
********************************************************************************
*
* Summary:
*  Restores CapSense configuration and non-retention register values. 
*  Restores enabled state of component by setting Active mode power template 
*  bits for number of component used within CapSense.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global Variables:
*  CSD_backup - used to save component state before enter sleep 
*  mode and none-retention registers.
*
*******************************************************************************/
void CSD_Wakeup(void) 
{
    CSD_RestoreConfig();
    
    /* Restore CapSense Enable state */
    if (CSD_backup.enableState != 0u)
    {
        CSD_Enable();
    }
}


/* [] END OF FILE */
