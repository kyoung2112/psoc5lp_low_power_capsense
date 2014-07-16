/*******************************************************************************
* File Name: CSD_CSHL.c
* Version 3.40
*
* Description:
*  This file provides the source code to the High Level APIs for the CapSesne
*  CSD component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "CSD_CSHL.h"

/* Storage of filters data */
#if ( (CSD_RAW_FILTER_MASK & CSD_MEDIAN_FILTER) | \
      (CSD_RAW_FILTER_MASK & CSD_AVERAGING_FILTER) )

    uint16 CSD_rawFilterData1[CSD_TOTAL_SENSOR_COUNT];
    uint16 CSD_rawFilterData2[CSD_TOTAL_SENSOR_COUNT];

#elif ( (CSD_RAW_FILTER_MASK & CSD_IIR2_FILTER)   | \
        (CSD_RAW_FILTER_MASK & CSD_IIR4_FILTER)   | \
        (CSD_RAW_FILTER_MASK & CSD_JITTER_FILTER) | \
        (CSD_RAW_FILTER_MASK & CSD_IIR8_FILTER)   | \
        (CSD_RAW_FILTER_MASK & CSD_IIR16_FILTER) )
        
    uint16 CSD_rawFilterData1[CSD_TOTAL_SENSOR_COUNT];

#else
    /* No Raw filters */
#endif  /* ( (CSD_RAW_FILTER_MASK & CSD_MEDIAN_FILTER) || \
        *    (CSD_RAW_FILTER_MASK & CSD_AVERAGING_FILTER) )
        */

uint16 CSD_sensorBaseline[CSD_TOTAL_SENSOR_COUNT];
uint8 CSD_sensorBaselineLow[CSD_TOTAL_SENSOR_COUNT];
uint8 CSD_sensorSignal[CSD_TOTAL_SENSOR_COUNT];
uint8 CSD_sensorOnMask[(((CSD_TOTAL_SENSOR_COUNT - 1u) / 8u) + 1u)];

uint8 CSD_lowBaselineResetCnt[CSD_TOTAL_SENSOR_COUNT];

uint8 CSD_fingerThreshold[] = {
    100u, 100u, 
};

uint8 CSD_noiseThreshold[] = {
    20u, 20u, 
};

uint8 CSD_hysteresis[] = {
    10u, 10u, 
};

uint8 CSD_debounce[] = {
    1u, 1u, 
};

uint8 CSD_debounceCounter[] = {
    0u, 0u, 0u, 
};

const uint8 CYCODE CSD_rawDataIndex[] = {
    0u, /* Button0__BTN */
    1u, /* Button1__BTN */

};

const uint8 CYCODE CSD_numberOfSensors[] = {
    1u, /* Button0__BTN */
    1u, /* Button1__BTN */

};




/*******************************************************************************
* Function Name: CSD_BaseInit
********************************************************************************
*
* Summary:
*  Loads the CSD_sensorBaseline[sensor] array element with an 
*  initial value which is equal to raw count value. 
*  Resets to zero CSD_sensorBaselineLow[sensor] and 
*  CSD_sensorSignal[sensor] array element.
*  Loads CSD_debounceCounter[sensor] array element with initial 
*  value equal CSD_debounce[].
*  Loads the CSD_rawFilterData2[sensor] and 
*  CSD_rawFilterData2[sensor] array element with an 
*  initial value which is equal raw count value if raw data filter is enabled.
*
* Parameters:
*  sensor:  Sensor number.
*
* Return:
*  None
*
* Global Variables:
*  CSD_sensorBaseline[]    - used to store baseline value.
*  CSD_sensorBaselineLow[] - used to store fraction byte of 
*  baseline value.
*  CSD_sensorSignal[]      - used to store diffence between 
*  current value of raw data and previous value of baseline.
*  CSD_debounceCounter[]   - used to store current debounce 
*  counter of sensor. Widget which has this parameter are buttons, matrix 
*  buttons, proximity, guard. All other widgets haven't debounce parameter
*  and use the last element of this array with value 0 (it means no debounce).
*  CSD_rawFilterData1[]    - used to store previous sample of 
*  any enabled raw data filter.
*  CSD_rawFilterData2[]    - used to store before previous sample
*  of enabled raw data filter. Only required for median or average filters.
*
* Reentrant:
*  No
*
*******************************************************************************/
void CSD_BaseInit(uint8 sensor) 
{
    #if ((CSD_TOTAL_BUTTONS_COUNT) || (CSD_TOTAL_MATRIX_BUTTONS_COUNT) || \
         (CSD_TOTAL_GENERICS_COUNT))
        uint8 widget = CSD_widgetNumber[sensor];
    #endif /* ((CSD_TOTAL_MATRIX_BUTTONS_COUNT) || (CSD_TOTAL_MATRIX_BUTTONS_COUNT)) */
    
    #if (CSD_TOTAL_MATRIX_BUTTONS_COUNT)
        uint8 debounceIndex;
    #endif  /* (CSD_TOTAL_MATRIX_BUTTONS_COUNT) */
    
    #if (CSD_TOTAL_GENERICS_COUNT)
        /* Exclude generic widget */
        if(widget < CSD_END_OF_WIDGETS_INDEX)
        {
    #endif  /* CSD_TOTAL_GENERICS_COUNT */
    
    /* Initialize Baseline */
    CSD_sensorBaseline[sensor] = CSD_sensorRaw[sensor];
    CSD_sensorBaselineLow[sensor] = 0u;
    CSD_sensorSignal[sensor] = 0u;
        
    CSD_debounceCounter[widget] =  CSD_debounce[widget];

    
    #if ((CSD_RAW_FILTER_MASK & CSD_MEDIAN_FILTER) |\
         (CSD_RAW_FILTER_MASK & CSD_AVERAGING_FILTER))

        CSD_rawFilterData1[sensor] = CSD_sensorRaw[sensor];
        CSD_rawFilterData2[sensor] = CSD_sensorRaw[sensor];
    
    #elif ((CSD_RAW_FILTER_MASK & CSD_IIR2_FILTER) |\
           (CSD_RAW_FILTER_MASK & CSD_IIR4_FILTER) |\
           (CSD_RAW_FILTER_MASK & CSD_JITTER_FILTER) |\
           (CSD_RAW_FILTER_MASK & CSD_IIR8_FILTER) |\
           (CSD_RAW_FILTER_MASK & CSD_IIR16_FILTER))
            
        CSD_rawFilterData1[sensor] = CSD_sensorRaw[sensor];
    
    #else
        /* No Raw filters */
    #endif  /* ((CSD_RAW_FILTER_MASK & CSD_MEDIAN_FILTER) | \
            *   (CSD_RAW_FILTER_MASK & CSD_AVERAGING_FILTER))
            */
    
    #if (CSD_TOTAL_GENERICS_COUNT)
        /* Exclude generic widget */
        }
    #endif  /* CSD_TOTAL_GENERICS_COUNT */
}


/*******************************************************************************
* Function Name: CSD_InitializeSensorBaseline
********************************************************************************
*
* Summary:
*  Loads the CSD_sensorBaseline[sensor] array element with an 
*  initial value by scanning the selected sensor (one channel design) or pair 
*  of sensors (two channels designs). The raw count value is copied into the 
*  baseline array for each sensor. The raw data filters are initialized if 
*  enabled.
*
* Parameters:
*  sensor:  Sensor number.
*
* Return:
*  None
*
* Reentrant:
*  No
*
*******************************************************************************/
void CSD_InitializeSensorBaseline(uint8 sensor)
                          
{
    /* Scan sensor */
    CSD_ScanSensor(sensor);
    while(CSD_IsBusy() != 0u)
    {
        /* Wait while busy */
    }
    
    #if (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN)
        /* Initialize Baseline, Signal and debounce counters */       
        CSD_BaseInit(sensor);
        
    #else
    
        if(sensor < CSD_TOTAL_SENSOR_COUNT__CH0)
        {
            /* Initialize Baseline, Signal and debounce counters */ 
            CSD_BaseInit(sensor);
        }
        
        if(sensor < CSD_TOTAL_SENSOR_COUNT__CH1)
        {
            /* Initialize Baseline, Signal and debounce counters */
            CSD_BaseInit(sensor + CSD_TOTAL_SENSOR_COUNT__CH0);
        }
    
    #endif  /* (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN) */
}


/*******************************************************************************
* Function Name: CSD_InitializeAllBaselines
********************************************************************************
*
* Summary:
*  Uses the CSD_InitializeSensorBaseline function to loads the 
*  CSD_sensorBaseline[] array with an initial values by scanning 
*  all sensors. The raw count values are copied into the baseline array for 
*  all sensors. The raw data filters are initialized if enabled.
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
void CSD_InitializeAllBaselines(void)
                          
{
    uint8 i;
    
    /* The baseline initialize by sensor of sensor pair */
    for(i = 0u; i < CSD_TOTAL_SCANSLOT_COUNT; i++)
    {
        CSD_InitializeSensorBaseline(i);
    }
}


/*******************************************************************************
* Function Name: CSD_InitializeEnabledBaselines
********************************************************************************
*
* Summary:
*  Scans all enabled widgets and the raw count values are copied into the 
*  baseline array for all sensors enabled in scanning process. Baselines 
*  initialize with zero values for sensors disabled from scanning process. 
*  The raw data filters are initialized if enabled.
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
void CSD_InitializeEnabledBaselines(void)
                             
{
    uint8 i;
    uint8 pos;
    uint8 enMask;
    
    CSD_ScanEnabledWidgets();
    while(CSD_IsBusy() != 0u)
    {
        /* Wait while busy */
    }
    
    for(i = 0u; i < CSD_TOTAL_SENSOR_COUNT; i++)
    {
        pos = (i >> 3u);
        enMask = 0x01u << (i & 0x07u);
        
        /* Clear raw data if sensor is disabled from scanning process */
        if((CSD_sensorEnableMask[pos] & enMask) == 0u)
        {
            CSD_sensorRaw[i] = 0u;
        }
        
        /* Initialize baselines */
        CSD_BaseInit(i);
    }
}  


/*******************************************************************************
* Function Name: CSD_UpdateSensorBaseline
********************************************************************************
*
* Summary:
*  Updates the CSD_sensorBaseline[sensor] array element using the 
*  LP filter with k = 256. The signal calculates the difference of count by 
*  subtracting the previous baseline from the current raw count value and stores
*  it in CSD_SensorSignal[sensor]. 
*  If auto reset option is enabled the baseline updated regards noise threshold. 
*  If auto reset option is disabled the baseline stops updating if signal is 
*  greater that zero and baseline loads with raw count value if signal is less 
*  that noise threshold.
*  Raw data filters are applied to the values if enabled before baseline 
*  calculation.
*
* Parameters:
*  sensor:  Sensor number.
*
* Return:
*  None
*
* Global Variables:
*  CSD_sensorBaseline[]    - used to store baseline value.
*  CSD_sensorBaselineLow[] - used to store fraction byte of 
*  baseline value.
*  CSD_sensorSignal[]      - used to store diffence between 
*  current value of raw data and previous value of baseline.
*  CSD_rawFilterData1[]    - used to store previous sample of 
*  any enabled raw data filter.
*  CSD_rawFilterData2[]    - used to store before previous sample
*  of enabled raw data filter. Only required for median or average filters.
*
* Reentrant:
*  No
*
*******************************************************************************/
 void CSD_UpdateSensorBaseline(uint8 sensor)
                                 
{
    uint32 calc;
    uint16 tempRaw;
    uint16 filteredRawData;
    uint8 widget = CSD_widgetNumber[sensor];
    uint8 noiseThreshold = CSD_noiseThreshold[widget];
    
    #if (CSD_TOTAL_GENERICS_COUNT)
        /* Exclude generic widget */
        if(widget < CSD_END_OF_WIDGETS_INDEX)
        {
    #endif  /* CSD_TOTAL_GENERICS_COUNT */
    
    filteredRawData = CSD_sensorRaw[sensor];
    
    #if (CSD_RAW_FILTER_MASK & CSD_MEDIAN_FILTER)
        tempRaw = filteredRawData;
        filteredRawData = CSD_MedianFilter(filteredRawData, CSD_rawFilterData1[sensor], 
                                                        CSD_rawFilterData2[sensor]);
        CSD_rawFilterData2[sensor] = CSD_rawFilterData1[sensor];
        CSD_rawFilterData1[sensor] = tempRaw;
        
    #elif (CSD_RAW_FILTER_MASK & CSD_AVERAGING_FILTER)
        tempRaw = filteredRawData;
        filteredRawData = CSD_AveragingFilter(filteredRawData, CSD_rawFilterData1[sensor],
                                                           CSD_rawFilterData2[sensor]);
        CSD_rawFilterData2[sensor] = CSD_rawFilterData1[sensor];
        CSD_rawFilterData1[sensor] = tempRaw;
        
    #elif (CSD_RAW_FILTER_MASK & CSD_IIR2_FILTER)
        filteredRawData = CSD_IIR2Filter(filteredRawData, CSD_rawFilterData1[sensor]);
        CSD_rawFilterData1[sensor] = filteredRawData;
        
    #elif (CSD_RAW_FILTER_MASK & CSD_IIR4_FILTER)
        filteredRawData = CSD_IIR4Filter(filteredRawData, CSD_rawFilterData1[sensor]);
        CSD_rawFilterData1[sensor] = filteredRawData;
            
    #elif (CSD_RAW_FILTER_MASK & CSD_JITTER_FILTER)
        filteredRawData = CSD_JitterFilter(filteredRawData, CSD_rawFilterData1[sensor]);
        CSD_rawFilterData1[sensor] = filteredRawData;
        
    #elif (CSD_RAW_FILTER_MASK & CSD_IIR8_FILTER)
        filteredRawData = CSD_IIR8Filter(filteredRawData, CSD_rawFilterData1[sensor]);
        CSD_rawFilterData1[sensor] = filteredRawData;
        
    #elif (CSD_RAW_FILTER_MASK & CSD_IIR16_FILTER)
        filteredRawData = CSD_IIR16Filter(filteredRawData, CSD_rawFilterData1[sensor]);
        CSD_rawFilterData1[sensor] = filteredRawData;
        
    #else
        /* No Raw filters */
    #endif  /* (CSD_RAW_FILTER_MASK & CSD_MEDIAN_FILTER) */
    
    #if (CSD_TUNING_METHOD == CSD_AUTO_TUNING)
        CSD_CalculateThresholds(sensor);
    #endif /* (CSD_TUNING_METHOD == CSD_AUTO_TUNING) */


    /* Baseline calculation */
    /* Calculate difference RawData[cur] - Baseline[prev] */
    if(filteredRawData >= CSD_sensorBaseline[sensor])
    {
        tempRaw = filteredRawData - CSD_sensorBaseline[sensor];
        widget = 1u;    /* Positive difference - Calculate the Signal */
    }
    else
    {
        tempRaw = CSD_sensorBaseline[sensor] - filteredRawData;
        widget = 0u;    /* Negative difference - Do NOT calculate the Signal */
    }

    if((widget == 0u) && (tempRaw > (uint16) CSD_NEGATIVE_NOISE_THRESHOLD))
    {
        if(CSD_lowBaselineResetCnt[sensor] >= CSD_LOW_BASELINE_RESET)
        {
            CSD_BaseInit(sensor);
            CSD_lowBaselineResetCnt[sensor] = 0u;
        }
        else
        {
            CSD_lowBaselineResetCnt[sensor]++;
        }
    }
    else
    {
        #if (CSD_AUTO_RESET == CSD_AUTO_RESET_DISABLE)
            /* Update Baseline if lower that noiseThreshold */
            if ( (tempRaw <= (uint16) noiseThreshold) || 
                 ((tempRaw < (uint16) CSD_NEGATIVE_NOISE_THRESHOLD)
                   && (widget == 0u)))
            {
        #endif /* (CSD_AUTO_RESET == CSD_AUTO_RESET_DISABLE) */
                /* Make full Baseline 23 bits */
                calc = (uint32) CSD_sensorBaseline[sensor] << 8u;
                calc |= (uint32) CSD_sensorBaselineLow[sensor];

                /* Add Raw Data to Baseline */
                calc += filteredRawData;

                /* Sub the high Baseline */
                calc -= CSD_sensorBaseline[sensor];

                /* Put Baseline and BaselineLow */
                CSD_sensorBaseline[sensor] = ((uint16) (calc >> 8u));
                CSD_sensorBaselineLow[sensor] = ((uint8) calc);

                CSD_lowBaselineResetCnt[sensor] = 0u;
        #if (CSD_AUTO_RESET == CSD_AUTO_RESET_DISABLE)
            }
        #endif /* (CSD_AUTO_RESET == CSD_AUTO_RESET_DISABLE) */
    }

    /* Calculate Signal if possitive difference > noiseThreshold */
    if((tempRaw > (uint16) noiseThreshold) && (widget != 0u))
    {
        #if (CSD_SIGNAL_SIZE == CSD_SIGNAL_SIZE_UINT8)
            /* Over flow defence for uint8 */
            if (tempRaw > 0xFFu)
            {
                CSD_sensorSignal[sensor] = 0xFFu;
            }    
            else 
            {
                CSD_sensorSignal[sensor] = ((uint8) tempRaw);
            }
        #else
            CSD_sensorSignal[sensor] = ((uint16) tempRaw);
        #endif  /* (CSD_SIGNAL_SIZE == CSD_SIGNAL_SIZE_UINT8) */
    }
    else
    {
        /* Signal is zero */
        CSD_sensorSignal[sensor] = 0u;
    }

    #if (CSD_TOTAL_GENERICS_COUNT)
        /* Exclude generic widget */
        }
    #endif  /* CSD_TOTAL_GENERICS_COUNT */
}


/*******************************************************************************
* Function Name: CSD_UpdateEnabledBaselines
********************************************************************************
*
* Summary:
*  Checks CSD_sensorEnableMask[] array and calls the 
*  CSD_UpdateSensorBaseline function to update the baselines 
*  for enabled sensors.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global Variables:
*  CSD_sensorEnableMask[] - used to store the sensor scanning 
*  state.
*  CSD_sensorEnableMask[0] contains the masked bits for sensors 
*   0 through 7 (sensor 0 is bit 0, sensor 1 is bit 1).
*  CSD_sensorEnableMask[1] contains the masked bits for 
*  sensors 8 through 15 (if needed), and so on.
*  0 - sensor doesn't scan by CSD_ScanEnabledWidgets().
*  1 - sensor scans by CSD_ScanEnabledWidgets().
*
* Reentrant:
*  No
*
*******************************************************************************/
 void CSD_UpdateEnabledBaselines(void)
                                 
{
    uint8 i;
    uint8 pos;
    uint8 enMask;
    
    for(i = 0u; i < CSD_TOTAL_SENSOR_COUNT; i++)
    {
        pos = (i >> 3u);
        enMask = (uint8)(0x01u << (i & 0x07u));
        if((CSD_sensorEnableMask[pos] & enMask) != 0u)
        {
            CSD_UpdateSensorBaseline(i);
        }
    }
}


/*******************************************************************************
* Function Name: CSD_CheckIsSensorActive
********************************************************************************
*
* Summary:
*  Compares the CSD_SensorSignal[sensor] array element to finger
*  threshold of widget it belongs to. The hysteresis and debounce are taken into 
*  account. The hysteresis is added or subtracted from the finger threshold 
*  based on whether the sensor is currently active. 
*  If the sensor is active, the threshold is lowered by the hysteresis amount.
*  If the sensor is inactive, the threshold is raised by the hysteresis amount.
*  The debounce counter added to the sensor active transition.
*  This function updates CSD_SensorOnMask[] array element.
*
* Parameters:
*  sensor:  Sensor number.
*
* Return:
*  Returns sensor state 1 if active, 0 if not active.
*
* Global Variables:
*  CSD_sensorSignal[]      - used to store diffence between 
*  current value of raw data and previous value of baseline.
*  CSD_debounceCounter[]   - used to store current debounce 
*  counter of sensor. Widget which has this parameter are buttons, matrix 
*  buttons, proximity, guard. All other widgets haven't debounce parameter
*  and use the last element of this array with value 0 (it means no debounce).
*  CSD_sensorOnMask[] - used to store sensors on/off state.
*  CSD_sensorOnMask[0] contains the masked bits for sensors 
*   0 through 7 (sensor 0 is bit 0, sensor 1 is bit 1).
*  CSD_sensorEnableMask[1] contains the masked bits for 
*  sensors 8 through 15 (if needed), and so on.
*  0 - sensor is inactive.
*  1 - sensor is active.
*
* Reentrant:
*  No
*
*******************************************************************************/
uint8 CSD_CheckIsSensorActive(uint8 sensor)
                                  
{
    uint8 debounceIndex;
    /* Get On/Off mask */
    uint8 pos = (sensor >> 3u);
    uint8 onMask = 0x01u << (sensor & 0x07u);
    /* Prepare to find debounce counter index */
    uint8 widget = CSD_widgetNumber[sensor];
    uint8 fingerThreshold = CSD_fingerThreshold[widget];
    uint8 hysteresis = CSD_hysteresis[widget];
    uint8 debounce = CSD_debounce[widget];
    
    debounceIndex = widget;

    
    /* Was on */
    if (0u != (CSD_sensorOnMask[pos] & onMask))
    {
        /* Hysteresis minus */
        if (CSD_sensorSignal[sensor] < (fingerThreshold - hysteresis))
        {
            CSD_sensorOnMask[pos] &= (uint8)~onMask;
            CSD_debounceCounter[debounceIndex] = debounce; 
        }
    }
    else    /* Was off */
    {
        /* Hysteresis plus */
        if (CSD_sensorSignal[sensor] > (fingerThreshold + hysteresis))
        {
            /* Sensor active, decrement debounce counter */
            if (CSD_debounceCounter[debounceIndex] <= 1u)
            {
                CSD_sensorOnMask[pos] |= onMask; 
            }
            CSD_debounceCounter[debounceIndex]--;
        }
        else
        {
            /* Sensor inactive - reset Debounce counter */
            CSD_debounceCounter[debounceIndex] = debounce;
        }
    }
    
    return (((CSD_sensorOnMask[pos] & onMask) != 0u) ? 1u : 0u);
}


/*******************************************************************************
* Function Name: CSD_CheckIsWidgetActive
********************************************************************************
*
* Summary:
*  Use function CSD_CheckIsSensorActive() to update 
*  CSD_sensorOnMask[] for all sensors within the widget.
*  If one of sensors within widget is active the function return that widget is 
*  active.
*  The touch pad and matrix buttons widgets need to have active sensor within 
*  col and row to return widget active status.
*
* Parameters:
*  widget:  widget number.
*
* Return:
*  Returns widget sensor state 1 if one or more sensors within widget is/are 
*  active, 0 if all sensors within widget are inactive.
*
* Reentrant:
*  No
*
*******************************************************************************/
uint8 CSD_CheckIsWidgetActive(uint8 widget)
                                 
{
    uint8 rawIndex = CSD_rawDataIndex[widget];
    uint8 numberOfSensors = CSD_numberOfSensors[widget] + rawIndex;
    uint8 state = 0u;

    /* Check all sensors of the widget */
    do
    {
        if(CSD_CheckIsSensorActive(rawIndex) != 0u)
        {
            state = CSD_SENSOR_1_IS_ACTIVE;
        }
        rawIndex++;
    }
    while(rawIndex < numberOfSensors);
    

    
    return (state);
}


/*******************************************************************************
* Function Name: CSD_CheckIsAnyWidgetActive
********************************************************************************
*
* Summary:
*  Compares all sensors of the CSD_signal[] array to their finger 
*  threshold. Calls CSD_CheckIsWidgetActive() for each widget so 
*  the CSD_SensorOnMask[] array is up to date after calling this 
*  function.
*
* Parameters:
*  widget:  widget number.
*
* Return:
*  Returns 1 if any widget is active, 0 none of widgets are active.
*
* Reentrant:
*  No
*
*******************************************************************************/
uint8 CSD_CheckIsAnyWidgetActive(void)
                                 
{
    uint8 i;
    uint8 state = 0u;
    
    for(i = 0u; i < CSD_TOTAL_WIDGET_COUNT; i++)
    {
        if (CSD_CheckIsWidgetActive(i) != 0u)
        {
            state = CSD_WIDGET_IS_ACTIVE;
        }
    }
    

    return (state);
}


/*******************************************************************************
* Function Name: CSD_EnableWidget
********************************************************************************
*
* Summary:
*  Enable all widget elements (sensors) to scanning process.
*
* Parameters:
*  widget:  widget number.
*
* Return:
*  None
*
* Global Variables:
*  CSD_sensorEnableMask[] - used to store the sensor scanning 
*  state.
*  CSD_sensorEnableMask[0] contains the masked bits for sensors 
*  0 through 7 (sensor 0 is bit 0, sensor 1 is bit 1).
*  CSD_sensorEnableMask[1] contains the masked bits for 
*  sensors 8 through 15 (if needed), and so on.
*  0 - sensor doesn't scan by CSD_ScanEnabledWidgets().
*  1 - sensor scans by CSD_ScanEnabledWidgets().
*
* Reentrant:
*  No
*
*******************************************************************************/
void CSD_EnableWidget(uint8 widget)
                                   
{
    uint8 pos;
    uint8 enMask;
    uint8 rawIndex = CSD_rawDataIndex[widget];
    uint8 numberOfSensors = CSD_numberOfSensors[widget] + rawIndex;
    
    /* Enable all sensors of the widget */
    do
    {
        pos = (rawIndex >> 3u);
        enMask = 0x01u << (rawIndex & 0x07u);
        
        CSD_sensorEnableMask[pos] |= enMask;
        rawIndex++;
    }
    while(rawIndex < numberOfSensors);
    

}


/*******************************************************************************
* Function Name: CSD_DisableWidget
********************************************************************************
*
* Summary:
*  Disable all widget elements (sensors) from scanning process.
*
* Parameters:
*  widget:  widget number.
*
* Return:
*  None
*
* Global Variables:
*  CSD_sensorEnableMask[] - used to store the sensor scanning 
*  state.
*  CSD_sensorEnableMask[0] contains the masked bits for sensors 
*  0 through 7 (sensor 0 is bit 0, sensor 1 is bit 1).
*  CSD_sensorEnableMask[1] contains the masked bits for 
*  sensors 8 through 15 (if needed), and so on.
*  0 - sensor doesn't scan by CSD_ScanEnabledWidgets().
*  1 - sensor scans by CSD_ScanEnabledWidgets().
*
* Reentrant:
*  No
*
*******************************************************************************/
void CSD_DisableWidget(uint8 widget)
                                    
{
    uint8 pos;
    uint8 enMask;
    uint8 rawIndex = CSD_rawDataIndex[widget];
    uint8 numberOfSensors = CSD_numberOfSensors[widget] + rawIndex;
   
    /* Disable all sensors of the widget */
    do
    {
        pos = (rawIndex >> 3u);
        enMask = 0x01u << (rawIndex & 0x07u);
        
        CSD_sensorEnableMask[pos] &= (uint8)~enMask;
        rawIndex++;
    }
    while(rawIndex < numberOfSensors);
    

}
#if(CSD_TOTAL_CENTROIDS_COUNT)
    /*******************************************************************************
    * Function Name: CSD_FindMaximum
    ********************************************************************************
    *
    * Summary:
    *  Finds index of maximum element within defined centroid. Checks 
    *  CSD_SensorSignal[] within defined cenrtoid and 
    *  returns index of maximum element. The values below finger threshold are 
    *  ignored.
    *  The centrod defines by offset of first element and number of elements - count.
    *  The diplexed centroid requires at least consecutive two elements above
    *  FingerThreshold to find index of maximum element.
    * 
    * Parameters:
    *  offset:  Start index of cetroid in CSD_sensorSignal[] array.
    *  count:   number of elements within centroid.
    *  fingerThreshold:  Finger threshould.
    *  diplex:   pointer to diplex table.
    * 
    * Return:
    *  Returns index of maximum element within defined centroid.
    *  If index of maximum element doesn't find the 0xFF returns.
    * 
    *******************************************************************************/
    #if (CSD_IS_DIPLEX_SLIDER)
        uint8 CSD_FindMaximum(uint8 offset, uint8 count, uint8 fingerThreshold, const uint8 CYCODE *diplex)
	                                       
    #else 
        uint8 CSD_FindMaximum(uint8 offset, uint8 count, uint8 fingerThreshold)
	                                       
    #endif /* (CSD_IS_DIPLEX_SLIDER) */
    {
        uint8 i;
        #if (CSD_IS_DIPLEX_SLIDER)        
            uint8 curPos = 0u;
            /* No centroid at the Start */
            uint8 curCntrdSize = 0u;
            uint8 curCtrdStartPos = 0xFFu;
            /* The biggset centroid is zero */
            uint8 biggestCtrdSize = 0u;
            uint8 biggestCtrdStartPos = 0u;
        #endif /* (CSD_IS_DIPLEX_SLIDER) */
        uint8 maximum = 0xFFu;
        uint8 temp = 0u;
        uint8 *startOfSlider = &CSD_sensorSignal[offset]; 

        #if (CSD_IS_DIPLEX_SLIDER)        
            if(diplex != 0u)
            {
                /* Initialize */
                i = 0u;
                
                /* Make slider x2 as Diplexed */
                count <<= 1u;
                while(1u)
                { 
                    if (startOfSlider[curPos] > 0u)    /* Looking for centroids */
                    {
                        if (curCtrdStartPos == 0xFFu)
                        {
                            /* Start of centroid */
                            curCtrdStartPos = i;
                            curCntrdSize++;
                        }
                        else
                        {
                            curCntrdSize++;
                        }
                    }
                    else   /* Select the bigest and indicate zero start */
                    {          
                        if(curCntrdSize > 0u)
                        {
                            /* We are in the end of current */
                            if(curCntrdSize > biggestCtrdSize)
                            {
                                biggestCtrdSize = curCntrdSize;
                                biggestCtrdStartPos = curCtrdStartPos;
                            }
                            
                            curCntrdSize = 0u;
                            curCtrdStartPos = 0xFFu;
                        }
                    }
                    
                    i++; 
                    curPos = diplex[i];
                    if(i == count)
                    {
                        break;
                    }            
                }
                    
                    /* Find the biggest centroid if two are the same size, last one wins
                       We are in the end of current */
                if (curCntrdSize >= biggestCtrdSize) 
                {
                    biggestCtrdSize = curCntrdSize;
                    biggestCtrdStartPos = curCtrdStartPos;
                }
            }
            else
            {
                /* Without diplexing */ 
                biggestCtrdSize = count;
            }
                        

            /* Check centroid size */
            #if (CSD_IS_NON_DIPLEX_SLIDER)
                if((biggestCtrdSize >= 2u) || ((biggestCtrdSize == 1u) && (diplex == 0u)))
            #else                    
                if(biggestCtrdSize >= 2u)
            #endif /* (CSD_IS_NON_DIPLEX_SLIDER) */
                {
                    for (i = biggestCtrdStartPos; i < (biggestCtrdStartPos + biggestCtrdSize); i++)
                    {
                        #if (CSD_IS_DIPLEX_SLIDER && CSD_IS_NON_DIPLEX_SLIDER)
                            if (diplex == 0u)
                            {
                                curPos = i;
                            }
                            else
                            {
                                curPos = diplex[i];
                            }                    
                        #elif (CSD_IS_DIPLEX_SLIDER)                    
                            curPos = diplex[i];                    
                        #endif /* (CSD_IS_DIPLEX_SLIDER && CSD_IS_NON_DIPLEX_SLIDER) */
                        /* Looking for the grater element within centroid */
                        if(startOfSlider[curPos] > fingerThreshold)
                        {
                            if(startOfSlider[curPos] > temp)
                            {
                                maximum = i;
                                temp = startOfSlider[curPos];
                            }
                        }
                    }
                } 
        #else
            for (i = 0u; i < count; i++)
            {                      
                /* Looking for the grater element within centroid */
                if(startOfSlider[i] > fingerThreshold)
                {
                    if(startOfSlider[i] > temp)
                    {
                        maximum = i;
                        temp = startOfSlider[i];
                    }
                }
            }    
        #endif /* (CSD_IS_DIPLEX_SLIDER) */
        return (maximum);
    }
    
    
    /*******************************************************************************
    * Function Name: CSD_CalcCentroid
    ********************************************************************************
    *
    * Summary:
    *  Returns position value calculated accoring index of maximum element and API
    *  resolution.
    *
    * Parameters:
    *  type:  widget type.
    *  diplex:  pointer to diplex table.
    *  maximum:  Index of maximum element within centroid.
    *  offset:   Start index of cetroid in CSD_SensorSignal[] array.
    *  count:    Number of elements within centroid.
    *  resolution:  multiplicator calculated according to centroid type and
    *  API resolution.
    *  noiseThreshold:  Noise threshould.
    * 
    * Return:
    *  Returns position value of the slider.
    * 
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 CSD_CalcCentroid(uint8 maximum, uint8 offset, 
                                        uint8 count, uint16 resolution, uint8 noiseThreshold)
	                                    
    {
        #if ((CSD_TOTAL_LINEAR_SLIDERS_COUNT > 0u) || (CSD_TOTAL_TOUCH_PADS_COUNT > 0u))
            uint8 posPrev = 0u;
            uint8 posNext = 0u;
        #endif /* ((CSD_TOTAL_LINEAR_SLIDERS_COUNT>0u) || (CSD_TOTAL_TOUCH_PADS_COUNT>0u)) */
               
        #if (CSD_IS_DIPLEX_SLIDER)                
            uint8 pos;
        #endif /* (CSD_IS_DIPLEX_SLIDER) */
        
        /* Helps while centroid calulation */
        static uint8 CSD_centroid[3u];

        uint8 position;
        uint32 numerator;
        int32 denominator;
        uint8 *startOfSlider = &CSD_sensorSignal[offset];
                    
        #if (CSD_ADD_SLIDER_TYPE)
            if(type == CSD_TYPE_RADIAL_SLIDER)
            {
        #endif /* (CSD_ADD_SLIDER_TYPE) */

            #if (CSD_TOTAL_RADIAL_SLIDERS_COUNT > 0u)                
                /* Copy Signal for found centriod */
                CSD_centroid[CSD_POS] = startOfSlider[maximum];
                 
                /* Check borders for ROTARY Slider */
                if (maximum == 0u)                   /* Start of centroid */
                { 
                    CSD_centroid[CSD_POS_PREV] = startOfSlider[count - 1u];
                    CSD_centroid[CSD_POS_NEXT] = startOfSlider[1u];
                }
                else if (maximum == (count - 1u))    /* End of centroid */
                {
                    CSD_centroid[CSD_POS_PREV] = startOfSlider[maximum - 1u];
                    CSD_centroid[CSD_POS_NEXT] = startOfSlider[0u];
                }
                else                                /* Not first Not last */
                {
                    CSD_centroid[CSD_POS_PREV] = startOfSlider[maximum - 1u];
                    CSD_centroid[CSD_POS_NEXT] = startOfSlider[maximum + 1u];
                }
            #endif /* (CSD_TOTAL_RADIAL_SLIDERS_COUNT > 0u) */

        #if (CSD_ADD_SLIDER_TYPE)
            }
            else
            {
        #endif /* (CSD_ADD_SLIDER_TYPE) */

            #if ((CSD_TOTAL_LINEAR_SLIDERS_COUNT > 0u) || (CSD_TOTAL_TOUCH_PADS_COUNT > 0u))
                #if (CSD_IS_DIPLEX_SLIDER && CSD_IS_NON_DIPLEX_SLIDER)                    
                    /* Calculate next and previous near to maximum */
                    if(diplex == 0u)
                    {
                        pos     = maximum;
                        posPrev = maximum - 1u;
                        posNext = maximum + 1u; 
                    }
                    else
                    {
                        pos     = diplex[maximum];
                        posPrev = diplex[maximum - 1u];
                        posNext = diplex[maximum + 1u];
                        count <<= 1u;
                    }                    
                #elif (CSD_IS_DIPLEX_SLIDER)
                    /* Calculate next and previous near to maximum */
                    pos     = diplex[maximum];
                    posPrev = diplex[maximum - 1u];
                    posNext = diplex[maximum + 1u];
                    count <<= 1u;                    
                #else                    
                    /* Calculate next and previous near to maximum */
                    if (maximum >= 1u)
                    {
                        posPrev = maximum - 1u;
                    }
                    posNext = maximum + 1u; 
                #endif /* (CSD_IS_DIPLEX_SLIDER && CSD_IS_NON_DIPLEX_SLIDER) */
                        
                /* Copy Signal for found centriod */
                #if (CSD_IS_DIPLEX_SLIDER)
                    CSD_centroid[CSD_POS] = startOfSlider[pos];
                #else
                    CSD_centroid[CSD_POS] = startOfSlider[maximum];
                #endif /* (CSD_IS_DIPLEX_SLIDER) */
                    
                /* Check borders for LINEAR Slider */
                if (maximum == 0u)                   /* Start of centroid */
                { 
                    CSD_centroid[CSD_POS_PREV] = 0u;
                    CSD_centroid[CSD_POS_NEXT] = startOfSlider[posNext];
                }
                else if (maximum == ((count) - 1u)) /* End of centroid */
                {
                    CSD_centroid[CSD_POS_PREV] = startOfSlider[posPrev];
                    CSD_centroid[CSD_POS_NEXT] = 0u;
                }
                else                                /* Not first Not last */
                {
                    CSD_centroid[CSD_POS_PREV] = startOfSlider[posPrev];
                    CSD_centroid[CSD_POS_NEXT] = startOfSlider[posNext];
                }
            #endif /* ((CSD_TOTAL_LINEAR_SLIDERS_COUNT>0u)||(CSD_TOTAL_TOUCH_PADS_COUNT>0u))*/

        #if (CSD_ADD_SLIDER_TYPE)
            }
        #endif /* (CSD_ADD_SLIDER_TYPE) */
    
        /* Subtract noiseThreshold */
        if(CSD_centroid[CSD_POS_PREV] > noiseThreshold)
        {
            CSD_centroid[CSD_POS_PREV] -= noiseThreshold;
        }
        else
        {
            CSD_centroid[CSD_POS_PREV] = 0u;
        }
        
        /* Maximum always grater than fingerThreshold, so grate than noiseThreshold */
        CSD_centroid[CSD_POS] -= noiseThreshold;
        
        /* Subtract noiseThreshold */
        if(CSD_centroid[CSD_POS_NEXT] > noiseThreshold)
        {
            CSD_centroid[CSD_POS_NEXT] -= noiseThreshold;
        }
        else
        {
            CSD_centroid[CSD_POS_NEXT] = 0u;
        }
        
        
        /* Si+1 - Si-1 */
        numerator = (uint32) CSD_centroid[CSD_POS_NEXT] -
                    (uint32) CSD_centroid[CSD_POS_PREV];

        /* Si+1 + Si + Si-1 */
        denominator = (int32) CSD_centroid[CSD_POS_PREV] + 
                      (int32) CSD_centroid[CSD_POS] + 
                      (int32) CSD_centroid[CSD_POS_NEXT];
        
        /* (numerator/denominator) + maximum */
        denominator = (((int32)(uint32)((uint32)numerator << 8u)/denominator) + (int32)(uint32)((uint32) maximum << 8u));

        #if(CSD_TOTAL_RADIAL_SLIDERS_COUNT > 0u)
            /* Only required for RADIAL Slider */
            if(denominator < 0)
            {
                denominator += (int32)((uint16)((uint16) count << 8u));
            }
        #endif /* (CSD_TOTAL_RADIAL_SLIDERS_COUNT > 0u) */

        denominator *= (int16)resolution;

        /* Round the relust and put it to uint8 */
        position = ((uint8) HI16((uint32)denominator + CSD_CENTROID_ROUND_VALUE));

        return (position);
    }    
#endif /* (CSD_TOTAL_CENTROIDS_COUNT) */


#if(CSD_TOTAL_LINEAR_SLIDERS_COUNT > 0u)
    /*******************************************************************************
    * Function Name: CSD_GetCentroidPos
    ********************************************************************************
    *
    * Summary:
    *  Checks the CSD_Signal[ ] array for a centroid within
    *  slider specified range. The centroid position is calculated to the resolution
    *  specified in the CapSense customizer. The position filters are applied to the
    *  result if enabled.
    *
    * Parameters:
    *  widget:  Widget number.
    *  For every linear slider widget there are defines in this format:
    *  #define CSD_LS__"widget_name"            5
    * 
    * Return:
    *  Returns position value of the linear slider.
    *
    * Side Effects:
    *  If any sensor within the slider widget is active, the function returns values
    *  from zero to the API resolution value set in the CapSense customizer. If no
    *  sensors are active, the function returns 0xFFFF. If an error occurs during
    *  execution of the centroid/diplexing algorithm, the function returns 0xFFFF.
    *  There are no checks of widget type argument provided to this function.
    *  The unproper widget type provided will cause unexpected position calculations.
    *
    * Note:
    *  If noise counts on the slider segments are greater than the noise
    *  threshold, this subroutine may generate a false centroid result. The noise
    *  threshold should be set carefully (high enough above the noise level) so
    *  that noise will not generate a false centroid.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint16 CSD_GetCentroidPos(uint8 widget) 
    {
        #if (CSD_IS_DIPLEX_SLIDER)
            const uint8 CYCODE *diplex;
        #endif /* (CSD_IS_DIPLEX_SLIDER) */
                
        #if (0u != CSD_LINEAR_SLIDERS_POS_FILTERS_MASK)
            uint8 posIndex;
            uint8 firstTimeIndex = CSD_posFiltersData[widget];
            uint8 posFiltersMask = CSD_posFiltersMask[widget];  
        #endif /* (0u != CSD_LINEAR_SLIDERS_POS_FILTERS_MASK) */

        #if ((0u != (CSD_MEDIAN_FILTER & CSD_LINEAR_SLIDERS_POS_FILTERS_MASK)) || \
             (0u != (CSD_AVERAGING_FILTER & CSD_LINEAR_SLIDERS_POS_FILTERS_MASK)))
            uint8 tempPos;
        #endif /* ((0u != (CSD_MEDIAN_FILTER & CSD_LINEAR_SLIDERS_POS_FILTERS_MASK)) || \
               *   (0u != (CSD_AVERAGING_FILTER & CSD_LINEAR_SLIDERS_POS_FILTERS_MASK)))
               */

        uint8 maximum;
        uint16 position;
        uint8 offset = CSD_rawDataIndex[widget];
        uint8 count = CSD_numberOfSensors[widget];
                        
        #if (CSD_IS_DIPLEX_SLIDER)
            if(widget < CSD_TOTAL_DIPLEXED_SLIDERS_COUNT)
            {
                maximum = CSD_diplexTable[widget];
                diplex = &CSD_diplexTable[maximum];
            }
            else
            {
                diplex = 0u;
            }
        #endif /* (CSD_IS_DIPLEX_SLIDER) */

        /* Find Maximum within centroid */      
        #if (CSD_IS_DIPLEX_SLIDER)        
            maximum = CSD_FindMaximum(offset, count, CSD_fingerThreshold[widget], diplex);
        #else
            maximum = CSD_FindMaximum(offset, count, CSD_fingerThreshold[widget]);
        #endif /* (CSD_IS_DIPLEX_SLIDER) */

        if (maximum != 0xFFu)
        {
            /* Calculate centroid */
            position = (uint16) CSD_CalcCentroid(maximum, 
                         offset, count, CSD_centroidMult[widget], CSD_noiseThreshold[widget]);

            #if (0u != CSD_LINEAR_SLIDERS_POS_FILTERS_MASK)
                /* Check if this linear slider has enabled filters */
                if (0u != (posFiltersMask & CSD_ANY_POS_FILTER))
                {
                    /* Caluclate position to store filters data */
                    posIndex  = firstTimeIndex + 1u;
                    
                    if (0u == CSD_posFiltersData[firstTimeIndex])
                    {
                        /* Init filters */
                        CSD_posFiltersData[posIndex] = (uint8) position;
                        #if ((0u != (CSD_MEDIAN_FILTER & \
                                     CSD_LINEAR_SLIDERS_POS_FILTERS_MASK)) || \
                             (0u != (CSD_AVERAGING_FILTER & \
                                     CSD_LINEAR_SLIDERS_POS_FILTERS_MASK)))

                            if ( (0u != (posFiltersMask & CSD_MEDIAN_FILTER)) || 
                                 (0u != (posFiltersMask & CSD_AVERAGING_FILTER)) )
                            {
                                CSD_posFiltersData[posIndex + 1u] = (uint8) position;
                            }
                        #endif /* ((0u != (CSD_MEDIAN_FILTER & \
                               *           CSD_LINEAR_SLIDERS_POS_FILTERS_MASK)) || \
                               *   (0u != (CSD_AVERAGING_FILTER & \
                               *           CSD_LINEAR_SLIDERS_POS_FILTERS_MASK)))
                               */
                        
                        CSD_posFiltersData[firstTimeIndex] = 1u;
                    }
                    else
                    {
                        /* Do filtering */
                        #if (0u != (CSD_MEDIAN_FILTER & CSD_LINEAR_SLIDERS_POS_FILTERS_MASK))
                            if (0u != (posFiltersMask & CSD_MEDIAN_FILTER))
                            {
                                tempPos = (uint8) position;
                                position = CSD_MedianFilter(position,
                                                                (uint16)CSD_posFiltersData[posIndex],
                                                                (uint16)CSD_posFiltersData[posIndex + 1u]);
                                CSD_posFiltersData[posIndex + 1u] =
                                                                             CSD_posFiltersData[posIndex];
                                CSD_posFiltersData[posIndex] = tempPos;
                            }
                        #endif /*(0u != (CSD_MEDIAN_FILTER &
                               *         CSD_LINEAR_SLIDERS_POS_FILTERS_MASK))
                               */

                        #if(0u!=(CSD_AVERAGING_FILTER & CSD_LINEAR_SLIDERS_POS_FILTERS_MASK))
                            if (0u != (posFiltersMask & CSD_AVERAGING_FILTER)) 
                            {
                                tempPos = (uint8) position;
                                position = CSD_AveragingFilter(position,
                                                                (uint16)CSD_posFiltersData[posIndex],
                                                                (uint16)CSD_posFiltersData[posIndex + 1u]);
                                CSD_posFiltersData[posIndex+1u]=CSD_posFiltersData[posIndex];
                                CSD_posFiltersData[posIndex] = tempPos;
                            }
                        #endif /* (0u != (CSD_AVERAGING_FILTER & \
                               *           CSD_LINEAR_SLIDERS_POS_FILTERS_MASK))
                               */

                        #if (0u != (CSD_IIR2_FILTER & CSD_LINEAR_SLIDERS_POS_FILTERS_MASK)) 
                            if (0u != (posFiltersMask & CSD_IIR2_FILTER)) 
                            {
                                position = CSD_IIR2Filter(position,
                                                                    (uint16)CSD_posFiltersData[posIndex]);
                                CSD_posFiltersData[posIndex] = (uint8) position;
                            }
                        #endif /* (0u != (CSD_IIR2_FILTER & \
                               *          CSD_LINEAR_SLIDERS_POS_FILTERS_MASK))
                               */

                        #if (0u != (CSD_IIR4_FILTER & CSD_LINEAR_SLIDERS_POS_FILTERS_MASK))
                            if (0u != (posFiltersMask & CSD_IIR4_FILTER))
                            {
                                position = CSD_IIR4Filter(position,
                                                                    (uint16)CSD_posFiltersData[posIndex]);
                                CSD_posFiltersData[posIndex] = (uint8) position;
                            }                                
                        #endif /* (0u != (CSD_IIR4_FILTER & \
                               *          CSD_LINEAR_SLIDERS_POS_FILTERS_MASK))
                               */

                        #if (0u != (CSD_JITTER_FILTER & CSD_LINEAR_SLIDERS_POS_FILTERS_MASK))
                            if (0u != (posFiltersMask & CSD_JITTER_FILTER))
                            {
                                position = CSD_JitterFilter(position,
                                                                    (uint16)CSD_posFiltersData[posIndex]);
                                CSD_posFiltersData[posIndex] = (uint8) position;
                            }
                        #endif /* (0u != (CSD_JITTER_FILTER & \
                               *           CSD_LINEAR_SLIDERS_POS_FILTERS_MASK))
                               */
                    }
                }
            #endif /* (0u != CSD_LINEAR_SLIDERS_POS_FILTERS_MASK) */

        }
        else
        {
            /* The maximum wasn't found */
            position = 0xFFFFu;

            #if(0u != CSD_LINEAR_SLIDERS_POS_FILTERS_MASK)
                /* Reset the filters */
                if(0u != (posFiltersMask & CSD_ANY_POS_FILTER))
                {
                    CSD_posFiltersData[firstTimeIndex] = 0u;
                }
            #endif /* (0u != CSD_LINEAR_SLIDERS_POS_FILTERS_MASK) */
        }

        
        return (position);
    }
#endif /* (CSD_TOTAL_LINEAR_SLIDERS_COUNT > 0u) */


#if(CSD_TOTAL_RADIAL_SLIDERS_COUNT > 0u)
    /*******************************************************************************
    * Function Name: CSD_GetRadialCentroidPos
    ********************************************************************************
    *
    * Summary:
    *  Checks the CSD_Signal[ ] array for a centroid within
    *  slider specified range. The centroid position is calculated to the resolution
    *  specified in the CapSense customizer. The position filters are applied to the
    *  result if enabled.
    *
    * Parameters:
    *  widget:  Widget number.
    *  For every radial slider widget there are defines in this format:
    *  #define CSD_RS_"widget_name"            5
    * 
    * Return:
    *  Returns position value of the radial slider.
    *
    * Side Effects:
    *  If any sensor within the slider widget is active, the function returns values
    *  from zero to the API resolution value set in the CapSense customizer. If no
    *  sensors are active, the function returns 0xFFFF.
    *  There are no checks of widget type argument provided to this function.
    *  The unproper widget type provided will cause unexpected position calculations.
    *
    * Note:
    *  If noise counts on the slider segments are greater than the noise
    *  threshold, this subroutine may generate a false centroid result. The noise
    *  threshold should be set carefully (high enough above the noise level) so 
    *  that noise will not generate a false centroid.
    *
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
     uint16 CSD_GetRadialCentroidPos(uint8 widget)
	                                       
    {
        #if (0u != CSD_RADIAL_SLIDERS_POS_FILTERS_MASK)
            uint8 posIndex;
            uint8 firstTimeIndex = CSD_posFiltersData[widget];
            uint8 posFiltersMask = CSD_posFiltersMask[widget]; 
        #endif /* (0u != CSD_RADIAL_SLIDERS_POS_FILTERS_MASK) */

        #if ((0u != (CSD_MEDIAN_FILTER & CSD_RADIAL_SLIDERS_POS_FILTERS_MASK)) || \
             (0u != (CSD_AVERAGING_FILTER & CSD_RADIAL_SLIDERS_POS_FILTERS_MASK)))
            uint8 tempPos;
        #endif /* ((0u != (CSD_MEDIAN_FILTER & CSD_RADIAL_SLIDERS_POS_FILTERS_MASK)) || \
               *   (0u != (CSD_AVERAGING_FILTER & CSD_RADIAL_SLIDERS_POS_FILTERS_MASK)))
               */

        uint8 maximum;
        uint16 position;
        uint8 offset = CSD_rawDataIndex[widget];
        uint8 count = CSD_numberOfSensors[widget];
        
        /* Find Maximum within centroid */        
        #if (CSD_IS_DIPLEX_SLIDER)
            maximum = CSD_FindMaximum(offset, count, CSD_fingerThreshold[widget], 0u);
        #else
            maximum = CSD_FindMaximum(offset, count, CSD_fingerThreshold[widget]);
        #endif /* (CSD_IS_DIPLEX_SLIDER) */
        
        if (maximum != 0xFFu)
        {
            /* Calculate centroid */
            position = (uint16) CSD_CalcCentroid(maximum, 
                         offset, count, CSD_centroidMult[widget], CSD_noiseThreshold[widget]);

            #if (0u != CSD_RADIAL_SLIDERS_POS_FILTERS_MASK)
                /* Check if this Radial slider has enabled filters */
                if (0u != (posFiltersMask & CSD_ANY_POS_FILTER))
                {
                    /* Caluclate position to store filters data */
                    posIndex  = firstTimeIndex + 1u;
                    
                    if (0u == CSD_posFiltersData[firstTimeIndex])
                    {
                        /* Init filters */
                        CSD_posFiltersData[posIndex] = (uint8) position;
                        #if ((0u != (CSD_MEDIAN_FILTER & \
                                     CSD_RADIAL_SLIDERS_POS_FILTERS_MASK)) || \
                             (0u != (CSD_AVERAGING_FILTER & \
                                     CSD_RADIAL_SLIDERS_POS_FILTERS_MASK)))

                            if ( (0u != (posFiltersMask & CSD_MEDIAN_FILTER))  || 
                                 (0u != (posFiltersMask & CSD_AVERAGING_FILTER)) )
                            {
                                CSD_posFiltersData[posIndex + 1u] = (uint8) position;
                            }
                        #endif /* ((0u != (CSD_MEDIAN_FILTER & \
                               *           CSD_RADIAL_SLIDERS_POS_FILTERS_MASK)) || \
                               *   (0u != (CSD_AVERAGING_FILTER & \
                               *           CSD_RADIAL_SLIDERS_POS_FILTERS_MASK)))
                               */
                        
                        CSD_posFiltersData[firstTimeIndex] = 1u;
                    }
                    else
                    {
                        /* Do filtering */
                        #if (0u != (CSD_MEDIAN_FILTER & CSD_RADIAL_SLIDERS_POS_FILTERS_MASK))
                            if (0u != (posFiltersMask & CSD_MEDIAN_FILTER))
                            {
                                tempPos = (uint8) position;
                                position = CSD_MedianFilter(position,
                                                                        CSD_posFiltersData[posIndex],
                                                                        CSD_posFiltersData[posIndex + 1u]);
                                CSD_posFiltersData[posIndex + 1u] = 
                                                                              CSD_posFiltersData[posIndex];
                                CSD_posFiltersData[posIndex] = tempPos;
                            }
                        #endif /* (0u != (CSD_MEDIAN_FILTER & 
                               *          CSD_RADIAL_SLIDERS_POS_FILTERS_MASK))
                               */

                        #if (0u != (CSD_AVERAGING_FILTER & \
                                    CSD_RADIAL_SLIDERS_POS_FILTERS_MASK))
                            if (0u != (posFiltersMask & CSD_AVERAGING_FILTER))
                            {
                                tempPos = (uint8) position;
                                position = CSD_AveragingFilter(position, 
                                                                       CSD_posFiltersData[posIndex],
                                                                       CSD_posFiltersData[posIndex + 1u]);
                                CSD_posFiltersData[posIndex+1u]= CSD_posFiltersData[posIndex];
                                CSD_posFiltersData[posIndex] = tempPos;
                            }
                        #endif /* (0u != (CSD_AVERAGING_FILTER & \
                               *          CSD_RADIAL_SLIDERS_POS_FILTERS_MASK))
                               */

                        #if (0u != (CSD_IIR2_FILTER & CSD_RADIAL_SLIDERS_POS_FILTERS_MASK))
                            if (0u != (posFiltersMask & CSD_IIR2_FILTER))
                            {
                                position = CSD_IIR2Filter(position,
                                                                    (uint16)CSD_posFiltersData[posIndex]);
                                CSD_posFiltersData[posIndex] = (uint8) position;
                            }
                        #endif /* (0u != (CSD_IIR2_FILTER & 
                               *          CSD_RADIAL_SLIDERS_POS_FILTERS_MASK))
                               */

                        #if (0u != (CSD_IIR4_FILTER & CSD_RADIAL_SLIDERS_POS_FILTERS_MASK))
                            if (0u != (posFiltersMask & CSD_IIR4_FILTER))
                            {
                                position = CSD_IIR4Filter(position,
                                                                    (uint16)CSD_posFiltersData[posIndex]);
                                CSD_posFiltersData[posIndex] = (uint8) position;
                            }
                        #endif /* (0u != (CSD_IIR4_FILTER & 
                               *          CSD_RADIAL_SLIDERS_POS_FILTERS_MASK))
                               */

                        #if (0u != (CSD_JITTER_FILTER & CSD_RADIAL_SLIDERS_POS_FILTERS_MASK))
                            if (0u != (posFiltersMask & CSD_JITTER_FILTER))
                            {
                                position = CSD_JitterFilter(position, 
                                                                         CSD_posFiltersData[posIndex]);
                                CSD_posFiltersData[posIndex] = (uint8) position;
                            }
                        #endif /* (0u != (CSD_JITTER_FILTER &
                               *           CSD_RADIAL_SLIDERS_POS_FILTERS_MASK))
                               */
                    }
                }
            #endif /* (0u != CSD_RADIAL_SLIDERS_POS_FILTERS_MASK) */

        }
        else
        {
            /* The maximum didn't find */
            position = 0xFFFFu;

            #if (0u != CSD_RADIAL_SLIDERS_POS_FILTERS_MASK)
                /* Reset the filters */
                if((posFiltersMask & CSD_ANY_POS_FILTER) != 0u)
                {
                    CSD_posFiltersData[firstTimeIndex] = 0u;
                }
            #endif /* (0u != CSD_RADIAL_SLIDERS_POS_FILTERS_MASK) */
        }
        
        return (position);
    }
#endif


#if(CSD_TOTAL_TOUCH_PADS_COUNT > 0u)
    /*******************************************************************************
    * Function Name: CSD_GetTouchCentroidPos
    ********************************************************************************
    *
    * Summary:
    *  If a finger is present on touch pad, this function calculates the X and Y
    *  position of the finger by calculating the centroids within touch pad specified
    *  range. The X and Y positions are calculated to the API resolutions set in the
    *  CapSense customizer. Returns a 1 if a finger is on the touchpad.
    *  The position filter is applied to the result if enabled.
    *  This function is available only if a touch pad is defined by the CapSense
    *  customizer.
    *
    * Parameters:
    *  widget:  Widget number. 
    *  For every touchpad widget there are defines in this format:
    *  #define CSD_TP_"widget_name"            5
    *
    *  pos:     Pointer to the array of two uint16 elements, where result
    *  result of calculation of X and Y position are stored.
    *  pos[0u]  - position of X
    *  pos[1u]  - position of Y
    *
    * Return:
    *  Returns a 1 if a finger is on the touch pad, 0 - if not.
    *
    * Side Effects:
    *   There are no checks of widget type argument provided to this function.
    *   The unproper widget type provided will cause unexpected position
    *   calculations.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 CSD_GetTouchCentroidPos(uint8 widget, uint16* pos)
	                                    
    {
        #if (0u != CSD_TOUCH_PADS_POS_FILTERS_MASK)
            uint8 posXIndex;
            uint8 posYIndex;
            uint8 firstTimeIndex = CSD_posFiltersData[widget];
            uint8 posFiltersMask = CSD_posFiltersMask[widget];
        #endif /* (0u != CSD_TOUCH_PADS_POS_FILTERS_MASK) */

        #if ((0u != (CSD_MEDIAN_FILTER & CSD_TOUCH_PADS_POS_FILTERS_MASK)) || \
             (0u != (CSD_AVERAGING_FILTER & CSD_TOUCH_PADS_POS_FILTERS_MASK)))
            uint16 tempPos;
        #endif /* ((0u != (CSD_MEDIAN_FILTER & CSD_TOUCH_PADS_POS_FILTERS_MASK)) || \
               *   (0u != (CSD_AVERAGING_FILTER & CSD_TOUCH_PADS_POS_FILTERS_MASK)))
               */

        uint8 maxX;
        uint8 maxY;
        uint8 posX;
        uint8 posY;
        uint8 touch = 0u;
        uint8 offset = CSD_rawDataIndex[widget];
        uint8 count = CSD_numberOfSensors[widget];
        
        /* Find Maximum within X centroid */
        #if (CSD_IS_DIPLEX_SLIDER)
            maxX = CSD_FindMaximum(offset, count, CSD_fingerThreshold[widget], 0u);
        #else
            maxX = CSD_FindMaximum(offset, count, CSD_fingerThreshold[widget]);
        #endif /* (CSD_IS_DIPLEX_SLIDER) */

        if (maxX != 0xFFu)
        {
            offset = CSD_rawDataIndex[widget + 1u];
            count = CSD_numberOfSensors[widget + 1u];

            /* Find Maximum within Y centroid */
            #if (CSD_IS_DIPLEX_SLIDER)
                maxY = CSD_FindMaximum(offset, count, CSD_fingerThreshold[widget + 1u], 0u);
            #else
                maxY = CSD_FindMaximum(offset, count, CSD_fingerThreshold[widget + 1u]);
            #endif /* (CSD_IS_DIPLEX_SLIDER) */

            if (maxY != 0xFFu)
            {
                /* X and Y maximums are found = true touch */
                touch = 1u;
                
                /* Calculate Y centroid */
                posY = CSD_CalcCentroid(maxY, offset, count, 
                            CSD_centroidMult[widget + 1u], CSD_noiseThreshold[widget + 1u]);
                
                /* Calculate X centroid */
                offset = CSD_rawDataIndex[widget];
                count = CSD_numberOfSensors[widget];
                
                posX = CSD_CalcCentroid(maxX, offset, count, 
                            CSD_centroidMult[widget],CSD_noiseThreshold[widget]);
    
                #if (0u != CSD_TOUCH_PADS_POS_FILTERS_MASK)
                    /* Check if this TP has enabled filters */
                    if (0u != (posFiltersMask & CSD_ANY_POS_FILTER))
                    {
                        /* Caluclate position to store filters data */
                        posXIndex  = firstTimeIndex + 1u;
                        posYIndex  = CSD_posFiltersData[widget + 1u];
                        
                        if (0u == CSD_posFiltersData[firstTimeIndex])
                        {
                            /* Init filters */
                            CSD_posFiltersData[posXIndex] = posX;
                            CSD_posFiltersData[posYIndex] = posY;

                            #if((0u != (CSD_MEDIAN_FILTER & \
                                        CSD_TOUCH_PADS_POS_FILTERS_MASK))|| \
                                (0u != (CSD_AVERAGING_FILTER & \
                                        CSD_TOUCH_PADS_POS_FILTERS_MASK)))

                                if ( (0u != (posFiltersMask & CSD_MEDIAN_FILTER)) || 
                                     (0u != (posFiltersMask & CSD_AVERAGING_FILTER)) )
                                {
                                    CSD_posFiltersData[posXIndex + 1u] = posX;
                                    CSD_posFiltersData[posYIndex + 1u] = posY;
                                }
                            #endif /* ((0u != (CSD_MEDIAN_FILTER & \
                                   *           CSD_TOUCH_PADS_POS_FILTERS_MASK)) || \
                                   *    (0u != (CSD_AVERAGING_FILTER & \
                                   *            CSD_TOUCH_PADS_POS_FILTERS_MASK)))
                                   */
                            
                            CSD_posFiltersData[firstTimeIndex] = 1u;
                        }
                        else
                        {
                            /* Do filtering */
                            #if (0u != (CSD_MEDIAN_FILTER & CSD_TOUCH_PADS_POS_FILTERS_MASK))
                                if (0u != (posFiltersMask & CSD_MEDIAN_FILTER))
                                {
                                    tempPos = posX;
                                    posX = (uint8) CSD_MedianFilter(posX,
                                                                      CSD_posFiltersData[posXIndex],
                                                                      CSD_posFiltersData[posXIndex + 1u]);
                                    CSD_posFiltersData[posXIndex + 1u] = 
                                                                             CSD_posFiltersData[posXIndex];
                                    CSD_posFiltersData[posXIndex] = tempPos;
                                    
                                    tempPos = posY;
                                    posY = (uint8) CSD_MedianFilter(posY,
                                                                       CSD_posFiltersData[posYIndex], 
                                                                       CSD_posFiltersData[posYIndex + 1u]);
                                    CSD_posFiltersData[posYIndex + 1u] = 
                                                                             CSD_posFiltersData[posYIndex];
                                    CSD_posFiltersData[posYIndex] = tempPos;
                                }
                                
                            #endif /* (0u != (CSD_MEDIAN_FILTER & \
                                   *          CSD_TOUCH_PADS_POS_FILTERS_MASK))
                                   */

                            #if(0u !=(CSD_AVERAGING_FILTER & CSD_TOUCH_PADS_POS_FILTERS_MASK))
                                if (0u != (posFiltersMask & CSD_AVERAGING_FILTER))
                                {
                                    tempPos = posX;
                                    posX = (uint8) CSD_AveragingFilter(posX,
                                                                       CSD_posFiltersData[posXIndex], 
                                                                       CSD_posFiltersData[posXIndex + 1u]);
                                    CSD_posFiltersData[posXIndex + 1u] = 
                                                                             CSD_posFiltersData[posXIndex];
                                    CSD_posFiltersData[posXIndex] = tempPos;
                                    
                                    tempPos = posY;
                                    posY = (uint8) CSD_AveragingFilter(posY, 
                                                                      CSD_posFiltersData[posYIndex], 
                                                                      CSD_posFiltersData[posYIndex + 1u]);
                                    CSD_posFiltersData[posYIndex + 1u] = 
                                                                            CSD_posFiltersData[posYIndex];
                                    CSD_posFiltersData[posYIndex] = tempPos;
                                }

                            #endif /* (0u != (CSD_AVERAGING_FILTER & \
                                   *           CSD_TOUCH_PADS_POS_FILTERS_MASK))
                                   */

                            #if (0u != (CSD_IIR2_FILTER & CSD_TOUCH_PADS_POS_FILTERS_MASK))
                                if (0u != (posFiltersMask & CSD_IIR2_FILTER))
                                {
                                    posX = (uint8) CSD_IIR2Filter(posX, 
                                                                           CSD_posFiltersData[posXIndex]);
                                    CSD_posFiltersData[posXIndex] = posX;
                                    
                                    posY = (uint8) CSD_IIR2Filter(posY, 
                                                                            CSD_posFiltersData[posYIndex]);
                                    CSD_posFiltersData[posYIndex] = posY;
                                }
                                
                            #endif /* (0u != (CSD_IIR2_FILTER & \
                                   *          CSD_TOUCH_PADS_POS_FILTERS_MASK))
                                   */

                            #if (0u != (CSD_IIR4_FILTER & CSD_TOUCH_PADS_POS_FILTERS_MASK))
                                if (0u != (posFiltersMask & CSD_IIR4_FILTER))
                                {
                                    posX = (uint8) CSD_IIR4Filter((uint16)posX,
                                                                    (uint16)CSD_posFiltersData[posXIndex]);
                                    CSD_posFiltersData[posXIndex] = posX;

                                    posY = (uint8) CSD_IIR4Filter((uint16)posY,
                                                                    (uint16)CSD_posFiltersData[posYIndex]);
                                    CSD_posFiltersData[posYIndex] = posY;
                                }
                                
                            #endif /* (0u != (CSD_IIR4_FILTER & \
                                   *           CSD_TOUCH_PADS_POS_FILTERS_MASK))
                                   */

                            #if (0u != (CSD_JITTER_FILTER & CSD_TOUCH_PADS_POS_FILTERS_MASK))
                                if (0u != (posFiltersMask & CSD_JITTER_FILTER))
                                    {
                                        posX = (uint8) CSD_JitterFilter(posX, 
                                                                            CSD_posFiltersData[posXIndex]);
                                        CSD_posFiltersData[posXIndex] = posX;
                                        
                                        posY = (uint8) CSD_JitterFilter(posY, 
                                                                            CSD_posFiltersData[posYIndex]);
                                        CSD_posFiltersData[posYIndex] = posY;
                                    }
                            #endif /* (0u != (CSD_JITTER_FILTER & \
                                   *           CSD_TOUCH_PADS_POS_FILTERS_MASK))
                                   */
                        }
                    }
                #endif /* (0u != CSD_TOUCH_PADS_POS_FILTERS_MASK) */

                /* Save positions */
                pos[0u] = posX;
                pos[1u] = posY;
            }
        }

        #if (0u != CSD_TOUCH_PADS_POS_FILTERS_MASK)
            if(touch == 0u)
            {
                /* Reset the filters */
                if ((posFiltersMask & CSD_ANY_POS_FILTER) != 0u)
                {
                    CSD_posFiltersData[firstTimeIndex] = 0u;
                }
            }
        #endif /* (0u != CSD_TOUCH_PADS_POS_FILTERS_MASK) */
        
        return (touch);
    }
#endif /* (CSD_TOTAL_TOUCH_PADS_COUNT > 0u) */


#if ( (CSD_RAW_FILTER_MASK & CSD_MEDIAN_FILTER) | \
      (CSD_POS_FILTERS_MASK & CSD_MEDIAN_FILTER) )
    /*******************************************************************************
    * Function Name: CSD_MedianFilter
    ********************************************************************************
    *
    * Summary:
    *  Median filter function. 
    *  The median filter looks at the three most recent samples and reports the 
    *  median value.
    *
    * Parameters:
    *  x1:  Current value.
    *  x2:  Previous value.
    *  x3:  Before previous value.
    *
    * Return:
    *  Returns filtered value.
    *
    *******************************************************************************/
    uint16 CSD_MedianFilter(uint16 x1, uint16 x2, uint16 x3)
                                         
    {
        uint16 tmp;
        
        if (x1 > x2)
        {
            tmp = x2;
            x2 = x1;
            x1 = tmp;
        }
        
        if (x2 > x3)
        {
            x2 = x3;
        }
        
        return ((x1 > x2) ? x1 : x2);
    }
#endif /* ( (CSD_RAW_FILTER_MASK & CSD_MEDIAN_FILTER) | \
       *    (CSD_POS_FILTERS_MASK & CSD_MEDIAN_FILTER) )
       */


#if ( (CSD_RAW_FILTER_MASK & CSD_AVERAGING_FILTER) | \
      (CSD_POS_FILTERS_MASK & CSD_AVERAGING_FILTER) )
    /*******************************************************************************
    * Function Name: CSD_AveragingFilter
    ********************************************************************************
    *
    * Summary:
    *  Averaging filter function.
    *  The averaging filter looks at the three most recent samples of position and
    *  reports the averaging value.
    *
    * Parameters:
    *  x1:  Current value.
    *  x2:  Previous value.
    *  x3:  Before previous value.
    *
    * Return:
    *  Returns filtered value.
    *
    *******************************************************************************/
    uint16 CSD_AveragingFilter(uint16 x1, uint16 x2, uint16 x3)
                                            
    {
        uint32 tmp = ((uint32)x1 + (uint32)x2 + (uint32)x3) / 3u;
        
        return ((uint16) tmp);
    }
#endif /* ( (CSD_RAW_FILTER_MASK & CSD_AVERAGING_FILTER) | \
       *    (CSD_POS_FILTERS_MASK & CSD_AVERAGING_FILTER) )
       */


#if ( (CSD_RAW_FILTER_MASK & CSD_IIR2_FILTER) | \
      (CSD_POS_FILTERS_MASK & CSD_IIR2_FILTER) )
    /*******************************************************************************
    * Function Name: CSD_IIR2Filter
    ********************************************************************************
    *
    * Summary:
    *  IIR1/2 filter function. IIR1/2 = 1/2current + 1/2previous.
    *
    * Parameters:
    *  x1:  Current value.
    *  x2:  Previous value.
    *
    * Return:
    *  Returns filtered value.
    *
    *******************************************************************************/
    uint16 CSD_IIR2Filter(uint16 x1, uint16 x2)
                                       
    {
        uint32 tmp;
        
        /* IIR = 1/2 Current Value+ 1/2 Previous Value */
        tmp = (uint32)x1 + (uint32)x2;
        tmp >>= 1u;
    
        return ((uint16) tmp);
    }
#endif /* ( (CSD_RAW_FILTER_MASK & CSD_IIR2_FILTER) | \
       *    (CSD_POS_FILTERS_MASK & CSD_IIR2_FILTER) )
       */


#if ( (CSD_RAW_FILTER_MASK & CSD_IIR4_FILTER) | \
      (CSD_POS_FILTERS_MASK & CSD_IIR4_FILTER) )
    /*******************************************************************************
    * Function Name: CSD_IIR4Filter
    ********************************************************************************
    *
    * Summary:
    *  IIR1/4 filter function. IIR1/4 = 1/4current + 3/4previous.
    *
    * Parameters:
    *  x1:  Current value.
    *  x2:  Previous value.
    *
    * Return:
    *  Returns filtered value.
    *
    *******************************************************************************/
    uint16 CSD_IIR4Filter(uint16 x1, uint16 x2)
                                       
    {
        uint32 tmp;
        
        /* IIR = 1/4 Current Value + 3/4 Previous Value */
        tmp = (uint32)x1 + (uint32)x2;
        tmp += ((uint32)x2 << 1u);
        tmp >>= 2u;
        
        return ((uint16) tmp);
    }
#endif /* ( (CSD_RAW_FILTER_MASK & CSD_IIR4_FILTER) | \
       *    (CSD_POS_FILTERS_MASK & CSD_IIR4_FILTER) )
       */


#if ( (CSD_RAW_FILTER_MASK & CSD_JITTER_FILTER) | \
      (CSD_POS_FILTERS_MASK & CSD_JITTER_FILTER) )
    /*******************************************************************************
    * Function Name: uint16 CSD_JitterFilter
    ********************************************************************************
    *
    * Summary:
    *  Jitter filter function.
    *
    * Parameters:
    *  x1:  Current value.
    *  x2:  Previous value.
    *
    * Return:
    *  Returns filtered value.
    *
    *******************************************************************************/
    uint16 CSD_JitterFilter(uint16 x1, uint16 x2)
                                         
    {
        if (x1 > x2)
        {
            x1--;
        }
        else
        {
            if (x1 < x2)
            {
                x1++;
            }
        }
    
        return x1;
    }
#endif /* ( (CSD_RAW_FILTER_MASK & CSD_JITTER_FILTER) | \
       *    (CSD_POS_FILTERS_MASK & CSD_JITTER_FILTER) )
       */


#if (CSD_RAW_FILTER_MASK & CSD_IIR8_FILTER)
    /*******************************************************************************
    * Function Name: CSD_IIR8Filter
    ********************************************************************************
    *
    * Summary:
    *  IIR1/8 filter function. IIR1/8 = 1/8current + 7/8previous.
    *  Only applies for raw data.
    *
    * Parameters:
    *  x1:  Current value.
    *  x2:  Previous value.
    *
    * Return:
    *  Returns filtered value.
    *
    *******************************************************************************/
    uint16 CSD_IIR8Filter(uint16 x1, uint16 x2)
                                       
    {
        uint32 tmp;
        
        /* IIR = 1/8 Current Value + 7/8 Previous Value */
        tmp = (uint32)x1;
        tmp += (((uint32)x2 << 3u) - ((uint32)x2));
        tmp >>= 3u;
    
        return ((uint16) tmp);
    }
#endif /* (CSD_RAW_FILTER_MASK & CSD_IIR8_FILTER) */


#if (CSD_RAW_FILTER_MASK & CSD_IIR16_FILTER)
    /*******************************************************************************
    * Function Name: CSD_IIR16Filter
    ********************************************************************************
    *
    * Summary:
    *  IIR1/16 filter function. IIR1/16 = 1/16current + 15/16previous.
    *  Only applies for raw data.
    *
    * Parameters:
    *  x1:  Current value.
    *  x2:  Previous value.
    *
    * Return:
    *  Returns filtered value.
    *
    *******************************************************************************/
    uint16 CSD_IIR16Filter(uint16 x1, uint16 x2)
                                        
    {
        uint32 tmp;
        
        /* IIR = 1/16 Current Value + 15/16 Previous Value */
        tmp = (uint32)x1;
        tmp += (((uint32)x2 << 4u) - ((uint32)x2));
        tmp >>= 4u;
        
        return ((uint16) tmp);
    }
#endif /* (CSD_RAW_FILTER_MASK & CSD_IIR16_FILTER) */


#if (CSD_TOTAL_MATRIX_BUTTONS_COUNT)

    /*******************************************************************************
    * Function Name: CSD_GetMatrixButtonPos
    ********************************************************************************
    *
    * Summary:
    *  Function calculates and returns touch position (column and row) for matrix
    *  button widget.
    *
    * Parameters:
    *  widget:  widget number;
    *  pos:     pointer to an array of two uint8, where touch postion will be 
    *           stored:
    *           pos[0] - column position;
    *           pos[1] - raw position.
    *
    * Return:
    *  Returns 1 if row and column sensors of matrix button are active, 0 - in other
    *  cases.
    *
    * Reentrant:
    *  No
    *
    *******************************************************************************/
    uint8 CSD_GetMatrixButtonPos(uint8 widget, uint8* pos)
	                                          
    {
        uint8 i;
        uint8 retVal = 0u;
        uint16 row_sig_max = 0u;
        uint16 col_sig_max = 0u;
        uint8 row_ind = 0u;
        uint8 col_ind = 0u;

        if (0u != CSD_CheckIsWidgetActive(widget))
        {
            /* Find row number with maximal signal value */
            for(i = CSD_rawDataIndex[widget]; i < (CSD_rawDataIndex[widget] + \
                 CSD_numberOfSensors[widget]); i++)
            {
                if (CSD_sensorSignal[i] > col_sig_max)
                {
                    col_ind = i;
                    col_sig_max = CSD_sensorSignal[i];
                }
            }

            /* Find row number with maximal signal value */
            for(i = CSD_rawDataIndex[widget+1u]; i < (CSD_rawDataIndex[widget+1u] + \
                 CSD_numberOfSensors[widget+1u]); i++)
            {
                if (CSD_sensorSignal[i] > row_sig_max)
                {
                    row_ind = i;
                    row_sig_max = CSD_sensorSignal[i];
                }
            }

            if((col_sig_max >= CSD_fingerThreshold[widget]) && \
               (row_sig_max >= CSD_fingerThreshold[widget+1u]))
            {
                pos[0u] = col_ind - CSD_rawDataIndex[widget];
                pos[1u] = row_ind - CSD_rawDataIndex[widget+1u];
                retVal = 1u;
            }
        }
        return (retVal);
    }

#endif /* (CSD_TOTAL_MATRIX_BUTTONS_COUNT) */

/* [] END OF FILE */
