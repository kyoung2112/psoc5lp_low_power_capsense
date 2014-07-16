/*******************************************************************************
* File Name: main.c
*
* Version: 1.00
*
* Description:
*  The project explains the usage of CapSense CSD component. The 2 buttons and
*  linear sliders are used as sensing elements. LED displays buttons active 
*  state and slider position is shown on LCD.
*
********************************************************************************
* Copyright 2012, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include <project.h>

#define PERMANENT_SLEEP 0u


/*******************************************************************************
* Function Name: WakeupIsr
********************************************************************************
* Summary:
*  The wakeup interrupt service routine.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
CY_ISR(WakeupIsr)
{
    /***************************************************************************
    * This function must always be called (when the Sleep Timer's interrupt
    * is disabled or enabled) after wake up to clear the ctw_int status bit.
    * It is required to call SleepTimer_GetStatus() within 1 ms (1 clock cycle
    * of the ILO) after CTW event occurred.
    ***************************************************************************/
    SleepTimer_GetStatus();
}



/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*  Main function performs following functions:
*   1. Enable global interrupts.
*   2. Initialize CapSense CSD and Start the sensor scanning loop.
*   3. Process scanning results and display it on LCD/LED.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
int main()
{
	
	
    /* Enable global interrupts */
    CyGlobalIntEnable;
	
	/* Start components */
    isr_StartEx(WakeupIsr);
    
    /* Start SleepTimer's operation with the customizer's settings */
    SleepTimer_Start();

    /* Optional section used for demonstration purposes */
//    SleepTimer_Stop();              /* Stop Sleep Timer's operation         */
//    SleepTimer_EnableInt();         /* Enable interrupts. Not required.     */
//    SleepTimer_SetInterval(SleepTimer__CTW_4_MS);    /* Set new interval    */
//    SleepTimer_Start();             /* Start with the new settings          */

	/* Start CapSense */
    CapSense_Start();
	
    /* Initialize baselines */ 
    CapSense_InitializeAllBaselines();
    
    while(1u)
    {
        /* Update all baselines */
        CapSense_UpdateEnabledBaselines();
        
   		/* Start scanning all enabled sensors */
    	CapSense_ScanEnabledWidgets();
    
        /* Wait for scanning to complete */
		while(CapSense_IsBusy() != 0)
		{
			 /* go into alt active mode(stall CPU), wake it back up on interrupt */
	        CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_INTERRUPT);
		}

		 /* Display BUTTON0 state */
	if (CapSense_CheckIsAnyWidgetActive()) 
	{
		LED_Write(1u);
	}
	else
	{
		LED_Write(0u);
	}
	
	CapSense_Sleep();
	
	#if !(PERMANENT_SLEEP)
	/*******************************************************************
    * Switch to the Sleep Mode for PSoC5 devices:
    *  - PM_SLEEP_TIME_NONE: wakeup time is defined by Sleep Timer
    *  - PM_SLEEP_SRC_NONE : all wakeup sources is allowed
    *******************************************************************/
	Debug_Out_Write(0u);
    CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_CTW);
	#else
	CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_NONE); /* Sleep permanently for current testing */
	while(1);	//If it hits here (high current), then sleep was not successful
	#endif
	
	/***************************************************************************
    * This function must always be called (when the Sleep Timer's interrupt
    * is disabled or enabled) after wake up to clear the ctw_int status bit.
    * It is required to call SleepTimer_GetStatus() within 1 ms (1 clock cycle
    * of the ILO) after CTW event occurred.
    ***************************************************************************/
    //SleepTimer_GetStatus();
	Debug_Out_Write(1u);
	CapSense_Wakeup();
	}
   
}

/* [] END OF FILE */
