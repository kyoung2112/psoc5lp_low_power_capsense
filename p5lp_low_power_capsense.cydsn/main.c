/*******************************************************************************
* File Name: main.c
*
*
********************************************************************************
* Copyright 2014, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include <project.h>

#define MEASURE_DEEPSLEEP_CURRENT 	0u
#define MEASURE_ACTIVE_CURRENT		0u
#define MEASURE_SCAN_CURRENT		0u

/* Calibration target for CapSense */
#define CALIBRATION_TARGET_DUTY_CYCLE 			(83u)	

/* Function prototypes */
void Calibrate_IDAC(uint8 SensorIndex, uint8 DutyCycle);
CY_ISR(WakeupIsr);
void CalibrateAllSensors(void);


/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*  Main function performs following functions:
*   1. Enable global interrupts.
*   2. Initialize CapSense CSD and Start the sensor scanning loop.
*   3. Process scanning results and display it on LED.
*	4. Handle power management.
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
	
	/* Enable wakeup interrupt */
    isr_StartEx(WakeupIsr);
    
    /* Start SleepTimer's operation with the customizer's settings */
    SleepTimer_Start();

	/* Start CapSense */
    CapSense_Start();
	
	/* Enable both sensors and leave them enabled to save time waking and sleeping.
	* Requires custom modification of CapSense component API */
	CapSense_EnableSensor(CapSense_SENSOR_BUTTON1__BTN);
	CapSense_EnableSensor(CapSense_SENSOR_BUTTON0__BTN);
	
	/* Calibrate IDAC on each sensor */
	Calibrate_IDAC(CapSense_SENSOR_BUTTON0__BTN,CALIBRATION_TARGET_DUTY_CYCLE );
	
    /* Initialize CapSense baselines */ 
    CapSense_InitializeAllBaselines();
    
    while(1u)
    {

   		/* Scan sensor 0 (with ganged sensor 1) */
    	CapSense_ScanSensor(CapSense_SENSOR_BUTTON0__BTN);
		
		#if !(MEASURE_SCAN_CURRENT)			/* If we are measuring scan current  
											* 	skip the firmware */
			
		/* Note - the capsense processing is done using the previous scan data to 
		*	save power. The only hit is the latency of one wake cycle */
			
		/* Update all baselines using previous scan data */
        CapSense_UpdateSensorBaseline(CapSense_SENSOR_BUTTON0__BTN);
		
		/* Display button state from previous scan */
		if (CapSense_CheckIsSensorActive(CapSense_SENSOR_BUTTON0__BTN)) 
		{
			LED_Write(1u);
		}
		else
		{
			LED_Write(0u);
		}
    
		#endif
		
        /* Wait for scanning to complete */
		while(CapSense_IsBusy() != 0)
		{
			#if !(MEASURE_ACTIVE_CURRENT)	/* If we are measuring active current, don't
											*	sleep CPU */
			//Debug_Out_Write(1u);
			 /* go into alt active mode(sleep CPU), wake it back up on interrupt */
	        CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_INTERRUPT);
			//Debug_Out_Write(0u);
			#endif
		}
	
		#if !(MEASURE_SCAN_CURRENT)			/* If we are measuring scan current  
											* 	immediately start a new scan and
											* 	don't sleep. */
	
		CapSense_Sleep();
	 
		#if !(MEASURE_ACTIVE_CURRENT)	/* Don't sleep if measuring active current */
		#if !(MEASURE_DEEPSLEEP_CURRENT )
		/*******************************************************************
	    * Switch to the Sleep Mode for PSoC5 devices:
	    *  - PM_SLEEP_TIME_NONE: wakeup time is defined by Sleep Timer
	    *  - PM_SLEEP_SRC_NONE : all wakeup sources is allowed
	    *******************************************************************/
		Debug_Out_Write(0u);
	    CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_CTW);
		#else
		/* Sleep permanently for sleep current measurement */
		CyGlobalIntDisable;	/* No wakey */
		CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_NONE); /* Sleep permanently for current testing */
		while(1);	//If it hits here (high current), then sleep was not successful
		#endif
		#endif
		Debug_Out_Write(1u);
		CapSense_Wakeup();
		#endif
	}
   
}

/* This function calibrates the IDAC value(s) for a specific sensor. The scan IDAC
value is written to the array of the CapSense component. The compensation IDAC value
if required is returned by the function. */

void Calibrate_IDAC(uint8 SensorIndex, uint8 DutyCycle)
{
	extern uint8 CapSense_idacSettings[];
	extern uint8 CapSense_widgetResolution[];
	extern uint16 CapSense_sensorRaw[];
	extern const uint8 CYCODE CapSense_widgetNumber[];
	uint8 Mask;
	uint8 idac;
	uint16 temp_raw;
	uint32 CalTarget;


	switch(CapSense_widgetResolution[CapSense_widgetNumber[SensorIndex]])
	{
		case CapSense_PWM_RESOLUTION_8_BITS:
			CalTarget = 0x000000FF;
			break;
		case CapSense_PWM_RESOLUTION_9_BITS:
			CalTarget = 0x000001FF;
			break;
		case CapSense_PWM_RESOLUTION_10_BITS:
			CalTarget = 0x000003FF;
			break;
		case CapSense_PWM_RESOLUTION_11_BITS:
			CalTarget = 0x000007FF;
			break;
		case CapSense_PWM_RESOLUTION_12_BITS:
			CalTarget = 0x00000FFF;
			break;
		case CapSense_PWM_RESOLUTION_13_BITS:
			CalTarget = 0x00001FFF;
			break;
		case CapSense_PWM_RESOLUTION_14_BITS:
			CalTarget = 0x00003FFF;
			break;
		case CapSense_PWM_RESOLUTION_15_BITS:
			CalTarget = 0x00007FFF;
			break;
		case CapSense_PWM_RESOLUTION_16_BITS:
			CalTarget = 0x0000FFFF;
			break;
		default:
			CalTarget = 0x000000FF;
			break;
	}

	CalTarget = (CalTarget * DutyCycle) / 100;
	
	/* Find a scan IDAC value closest to the target. This routine iterates through the
	8-bit scan IDAC value by setting each bit, scanning, and testing the output. If less
	than the target the bit is cleared before setting the next lower bit.  */
	
	Mask = 0x80; //Start with the MSB
	idac = 0xff;
	
	while(Mask)
	{
		CapSense_idacSettings[SensorIndex] = idac;	//Set the idac value	
		
		CapSense_ScanSensor(SensorIndex);
        
		while(CapSense_IsBusy() != 0);
		
		temp_raw = CapSense_sensorRaw[SensorIndex];
		
		if (temp_raw > CalTarget )
		{
			idac += Mask;
		}
		else if (temp_raw < CalTarget)
		{
			idac -= Mask;
		}
		else
		{
			break;
		}
		
		Mask >>= 1; //Move to next bit to the right
	}
	
}

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

void CalibrateAllSensors(void)
{
	uint8 i;
	for(i = 0u; i < CapSense_TOTAL_SENSOR_COUNT; i++)
    {
        Calibrate_IDAC(i,CALIBRATION_TARGET_DUTY_CYCLE);
    }
}



/* [] END OF FILE */
