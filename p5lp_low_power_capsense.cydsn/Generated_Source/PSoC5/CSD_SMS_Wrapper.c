/*******************************************************************************
* File Name: CSD_SMS_Wrapper.c
* Version 3.40
*
* Description:
*  This file provides the source code of wrapper between CapSense CSD component 
*  and Auto Tuning library.
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "CSD.h"
#include "CSD_CSHL.h"

#if (CSD_TUNING_METHOD == CSD_AUTO_TUNING)

extern uint8 CSD_noiseThreshold[CSD_WIDGET_CSHL_PARAMETERS_COUNT];
extern uint8 CSD_hysteresis[CSD_WIDGET_CSHL_PARAMETERS_COUNT];
extern uint8 CSD_widgetResolution[CSD_WIDGET_RESOLUTION_PARAMETERS_COUNT];
extern const uint8 CYCODE CSD_numberOfSensors[CSD_SENSORS_TBL_SIZE];

extern uint8 CSD_fingerThreshold[CSD_WIDGET_CSHL_PARAMETERS_COUNT];
extern uint8 CSD_idacSettings[CSD_TOTAL_SENSOR_COUNT];
extern uint8 CSD_analogSwitchDivider[CSD_TOTAL_SCANSLOT_COUNT];

extern void SMS_LIB_V3_CalculateThresholds(uint8 SensorNumber);
extern void SMS_LIB_V3_AutoTune1Ch(void);
extern void SMS_LIB_V3_AutoTune2Ch(void);

uint8  * SMS_LIB_noiseThreshold = CSD_noiseThreshold;
uint8 * SMS_LIB_hysteresis = CSD_hysteresis;

uint8 * SMS_LIB_widgetResolution = CSD_widgetResolution;

const uint8 CYCODE * SMS_LIB_widgetNumber = CSD_widgetNumber;
const uint8 CYCODE * SMS_LIB_numberOfSensors = CSD_numberOfSensors;

uint8 * SMS_LIB_fingerThreshold = CSD_fingerThreshold;
uint8 * SMS_LIB_idacSettings = CSD_idacSettings;
uint8 * SMS_LIB_prescaler = CSD_analogSwitchDivider;

uint16 * SMS_LIB_SensorRaw = CSD_sensorRaw;
uint16 * SMS_LIB_SensorBaseline = CSD_sensorBaseline;

const uint8 CYCODE SMS_LIB_SensorSensitivity[] = {
    2, 2, 
};


const uint8 CYCODE SMS_LIB_PrescalerTbl[] = {
    1u, 1u, 1u, 1u, 1u, 1u, 2u, 2u, 2u, 2u, 2u, 3u, 3u, 3u, 3u, 3u, 3u, 4u, 4u, 4u, 4u, 4u, 4u, 5u, 5u, 5u, 5u, 5u, 5u, 6u, 6u, 6u, 
};



uint8 SMS_LIB_Table1[CSD_TOTAL_SENSOR_COUNT];
uint8 SMS_LIB_Table2[CSD_TOTAL_SENSOR_COUNT];
uint8 SMS_LIB_Table3[CSD_TOTAL_SENSOR_COUNT];
uint16 SMS_LIB_Table4[CSD_TOTAL_SENSOR_COUNT];
uint16 SMS_LIB_Table5[CSD_TOTAL_SENSOR_COUNT];
uint8 SMS_LIB_Table6[CSD_TOTAL_SENSOR_COUNT];
uint8 SMS_LIB_Table7[CSD_TOTAL_SENSOR_COUNT];

uint8 SMS_LIB_Table8[CSD_END_OF_WIDGETS_INDEX];
uint8 SMS_LIB_Table9[CSD_END_OF_WIDGETS_INDEX];
uint8 SMS_LIB_Table10[CSD_END_OF_WIDGETS_INDEX];

uint8 SMS_LIB_Var1 = (5u);
uint16 SMS_LIB_Var2 = (10738u);

uint8 SMS_LIB_TotalSnsCnt = CSD_TOTAL_SENSOR_COUNT;
uint8 SMS_LIB_TotalScanSlCnt = CSD_TOTAL_SCANSLOT_COUNT;
uint8 SMS_LIB_EndOfWidgInd = CSD_END_OF_WIDGETS_INDEX;

#if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
    uint8 SMS_LIB_TotalSnsCnt_CH0 = CSD_TOTAL_SENSOR_COUNT__CH0;
    uint8 SMS_LIB_TotalSnsCnt_CH1 = CSD_TOTAL_SENSOR_COUNT__CH1;
#else
    uint8 SMS_LIB_TotalSnsCnt_CH0 = 0u;
    uint8 SMS_LIB_TotalSnsCnt_CH1 = 0u;
#endif  /* (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) */

/*******************************************************************************
* Function Name: SMS_LIB_ScanSensor
********************************************************************************
*
* Summary:
*  Wrapper to CSD_ScanSensor function.
*
* Parameters:
*  SensorNumber:  Sensor number.
*
* Return:
*  None
*
* Reentrant:
*  No
*
*******************************************************************************/
void SMS_LIB_ScanSensor(uint8 SensorNumber) 
{
    CSD_ScanSensor(SensorNumber);
}

/*******************************************************************************
* Function Name: SMS_LIB_IsBusy
********************************************************************************
*
* Summary:
*  Wrapper to CSD_IsBusy function.
*  
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
uint8 SMS_LIB_IsBusy(void) 
{
    return CSD_IsBusy();
}


/*******************************************************************************
* Function Name: CSD_CalculateThresholds
********************************************************************************
*
* Summary:
*  Wrapper to SMS_LIB_CalculateThresholds function.
*
* Parameters:
*  SensorNumber:  Sensor number.
*
* Return:
*  None
*
* Reentrant:
*  No
*
*******************************************************************************/
void CSD_CalculateThresholds(uint8 SensorNumber) 
{
    SMS_LIB_V3_CalculateThresholds(SensorNumber);
}


/*******************************************************************************
* Function Name: CSD_AutoTune
********************************************************************************
*
* Summary:
*  Wrapper for SMS_LIB_AutoTune1Ch or SMS_LIB_AutoTune2Ch function.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Reentrant:
*  No
*
*******************************************************************************/
void CSD_AutoTune(void) 
{
    #if (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN)
        SMS_LIB_V3_AutoTune1Ch();
    #elif (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
        SMS_LIB_V3_AutoTune2Ch();
    #endif /* (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN) */
}

/*******************************************************************************
* Function Name: SMS_LIB_SetPrescaler
********************************************************************************
*
* Summary:
*  Empty wrapper for version compliance.
*
* Parameters:
*  prescaler:  prascaler value.
*
* Return:
*  None
*
*******************************************************************************/
void SMS_LIB_SetPrescaler(uint8 prescaler) 
{
    prescaler = prescaler;
}

void SMS_LIB_V3_SetAnalogSwitchesSrc_PRS(void) 
{
	CSD_SetAnalogSwitchesSource(CSD_ANALOG_SWITCHES_SRC_PRS);
}

void SMS_LIB_V3_SetAnalogSwitchesSrc_Prescaler(void) 
{
	CSD_SetAnalogSwitchesSource(CSD_ANALOG_SWITCHES_SRC_PRESCALER);
}

#endif  /* (CSD_TUNING_METHOD == CSD_AUTO_TUNING) */


/* [] END OF FILE */
