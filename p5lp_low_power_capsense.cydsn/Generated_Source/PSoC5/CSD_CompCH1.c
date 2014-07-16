/*******************************************************************************
* File Name: CSD_CompCH1.c
* Version 2.0
*
* Description:
*  This file provides the source code to the API for the Comparator component
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

#include "CSD_CompCH1.h"

uint8 CSD_CompCH1_initVar = 0u;

/* Internal functions definitoin */
static void CSD_CompCH1_trimAdjust(uint8 nibble) ;

/* static CSD_CompCH1_backupStruct  CSD_CompCH1_backup; */
#if (CY_PSOC5A)
    static CSD_CompCH1_LOWPOWER_BACKUP_STRUCT  CSD_CompCH1_lowPowerBackup;
#endif /* CY_PSOC5A */

/* variable to decide whether or not to restore the control register in 
   Enable() API for PSoC5A only */
#if (CY_PSOC5A)
    static uint8 CSD_CompCH1_restoreReg = 0u;
#endif /* CY_PSOC5A */


/*******************************************************************************
* Function Name: CSD_CompCH1_Init
********************************************************************************
*
* Summary:
*  Initialize to the schematic state
* 
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void CSD_CompCH1_Init(void) 
{
    /* Set default speed/power */
    CSD_CompCH1_SetSpeed(CSD_CompCH1_DEFAULT_SPEED);

    /* Set default Hysteresis */
    #if ( CSD_CompCH1_DEFAULT_HYSTERESIS == 0u )
        CSD_CompCH1_CR |= CSD_CompCH1_HYST_OFF;
    #else
        CSD_CompCH1_CR &= (uint8)(~CSD_CompCH1_HYST_OFF);
    #endif /* CSD_CompCH1_DEFAULT_HYSTERESIS == 0u */
    /* Power down override feature is not supported for PSoC5A. */
    #if (CY_PSOC3 || CY_PSOC5LP)
        /* Set default Power Down Override */
        #if ( CSD_CompCH1_DEFAULT_PWRDWN_OVRD == 0u )
            CSD_CompCH1_CR &= (uint8)(~CSD_CompCH1_PWRDWN_OVRD);
        #else 
            CSD_CompCH1_CR |= CSD_CompCH1_PWRDWN_OVRD;
        #endif /* CSD_CompCH1_DEFAULT_PWRDWN_OVRD == 0u */
    #endif /* CY_PSOC3 || CY_PSOC5LP */
    
    /* Set mux always on logic */
    CSD_CompCH1_CR |= CSD_CompCH1_MX_AO;

    /* Set default sync */
    CSD_CompCH1_CLK &= (uint8)(~CSD_CompCH1_SYNCCLK_MASK);
    #if ( CSD_CompCH1_DEFAULT_BYPASS_SYNC == 0u )
        CSD_CompCH1_CLK |= CSD_CompCH1_SYNC_CLK_EN;
    #else
        CSD_CompCH1_CLK |= CSD_CompCH1_BYPASS_SYNC;
    #endif /* CSD_CompCH1_DEFAULT_BYPASS_SYNC == 0u */
}


/*******************************************************************************
* Function Name: CSD_CompCH1_Enable
********************************************************************************
*
* Summary:
*  Enable the Comparator
* 
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void CSD_CompCH1_Enable(void) 
{
    CSD_CompCH1_PWRMGR |= CSD_CompCH1_ACT_PWR_EN;
    CSD_CompCH1_STBY_PWRMGR |= CSD_CompCH1_STBY_PWR_EN;
     
     /* This is to restore the value of register CR which is saved 
    in prior to the modification in stop() API */
    #if (CY_PSOC5A)
        if(CSD_CompCH1_restoreReg == 1u)
        {
            CSD_CompCH1_CR = CSD_CompCH1_lowPowerBackup.compCRReg;

            /* Clear the flag */
            CSD_CompCH1_restoreReg = 0u;
        }
    #endif /* CY_PSOC5A */
}


/*******************************************************************************
* Function Name: CSD_CompCH1_Start
********************************************************************************
*
* Summary:
*  The start function initializes the Analog Comparator with the default values.
*
* Parameters:
*  void
*
* Return:
*  void 
*
* Global variables:
*  CSD_CompCH1_initVar: Is modified when this function is called for the 
*   first time. Is used to ensure that initialization happens only once.
*  
*******************************************************************************/
void CSD_CompCH1_Start(void) 
{

    if ( CSD_CompCH1_initVar == 0u )
    {
        CSD_CompCH1_Init();
        
        CSD_CompCH1_initVar = 1u;
    }   

    /* Enable power to comparator */
    CSD_CompCH1_Enable();    
}


/*******************************************************************************
* Function Name: CSD_CompCH1_Stop
********************************************************************************
*
* Summary:
*  Powers down amplifier to lowest power state.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void CSD_CompCH1_Stop(void) 
{
    /* Disable power to comparator */
    CSD_CompCH1_PWRMGR &= (uint8)(~CSD_CompCH1_ACT_PWR_EN);
    CSD_CompCH1_STBY_PWRMGR &= (uint8)(~CSD_CompCH1_STBY_PWR_EN);
    
    #if (CY_PSOC5A)
        /* Enable the variable */
        CSD_CompCH1_restoreReg = 1u;

        /* Save the control register before clearing it */
        CSD_CompCH1_lowPowerBackup.compCRReg = CSD_CompCH1_CR;
        CSD_CompCH1_CR = CSD_CompCH1_COMP_REG_CLR;
    #endif /* CY_PSOC5A */
}


/*******************************************************************************
* Function Name: CSD_CompCH1_SetSpeed
********************************************************************************
*
* Summary:
*  This function sets the speed of the Analog Comparator. The faster the speed
*  the more power that is used.
*
* Parameters:
*  speed: (uint8) Sets operation mode of Comparator
*
* Return:
*  void
*
*******************************************************************************/
void CSD_CompCH1_SetSpeed(uint8 speed) 
{
    /* Clear and Set power level */    
    CSD_CompCH1_CR = (CSD_CompCH1_CR & (uint8)(~CSD_CompCH1_PWR_MODE_MASK)) |
                           (speed & CSD_CompCH1_PWR_MODE_MASK);

    /* Set trim value for high speed comparator */
    if(speed == CSD_CompCH1_HIGHSPEED)
    {
        /* PSoC5A */
        #if (CY_PSOC5A)
            CSD_CompCH1_TR = CSD_CompCH1_HS_TRIM_TR0;
        #endif /* CY_PSOC5A */
        
        /* PSoC3, PSoC5LP or later */
        #if (CY_PSOC3 || CY_PSOC5LP) 
            CSD_CompCH1_TR0 = CSD_CompCH1_HS_TRIM_TR0;
            CSD_CompCH1_TR1 = CSD_CompCH1_HS_TRIM_TR1;
        #endif /* CY_PSOC3 || CY_PSOC5LP */
    }
    else
    {
    /* PSoC5A */
        #if (CY_PSOC5A)
            CSD_CompCH1_TR = CSD_CompCH1_LS_TRIM_TR0;
        #endif /* CY_PSOC5A */
        
        /* PSoC3, PSoC5LP or later */
        #if (CY_PSOC3 || CY_PSOC5LP) 
            CSD_CompCH1_TR0 = CSD_CompCH1_LS_TRIM_TR0;
            CSD_CompCH1_TR1 = CSD_CompCH1_LS_TRIM_TR1;
        #endif /* CY_PSOC3 || CY_PSOC5LP */
    }

}


/*******************************************************************************
* Function Name: CSD_CompCH1_GetCompare
********************************************************************************
*
* Summary:
*  This function returns the comparator output value.
*  This value is not affected by the Polarity parameter.
*  This valuea lways reflects a noninverted state.
*
* Parameters:
*   None
*
* Return:
*  (uint8)  0     - if Pos_Input less than Neg_input
*           non 0 - if Pos_Input greater than Neg_input.
*
*******************************************************************************/
uint8 CSD_CompCH1_GetCompare(void) 
{
    return( CSD_CompCH1_WRK & CSD_CompCH1_CMP_OUT_MASK);
}


/*******************************************************************************
* Function Name: CSD_CompCH1_trimAdjust
********************************************************************************
*
* Summary:
*  This function adjusts the value in the low nibble/high nibble of the Analog 
*  Comparator trim register
*
* Parameters:  
*  nibble:
*      0 -- adjusts the value in the low nibble
*      1 -- adjusts the value in the high nibble
*
* Return:
*  None
*
* Theory: 
*  Function assumes comparator block is setup for trim adjust.
*  Intended to be called from Comp_ZeroCal()
* 
* Side Effects:
*  Routine uses a course 1ms delay following each trim adjustment to allow 
*  the comparator output to respond.
*
*******************************************************************************/
static void CSD_CompCH1_trimAdjust(uint8 nibble) 
{
    uint8 trimCnt, trimCntMax;
    uint8 cmpState;   

    /* get current state of comparator output */
    cmpState = CSD_CompCH1_WRK & CSD_CompCH1_CMP_OUT_MASK;
    
    if (nibble == 0u)
    {    
        /* if comparator output is high, negative offset adjust is required */
        if ( cmpState != 0u )
        {
            /* PSoC5A */
            #if (CY_PSOC5A)
                CSD_CompCH1_TR |= CSD_CompCH1_CMP_TRIM1_DIR;
            #endif /* CY_PSOC5A */
            
            /* PSoC3, PSoC5LP or later */
            #if (CY_PSOC3 || CY_PSOC5LP)
                CSD_CompCH1_TR0 |= CSD_CompCH1_CMP_TR0_DIR;
            #endif /* CY_PSOC3 || CY_PSOC5LP */
        }
    }
    else
    {
        /* if comparator output is low, positive offset adjust is required */
        if ( cmpState == 0u )
        {
            /* PSoC5A */
            #if (CY_PSOC5A)
                CSD_CompCH1_TR |= CSD_CompCH1_CMP_TRIM2_DIR; 
            #endif /* CY_PSOC5A */
            
            /* PSoC3, PSoC5LP or later */
            #if (CY_PSOC3 || CY_PSOC5LP)
                CSD_CompCH1_TR1 |= CSD_CompCH1_CMP_TR1_DIR;
            #endif /* CY_PSOC3 || CY_PSOC5LP */
        }
    }

    /* Increment trim value until compare output changes state */
	
    /* PSoC5A */
	#if (CY_PSOC5A)
	    trimCntMax = 7u;
    #endif
	
	/* PSoC3, PSoC5LP or later */
	#if (CY_PSOC3 || CY_PSOC5LP)
    	if(nibble == 0u)
    	{
    		trimCntMax = 15u;
    	}
    	else
    	{
    		trimCntMax = 7u;
    	}
	#endif
	
    for ( trimCnt = 0u; trimCnt < trimCntMax; trimCnt++ )
	{
        if (nibble == 0u)
        {
            /* PSoC5A */
            #if (CY_PSOC5A)
                CSD_CompCH1_TR += 1u;
            #endif /* CY_PSOC5A */
            
            /* PSoC3, PSoC5LP or later */
            #if (CY_PSOC3 || CY_PSOC5LP)
                CSD_CompCH1_TR0 += 1u;
            #endif /* CY_PSOC3 || CY_PSOC5LP */
        }
        else
        {
            /* PSoC5A */
            #if (CY_PSOC5A)
                CSD_CompCH1_TR += 0x10u;
            #endif /* CY_PSOC5A */
            
            /* PSoC3, PSoC5LP or later */
            #if (CY_PSOC3 || CY_PSOC5LP)
                CSD_CompCH1_TR1 += 1u;
            #endif /* CY_PSOC3 || CY_PSOC5LP */
        }
        
        CyDelayUs(10u);
        
        /* Check for change in comparator output */
        if ((CSD_CompCH1_WRK & CSD_CompCH1_CMP_OUT_MASK) != cmpState)
        {
            break;      /* output changed state, trim phase is complete */
        }        
    }    
}


/*******************************************************************************
* Function Name: CSD_CompCH1_ZeroCal
********************************************************************************
*
* Summary:
*  This function calibrates the offset of the Analog Comparator.
*
* Parameters:
*  None
*
* Return:
*  (uint16)  value written in trim register when calibration complete.
*
* Theory: 
*  This function is used to optimize the calibration for user specific voltage
*  range.  The comparator trim is adjusted to counter transistor offsets
*   - offset is defined as positive if the output transitions to high before inP
*     is greater than inN
*   - offset is defined as negative if the output transitions to high after inP
*     is greater than inP
*
*  PSoC5A
*  The Analog Comparator provides 1 byte for offset trim.  The byte contains two
*  4 bit trim fields - one is a course trim and the other allows smaller
*  offset adjustments only for slow modes.
*  - low nibble - fine trim
*  - high nibble - course trim
*  PSoC3, PSoC5LP or later
*  The Analog Comparator provides 2 bytes for offset trim.  The bytes contain two
*  5 bit trim fields - one is a course trim and the other allows smaller
*  offset adjustments only for slow modes.
*  - TR0 - fine trim
*  - TR1 - course trim
*
*  Trim algorithm is a two phase process
*  The first phase performs course offset adjustment
*  The second phase serves one of two purposes depending on the outcome of the
*  first phase
*  - if the first trim value was maxed out without a comparator output 
*    transition, more offset will be added by adjusting the second trim value.
*  - if the first trim phase resulted in a comparator output transition, the
*    second trim value will serve as fine trim (in the opposite direction)to
*    ensure the offset is < 1 mV.
*
* Trim Process:   
*  1) User applies a voltage to the negative input.  Voltage should be in the
*     comparator operating range or an average of the operating voltage range.
*  2) Clear registers associated with analog routing to the positive input.
*  3) Disable Hysteresis
*  4) Set the calibration bit to short the negative and positive inputs to
*     the users calibration voltage.
*  5) Clear the TR register  ( TR = 0x00 )
*  ** LOW MODES
*  6) Check if compare output is high, if so, set the MSb of course trim field 
*     to a 1.
*  7) Increment the course trim field until the compare output changes
*  8) Check if compare output is low, if so, set the MSb of fine trim field
*     to a 1.
*  9) Increment the fine trim field until the compare output changes
*  ** FAST MODE - skip the steps 8,9
*
* Side Effects:
*  Routine clears analog routing associated with the comparator positive input.  
*  This may affect routing of signals from other components that are connected
*  to the positive input of the comparator.
*
*******************************************************************************/
uint16 CSD_CompCH1_ZeroCal(void) 
{
    uint8 tmpSW0;
    uint8 tmpSW2;
    uint8 tmpSW3;
    uint8 tmpCR;

    /* Save a copy of routing registers associated with inP */
    tmpSW0 = CSD_CompCH1_SW0;
    tmpSW2 = CSD_CompCH1_SW2;
    tmpSW3 = CSD_CompCH1_SW3;

     /* Clear routing for inP, retain routing for inN */
    CSD_CompCH1_SW0 = 0x00u;
    CSD_CompCH1_SW2 = 0x00u;
    CSD_CompCH1_SW3 = tmpSW3 & (uint8)(~CSD_CompCH1_CMP_SW3_INPCTL_MASK);

    /* Preserve original configuration
     * - turn off Hysteresis
     * - set calibration bit - shorts inN to inP
    */
    tmpCR = CSD_CompCH1_CR;
    CSD_CompCH1_CR |= (CSD_CompCH1_CAL_ON | CSD_CompCH1_HYST_OFF);
    
    /* Write default low values to trim register - no offset adjust */
    /* PSoC5A */
    #if (CY_PSOC5A)
        CSD_CompCH1_TR = CSD_CompCH1_DEFAULT_CMP_TRIM;
    #endif /* CY_PSOC5A */
    
    /* PSoC3, PSoC5LP or later */
    #if (CY_PSOC3 || CY_PSOC5LP)
        CSD_CompCH1_TR0 = CSD_CompCH1_DEFAULT_CMP_TRIM;
        CSD_CompCH1_TR1 = CSD_CompCH1_DEFAULT_CMP_TRIM;
    #endif /* CY_PSOC3 || CY_PSOC5LP */
	
	/* Two phase trim - slow modes, one phase trim - for fast */ 
    if ( (CSD_CompCH1_CR & CSD_CompCH1_PWR_MODE_MASK) == CSD_CompCH1_PWR_MODE_FAST)
    {
        CSD_CompCH1_trimAdjust(0u);
    }
    else /* default to trim for fast modes */
    {
        CSD_CompCH1_trimAdjust(1u);
		CSD_CompCH1_trimAdjust(0u);
    }
   
    /* Restore Config Register */
    CSD_CompCH1_CR = tmpCR;
    
    /* Restore routing registers for inP */
    CSD_CompCH1_SW0 = tmpSW0;
    CSD_CompCH1_SW2 = tmpSW2;
    CSD_CompCH1_SW3 = tmpSW3;
    
    /* PSoC5A */
    #if (CY_PSOC5A)
        return (uint16) CSD_CompCH1_TR;
    #endif /* CY_PSOC5A */
    
    /* PSoC3, PSoC5LP or later */
    #if (CY_PSOC3 || CY_PSOC5LP)
        return ((uint16)((uint16)CSD_CompCH1_TR1 << 8u) | (CSD_CompCH1_TR0));        
    #endif /* CY_PSOC3 || CY_PSOC5LP */
}


/*******************************************************************************
* Function Name: CSD_CompCH1_LoadTrim
********************************************************************************
*
* Summary:
*  This function stores a value in the Analog Comparator trim register.
*
* Parameters:  
*  uint8 trimVal - trim value.  This value is the same format as the value 
*  returned by the _ZeroCal routine.
*
* Return:
*  None
*
*******************************************************************************/
void CSD_CompCH1_LoadTrim(uint16 trimVal) 
{
    /* Stores value in the Analog Comparator trim register */
    /* PSoC5A */
    #if (CY_PSOC5A)
        CSD_CompCH1_TR = (uint8) trimVal;
    #endif /* CY_PSOC5A */
    
    /* PSoC3, PSoC5LP or later */
    #if (CY_PSOC3 || CY_PSOC5LP)
        /* Stores value in the Analog Comparator trim register for P-type load */
        CSD_CompCH1_TR0 = (uint8) trimVal;
        
        /* Stores value in the Analog Comparator trim register for N-type load */
        CSD_CompCH1_TR1 = (uint8) (trimVal >> 8); 
    #endif /* CY_PSOC3 || CY_PSOC5LP */
}


#if (CY_PSOC3 || CY_PSOC5LP)

    /*******************************************************************************
    * Function Name: CSD_CompCH1_PwrDwnOverrideEnable
    ********************************************************************************
    *
    * Summary:
    *  This is the power down over-ride feature. This function ignores sleep 
    *  parameter and allows the component to stay active during sleep mode.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void CSD_CompCH1_PwrDwnOverrideEnable(void) 
    {
        /* Set the pd_override bit in CMP_CR register */
        CSD_CompCH1_CR |= CSD_CompCH1_PWRDWN_OVRD;
    }


    /*******************************************************************************
    * Function Name: CSD_CompCH1_PwrDwnOverrideDisable
    ********************************************************************************
    *
    * Summary:
    *  This is the power down over-ride feature. This allows the component to stay
    *  inactive during sleep.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void CSD_CompCH1_PwrDwnOverrideDisable(void) 
    {
        /* Reset the pd_override bit in CMP_CR register */
        CSD_CompCH1_CR &= (uint8)(~CSD_CompCH1_PWRDWN_OVRD);
    }
#endif /* (CY_PSOC3 || CY_PSOC5LP) */


/* [] END OF FILE */
