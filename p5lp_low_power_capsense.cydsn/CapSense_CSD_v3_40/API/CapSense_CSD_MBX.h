/*******************************************************************************
* File Name: `$INSTANCE_NAME`_MBX.h
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file provides constants and structure declarations for the tuner 
*  communication the for CapSense CSD component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CAPSENSE_CSD_MBX_`$INSTANCE_NAME`_H)
#define CY_CAPSENSE_CSD_MBX_`$INSTANCE_NAME`_H

#include "`$INSTANCE_NAME`.h"
#include "`$INSTANCE_NAME`_CSHL.h"


/***************************************
*   Condition compilation parameters
***************************************/

/* Check Sum to identify data */
#define `$INSTANCE_NAME`_CHECK_SUM      (`$CheckSum`u)

#define `$INSTANCE_NAME`_TOTAL_SENSOR_MASK_COUNT     (((`$INSTANCE_NAME`_TOTAL_SENSOR_COUNT - 1u) / 8u) + 1u)

/* Do nothing from big endian compilers (__C51__), for ARM compilers need to swap bytes */
#if defined(__C51__) || defined(__CX51__)
    #define `$INSTANCE_NAME`_SWAP_ENDIAN16(x) (x)
#else
    #define `$INSTANCE_NAME`_SWAP_ENDIAN16(x) ((uint16)(((uint16)(x) << 8) | ((x) >> 8)))
#endif  /* (defined(__C51__)) */

#if defined(__GNUC__) || defined(__ARMCC_VERSION)
    #define `$INSTANCE_NAME`_PACKED_STRUCT __attribute__ ((packed))
#elif defined(__ICCARM__)
    #define `$INSTANCE_NAME`_PACKED_STRUCT __packed
#else
    #define `$INSTANCE_NAME`_PACKED_STRUCT
#endif /* (defined(__GNUC__) || defined(__ARMCC_VERSION)) */

#if defined(__GNUC__)
	#define `$INSTANCE_NAME`_NO_STRICT_VOLATILE __attribute__((optimize("-fno-strict-volatile-bitfields")))
#else
	#define `$INSTANCE_NAME`_NO_STRICT_VOLATILE	
#endif /* (defined(__GNUC__)) */

/***************************************
*      Type defines for mailboxes
***************************************/

/* Outbox structure definition */
typedef struct
{       
    #if (`$INSTANCE_NAME`_TOTAL_CENTROIDS_COUNT)
        uint16   position[`$INSTANCE_NAME`_TOTAL_CENTROIDS_COUNT];
    #endif  /* (`$INSTANCE_NAME`_TOTAL_CENTROIDS_COUNT) */

    #if (`$INSTANCE_NAME`_TOTAL_MATRIX_BUTTONS_COUNT)
        uint8   mbPosition[`$INSTANCE_NAME`_TOTAL_MATRIX_BUTTONS_COUNT * 2u];
    #endif /* (`$INSTANCE_NAME`_TOTAL_MATRIX_BUTTONS_COUNT) */

    uint16   rawData[`$INSTANCE_NAME`_TOTAL_SENSOR_COUNT];
    uint16   baseLine[`$INSTANCE_NAME`_TOTAL_SENSOR_COUNT];
    `$SizeReplacementString`    signal[`$INSTANCE_NAME`_TOTAL_SENSOR_COUNT];
    uint8    onMask[`$INSTANCE_NAME`_TOTAL_SENSOR_MASK_COUNT];
    
    #if (`$INSTANCE_NAME`_TUNING_METHOD == `$INSTANCE_NAME`_AUTO_TUNING)
        `$SizeReplacementString`   fingerThreshold[`$INSTANCE_NAME`_WIDGET_CSHL_PARAMETERS_COUNT];
        `$SizeReplacementString`   noiseThreshold[`$INSTANCE_NAME`_WIDGET_CSHL_PARAMETERS_COUNT];
        uint8   scanResolution[`$INSTANCE_NAME`_WIDGET_RESOLUTION_PARAMETERS_COUNT];
        uint8   idacValue[`$INSTANCE_NAME`_TOTAL_SENSOR_COUNT];
		uint8   analogSwitchDivider[`$INSTANCE_NAME`_TOTAL_SCANSLOT_COUNT];
    #endif  /* (`$INSTANCE_NAME`_TUNING_METHOD == `$INSTANCE_NAME`_MANUAL_TUNING) */

    #if(`$INSTANCE_NAME`_TUNING_METHOD == `$INSTANCE_NAME`_MANUAL_TUNING)
        uint8   noReadMsg;
    #endif  /* (`$INSTANCE_NAME`_TUNING_METHOD == `$INSTANCE_NAME`_MANUAL_TUNING) */
    uint16  checkSum;
    
} `$INSTANCE_NAME`_PACKED_STRUCT `$INSTANCE_NAME`_OUTBOX;

/* Inbox structure definition */
#if (`$INSTANCE_NAME`_TUNING_METHOD == `$INSTANCE_NAME`_MANUAL_TUNING)
    typedef struct
    {
        uint8   sensorIndex;
        #if (`$INSTANCE_NAME`_CURRENT_SOURCE)
            uint8   idacSettings;
        #endif /* `$INSTANCE_NAME`_CURRENT_SOURCE */
        uint8   resolution;
        `$SizeReplacementString`   fingerThreshold;
        `$SizeReplacementString`   noiseThreshold;
        `$SizeReplacementString`   hysteresis;
        uint8   debounce;
        uint8   analogSwitchDivider;
    } `$INSTANCE_NAME`_PACKED_STRUCT `$INSTANCE_NAME`_INBOX;
    
#endif  /* (`$INSTANCE_NAME`_TUNING_METHOD == `$INSTANCE_NAME`_MANUAL_TUNING) */

/* Mailbox structure definition */
typedef struct
{
    uint8   type;               /* Must be the first byte with values ranging from 0-119 */
    uint16  size;               /* Must be the size of this data structure. Range between 0-127 */
    `$INSTANCE_NAME`_OUTBOX  outbox;
    #if (`$INSTANCE_NAME`_TUNING_METHOD == `$INSTANCE_NAME`_MANUAL_TUNING)
        `$INSTANCE_NAME`_INBOX inbox;
    #endif  /* (`$INSTANCE_NAME`_TUNING_METHOD == `$INSTANCE_NAME`_MANUAL_TUNING) */
    
} `$INSTANCE_NAME`_PACKED_STRUCT `$INSTANCE_NAME`_MAILBOX;


/* Mailboxes structure definition */
typedef struct
{
    uint8   numMailBoxes;       /* This must always be the first. Represents # of mailboxes */
    `$INSTANCE_NAME`_MAILBOX    csdMailbox;
} `$INSTANCE_NAME`_PACKED_STRUCT `$INSTANCE_NAME`_MAILBOXES;


/***************************************
*        Function Prototypes
***************************************/

`$INSTANCE_NAME`_NO_STRICT_VOLATILE void `$INSTANCE_NAME`_InitMailbox(volatile `$INSTANCE_NAME`_MAILBOX *mbx) \
                                  `=ReentrantKeil($INSTANCE_NAME . "_InitMailbox")`;
`$INSTANCE_NAME`_NO_STRICT_VOLATILE void `$INSTANCE_NAME`_PostMessage(volatile `$INSTANCE_NAME`_MAILBOX *mbx) \
                                  `=ReentrantKeil($INSTANCE_NAME . "_PostMessage")`;
#if (`$INSTANCE_NAME`_TUNING_METHOD == `$INSTANCE_NAME`_MANUAL_TUNING)
`$INSTANCE_NAME`_NO_STRICT_VOLATILE void `$INSTANCE_NAME`_ReadMessage(volatile `$INSTANCE_NAME`_MAILBOX *mbx) \
	                                  `=ReentrantKeil($INSTANCE_NAME . "_ReadMessage")`;
#endif  /* (`$INSTANCE_NAME`_TUNING_METHOD == `$INSTANCE_NAME`_MANUAL_TUNING) */


/***************************************
*           API Constants        
***************************************/

/* The selected ID for this version of Tuner */
#define `$INSTANCE_NAME`_TYPE_ID        (0x04u)

#define `$INSTANCE_NAME`_BUSY_FLAG      (`$INSTANCE_NAME`_TYPE_ID | 0x80u)
#define `$INSTANCE_NAME`_HAVE_MSG       (sizeof(`$INSTANCE_NAME`_MAILBOX) | 0x8000u)


#if (`$INSTANCE_NAME`_CURRENT_SOURCE)
    extern uint8 `$INSTANCE_NAME`_idacSettings[`$INSTANCE_NAME`_TOTAL_SENSOR_COUNT];
#endif /* `$INSTANCE_NAME`_CURRENT_SOURCE */

extern uint8 `$INSTANCE_NAME`_widgetResolution[`$INSTANCE_NAME`_WIDGET_RESOLUTION_PARAMETERS_COUNT];

extern `$SizeReplacementString` `$INSTANCE_NAME`_fingerThreshold[`$INSTANCE_NAME`_WIDGET_CSHL_PARAMETERS_COUNT];
extern `$SizeReplacementString` `$INSTANCE_NAME`_noiseThreshold[`$INSTANCE_NAME`_WIDGET_CSHL_PARAMETERS_COUNT];
extern `$SizeReplacementString` `$INSTANCE_NAME`_hysteresis[`$INSTANCE_NAME`_WIDGET_CSHL_PARAMETERS_COUNT];
extern uint8 `$INSTANCE_NAME`_debounce[`$INSTANCE_NAME`_WIDGET_CSHL_PARAMETERS_COUNT];
   
#if (`$INSTANCE_NAME`_TUNING_METHOD == `$INSTANCE_NAME`_AUTO_TUNING)
    extern uint8 `$INSTANCE_NAME`_lowLevelTuningDone;
    extern uint8 `$INSTANCE_NAME`_GetPrescaler(void);
#endif  /* (`$INSTANCE_NAME`_TUNING_METHOD == `$INSTANCE_NAME`_MANUAL_TUNING) */

#if ( (`$INSTANCE_NAME`_MULTIPLE_PRESCALER_ENABLED) || \
      (`$INSTANCE_NAME`_TUNING_METHOD == `$INSTANCE_NAME`_AUTO_TUNING) )
    extern uint8 `$INSTANCE_NAME`_analogSwitchDivider[`$INSTANCE_NAME`_TOTAL_SCANSLOT_COUNT];
#else
    extern uint8 `$INSTANCE_NAME`_analogSwitchDivider;
#endif /* ( (`$INSTANCE_NAME`_MULTIPLE_PRESCALER_ENABLED) || \
       *    (`$INSTANCE_NAME`_TUNING_METHOD == `$INSTANCE_NAME`_AUTO_TUNING) )
       */


#endif  /* (CY_CAPSENSE_CSD_MBX_`$INSTANCE_NAME`_H) */

/* [] END OF FILE */
