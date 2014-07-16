/*******************************************************************************
* File Name: CSD_CSHL.h
* Version 3.40
*
* Description:
*  This file provides constants and parameter values for the High Level APIs
*  for CapSense CSD component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CAPSENSE_CSD_CSHL_CSD_H)
#define CY_CAPSENSE_CSD_CSHL_CSD_H

#include "CSD.h"


/***************************************
*   Condition compilation parameters
***************************************/

#define CSD_SIGNAL_SIZE                (8u)
#define CSD_AUTO_RESET                 (0u)
#define CSD_RAW_FILTER_MASK            (8u)

/* Signal size definition */
#define CSD_SIGNAL_SIZE_UINT8          (8u)
#define CSD_SIGNAL_SIZE_UINT16         (16u)

/* Auto reset definition */
#define CSD_AUTO_RESET_DISABLE         (0u)
#define CSD_AUTO_RESET_ENABLE          (1u)

/* Mask for RAW and POS filters */
#define CSD_MEDIAN_FILTER              (0x01u)
#define CSD_AVERAGING_FILTER           (0x02u)
#define CSD_IIR2_FILTER                (0x04u)
#define CSD_IIR4_FILTER                (0x08u)
#define CSD_JITTER_FILTER              (0x10u)
#define CSD_IIR8_FILTER                (0x20u)
#define CSD_IIR16_FILTER               (0x40u)


/***************************************
*           API Constants
***************************************/

/* Widgets constants definition */
#define CSD_BUTTON0__BTN        (0u)
#define CSD_BUTTON1__BTN        (1u)

#define CSD_TOTAL_DIPLEXED_SLIDERS_COUNT        (0u)
#define CSD_TOTAL_LINEAR_SLIDERS_COUNT          (0u)
#define CSD_TOTAL_RADIAL_SLIDERS_COUNT          (0u)
#define CSD_TOTAL_TOUCH_PADS_COUNT              (0u)
#define CSD_TOTAL_BUTTONS_COUNT                 (2u)
#define CSD_TOTAL_MATRIX_BUTTONS_COUNT          (0u)
#define CSD_TOTAL_GENERICS_COUNT                (0u)

#define CSD_POS_FILTERS_MASK        (0u)
#define CSD_LINEAR_SLIDERS_POS_FILTERS_MASK        (0u)
#define CSD_RADIAL_SLIDERS_POS_FILTERS_MASK        (0u)
#define CSD_TOUCH_PADS_POS_FILTERS_MASK        (0u)

#define CSD_UNUSED_DEBOUNCE_COUNTER_INDEX   (2u)


#define CSD_END_OF_SLIDERS_INDEX   (0u)
#define CSD_END_OF_TOUCH_PAD_INDEX   (0u)
#define CSD_END_OF_BUTTONS_INDEX   (1u)
#define CSD_END_OF_MATRIX_BUTTONS_INDEX   (1u)
#define CSD_END_OF_WIDGETS_INDEX   (2u)


#define CSD_TOTAL_SLIDERS_COUNT            ( CSD_TOTAL_LINEAR_SLIDERS_COUNT + \
                                                          CSD_TOTAL_RADIAL_SLIDERS_COUNT )
                                                          
#define CSD_TOTAL_CENTROIDS_COUNT          ( CSD_TOTAL_SLIDERS_COUNT + \
                                                         (CSD_TOTAL_TOUCH_PADS_COUNT*2u) )

#define CSD_TOTAL_WIDGET_COUNT             ( CSD_TOTAL_LINEAR_SLIDERS_COUNT + \
                                                          CSD_TOTAL_RADIAL_SLIDERS_COUNT + \
                                                          CSD_TOTAL_TOUCH_PADS_COUNT + \
                                                          CSD_TOTAL_BUTTONS_COUNT + \
                                                          CSD_TOTAL_MATRIX_BUTTONS_COUNT )
                                                           
#define CSD_ANY_POS_FILTER                 ( CSD_MEDIAN_FILTER | \
                                                          CSD_AVERAGING_FILTER | \
                                                          CSD_IIR2_FILTER | \
                                                          CSD_IIR4_FILTER | \
                                                          CSD_JITTER_FILTER )
                                                         
#define CSD_IS_DIPLEX_SLIDER               ( CSD_TOTAL_DIPLEXED_SLIDERS_COUNT > 0u)

#define CSD_IS_NON_DIPLEX_SLIDER           ( (CSD_TOTAL_LINEAR_SLIDERS_COUNT - \
                                                           CSD_TOTAL_DIPLEXED_SLIDERS_COUNT) > 0u)
#define CSD_ADD_SLIDER_TYPE                ((CSD_TOTAL_RADIAL_SLIDERS_COUNT > 0u) ? \
                                                        ((CSD_TOTAL_TOUCH_PADS_COUNT > 0u) || \
                                                         (CSD_TOTAL_LINEAR_SLIDERS_COUNT > 0u)) : 0u)
                                                         
#define CSD_WIDGET_CSHL_PARAMETERS_COUNT           (CSD_TOTAL_WIDGET_COUNT + \
                                                                 CSD_TOTAL_TOUCH_PADS_COUNT + \
                                                                 CSD_TOTAL_MATRIX_BUTTONS_COUNT)

#define CSD_WIDGET_RESOLUTION_PARAMETERS_COUNT     (CSD_WIDGET_CSHL_PARAMETERS_COUNT + \
                                                                 CSD_TOTAL_GENERICS_COUNT)
                                                                 
#define CSD_SENSORS_TBL_SIZE (CSD_TOTAL_LINEAR_SLIDERS_COUNT + \
                                        CSD_TOTAL_RADIAL_SLIDERS_COUNT + \
                                        CSD_TOTAL_BUTTONS_COUNT + \
                                        CSD_TOTAL_GENERICS_COUNT + \
                                        (CSD_TOTAL_TOUCH_PADS_COUNT * 2u) + \
                                        (CSD_TOTAL_MATRIX_BUTTONS_COUNT * 2u))
                                        
/*Types of centroids */
#define CSD_TYPE_RADIAL_SLIDER         (0x01u)
#define CSD_TYPE_LINEAR_SLIDER         (0x02u)
#define CSD_TYPE_GENERIC               (0xFFu)

/* Defines is slot active */
#define CSD_SENSOR_1_IS_ACTIVE     (0x01u)
#define CSD_SENSOR_2_IS_ACTIVE     (0x02u)
#define CSD_WIDGET_IS_ACTIVE       (0x01u)

/* Defines diplex type of Slider */
#define CSD_IS_DIPLEX              (0x80u)

/* Defines max fingers on TouchPad  */
#define CSD_POS_PREV               (0u)
#define CSD_POS                    (1u)
#define CSD_POS_NEXT               (2u)
#define CSD_CENTROID_ROUND_VALUE   (0x7F00u)

#define CSD_NEGATIVE_NOISE_THRESHOLD        (20u)
#define CSD_LOW_BASELINE_RESET              (5u)


/***************************************
*        Function Prototypes
***************************************/
void CSD_BaseInit(uint8 sensor) ;
void CSD_InitializeSensorBaseline(uint8 sensor) \
                                               ;
void CSD_InitializeAllBaselines(void) ;
void CSD_InitializeEnabledBaselines(void) \
                                                 ;
void CSD_UpdateSensorBaseline(uint8 sensor) ;
void CSD_UpdateEnabledBaselines(void) ;
uint8 CSD_CheckIsSensorActive(uint8 sensor) ;
uint8 CSD_CheckIsWidgetActive(uint8 widget) ;
uint8 CSD_CheckIsAnyWidgetActive(void) ;
void CSD_EnableWidget(uint8 widget) ;
void CSD_DisableWidget(uint8 widget) ;
#if (CSD_TOTAL_MATRIX_BUTTONS_COUNT)
    uint8 CSD_GetMatrixButtonPos(uint8 widget, uint8* pos) \
	                                          ;
#endif /* (CSD_TOTAL_MATRIX_BUTTONS_COUNT) */

#if (CSD_TOTAL_LINEAR_SLIDERS_COUNT)
    uint16 CSD_GetCentroidPos(uint8 widget) ;
#endif /* (CSD_TOTAL_LINEAR_SLIDERS_COUNT) */
#if (CSD_TOTAL_RADIAL_SLIDERS_COUNT)
    uint16 CSD_GetRadialCentroidPos(uint8 widget) \
                                                 ;
#endif /* (CSD_TOTAL_RADIAL_SLIDERS_COUNT) */
#if (CSD_TOTAL_TOUCH_PADS_COUNT)
    uint8 CSD_GetTouchCentroidPos(uint8 widget, uint16* pos) \
	                                           ;
#endif /* (CSD_TOTAL_TOUCH_PADS_COUNT) */

#if (CSD_IS_DIPLEX_SLIDER)
    uint8 CSD_FindMaximum(uint8 offset, uint8 count, uint8 fingerThreshold, const uint8 CYCODE *diplex)
                                       ;
#else 
    uint8 CSD_FindMaximum(uint8 offset, uint8 count, uint8 fingerThreshold)
                                       ;
#endif /* (CSD_IS_DIPLEX_SLIDER) */

#if(CSD_TOTAL_CENTROIDS_COUNT)
    uint8 CSD_CalcCentroid(uint8 maximum, uint8 offset, 
                                        uint8 count, uint16 resolution, uint8 noiseThreshold)
	                                    ;
#endif /* (CSD_TOTAL_CENTROIDS_COUNT) */

/* SmartSense functions */
#if (CSD_TUNING_METHOD == CSD_AUTO_TUNING)
    extern void CSD_CalculateThresholds(uint8 SensorNumber)
           ;
#endif /* (CSD_TUNING_METHOD == CSD_AUTO_TUNING) */

/* Median filter function prototype */
#if ( (CSD_RAW_FILTER_MASK & CSD_MEDIAN_FILTER) | \
      (CSD_POS_FILTERS_MASK & CSD_MEDIAN_FILTER) )
    uint16 CSD_MedianFilter(uint16 x1, uint16 x2, uint16 x3)
    ;
#endif /* CSD_RAW_FILTER_MASK && CSD_POS_FILTERS_MASK */

/* Averaging filter function prototype */
#if ( (CSD_RAW_FILTER_MASK & CSD_AVERAGING_FILTER) | \
      (CSD_POS_FILTERS_MASK & CSD_AVERAGING_FILTER) )
    uint16 CSD_AveragingFilter(uint16 x1, uint16 x2, uint16 x3)
    ;
#endif /* CSD_RAW_FILTER_MASK && CSD_POS_FILTERS_MASK */

/* IIR2Filter(1/2prev + 1/2cur) filter function prototype */
#if ( (CSD_RAW_FILTER_MASK & CSD_IIR2_FILTER) | \
      (CSD_POS_FILTERS_MASK & CSD_IIR2_FILTER) )
    uint16 CSD_IIR2Filter(uint16 x1, uint16 x2) ;
#endif /* CSD_RAW_FILTER_MASK && CSD_POS_FILTERS_MASK */

/* IIR4Filter(3/4prev + 1/4cur) filter function prototype */
#if ( (CSD_RAW_FILTER_MASK & CSD_IIR4_FILTER) | \
      (CSD_POS_FILTERS_MASK & CSD_IIR4_FILTER) )
    uint16 CSD_IIR4Filter(uint16 x1, uint16 x2) ;
#endif /* CSD_RAW_FILTER_MASK && CSD_POS_FILTERS_MASK */

/* IIR8Filter(7/8prev + 1/8cur) filter function prototype - RawCounts only */
#if (CSD_RAW_FILTER_MASK & CSD_IIR8_FILTER)
    uint16 CSD_IIR8Filter(uint16 x1, uint16 x2) ;
#endif /* CSD_RAW_FILTER_MASK */

/* IIR16Filter(15/16prev + 1/16cur) filter function prototype - RawCounts only */
#if (CSD_RAW_FILTER_MASK & CSD_IIR16_FILTER)
    uint16 CSD_IIR16Filter(uint16 x1, uint16 x2) ;
#endif /* CSD_RAW_FILTER_MASK */

/* JitterFilter filter function prototype */
#if ( (CSD_RAW_FILTER_MASK & CSD_JITTER_FILTER) | \
      (CSD_POS_FILTERS_MASK & CSD_JITTER_FILTER) )
    uint16 CSD_JitterFilter(uint16 x1, uint16 x2) ;
#endif /* CSD_RAW_FILTER_MASK && CSD_POS_FILTERS_MASK */

/* Storage of filters data */
#if ( (CSD_RAW_FILTER_MASK & CSD_MEDIAN_FILTER) | \
      (CSD_RAW_FILTER_MASK & CSD_AVERAGING_FILTER) )

    extern uint16 CSD_rawFilterData1[CSD_TOTAL_SENSOR_COUNT];
    extern uint16 CSD_rawFilterData2[CSD_TOTAL_SENSOR_COUNT];

#elif ( (CSD_RAW_FILTER_MASK & CSD_IIR2_FILTER)   | \
        (CSD_RAW_FILTER_MASK & CSD_IIR4_FILTER)   | \
        (CSD_RAW_FILTER_MASK & CSD_JITTER_FILTER) | \
        (CSD_RAW_FILTER_MASK & CSD_IIR8_FILTER)   | \
        (CSD_RAW_FILTER_MASK & CSD_IIR16_FILTER) )
        
    extern uint16 CSD_rawFilterData1[CSD_TOTAL_SENSOR_COUNT];

#else
    /* No Raw filters */
#endif  /* ( (CSD_RAW_FILTER_MASK & CSD_MEDIAN_FILTER) || \
        *    (CSD_RAW_FILTER_MASK & CSD_AVERAGING_FILTER) )
        */

extern uint8 CSD_sensorEnableMask[(((CSD_TOTAL_SENSOR_COUNT - 1u) / 8u) + 1u)];
extern const uint8 CYCODE CSD_widgetNumber[CSD_TOTAL_SENSOR_COUNT];

extern uint16 CSD_sensorBaseline[CSD_TOTAL_SENSOR_COUNT];
extern uint8 CSD_sensorBaselineLow[CSD_TOTAL_SENSOR_COUNT];
extern uint8 CSD_sensorSignal[CSD_TOTAL_SENSOR_COUNT];
extern uint8 CSD_sensorOnMask[(((CSD_TOTAL_SENSOR_COUNT - 1u) / 8u) + 1u)];

extern uint8 CSD_lowBaselineResetCnt[CSD_TOTAL_SENSOR_COUNT];

/****************************************************************************************
*       Obsolete definitions. Not recommended to use. Will be removed in future releases.
*****************************************************************************************/

/* Obsolete names of variables */
//#define CSD_SensorBaseline          CSD_sensorBaseline
//#define CSD_SensorBaselineLow       CSD_sensorBaselineLow
//#define CSD_SensorSignal            CSD_sensorSignal
//#define CSD_SensorOnMask            CSD_sensorOnMask
//#define CSD_LowBaselineResetCnt     CSD_lowBaselineResetCnt


#endif /* CY_CAPSENSE_CSD_CSHL_CSD_H */

/* [] END OF FILE */
