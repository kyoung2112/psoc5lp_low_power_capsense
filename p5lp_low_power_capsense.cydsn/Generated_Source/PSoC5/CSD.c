/*******************************************************************************
* File Name: CSD.c
* Version 3.40
*
* Description:
*  This file provides the source code of scanning APIs for the CapSense CSD 
*  component.
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


static uint8 CSD_initVar = 0u;
            
/* Global software variables */
volatile uint8 CSD_csv;            /* CapSense CSD status, control variable */
volatile uint8 CSD_sensorIndex;    /* Index of scannig sensor */

/* AutoTunning start */
#if (CSD_TUNING_METHOD == CSD_AUTO_TUNING)
    uint8 CSD_lowLevelTuningDone = 1u;
#endif /* (CSD_TUNING_METHOD == CSD_AUTO_TUNING) */

#if (CSD_CURRENT_SOURCE == CSD_EXTERNAL_RB)
    uint8  CSD_extRbCh0Cur = CSD_RBLEED1;
    #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
        uint8  CSD_extRbCh1Cur = (CSD_RBLEED1 + CSD_TOTAL_RB_NUMBER__CH0);
    #endif /* (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)*/ 
#endif /* (CSD_CURRENT_SOURCE == CSD_EXTERNAL_RB) */ 
        
/* Global array of Raw Counts */
uint16 CSD_sensorRaw[CSD_TOTAL_SENSOR_COUNT]; 

uint8 CSD_sensorEnableMask[(((CSD_TOTAL_SENSOR_COUNT - 1u) / 8u) + 1u)] = {
0x3u, };

uint8 CYXDATA * const CYCODE CSD_pcTable[] = {
    (uint8 CYXDATA *)CSD_PortCH0__Button0__BTN__PC, 
    (uint8 CYXDATA *)CSD_PortCH1__Button1__BTN__PC, 
};

const uint8 CYCODE CSD_portTable[] = {
    CSD_PortCH0__Button0__BTN__PORT, 
    CSD_PortCH1__Button1__BTN__PORT, 
};

const uint8 CYCODE CSD_maskTable[] = {
    CSD_PortCH0__Button0__BTN__MASK,
    CSD_PortCH1__Button1__BTN__MASK,
};

uint8 CYXDATA * const CYCODE CSD_csTable[] = {
    (uint8 CYXDATA *)CYREG_PRT0_CAPS_SEL, (uint8 CYXDATA *)CYREG_PRT1_CAPS_SEL, (uint8 CYXDATA *)CYREG_PRT2_CAPS_SEL,
    (uint8 CYXDATA *)CYREG_PRT3_CAPS_SEL, (uint8 CYXDATA *)CYREG_PRT4_CAPS_SEL, (uint8 CYXDATA *)CYREG_PRT5_CAPS_SEL,
    (uint8 CYXDATA *)CYREG_PRT6_CAPS_SEL, (uint8 CYXDATA *)CYREG_PRT15_CAPS_SEL,
};

uint8 CSD_idacSettings[] = {
    200u,200u,
};

uint8 CSD_widgetResolution[] = {
    CSD_PWM_RESOLUTION_10_BITS,
    CSD_PWM_RESOLUTION_10_BITS,
};

uint8 CSD_analogSwitchDivider[CSD_TOTAL_SCANSLOT_COUNT];
const uint8 CYCODE CSD_widgetNumber[] = {
    0u, /* Button0__BTN */
    1u, /* Button1__BTN */
    
};




/*******************************************************************************
* Function Name: CSD_Init
********************************************************************************
*
* Summary:
*  Inits default CapSense configuration provided with customizer that defines 
*  mode of component operations and resets all sensors to an inactive state.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void CSD_Init(void) 
{
    #if ( (CSD_PRS_OPTIONS) || \
          (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_UDB) || \
          ( (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) && \
            (CSD_IMPLEMENTATION_CH1 == CSD_MEASURE_IMPLEMENTATION_UDB)) )
        
        uint8 enableInterrupts;
    #endif /* ( (CSD_PRS_OPTIONS) || \
           * (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_UDB) || \
           * ( (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) && \
           * (CSD_IMPLEMENTATION_CH1 == CSD_MEASURE_IMPLEMENTATION_UDB)) ) 
           */
    
    /* Clear all sensors */
    CSD_ClearSensors();

    /* Set Prescaler */
    #if (CSD_PRESCALER_OPTIONS == CSD_PRESCALER_UDB)
        /* Do nothing = config without prescaler */
    #elif (CSD_PRESCALER_OPTIONS == CSD_PRESCALER_FF)
        CSD_PRESCALER_CONTROL_REG   = (CSD_PRESCALER_CTRL_ENABLE |
                                                    CSD_PRESCALER_CTRL_MODE_CMP);
                                               
        CSD_PRESCALER_CONTROL2_REG |= CSD_PRESCALER_CTRL_CMP_LESS_EQ;
    #else
        /* Do nothing = config without prescaler */
    #endif  /* (CSD_PRESCALER_OPTIONS == CSD_PRESCALER_UDB) */

    /* Set PRS */
    #if (CSD_PRS_OPTIONS)
        CSD_SetAnalogSwitchesSource(CSD_ANALOG_SWITCHES_SRC_PRS);
    #endif /* (CSD_PRS_OPTIONS) */

    #if (CSD_PRS_OPTIONS == CSD_PRS_8BITS)
        /* Aux control set FIFO as REG */
        enableInterrupts = CyEnterCriticalSection();
        CSD_AUX_CONTROL_A_REG |= CSD_AUXCTRL_FIFO_SINGLE_REG;
        CyExitCriticalSection(enableInterrupts);
        
        /* Write polynomial */
        CSD_POLYNOM_REG   = CSD_PRS8_DEFAULT_POLYNOM;
        /* Write FIFO with seed */
        CSD_SEED_COPY_REG = CSD_MEASURE_FULL_RANGE_LOW;
        
    #elif (CSD_PRS_OPTIONS == CSD_PRS_16BITS)
        /* Aux control set FIFO as REG */ 
        enableInterrupts = CyEnterCriticalSection();  
        CSD_AUX_CONTROL_A_REG |= CSD_AUXCTRL_FIFO_SINGLE_REG;
        CSD_AUX_CONTROL_B_REG |= CSD_AUXCTRL_FIFO_SINGLE_REG;
        CyExitCriticalSection(enableInterrupts);
        
        /* Write polynomial */
        CY_SET_REG16(CSD_POLYNOM_PTR, CSD_PRS16_DEFAULT_POLYNOM);
        /* Write FIFO with seed */
        CY_SET_REG16(CSD_SEED_COPY_PTR, CSD_MEASURE_FULL_RANGE);
                
    #elif (CSD_PRS_OPTIONS == CSD_PRS_16BITS_4X)
        /* Aux control set FIFO as REG */
        enableInterrupts = CyEnterCriticalSection();
        CSD_AUX_CONTROL_A_REG  |= CSD_AUXCTRL_FIFO_SINGLE_REG;
        CyExitCriticalSection(enableInterrupts);
        
        /* Write polynomial */
        CSD_POLYNOM_A__D1_REG   = HI8(CSD_PRS16_DEFAULT_POLYNOM);
        CSD_POLYNOM_A__D0_REG   = LO8(CSD_PRS16_DEFAULT_POLYNOM);
        /* Write FIFO with seed */
        CSD_SEED_COPY_A__F1_REG = CSD_MEASURE_FULL_RANGE_LOW;
        CSD_SEED_COPY_A__F0_REG = CSD_MEASURE_FULL_RANGE_LOW; 
        
    #else
        /* Do nothing = config without PRS */
    #endif  /* (CSD_PRS_OPTIONS == CSD_PRS_8BITS) */ 
    
    /* Set ScanSpeed */
    CSD_SCANSPEED_PERIOD_REG = CSD_SCANSPEED_VALUE;
    
    /* Set the Measure */
    #if (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_FF)
        /* Window PWM */
        CSD_PWM_CH0_CONTROL_REG      = CSD_MEASURE_CTRL_ENABLE;
        CSD_PWM_CH0_CONTROL2_REG    |= CSD_MEASURE_CTRL_PULSEWIDTH;
        CY_SET_REG16(CSD_PWM_CH0_COUNTER_PTR, CSD_MEASURE_FULL_RANGE);
        
        /* Raw Counter */
        CSD_RAW_CH0_CONTROL_REG      = CSD_MEASURE_CTRL_ENABLE;
        CSD_RAW_CH0_CONTROL2_REG    |= CSD_MEASURE_CTRL_PULSEWIDTH;
        CY_SET_REG16(CSD_RAW_CH0_PERIOD_PTR, CSD_MEASURE_FULL_RANGE);
    
    #else
        /*Window PWM and Raw Counter AUX set */
        enableInterrupts = CyEnterCriticalSection();
        CSD_PWM_CH0_AUX_CONTROL_REG |= CSD_AUXCTRL_FIFO_SINGLE_REG;
        CSD_RAW_CH0_AUX_CONTROL_REG |= CSD_AUXCTRL_FIFO_SINGLE_REG;
        CyExitCriticalSection(enableInterrupts);
        
        /* Window PWM */
        CSD_PWM_CH0_ADD_VALUE_REG    = CSD_MEASURE_FULL_RANGE_LOW;
        CSD_PWM_CH0_PERIOD_LO_REG    = CSD_MEASURE_FULL_RANGE_LOW;
        CSD_PWM_CH0_COUNTER_LO_REG   = CSD_MEASURE_FULL_RANGE_LOW;
        
        /* Raw Counter */
        CSD_RAW_CH0_ADD_VALUE_REG    = CSD_MEASURE_FULL_RANGE_LOW;
        CSD_RAW_CH0_PERIOD_HI_REG    = CSD_MEASURE_FULL_RANGE_LOW;
        CSD_RAW_CH0_PERIOD_LO_REG    = CSD_MEASURE_FULL_RANGE_LOW;
        
    #endif  /* (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_FF) */ 
    
    #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
        #if (CSD_IMPLEMENTATION_CH1 == CSD_MEASURE_IMPLEMENTATION_FF)
            /* Window PWM */
            CSD_PWM_CH1_CONTROL_REG      = CSD_MEASURE_CTRL_ENABLE;
            CSD_PWM_CH1_CONTROL2_REG    |= CSD_MEASURE_CTRL_PULSEWIDTH;
            CY_SET_REG16(CSD_PWM_CH1_COUNTER_PTR, CSD_MEASURE_FULL_RANGE);
            
            /* Raw Counter */
            CSD_RAW_CH1_CONTROL_REG      = CSD_MEASURE_CTRL_ENABLE;
            CSD_RAW_CH1_CONTROL2_REG    |= CSD_MEASURE_CTRL_PULSEWIDTH;
            CY_SET_REG16(CSD_RAW_CH1_PERIOD_PTR, CSD_MEASURE_FULL_RANGE);
           
        #else
            /*Window PWM and Raw Counter AUX set */
            enableInterrupts = CyEnterCriticalSection();
            CSD_PWM_CH1_AUX_CONTROL_REG |= CSD_AUXCTRL_FIFO_SINGLE_REG;
            CSD_RAW_CH1_AUX_CONTROL_REG |= CSD_AUXCTRL_FIFO_SINGLE_REG;
            CyExitCriticalSection(enableInterrupts);
            
            /* Window PWM */
            CSD_PWM_CH1_ADD_VALUE_REG    = CSD_MEASURE_FULL_RANGE_LOW;
            CSD_PWM_CH1_PERIOD_LO_REG    = CSD_MEASURE_FULL_RANGE_LOW;
            CSD_PWM_CH1_COUNTER_LO_REG   = CSD_MEASURE_FULL_RANGE_LOW;
            
            /* Raw Counter */
            
            CSD_RAW_CH1_ADD_VALUE_REG    = CSD_MEASURE_FULL_RANGE_LOW;
            CSD_RAW_CH1_PERIOD_HI_REG    = CSD_MEASURE_FULL_RANGE_LOW;
            CSD_RAW_CH1_PERIOD_LO_REG    = CSD_MEASURE_FULL_RANGE_LOW;
            
        #endif  /* (CSD_IMPLEMENTATION_CH1 == CSD_MEASURE_IMPLEMENTATION_FF) */
    
    #endif  /* (CSD_DESIGN_TYPE == TWO_CHANNELS_DESIGN) */
    
    /* Setup ISR */
    CyIntDisable(CSD_IsrCH0_ISR_NUMBER);
    (void)CyIntSetVector(CSD_IsrCH0_ISR_NUMBER, &CSD_IsrCH0_ISR);
    CyIntSetPriority(CSD_IsrCH0_ISR_NUMBER, CSD_IsrCH0_ISR_PRIORITY);
    
    #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
        CyIntDisable(CSD_IsrCH1_ISR_NUMBER);
        CyIntSetVector(CSD_IsrCH1_ISR_NUMBER, CSD_IsrCH1_ISR);
        CyIntSetPriority(CSD_IsrCH1_ISR_NUMBER, CSD_IsrCH1_ISR_PRIORITY);
    #endif  /* CSD_DESIGN_TYPE */
    
    /* Setup AMux Bus: Connect Cmod, Cmp, Idac */
    CSD_AMuxCH0_Init();
    CSD_AMuxCH0_Connect(CSD_AMuxCH0_CMOD_CHANNEL);
    CSD_AMuxCH0_Connect(CSD_AMuxCH0_CMP_VP_CHANNEL);
    #if (CSD_CURRENT_SOURCE)
        CSD_AMuxCH0_Connect(CSD_AMuxCH0_IDAC_CHANNEL);
    #endif  /* CSD_CURRENT_SOURCE */
    
    #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) 
        CSD_AMuxCH1_Init();
        CSD_AMuxCH1_Connect(CSD_AMuxCH1_CMOD_CHANNEL);
        CSD_AMuxCH1_Connect(CSD_AMuxCH1_CMP_VP_CHANNEL);
        #if (CSD_CURRENT_SOURCE)
            CSD_AMuxCH1_Connect(CSD_AMuxCH1_IDAC_CHANNEL);
        #endif  /* CSD_CURRENT_SOURCE */
    #endif  /* CSD_DESIGN_TYPE */
    
    /* Int Rb */
    #if (CSD_CURRENT_SOURCE == CSD_EXTERNAL_RB)
        CSD_InitRb();
    #endif /* (CSD_CURRENT_SOURCE == CSD_EXTERNAL_RB) */
    
    /* Enable window generation */
    CSD_CONTROL_REG |= CSD_CTRL_WINDOW_EN__CH0;
    #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
        CSD_CONTROL_REG |= CSD_CTRL_WINDOW_EN__CH1;
    #endif  /* CSD_DESIGN_TYPE */
    
    /* Initialize Cmp and Idac */
    CSD_CompCH0_Init();
    #if (CSD_CURRENT_SOURCE)
        CSD_IdacCH0_Init();
        CSD_IdacCH0_SetPolarity(CSD_IdacCH0_IDIR);
        CSD_IdacCH0_SetRange(CSD_IDAC_RANGE_VALUE);
        CSD_IdacCH0_SetValue(CSD_TURN_OFF_IDAC);
    #endif  /* CSD_CURRENT_SOURCE */
    
    #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) 
        CSD_CompCH1_Init();
        #if (CSD_CURRENT_SOURCE)
            CSD_IdacCH1_Init();
            CSD_IdacCH1_SetPolarity(CSD_IdacCH1_IDIR);
            CSD_IdacCH1_SetRange(CSD_IDAC_RANGE_VALUE);
            CSD_IdacCH1_SetValue(CSD_TURN_OFF_IDAC);
        #endif  /* CSD_CURRENT_SOURCE */
    #endif  /* CSD_DESIGN_TYPE */
    
    /* Initialize Vref if as VDAC */
    #if (CSD_VREF_OPTIONS == CSD_VREF_VDAC)
        CSD_VdacRefCH0_Init();
        #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
            CSD_VdacRefCH1_Init();
        #endif  /* CSD_DESIGN_TYPE */
    #endif  /* CSD_VREF_OPTIONS */
}


/*******************************************************************************
* Function Name: CSD_Enable
********************************************************************************
*
* Summary:
*  Enables active mode power template bits for number of component used within 
*  CapSense.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void CSD_Enable(void) 
{
    uint8 enableInterrupts;
    
    enableInterrupts = CyEnterCriticalSection();
    
    /* Enable Prescaler */
    #if (CSD_PRESCALER_OPTIONS == CSD_PRESCALER_UDB)
        /* Do nothing  for UDB */
    #elif (CSD_PRESCALER_OPTIONS == CSD_PRESCALER_FF)
        CSD_PRESCALER_ACT_PWRMGR_REG  |= CSD_PRESCALER_ACT_PWR_EN;
        CSD_PRESCALER_STBY_PWRMGR_REG |= CSD_PRESCALER_STBY_PWR_EN;
        
    #else
        /* Do nothing = config without prescaler */
    #endif  /* (CSD_PRESCALER_OPTIONS == CSD_PRESCALER_UDB) */
    
    /* Enable ScanSpeed */
    CSD_SCANSPEED_AUX_CONTROL_REG |= CSD_SCANSPEED_CTRL_ENABLE;
    
    /* Enable Measure CH0 */
    #if (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_FF)
        /* Window PWM */
        CSD_PWM_CH0_ACT_PWRMGR_REG  |= CSD_PWM_CH0_ACT_PWR_EN;
        CSD_PWM_CH0_STBY_PWRMGR_REG |= CSD_PWM_CH0_STBY_PWR_EN;
        
        /* Raw Counter */
        CSD_RAW_CH0_ACT_PWRMGR_REG  |= CSD_RAW_CH0_ACT_PWR_EN;
        CSD_RAW_CH0_STBY_PWRMGR_REG |= CSD_RAW_CH0_STBY_PWR_EN;
        
    #else
        /* Window PWM -  Do nothing */
        /* Raw Counter - Do nothing */
        
    #endif  /* (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_FF) */ 
    
    /* Enable Measure CH1*/
    #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
        #if (CSD_IMPLEMENTATION_CH1 == CSD_MEASURE_IMPLEMENTATION_FF)
            /* Window PWM */
            CSD_PWM_CH1_ACT_PWRMGR_REG  |= CSD_PWM_CH1_ACT_PWR_EN;
            CSD_PWM_CH1_STBY_PWRMGR_REG |= CSD_PWM_CH1_STBY_PWR_EN;
            
            /* Raw Counter */
            CSD_RAW_CH1_ACT_PWRMGR_REG  |= CSD_RAW_CH1_ACT_PWR_EN;
            CSD_RAW_CH1_STBY_PWRMGR_REG |= CSD_RAW_CH1_STBY_PWR_EN;
           
        #else
        /* Window PWM -  Do nothing */
        /* Raw Counter - Do nothing */
        
        #endif  /* (CSD_IMPLEMENTATION_CH1 == CSD_MEASURE_IMPLEMENTATION_FF) */
    
    #endif  /* (CSD_DESIGN_TYPE == TWO_CHANNELS_DESIGN)*/
    
    /* Enable the Clock */
    #if (CSD_CLOCK_SOURCE == CSD_INTERNAL_CLOCK)
       CSD_IntClock_Enable();
    #endif  /* CSD_CLOCK_SOURCE */
    
    /* Setup Cmp and Idac */
    CSD_CompCH0_Enable();
    #if (CSD_CURRENT_SOURCE)
        CSD_IdacCH0_Enable();
    #endif  /* CSD_CURRENT_SOURCE */
    
    #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) 
        CSD_CompCH1_Enable();
        #if (CSD_CURRENT_SOURCE)
            CSD_IdacCH1_Enable();
        #endif  /* CSD_CURRENT_SOURCE */
    #endif  /* CSD_DESIGN_TYPE */
    
    /* Enable Vref */
    #if (CSD_VREF_OPTIONS == CSD_VREF_VDAC)
        CSD_VdacRefCH0_Enable();
        CSD_VdacRefCH0_SetValue(CSD_VdacRefCH0_DEFAULT_DATA);
        #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
            CSD_VdacRefCH1_Enable();
            CSD_VdacRefCH1_SetValue(CSD_VdacRefCH1_DEFAULT_DATA);
        #endif  /* (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) */
    #else
        /* Enable CapSense Buf */
        CSD_BufCH0_STBY_PWRMGR_REG |= CSD_BufCH0_STBY_PWR_EN;
        CSD_BufCH0_ACT_PWRMGR_REG  |= CSD_BufCH0_ACT_PWR_EN;
        
        #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
            CSD_BufCH1_STBY_PWRMGR_REG |= CSD_BufCH1_STBY_PWR_EN;
            CSD_BufCH1_ACT_PWRMGR_REG  |= CSD_BufCH1_ACT_PWR_EN;
        #endif  /* (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) */
    #endif  /* (CSD_VREF_VDAC == CSD_VREF_OPTIONS) */
    
    /* Set reference on AMux Bus */
    #if (CSD_VREF_OPTIONS == CSD_VREF_VDAC)
        /* Connect Vdac to AMux Bus */
        CSD_AMuxCH0_Connect(CSD_AMuxCH0_VREF_CHANNEL);
        #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
            CSD_AMuxCH1_Connect(CSD_AMuxCH1_VREF_CHANNEL);
        #endif  /* (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) */
        
    #else
        /* Enable CapSense Buf */
        CSD_BufCH0_CAPS_CFG0_REG |= CSD_CSBUF_ENABLE;
        
        #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
            CSD_BufCH1_CAPS_CFG0_REG |= CSD_CSBUF_ENABLE;
        #endif  /* (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) */
    #endif  /* (CSD_VREF_VDAC == CSD_VREF_OPTIONS)*/
    
    CyExitCriticalSection(enableInterrupts);
    
    /* Enable interrupt */
    CyIntEnable(CSD_IsrCH0_ISR_NUMBER);
    #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) 
        CyIntEnable(CSD_IsrCH1_ISR_NUMBER);
    #endif  /* CSD_DESIGN_TYPE */
    
    /* Set CapSense Enable state */
    CSD_CONTROL_REG |= CSD_CTRL_CAPSENSE_EN;
}


/*******************************************************************************
* Function Name: CSD_Start
********************************************************************************
*
* Summary:
*  Initializes registers and starts the CSD method of CapSense component. Reset 
*  all sensors to an inactive state. Enables interrupts for sensors scanning.
*  When Auto Tuning (SmartSense) mode is selected the tuning procedure is 
*  applied for all sensors.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global Variables:
*  CSD_initVar - used to check initial configuration, modified on 
*  first function call.
*  CSD_lowLevelTuningDone - used to notify the Tuner GUI that 
*  tuning of scanning parameters are done.
*
* Reentrant:
*  No
*
* Note:
*  All widgets are enabled by default except proximity widgets. Proximity widgets 
*  must be manually enabled as their long scan time is incompatible 
*  with the fast response required of other widget types. 
*
*******************************************************************************/
void CSD_Start(void)  
{
    if (CSD_initVar == 0u)
    {
        CSD_Init();
        CSD_initVar = 1u;
    }
    CSD_Enable();
    
    /* AutoTunning start */
    #if (CSD_TUNING_METHOD == CSD_AUTO_TUNING)
        /* AutoTune by sensor or pair of sensor basis */
        CSD_AutoTune();
        CSD_lowLevelTuningDone = 1u;
    #endif /* (CSD_TUNING_METHOD == CSD_AUTO_TUNING) */
}


/*******************************************************************************
* Function Name: CSD_Stop
********************************************************************************
*
* Summary:
*  Stops the sensors scanner, disables internal interrupts, and resets all 
*  sensors to an inactive state. Disables Active mode power template bits for 
*  number of component used within CapSense.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  This function should be called after scans will be completed.
*
*******************************************************************************/
void CSD_Stop(void) 
{
    /* Stop Capsensing */
    CSD_CONTROL_REG &= (uint8)(~CSD_CTRL_START);
    
    /* Disable interrupt */
    CyIntDisable(CSD_IsrCH0_ISR_NUMBER);
    #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) 
        CyIntDisable(CSD_IsrCH1_ISR_NUMBER);
    #endif  /* CSD_DESIGN_TYPE */
    
    /* Clear all sensors */
    CSD_ClearSensors();
    
    /* Disable Prescaler */
    #if (CSD_PRESCALER_OPTIONS == CSD_PRESCALER_UDB)
        /* Do nothing  for UDB */
    #elif (CSD_PRESCALER_OPTIONS == CSD_PRESCALER_FF)        
        CSD_PRESCALER_ACT_PWRMGR_REG  &= ~CSD_PRESCALER_ACT_PWR_EN;
        CSD_PRESCALER_STBY_PWRMGR_REG &= ~CSD_PRESCALER_STBY_PWR_EN;
        
    #else
        /* Do nothing = config without prescaler */
    #endif  /* (CSD_PRESCALER_OPTIONS == CSD_PRESCALER_UDB) */
    
    /* Disable ScanSpeed */
    CSD_SCANSPEED_AUX_CONTROL_REG &= (uint8)(~CSD_SCANSPEED_CTRL_ENABLE);
    
    /* Disable Measure CH0 */
    #if (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_FF)
        /* Window PWM */
        CSD_PWM_CH0_ACT_PWRMGR_REG  &= ~CSD_PWM_CH0_ACT_PWR_EN;
        CSD_PWM_CH0_STBY_PWRMGR_REG &= ~CSD_PWM_CH0_STBY_PWR_EN;

        /* Raw Counter */
        CSD_RAW_CH0_ACT_PWRMGR_REG  &= ~CSD_RAW_CH0_ACT_PWR_EN;
        CSD_RAW_CH0_STBY_PWRMGR_REG &= ~CSD_RAW_CH0_STBY_PWR_EN;

    #else
        /* Window PWM -  Do nothing */
        /* Raw Counter - Do nothing */
        
    #endif  /* (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_FF) */ 
    
    /* Disable Measure CH1 */
    #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
        #if (CSD_IMPLEMENTATION_CH1 == CSD_MEASURE_IMPLEMENTATION_FF)
            /* Window PWM */
            CSD_PWM_CH1_ACT_PWRMGR_REG  &= ~CSD_PWM_CH1_ACT_PWR_EN;
            CSD_PWM_CH1_STBY_PWRMGR_REG &= ~CSD_PWM_CH1_STBY_PWR_EN;
    
            /* Raw Counter */
            CSD_RAW_CH1_ACT_PWRMGR_REG  &= ~CSD_RAW_CH1_ACT_PWR_EN;
            CSD_RAW_CH1_STBY_PWRMGR_REG &= ~CSD_RAW_CH1_STBY_PWR_EN;
           
        #else
        /* Window PWM -  Do nothing */
        /* Raw Counter - Do nothing */
        
        #endif  /* (CSD_IMPLEMENTATION_CH1 == CSD_MEASURE_IMPLEMENTATION_FF) */
    
    #endif  /* (CSD_DESIGN_TYPE == TWO_CHANNELS_DESIGN)*/
    
    /* Disable the Clock */
    #if (CSD_CLOCK_SOURCE == CSD_INTERNAL_CLOCK)
       CSD_IntClock_Stop();
    #endif  /* CSD_CLOCK_SOURCE */
    
    /* Disable power from Cmp and Idac */
    CSD_CompCH0_Stop();
    #if (CSD_CURRENT_SOURCE)
        CSD_IdacCH0_Stop();
    #endif  /* CSD_CURRENT_SOURCE */
    
    #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) 
        CSD_CompCH1_Stop();
        #if (CSD_CURRENT_SOURCE)
            CSD_IdacCH1_Stop();
        #endif  /* CSD_CURRENT_SOURCE */
    #endif  /* CSD_DESIGN_TYPE */    
    
    /* Disable Vref if as VDAC */
    #if (CSD_VREF_OPTIONS == CSD_VREF_VDAC)
        CSD_VdacRefCH0_Stop();
        #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
            CSD_VdacRefCH1_Stop();
        #endif  /* CSD_DESIGN_TYPE */
    #endif  /* CSD_VREF_OPTIONS */

    #if (CSD_VREF_VDAC == CSD_VREF_OPTIONS)
        /* The Idac turn off before */
    #else
        /* Enable CapSense Buf */
        CSD_BufCH0_CAPS_CFG0_REG &= (uint8)(~CSD_CSBUF_ENABLE);
        CSD_BufCH0_ACT_PWRMGR_REG &= (uint8)(~CSD_BufCH0_ACT_PWR_EN);
        CSD_BufCH0_STBY_PWRMGR_REG &= (uint8)(~CSD_BufCH0_STBY_PWR_EN);
        
        #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
            CSD_BufCH1_CAPS_CFG0_REG &= (uint8)(~CSD_CSBUF_ENABLE);
            CSD_BufCH1_ACT_PWRMGR_REG &= (uint8)(~CSD_BufCH1_ACT_PWR_EN);
            CSD_BufCH1_STBY_PWRMGR_REG &= (uint8)(~CSD_BufCH1_STBY_PWR_EN);
        #endif  /*(CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) */
    #endif  /* (CSD_VREF_VDAC == CSD_VREF_OPTIONS) */
    
    /* Set CapSense Disable state */
    CSD_CONTROL_REG &= (uint8)(~CSD_CTRL_CAPSENSE_EN);
}


#if (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN)
    /*******************************************************************************
    * Function Name: CSD_FindNextSensor
    ********************************************************************************
    *
    * Summary:
    *  Finds next sensor to scan. 
    *
    * Parameters:
    *  snsIndex:  Current index of sensor.
    *
    * Return:
    *  Returns the next sensor index to scan.
    *
    * Global Variables:
    *  CSD_sensorEnableMask[ ] - used to store bit masks of enabled 
    *  sensors.
    *  CSD_sensorEnableMask[0] contains the masked bits for sensors 0
    *  through 7 (sensor 0 is bit 0, sensor 1 is bit 1).
    *  CSD_sensorEnableMask[1] contains the masked bits for sensors 
    *  8 through 15 (if needed), and so on.
    *    0 - sensor doesn't scan by CSD_ScanEnabledWidgets().
    *    1 - sensor scans by CSD_ScanEnabledWidgets().
    *
    * Note: 
    *  This function has effect on current scanning scanning and should not
    *  be used outisde of component.
    *
    *******************************************************************************/
    uint8 CSD_FindNextSensor(uint8 snsIndex) CYREENTRANT
    {
        uint8 pos;
        uint8 enMask;
        
        /* Check if sensor enabled */
        do
        {
            /* Proceed with the next sensor */
            snsIndex++;
            if(snsIndex == CSD_TOTAL_SENSOR_COUNT)
            {
                break;
            }
            pos = (snsIndex >> 3u);
            enMask = 0x01u << (snsIndex & 0x07u);
        }    
        while((CSD_sensorEnableMask[pos] & enMask) == 0u);
        
        return snsIndex;
    }
 #endif  /* CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN */
 
 
#if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)
    /*******************************************************************************
    * Function Name: CSD_FindNextPair
    ********************************************************************************
    *
    * Summary:
    *  Finds next pair or sensor to scan. Sets condition bits to skip scanning.
    *  
    * Parameters:
    *  snsIndex:  Current index pair of sensors.
    *
    * Return:
    *  Returns the next pair of sensors index to scan.
    *
    * Global Variables:
    *  CSD_sensorEnableMask[ ] - used to store bit masks of enabled 
    *  sensors.
    *  CSD_sensorEnableMask[0] contains the masked bits for sensors 0
    *  through 7 (sensor 0 is bit 0, sensor 1 is bit 1).
    *  CSD_sensorEnableMask[1] contains the masked bits for sensors 
    *  8 through 15 (if needed), and so on.
    *    0 - sensor doesn't scan by CSD_ScanEnabledWidgets().
    *    1 - sensor scans by CSD_ScanEnabledWidgets().
    *
    * Note: 
    *  This function has effect on control signals set for scanning and should not
    *  be used outisde of component.
    *
    *******************************************************************************/
    uint8 CSD_FindNextPair(uint8 snsIndex) CYREENTRANT
    {
        uint8 posCh;
        uint8 enMaskCh;
        uint8 newRegValue;
        uint8 indexCh0 = snsIndex;
        uint8 indexCh1 = snsIndex + CSD_TOTAL_SENSOR_COUNT__CH0;
        
        /* Find enabled sensor on channel 0 */
        do
        {
            /* Procced the scanning */
            indexCh0++;
            if (indexCh0 >= CSD_TOTAL_SENSOR_COUNT__CH0)
            {
                /* Lets hadle now all from CH1 */
                indexCh0 = CSD_END_OF_SCAN__CH0;
                break;
            }
            
            posCh = (indexCh0 >> 3u);
            enMaskCh = 0x01u << (indexCh0 & 0x07u);
        }
        while((CSD_sensorEnableMask[posCh] & enMaskCh) == 0u);
        
        /* Find enabled sensor on channel 1 */
        do
        {
            /* Procced the scanning */
            indexCh1++;        
            if (indexCh1 >= CSD_TOTAL_SENSOR_COUNT)
            {
                /* Lets hadle now all from CH0 */
                indexCh1 = CSD_END_OF_SCAN__CH1;
                break;
            }
            
            posCh = (indexCh1 >> 3u);
            enMaskCh = 0x01u << (indexCh1 & 0x07u);
        } 
        while((CSD_sensorEnableMask[posCh] & enMaskCh) == 0u);
        
        indexCh1 -= CSD_TOTAL_SENSOR_COUNT__CH0;
        
        /* Find the pair to scan */
        if(indexCh0 == indexCh1)
        {
            /* Scans TWO Channels */
            snsIndex = indexCh0;
            
            CSD_CONTROL_REG |= (CSD_CTRL_WINDOW_EN__CH0 | 
                                             CSD_CTRL_WINDOW_EN__CH1);
        }
        else if(indexCh0 < indexCh1)
        {
           /* Scans Channel ONE only */
           snsIndex = indexCh0;
           
            newRegValue = CSD_CONTROL_REG;
            newRegValue |= CSD_CTRL_WINDOW_EN__CH0;
            newRegValue &= ~CSD_CTRL_WINDOW_EN__CH1;
            CSD_CONTROL_REG = newRegValue;
        }
        else
        {
            /* Scans Channel TWO only */
            snsIndex = indexCh1;
            
            newRegValue = CSD_CONTROL_REG;
            newRegValue |= CSD_CTRL_WINDOW_EN__CH1;
            newRegValue &= ~CSD_CTRL_WINDOW_EN__CH0;
            CSD_CONTROL_REG = newRegValue;
        }
        
        return (snsIndex);
    }
#endif  /* CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN */


/*******************************************************************************
* Function Name: CSD_SetScanSlotSettings
********************************************************************************
*
* Summary:
*  Sets the scan settings of the selected scan slot (sensor or pair of sensors). 
*  The scan settings incorporate IDAC value (for IDAC configurations) for every 
*  sensor and resolution. The resolution is the same for all sensors within 
*  widget.
*
* Parameters:
*  slot:  Scan slot number (sensor or pair of sensors).
*
* Return:
*  None
*
* Global Variables:
*  CSD_idacSettings[] - used to store idac value for every sensor.
*  CSD_widgetResolution[] - used to store scan resolution of every 
*  widget.
*
*******************************************************************************/
void CSD_SetScanSlotSettings(uint8 slot) CYREENTRANT
{
    uint8 widget;
    
    #if (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN)
        /* Define widget sensor belongs to */
        widget = CSD_widgetNumber[slot];
        
        /* Set Idac Value */
        #if (CSD_CURRENT_SOURCE)
            CSD_IdacCH0_SetValue(CSD_idacSettings[slot]);
        #endif  /* CSD_CURRENT_SOURCE */
        
        /* Window PWM */
        #if (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_FF)
            CY_SET_REG16(CSD_PWM_CH0_PERIOD_PTR,
                ((uint16) CSD_widgetResolution[widget] << 8u) | CSD_MEASURE_FULL_RANGE_LOW);
        #else
            CSD_PWM_CH0_PERIOD_HI_REG = CSD_widgetResolution[widget];
        #endif  /* (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_FF) */ 

        #if ( (CSD_MULTIPLE_PRESCALER_ENABLED) || \
              (CSD_TUNING_METHOD == CSD_AUTO_TUNING) )
            CSD_SetPrescaler(CSD_analogSwitchDivider[slot]);
        #elif (CSD_PRESCALER_OPTIONS)
            CSD_SetPrescaler(CSD_analogSwitchDivider);
        #endif /* ((CSD_MULTIPLE_PRESCALER_ENABLED) || \
               *   (CSD_TUNING_METHOD == CSD_AUTO_TUNING))
               */

    #else
        if(slot < CSD_TOTAL_SENSOR_COUNT__CH0)
        {
            /* Define widget sensor belongs to */
            widget = CSD_widgetNumber[slot];
            
            /* Set Idac Value */
            #if (CSD_CURRENT_SOURCE)
                CSD_IdacCH0_SetValue(CSD_idacSettings[slot]);
            #endif  /* CSD_CURRENT_SOURCE */
            
            /* Set Pwm Resolution */
            #if (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_FF)
                CY_SET_REG16(CSD_PWM_CH0_PERIOD_PTR,
                  ((uint16) CSD_widgetResolution[widget] << 8u) | CSD_MEASURE_FULL_RANGE_LOW);
            #else
                CSD_PWM_CH0_PERIOD_HI_REG = CSD_widgetResolution[widget];
            #endif  /* (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_FF)*/ 
        }
        
        if(slot < CSD_TOTAL_SENSOR_COUNT__CH1)
        {
            widget = CSD_widgetNumber[slot+CSD_TOTAL_SENSOR_COUNT__CH0];
        
            /* Set Idac Value */
            #if (CSD_CURRENT_SOURCE)
                CSD_IdacCH1_SetValue(CSD_idacSettings[slot+
                                                                             CSD_TOTAL_SENSOR_COUNT__CH0]);
            #endif  /* CSD_CURRENT_SOURCE */
            
            /* Set Pwm Resolution */
            #if (CSD_IMPLEMENTATION_CH1 == CSD_MEASURE_IMPLEMENTATION_FF)
                CY_SET_REG16(CSD_PWM_CH1_PERIOD_PTR,
                  ((uint16) CSD_widgetResolution[widget] << 8u) | CSD_MEASURE_FULL_RANGE_LOW);
            #else
                CSD_PWM_CH1_PERIOD_HI_REG = CSD_widgetResolution[widget];
            #endif  /* (CSD_IMPLEMENTATION_CH1 == CSD_MEASURE_IMPLEMENTATION_FF)*/ 
        }

        #if ( (CSD_MULTIPLE_PRESCALER_ENABLED) || \
              (CSD_TUNING_METHOD == CSD_AUTO_TUNING) )
            CSD_SetPrescaler(CSD_analogSwitchDivider[slot]);
        #elif (CSD_PRESCALER_OPTIONS)
            CSD_SetPrescaler(CSD_analogSwitchDivider);
        #endif /* ((CSD_MULTIPLE_PRESCALER_ENABLED) || \
               *   (CSD_TUNING_METHOD == CSD_AUTO_TUNING))
               */

    #endif  /* CSD_DESIGN_TYPE */
}


/*******************************************************************************
* Function Name: CSD_ScanSensor
********************************************************************************
*
* Summary:
*  Sets scan settings and starts scanning a sensor or pair of combined sensors
*  on each channel. If two channels are configured, two sensors may be scanned 
*  at the same time. After scanning is complete the isr copies the measured 
*  sensor raw data to the global array. Use of the isr ensures this function 
*  is non-blocking. Each sensor has a unique number within the sensor array. 
*  This number is assigned by the CapSense customizer in sequence.
*
* Parameters:
*  sensor:  Sensor number.
*
* Return:
*  None
*
* Global Variables:
*  CSD_csv - used to provide status and mode of scanning process. 
*  Sets busy status(scan in progress) and mode of scan as single scan.
*  For two channel design the additional bits are set to define if scan a 
*  pair of sensors or single one.
*  CSD_sensorIndex - used to store sensor scanning sensor number.
*  Sets to provided sensor argument.
*
* Reentrant:
*  No
*
*******************************************************************************/
void CSD_ScanSensor(uint8 sensor)  
{
    /* Clears status/control variable and set sensorIndex */
    CSD_csv = 0u;
    CSD_sensorIndex = sensor;
    
    #if (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN)
        /* Start of sensor scan */
        CSD_csv = (CSD_SW_STS_BUSY | CSD_SW_CTRL_SINGLE_SCAN);
        CSD_PreScan(sensor);
        
    #else
        /* CH0: check end of scan conditions */
        if(sensor < CSD_TOTAL_SENSOR_COUNT__CH0)
        {
            CSD_CONTROL_REG |= CSD_CTRL_WINDOW_EN__CH0;
        }
        else
        {
            CSD_CONTROL_REG &= ~CSD_CTRL_WINDOW_EN__CH0;
        }
        
        /* CH1: check end of scan conditions */
        if(sensor < CSD_TOTAL_SENSOR_COUNT__CH1)
        {
            CSD_CONTROL_REG |= CSD_CTRL_WINDOW_EN__CH1;
        }
        else
        {
            CSD_CONTROL_REG &= ~CSD_CTRL_WINDOW_EN__CH1;
        }
        
        /* Start sensor scan */
        if( ((CSD_CONTROL_REG & CSD_CTRL_WINDOW_EN__CH0) != 0u) || 
            ((CSD_CONTROL_REG & CSD_CTRL_WINDOW_EN__CH1) != 0u) )
        {
        
            CSD_csv |= (CSD_SW_STS_BUSY | CSD_SW_CTRL_SINGLE_SCAN);
            CSD_PreScan(sensor);
        }
        
    #endif  /* CSD_DESIGN_TYPE */
}


/*******************************************************************************
* Function Name: CSD_ScanEnableWidgets
********************************************************************************
*
* Summary:
*  Scans all of the enabled widgets. Starts scanning a sensor or pair of sensors 
*  within enabled widget. The isr proceeding scanning next sensor or pair till 
*  all enabled widgets will be scanned. Use of the isr ensures this function is 
*  non-blocking. All widgets are enabled by default except proximity widgets. 
*  Proximity widgets must be manually enabled as their long scan time is 
*  incompatible with fast response desired of other widget types.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global Variables:
*  CSD_csv - used to provide status and mode of scanning process. 
*  Sets busy status(scan in progress) and clears single scan mode.
*  For two channel design the additional bits are set to define if scan a 
*  pair of sensors or single one. 
*  CSD_sensorIndex - used to store sensor scanning sensor number.
*  Sets to 0xFF and provided to function CSD_FindNextSensor or
*  CSD_FindNextPair, these functions starts with sensor index
*  increment and overflow of uint8 gives desired index 0.
*
* Reentrant:
*  No
*
*******************************************************************************/
void CSD_ScanEnabledWidgets(void) 
{
    /* Clears status/control variable and set sensorIndex */
    CSD_csv = 0u;
    CSD_sensorIndex = 0xFFu;
    
    #if (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN)
        /* Find next sensor */
        CSD_sensorIndex = CSD_FindNextSensor(CSD_sensorIndex);

        /* Check end of scan condition */
        if(CSD_sensorIndex < CSD_TOTAL_SENSOR_COUNT)
        {
            CSD_csv |= CSD_SW_STS_BUSY;
            CSD_PreScan(CSD_sensorIndex);
        }
        
    #else
        /* Find next sensor and set proper control register */
        CSD_sensorIndex = CSD_FindNextPair(CSD_sensorIndex);
        
        /* Start sensor scan */
        if((CSD_sensorIndex < CSD_TOTAL_SENSOR_COUNT__CH0) || 
           (CSD_sensorIndex < CSD_TOTAL_SENSOR_COUNT__CH1))
        {
            CSD_csv |= CSD_SW_STS_BUSY;
            CSD_PreScan(CSD_sensorIndex);
        }
        
    #endif  /* CSD_DESIGN_TYPE */
}


/*******************************************************************************
* Function Name: CSD_IsBusy
********************************************************************************
*
* Summary:
*  Returns the state of CapSense component. The 1 means that scanning in 
*  progress and 0 means that scanning is complete.
*
* Parameters:
*  None
*
* Return:
*  Returns the state of scanning. 1 - scanning in progress, 0 - scanning 
*  completed.
*
* Global Variables:
*  CSD_csv - used to provide status and mode of scanning process. 
*  Checks the busy status.
*
*******************************************************************************/
uint8 CSD_IsBusy(void) 
{
    return ((0u != (CSD_csv & CSD_SW_STS_BUSY)) ? 1u : 0u);
}


/*******************************************************************************
* Function Name: CSD_ReadSensorRaw
********************************************************************************
*
* Summary:
*  Returns scan sensor raw data from the CSD_sensorRaw[] array. 
*  Each scan sensor has a unique number within the sensor array. This number 
*  is assigned by the CapSense customizer in sequence.
*
* Parameters:
*  sensor:  Sensor number.
*
* Return:
*  Returns current raw data value for defined sensor number.
*
* Global Variables:
*  CSD_sensorRaw[] - used to store sensors raw data.
*
*******************************************************************************/
uint16 CSD_ReadSensorRaw(uint8 sensor) 
{
    return (CSD_sensorRaw[sensor]);
}


/*******************************************************************************
* Function Name: CSD_ClearSensors
********************************************************************************
*
* Summary:
*  Resets all sensors to the non-sampling state by sequentially disconnecting
*  all sensors from Analog MUX Bus and putting them to inactive state.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void CSD_ClearSensors(void) 
{
    uint8 i;
   
    for (i = 0u; i < CSD_TOTAL_SENSOR_COUNT; i++)
    {
        CSD_DisableScanSlot(i);
    }
}


#if (CSD_IS_COMPLEX_SCANSLOTS)
    /*******************************************************************************
    * Function Name: CSD_EnableScanSlot
    ********************************************************************************
    *
    * Summary:
    *  Configures the selected slot to measure during the next measurement 
    *  cycle. The corresponding pin/pins are set to Analog High-Z mode and 
    *  connected to the Analog Mux Bus. This also enables the comparator function.
    *
    * Parameters:
    *  slot:  Slot number.
    *
    * Return:
    *  None
    *
    * Global Constants:
    *  CSD_portTable[]  - used to store the port number that pin 
    *  belongs to for every sensor.
    *  CSD_maskTable[]  - used to store the pin within the port for 
    *  every sensor.
    *  CSD_indexTable[] - used to store indexes of complex sensors.
    *  The offset and position in this array are stored in port and mask table for 
    *  complex sensors.
    *  The bit 7 (msb) is used to define the sensor type: single or complex.
    *
    *******************************************************************************/
    void CSD_EnableScanSlot(uint8 slot) CYREENTRANT
    {
        uint8 j;
        uint8 snsNumber;
        const uint8 CYCODE *index;
        /* Read the sensor type: single or complex */
        uint8 snsType = CSD_portTable[slot];
        
        /* Check if sensor is complex */
        if ((snsType & CSD_COMPLEX_SS_FLAG) == 0u)
        {
            /* Enable sensor (signle) */
            CSD_EnableSensor(slot);
        }
        else
        {
            /* Enable complex sensor */
            snsType &= ~CSD_COMPLEX_SS_FLAG;
            index = &CSD_indexTable[snsType];
            snsNumber = CSD_maskTable[slot];
                        
            for (j=0u; j < snsNumber; j++)
            {
                CSD_EnableSensor(index[j]);
            }
        } 
    }
    
    
    /*******************************************************************************
    * Function Name: CSD_DisableScanSlot
    ********************************************************************************
    *
    * Summary:
    *  Disables the selected slot. The corresponding pin/pis is/are disconnected 
    *  from the Analog Mux Bus and connected to GND, High_Z or Shield electrode.
    *
    * Parameters:
    *  slot:  Slot number.
    *
    * Return:
    *  None
    *
    * Global Variables:
    *  CSD_portTable[]  - used to store the port number that pin 
    *  belongs to for every sensor.
    *  CSD_maskTable[]  - used to store the pin within the port for 
    *  every sensor.
    *  CSD_indexTable[] - used to store indexes of complex sensors.
    *  The offset and position in this array are stored in port and mask table for 
    *  complex sensors.
    *  The 7bit(msb) is used to define the sensor type: single or complex.
    *
    *******************************************************************************/
    void CSD_DisableScanSlot(uint8 slot) CYREENTRANT
    {
        uint8 j;
        uint8 snsNumber;
        const uint8 CYCODE *index;
        /* Read the sensor type: single or complex */
        uint8 snsType = CSD_portTable[slot];
        
        /* Check if sensor is complex */
        if ((snsType & CSD_COMPLEX_SS_FLAG) == 0u)
        {
            /* Disable sensor (signle) */
            CSD_DisableSensor(slot);
        }
        else
        {
            /* Disable complex sensor */
            snsType &= ~CSD_COMPLEX_SS_FLAG;
            index = &CSD_indexTable[snsType];
            snsNumber = CSD_maskTable[slot];
                        
            for (j=0u; j < snsNumber; j++)
            {
                CSD_DisableSensor(index[j]);
            }
        } 
    }
#endif  /* CSD_IS_COMPLEX_SCANSLOTS */


/*******************************************************************************
* Function Name: CSD_EnableSensor
********************************************************************************
*
* Summary:
*  Configures the selected sensor to measure during the next measurement cycle.
*  The corresponding pins are set to Analog High-Z mode and connected to the
*  Analog Mux Bus. This also enables the comparator function.
*
* Parameters:
*  sensor:  Sensor number.
*
* Return:
*  None
*
* Global Variables:
*  CSD_portTable[] - used to store the port number that pin 
*  belongs to for every sensor.
*  CSD_maskTable[] - used to store the pin within the port for 
*  every sensor.
*  CSD_csTable[]   - used to store the pointers to CAPS_SEL 
*  registers for every port.
*  CSD_pcTable[]   - used to store the pointers to PC pin 
*  register for every sensor.
*  CSD_amuxIndex[] - used to store corrected AMUX index when 
*  complex sensors are defeined.
*
*******************************************************************************/
void CSD_EnableSensor(uint8 sensor) CYREENTRANT
{
    uint8 port = CSD_portTable[sensor];
    uint8 mask = CSD_maskTable[sensor];
    
    #if ((CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) && \
         (CSD_IS_COMPLEX_SCANSLOTS))
        uint8 amuxCh = CSD_amuxIndex[sensor];
    #endif  /* ((CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) && \
            *   (CSD_IS_COMPLEX_SCANSLOTS))
            */
    
    /* Make sensor High-Z */
    *CSD_pcTable[sensor] = CSD_PRT_PC_HIGHZ;
    
    /* Connect to DSI output */
	if(port == 15u)
	{
		port = 7u;
	}
    *CSD_csTable[port] |= mask;
    
    /* Connect to AMUX */
    #if (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN)
        #if (CSD_IS_COMPLEX_SCANSLOTS)
            CSD_AMuxCH0_Connect(CSD_amuxIndex[sensor]);
        #else
            CSD_AMuxCH0_Connect(sensor);
        #endif  /* CSD_IS_COMPLEX_SCANSLOTS */
                
    #else
        #if (CSD_IS_COMPLEX_SCANSLOTS)
            if ((amuxCh & CSD_CHANNEL1_FLAG) == 0u)
            {
                CSD_AMuxCH0_Connect(amuxCh);
            } 
            else
            {
                amuxCh &= ~ CSD_CHANNEL1_FLAG;
                CSD_AMuxCH1_Connect(amuxCh);
            }
            
        #else
            if (sensor < CSD_TOTAL_SENSOR_COUNT__CH0) 
            {
                CSD_AMuxCH0_Connect(sensor);
            } 
            else
            {
                CSD_AMuxCH1_Connect(sensor - CSD_TOTAL_SENSOR_COUNT__CH0);
            }
            
        #endif  /* CSD_IS_COMPLEX_SCANSLOTS */
        
    #endif  /* CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN */
}


/*******************************************************************************
* Function Name: CSD_DisableSensor
********************************************************************************
*
* Summary:
*  Disables the selected sensor. The corresponding pin is disconnected from the
*  Analog Mux Bus and connected to GND, High_Z or Shield electrode.
*
* Parameters:
*  sensor:  Sensor number
*
* Return:
*  None
*
* Global Variables:
*  CSD_portTable[] - used to store the port number that pin 
*  belongs to for every sensor.
*  CSD_maskTable[] - used to store the pin within the port for 
*  every sensor.
*  CSD_csTable[]   - used to store the pointers to CAPS_SEL 
*  registers for every port.
*  CSD_pcTable[]   - used to store the pointers to PC pin 
*  register for every sensor.
*  CSD_amuxIndex[] - used to store corrected AMUX index when 
*  complex sensors are defeined.
*
*******************************************************************************/
void CSD_DisableSensor(uint8 sensor) CYREENTRANT
{
    uint8 port = CSD_portTable[sensor];
    uint8 mask = CSD_maskTable[sensor];
    
    #if ((CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) && \
         (CSD_IS_COMPLEX_SCANSLOTS))
        uint8 amuxCh = CSD_amuxIndex[sensor];
    #endif  /* ((CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) && \
            *   (CSD_IS_COMPLEX_SCANSLOTS))
            */
    
    /* Disconnect from AMUX */
    #if (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN)
        #if (CSD_IS_COMPLEX_SCANSLOTS)
            CSD_AMuxCH0_Disconnect(CSD_amuxIndex[sensor]);
        #else
            CSD_AMuxCH0_Disconnect(sensor);
        #endif  /* CSD_IS_COMPLEX_SCANSLOTS */
                
    #else
        #if (CSD_IS_COMPLEX_SCANSLOTS)
            if ((amuxCh & CSD_CHANNEL1_FLAG) == 0u)
            {
                CSD_AMuxCH0_Disconnect(amuxCh);
            } 
            else
            {
                amuxCh &= ~ CSD_CHANNEL1_FLAG;
                CSD_AMuxCH1_Disconnect(amuxCh);
            }
            
        #else
            if (sensor < CSD_TOTAL_SENSOR_COUNT__CH0) 
            {
                CSD_AMuxCH0_Disconnect(sensor);
            } 
            else
            {
                CSD_AMuxCH1_Disconnect(sensor - CSD_TOTAL_SENSOR_COUNT__CH0);
            }
            
        #endif  /* CSD_IS_COMPLEX_SCANSLOTS */
        
    #endif  /* CSD_DESIGN_TYPE */
    
    /* Disconnect from DSI output */
	if(port == 15u)
	{
		port = 7u;
	}
    *CSD_csTable[port] &= (uint8)~mask;
    
    /* Set sensor to inactive state */
    #if (CSD_CONNECT_INACTIVE_SNS == CSD_CIS_GND)
        *CSD_pcTable[sensor] = CSD_PRT_PC_GND;
    #elif (CSD_CONNECT_INACTIVE_SNS == CSD_CIS_HIGHZ)
        *CSD_pcTable[sensor] = CSD_PRT_PC_HIGHZ;
    #else
        *CSD_pcTable[sensor] = CSD_PRT_PC_SHIELD;
    #endif  /* (CSD_CONNECT_INACTIVE_SNS == CSD_CIS_GND) */
}


/*******************************************************************************
* Function Name: CSD_PreScan
********************************************************************************
*
* Summary:
*  Set required settings, enable sensor, remove Vref from AMUX and start the 
*  scanning process of the sensor.
*
* Parameters:
*  sensor:  Sensor number.
*
* Return:
*  None
*
* Global Variables:
*  CSD_rbTable[] - used to store pointers to PC pin registers for 
*  every bleed resistor (Rb). Only available when Current Source is External 
*  resistor.
*
*******************************************************************************/
void CSD_PreScan(uint8 sensor) CYREENTRANT
{
    /* Set Sensor Settings */
    CSD_SetScanSlotSettings(sensor);
    
    /* Place disable interrupts here to eliminate influence on start of scanning */
    /* `#START CSD_PreScan_DisableInt` */

    /* `#END` */
    
    /* Resets digital and pre-charge clocks */
    CSD_CONTROL_REG |= CSD_CTRL_SYNC_EN;
        
    #if (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN)
        #if (CSD_CURRENT_SOURCE == CSD_IDAC_SOURCE)
            /* Disable Vref from AMux */
            #if (CSD_VREF_VDAC == CSD_VREF_OPTIONS)
                CSD_AMuxCH0_Disconnect(CSD_AMuxCH0_VREF_CHANNEL);
            #else
                CSD_BufCH0_CAPS_CFG0_REG &= ~CSD_CSBUF_ENABLE;
            #endif  /* (CSD_VREF_VDAC != CSD_VREF_OPTIONS) */

            /* Enable Sensor */
            CSD_EnableScanSlot(sensor);
            
        #elif (CSD_CURRENT_SOURCE == CSD_IDAC_SINK)
            /* Connect IDAC */
            CSD_AMuxCH0_Connect(CSD_AMuxCH0_IDAC_CHANNEL);
            
            /* Enable Sensor */
            CSD_EnableScanSlot(sensor);
                
            /* Disable CapSense Buffer */
            CSD_BufCH0_CAPS_CFG0_REG &= (uint8)~CSD_CSBUF_ENABLE;
            
        #else
            /* Connect DSI output to Rb */
            *CSD_rbTable[CSD_extRbCh0Cur] |= CSD_BYP_MASK;
            
            /* Enable Sensor */
            CSD_EnableScanSlot(sensor);
             
            /* Disable CapSense Buffer */
            CSD_BufCH0_CAPS_CFG0_REG &= ~CSD_CSBUF_ENABLE;
        
        #endif  /* (CSD_CURRENT_SOURCE == CSD_IDAC_SOURCE) */
        
    #else

        if((CSD_CONTROL_REG & CSD_CTRL_WINDOW_EN__CH0) != 0u)
        {
            #if (CSD_CURRENT_SOURCE == CSD_IDAC_SOURCE)
                /* Disable Vref from AMux */
                #if (CSD_VREF_VDAC == CSD_VREF_OPTIONS)
                    CSD_AMuxCH0_Disconnect(CSD_AMuxCH0_VREF_CHANNEL);
                #else
                    CSD_BufCH0_CAPS_CFG0_REG &= ~CSD_CSBUF_ENABLE;
                #endif  /* (CSD_VREF_VDAC != CSD_VREF_OPTIONS) */
                
                /* Enable Sensor */
                CSD_EnableScanSlot(sensor);
                
            #elif (CSD_CURRENT_SOURCE == CSD_IDAC_SINK)
                /* Connect IDAC */
                CSD_AMuxCH0_Connect(CSD_AMuxCH0_IDAC_CHANNEL);
                
                /* Enable Sensor */
                CSD_EnableScanSlot(sensor);
                    
                /* Disable Vref from AMux */
                CSD_BufCH0_CAPS_CFG0_REG &= ~CSD_CSBUF_ENABLE;
                
            #else
                /* Connect DSI output to Rb */
                *CSD_rbTable[CSD_extRbCh0Cur] |= CSD_BYP_MASK;
                
                /* Enable Sensor */
                CSD_EnableScanSlot(sensor);
                    
                /* Disable Vref from AMux */
                CSD_BufCH0_CAPS_CFG0_REG &= ~CSD_CSBUF_ENABLE;
            
            #endif  /* (CSD_CURRENT_SOURCE == CSD_IDAC_SOURCE) */
            
        }
        
        if((CSD_CONTROL_REG & CSD_CTRL_WINDOW_EN__CH1) != 0u)
        {
            sensor += CSD_TOTAL_SENSOR_COUNT__CH0;
            
            #if (CSD_CURRENT_SOURCE == CSD_IDAC_SOURCE)
                /* Disable Vref from AMux */
                #if (CSD_VREF_VDAC == CSD_VREF_OPTIONS)
                   CSD_AMuxCH1_Disconnect(CSD_AMuxCH1_VREF_CHANNEL);
                #else 
                    CSD_BufCH1_CAPS_CFG0_REG &= ~CSD_CSBUF_ENABLE;
                #endif  /* (CSD_VREF_VDAC == CSD_VREF_OPTIONS) */
                
                /* Enable Sensor */
                CSD_EnableScanSlot(sensor);
                
            #elif (CSD_CURRENT_SOURCE == CSD_IDAC_SINK)
                /* Connect IDAC */
                CSD_AMuxCH1_Connect(CSD_AMuxCH1_IDAC_CHANNEL);
                
                /* Enable Sensor */
                CSD_EnableScanSlot(sensor);
                    
                /* Disable Vref from AMux */
                CSD_BufCH1_CAPS_CFG0_REG &= ~CSD_CSBUF_ENABLE;
                
            #else
                /* Connect DSI output to Rb */
                *CSD_rbTable[CSD_extRbCh1Cur] |= CSD_BYP_MASK;
                
                /* Enable Sensor */
                CSD_EnableScanSlot(sensor);
                
                /* Disable Vref from AMux */
                CSD_BufCH1_CAPS_CFG0_REG &= ~CSD_CSBUF_ENABLE;
            
            #endif  /* (CSD_CURRENT_SOURCE == CSD_IDAC_SOURCE) */
        }
    
    #endif  /* (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN) */
    
    /* Start measurament, pre-charge clocks are running and PRS as well */
    CSD_CONTROL_REG |= CSD_CTRL_START;
    
    /* Place enable interrupts here to eliminate influence on start of scanning */
    /* `#START CSD_PreScan_EnableInt` */

    /* `#END` */
}


#if (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN)
    /*******************************************************************************
    * Function Name: CSD_PostScan
    ********************************************************************************
    *
    * Summary:
    *  Store results of measurament in CSD_SensorResult[] array,
    *  sets scan sensor in none sampling state, turn off Idac(Current Source IDAC),
    *  disconnect IDAC(Sink mode) or bleed resistor (Rb) and apply Vref on AMUX.
    *  Only one channel designs.
    *
    * Parameters:
    *  sensor:  Sensor number.
    *
    * Return:
    *  None
    *
    * Global Variables:
    *  CSD_sensorRaw[] - used to store sensors raw data.
    *
    * Reentrant:
    *  No
    *
    *******************************************************************************/
    void CSD_PostScan(uint8 sensor) CYREENTRANT
    {
        /* Stop Capsensing and rearm sync */
        CSD_CONTROL_REG &= (uint8)~(CSD_CTRL_START | CSD_CTRL_SYNC_EN);
        
        /* Read SlotResult from Raw Counter */
        #if (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_FF)
            CSD_sensorRaw[sensor]  = CSD_MEASURE_FULL_RANGE - 
                                                      CY_GET_REG16(CSD_RAW_CH0_COUNTER_PTR);
        #else
            CSD_sensorRaw[sensor]  = ((uint16) CSD_RAW_CH0_COUNTER_HI_REG << 8u);
            CSD_sensorRaw[sensor] |= (uint16) CSD_RAW_CH0_COUNTER_LO_REG;
            CSD_sensorRaw[sensor]  = CSD_MEASURE_FULL_RANGE -
                                                      CSD_sensorRaw[sensor];
        #endif  /* (CSD_IMPLEMENTATION == CSD_MEASURE_IMPLEMENTATION_FF) */
        
        /* Disable Sensor */
        CSD_DisableScanSlot(sensor);
        
        #if(CSD_CURRENT_SOURCE)
            /* Turn off IDAC */
            CSD_IdacCH0_SetValue(CSD_TURN_OFF_IDAC);
            #if (CSD_CURRENT_SOURCE == CSD_IDAC_SINK)
                /* Disconnect IDAC */
                CSD_AMuxCH0_Disconnect(CSD_AMuxCH0_IDAC_CHANNEL);
            #endif  /* (CSD_CURRENT_SOURCE == CSD_IDAC_SINK) */
        #else
            /* Disconnect DSI output from Rb */
            *CSD_rbTable[CSD_extRbCh0Cur] &= ~CSD_BYP_MASK; 
        #endif  /* (CSD_CURRENT_SOURCE)*/
            
        /* Enable Vref on AMUX */
        #if (CSD_VREF_OPTIONS == CSD_VREF_VDAC)
            CSD_AMuxCH0_Connect(CSD_AMuxCH0_VREF_CHANNEL);
        #else
            CSD_BufCH0_CAPS_CFG0_REG |= CSD_CSBUF_ENABLE;
        #endif  /* (CSD_VREF_VDAC == CSD_VREF_OPTIONS) */
    }
    
#else

    /*******************************************************************************
    * Function Name: CSD_PostScan
    ********************************************************************************
    *
    * Summary:
    *  Store results of measurament in CSD_SensorResult[] array,
    *  sets scan sensor in none sampling state, turn off Idac(Current Source IDAC),
    *  disconnect IDAC(Sink mode) or bleed resistor (Rb) and apply Vref on AMUX.
    *  Only used for channel 0 in two channes designs.
    *
    * Parameters:
    *  sensor:  Sensor number.
    *
    * Return:
    *  None
    *
    * Global Variables:
    *  CSD_sensorRaw[] - used to store sensors raw data.
    *
    * Reentrant:
    *  No
    *
    *******************************************************************************/
    void CSD_PostScanCh0(uint8 sensor) CYREENTRANT
    {
        if (((CSD_CONTROL_REG & CSD_CTRL_WINDOW_EN__CH0) == 0u) && 
            ((CSD_CONTROL_REG & CSD_CTRL_WINDOW_EN__CH1) == 0u)) 
        {
            /* Stop Capsensing and rearm sync */
            CSD_CONTROL_REG &= ~(CSD_CTRL_START | CSD_CTRL_SYNC_EN);
        }
        
        /* Read SlotResult from Raw Counter */
        #if (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_FF)
            CSD_sensorRaw[sensor]  = CSD_MEASURE_FULL_RANGE - 
                                                      CY_GET_REG16(CSD_RAW_CH0_COUNTER_PTR);
        #else
            CSD_sensorRaw[sensor]  = ((uint16) CSD_RAW_CH0_COUNTER_HI_REG << 8u);
            CSD_sensorRaw[sensor] |= (uint16) CSD_RAW_CH0_COUNTER_LO_REG;
            CSD_sensorRaw[sensor]  = CSD_MEASURE_FULL_RANGE - 
                                                      CSD_sensorRaw[sensor];
        #endif  /* (CSD_IMPLEMENTATION_CH0 == CSD_MEASURE_IMPLEMENTATION_FF)*/
        
        /* Disable Sensor */
        CSD_DisableScanSlot(sensor);
        
        #if (CSD_CURRENT_SOURCE)
            /* Turn off IDAC */
            CSD_IdacCH0_SetValue(CSD_TURN_OFF_IDAC);
            #if (CSD_CURRENT_SOURCE == CSD_IDAC_SINK)
                /* Disconnect IDAC */
                CSD_AMuxCH0_Disconnect(CSD_AMuxCH0_IDAC_CHANNEL);
            #endif  /* (CSD_CURRENT_SOURCE == CSD_IDAC_SINK) */
        #else
            /* Disconnect DSI output from Rb */
            *CSD_rbTable[CSD_extRbCh0Cur] &= ~CSD_BYP_MASK; 
        #endif  /* (CSD_CURRENT_SOURCE)*/
        
        /* Enable Vref on AMUX */
        #if (CSD_VREF_OPTIONS == CSD_VREF_VDAC)
            CSD_AMuxCH0_Connect(CSD_AMuxCH0_VREF_CHANNEL);
        #else
            CSD_BufCH0_CAPS_CFG0_REG |= CSD_CSBUF_ENABLE;
        #endif  /* (CSD_VREF_VDAC == CSD_VREF_OPTIONS) */
    }
    
    
    /*******************************************************************************
    * Function Name: CSD_PostScanCh1
    ********************************************************************************
    *
    * Summary:
    *  Store results of measurament in CSD_SensorResult[] array,
    *  sets scan sensor in none sampling state, turn off Idac(Current Source IDAC), 
    *  disconnect IDAC(Sink mode) or bleed resistor (Rb) and apply Vref on AMUX.
    *  Only used for channel 1 in two channes designs.
    *
    * Parameters:
    *  sensor:  Sensor number.
    *
    * Return:
    *  None
    *
    * Global Variables:
    *  CSD_sensorRaw[] - used to store sensors raw data.
    *
    * Reentrant:
    *  No
    *
    *******************************************************************************/
    void CSD_PostScanCh1(uint8 sensor) CYREENTRANT
    {
        if (((CSD_CONTROL_REG & CSD_CTRL_WINDOW_EN__CH0) == 0u) && 
            ((CSD_CONTROL_REG & CSD_CTRL_WINDOW_EN__CH1) == 0u))
        {
            /* Stop Capsensing and rearm sync */
            CSD_CONTROL_REG &= ~(CSD_CTRL_START | CSD_CTRL_SYNC_EN);
        }
        
        /* Read SlotResult from Raw Counter */
        #if (CSD_IMPLEMENTATION_CH1 == CSD_MEASURE_IMPLEMENTATION_FF)
            CSD_sensorRaw[sensor]  = CSD_MEASURE_FULL_RANGE - 
                                                      CY_GET_REG16(CSD_RAW_CH1_COUNTER_PTR);
        #else
            CSD_sensorRaw[sensor]  = ((uint16) CSD_RAW_CH1_COUNTER_HI_REG << 8u);
            CSD_sensorRaw[sensor] |= (uint16) CSD_RAW_CH1_COUNTER_LO_REG;
            CSD_sensorRaw[sensor]  = CSD_MEASURE_FULL_RANGE - 
                                                      CSD_sensorRaw[sensor];
        #endif  /* (CSD_IMPLEMENTATION_CH1 == CSD_MEASURE_IMPLEMENTATION_FF)*/
        
        /* Disable Sensor */
        CSD_DisableScanSlot(sensor);
        
        #if (CSD_CURRENT_SOURCE)
            /* Turn off IDAC */
            CSD_IdacCH1_SetValue(CSD_TURN_OFF_IDAC);
            #if (CSD_CURRENT_SOURCE == CSD_IDAC_SINK)
                /* Disconnect IDAC */
                CSD_AMuxCH1_Disconnect(CSD_AMuxCH1_IDAC_CHANNEL);
            #endif  /* (CSD_CURRENT_SOURCE == CSD_IDAC_SINK) */
        #else
            /* Disconnect DSI output from Rb */
            *CSD_rbTable[CSD_extRbCh1Cur] &= ~CSD_BYP_MASK; 
        #endif  /* (CSD_CURRENT_SOURCE)*/

        /* Enable Vref on AMUX */
        #if (CSD_VREF_OPTIONS == CSD_VREF_VDAC)
            CSD_AMuxCH1_Connect(CSD_AMuxCH1_VREF_CHANNEL);
        #else
            CSD_BufCH1_CAPS_CFG0_REG |= CSD_CSBUF_ENABLE;
        #endif  /* (CSD_VREF_VDAC == CSD_VREF_OPTIONS) */
    }
    
#endif  /* CSD_DESIGN_TYPE */


#if (CSD_CURRENT_SOURCE == CSD_EXTERNAL_RB)
    /*******************************************************************************
    * Function Name:  CSD_InitRb
    ********************************************************************************
    *
    * Summary:
    *  Sets all Rbleed resistor to High-Z mode. The first Rbleed resistor is active
    *  while next measure.
    *  This function is available only if Current Source is External Resistor.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    ********************************************************************************/
    void CSD_InitRb(void) 
    {
        uint8 i;
        
        /* Disable all Rb */
        for(i = 0u; i < CSD_TOTAL_RB_NUMBER; i++)
        {
            /* Make High-Z */
            *CSD_rbTable[i] = CSD_PRT_PC_HIGHZ;
        }
    }
    
    
    /*******************************************************************************
    * Function Name: CSD_SetRBleed
    ********************************************************************************
    *
    * Summary:
    *  Sets the pin to use for the bleed resistor (Rb) connection. This function
    *  can be called at runtime to select the current Rb pin setting from those 
    *  defined customizer. The function overwrites the component parameter setting. 
    *  This function is available only if Current Source is External Resistor.
    * 
    * Parameters:
    *  rbleed:  Ordering number for bleed resistor terminal defined in CapSense
    *  customizer.
    *
    * Return:
    *  None
    *
    * Global Variables:
    *  CSD_extRbCh0Cur - used to store current number of active 
    *  bleed resistor (Rb) of channel 0.
    *  CSD_extRbCh1Cur - used to store current number of active 
    *  bleed resistor (Rb) of channel 1.
    *  The active bleed resistor (Rb) pin will be used while next measurement  
    *  cycle.
    *
    * Reentrant:
    *  No
    *
    *******************************************************************************/
    void CSD_SetRBleed(uint8 rbleed) 
    {
        #if (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN)
            CSD_extRbCh0Cur = rbleed;
            
        #else
            if(rbleed < CSD_TOTAL_RB_NUMBER__CH0)
            {
                CSD_extRbCh0Cur = rbleed;
            }
            else
            {
                CSD_extRbCh1Cur = (rbleed - CSD_TOTAL_RB_NUMBER__CH0);   
            }
    
        #endif  /* CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN */ 
    }
#endif /* CSD_CURRENT_SOURCE == CSD_EXTERNAL_RB */ 

#if (CSD_PRESCALER_OPTIONS)
    /*******************************************************************************
    * Function Name: CSD_SetPrescaler
    ********************************************************************************
    *
    * Summary:
    *  Sets analog switch divider.
    *
    * Parameters:
    *  prescaler:  Sets prescaler divider values.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void CSD_SetPrescaler(uint8 prescaler) CYREENTRANT
    {
        /* Set Prescaler */
        #if (CSD_PRESCALER_OPTIONS == CSD_PRESCALER_UDB)
            CSD_PRESCALER_PERIOD_REG = prescaler;
            CSD_PRESCALER_COMPARE_REG = (prescaler >> 1u);
        #elif (CSD_PRESCALER_OPTIONS == CSD_PRESCALER_FF)
            CY_SET_REG16(CSD_PRESCALER_PERIOD_PTR, (uint16) prescaler);
            CY_SET_REG16(CSD_PRESCALER_COMPARE_PTR, (uint16) (prescaler >> 1u));
        #else
            /* Do nothing = config without prescaler */
        #endif  /* (CSD_PRESCALER_OPTIONS == CSD_PRESCALER_UDB) */
    }


    /*******************************************************************************
    * Function Name: CSD_GetPrescaler
    ********************************************************************************
    *
    * Summary:
    *  Gets analog switch divider.
    *
    * Parameters:
    *  None
    *
    * Return:
    *   Returns the prescaler divider value.
    *
    *******************************************************************************/
    uint8 CSD_GetPrescaler(void) 
    {
        uint8 prescaler = 0u;

        /* Get Prescaler */
        #if (CSD_PRESCALER_OPTIONS == CSD_PRESCALER_UDB)
            prescaler = CSD_PRESCALER_PERIOD_REG;
            
        #elif (CSD_PRESCALER_OPTIONS == CSD_PRESCALER_FF)
            prescaler = (uint8) CY_GET_REG16(CSD_PRESCALER_PERIOD_PTR);
            
        #else
            /* Do nothing = config without prescaler */
        #endif  /* (CSD_PRESCALER_OPTIONS == CSD_PRESCALER_UDB) */
        
        return (prescaler);
    }
#endif  /* CSD_PRESCALER_OPTIONS */


/*******************************************************************************
* Function Name: CSD_SetScanSpeed
********************************************************************************
*
* Summary:
*  Sets ScanSpeed divider.
*
* Parameters:
*  scanspeed:  Sets ScanSpeed divider.
*
* Return:
*  None
*
*******************************************************************************/
void CSD_SetScanSpeed(uint8 scanSpeed) 
{
    CSD_SCANSPEED_PERIOD_REG = scanSpeed; 
}


#if (CSD_PRS_OPTIONS)
    /*******************************************************************************
    * Function Name: CSD_SetAnalogSwitchesSource
    ********************************************************************************
    *
    * Summary:
    *  Selects the Analog switches source between PRS and prescaler. It is useful
    *  for sensor capacitance determination for sensors with low self-capacitance.
    *  This function is used in auto-tuning procedure.
    *
    * Parameters:
    *  src:  analog switches source:
    *           CSD_ANALOG_SWITCHES_SRC_PRESCALER - selects prescaler
    *           CSD_ANALOG_SWITCHES_SRC_PRS - selects PRS
    *
    * Return:
    *  None
    *
    * Reentrant:
    *  No
    *******************************************************************************/
    void CSD_SetAnalogSwitchesSource(uint8 src)
                      
    {
        if(src == CSD_ANALOG_SWITCHES_SRC_PRESCALER)
        {
            CSD_CONTROL_REG &= (uint8)~0x10u;
        }
        else
        {
            CSD_CONTROL_REG |= 0x10u;
        }
    }
#endif /* (CSD_PRS_OPTIONS) */

/* [] END OF FILE */
