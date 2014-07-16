/*******************************************************************************
* File Name: CSD_INT.c
* Version 3.40
*
* Description:
*  This file provides the source code of Interrupt Service Routine (ISR)
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

#include "CSD.h"

/*******************************************************************************
*  Place your includes, defines and code here 
********************************************************************************/
/* `#START CSD_IsrCH_ISR_intc` */

/* `#END` */


#if (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN)
    /*******************************************************************************
    * Function Name: CSD_IsrCH0_ISR
    ********************************************************************************
    *
    * Summary:
    *  This ISR is executed when measure window is closed. The measure window 
    *  depends on scan resolution parameter of sensor. 
    *  The ISR has two modes: 
    *   1. Scans single sensor and stores measure result.
    *   2. Stores result of current measure and starts next scan till all 
    *      enabled sensors will be scanned.
    *  This interrupt handler only used in one channel designs.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    * Global Variables:
    *  CSD_csv - used to provide status and mode of scanning process.
    *   Mode - single scan or scan of all enabled sensors.
    *   Status - scan in progress or ready for new scan.
    *  CSD_sensorIndex - used to store sensor scanning sensor number.
    *
    * Reentrant:
    *  No
    *
    *******************************************************************************/
    CY_ISR(CSD_IsrCH0_ISR)
    {
        /*  Place your Interrupt code here. */
        /* `#START CSD_ONE_CHANNEL_IsrCH0_ISR_ENTER` */

        /* `#END` */
    
        /* Save results and disable sensor */
        CSD_PostScan(CSD_sensorIndex);
                
        if ((CSD_csv & CSD_SW_CTRL_SINGLE_SCAN) != 0u)
        {
            CSD_csv &= (uint8)~CSD_SW_STS_BUSY;
        }
        else
        {
            /* Proceed the scanning */
            CSD_sensorIndex = CSD_FindNextSensor(CSD_sensorIndex);
            
            /* Check end of scan */
            if(CSD_sensorIndex < CSD_TOTAL_SENSOR_COUNT)
            {
                CSD_PreScan(CSD_sensorIndex);
            }
            else
            {
                CSD_csv &= (uint8)~CSD_SW_STS_BUSY;
            } 
        }
		/*  Place your Interrupt code here. */
        /* `#START CSD_ONE_CHANNEL_IsrCH0_ISR_EXIT` */

        /* `#END` */
    }
 #endif  /* (CSD_DESIGN_TYPE == CSD_ONE_CHANNEL_DESIGN) */


 #if (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN)   
    /*******************************************************************************
    * Function Name: CSD_IsrCH0_ISR
    ********************************************************************************
    *
    * Summary:
    *  This ISR is executed when measure window is closed. The measure window 
    *  depends on scan resolution parameter of sensor. 
    *  The ISR has two modes: 
    *   1. Scans single sensor or pair of sensors and stores measure result.
    *   2. Stores results of current measure when complete scan of sensors pair or 
    *      sensor scan and starts next enabled sensors pair or sensor scan. 
    *  This interrupt handler only used in two channel designs.
    *  The synchronization is added to scanning process. The ISR channel 0 or 
    *  channel 1 stores the result and starts next scan. It depends which channel
    *  scans longer. The biggest scan resolution parameter of sensor within pair 
    *  defines scanning time of pair.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    * Global Variables:
    *  CSD_csv - used to provide status and mode of scanning process.
    *   Mode - single scan or scan of all enabled sensors.
    *   Status - scan in progress or ready for new scan.
    *  CSD_sensorIndex - used to store sensor scanning sensor number.
    *
    * Reentrant:
    *  No
    *
    *******************************************************************************/
    CY_ISR(CSD_IsrCH0_ISR)
    {
        /*  Place your Interrupt code here. */
        /* `#START CSD_TWO_CHANNELS_IsrCH0_ISR_ENTER` */

        /* `#END` */
        
        CSD_CONTROL_REG &= ~CSD_CTRL_WINDOW_EN__CH0;
        CSD_PostScanCh0(CSD_sensorIndex);
        
        if((CSD_CONTROL_REG & CSD_CTRL_WINDOW_EN__CH1) == 0u)
        {
            if ((CSD_csv & CSD_SW_CTRL_SINGLE_SCAN) != 0u)
            {
                CSD_csv &= (uint8)~CSD_SW_STS_BUSY;
            }
            else
            {
                /* Procced the scanning */
                CSD_sensorIndex = CSD_FindNextPair(CSD_sensorIndex);
                
                /* Check end of scan conditions */
                if((CSD_sensorIndex < CSD_TOTAL_SENSOR_COUNT__CH0) || 
                   (CSD_sensorIndex < CSD_TOTAL_SENSOR_COUNT__CH1))
                {
                    CSD_PreScan(CSD_sensorIndex);
                }
                else
                {
                    CSD_csv &= (uint8)~CSD_SW_STS_BUSY;
                }
            }
        }
        else
        {
            /* Do nothing */
        }
		/*  Place your Interrupt code here. */
        /* `#START CSD_TWO_CHANNELS_IsrCH0_ISR_EXIT` */

        /* `#END` */
    }

    
    /*******************************************************************************
    * Function Name: CSD_IsrCH1_ISR
    ********************************************************************************
    *
    * Summary:
    *  This ISR is executed when measure window is closed. The measure window 
    *  depends on scan resolution parameter of sensor. 
    *  The ISR has two modes: 
    *   1. Scans single sensor or pair of sensors and stores measure result.
    *   2. Stores results of current measure when complete scan of sensors pair or 
    *      sensor scan and starts next enabled sensors pair or sensor scan. 
    *  This interrupt handler only used in two channel designs.
    *  The synchronization is added to scanning process. The ISR channel 0 or 
    *  channel 1 stores the result and starts next scan. It depends which channel
    *  scans longer. The biggest scan resolution parameter of sensor within pair 
    *  defines scanning time of pair.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    * Global Variables:
    *  CSD_csv - used to provide status and mode of scanning process.
    *   Mode - single scan or scan of all enabled sensors.
    *   Status - scan in progress or ready for new scan.
    *  CSD_sensorIndex - used to store sensor scanning sensor number.
    *
    * Reentrant:
    *  No
    *
    *******************************************************************************/
    CY_ISR(CSD_IsrCH1_ISR)
    {
        /*  Place your Interrupt code here */
        /* `#START CSD_TWO_CHANNELS_IsrCH1_ISR_ENTER` */

        /* `#END` */
        
        CSD_CONTROL_REG &= (uint8)~CSD_CTRL_WINDOW_EN__CH1;
        CSD_PostScanCh1(CSD_sensorIndex + CSD_TOTAL_SENSOR_COUNT__CH0);
        
        if((CSD_CONTROL_REG & CSD_CTRL_WINDOW_EN__CH0) == 0u)
        {
            if ((CSD_csv & CSD_SW_CTRL_SINGLE_SCAN) != 0u)
            {
                CSD_csv &= (uint8)~CSD_SW_STS_BUSY;
            }
            else 
            {
                /* Procced the scanning */
                CSD_sensorIndex = CSD_FindNextPair(CSD_sensorIndex);
                
                /* Check end of scan conditions */
                if((CSD_sensorIndex < CSD_TOTAL_SENSOR_COUNT__CH0) || 
                   (CSD_sensorIndex < CSD_TOTAL_SENSOR_COUNT__CH1))
                {
                    CSD_PreScan(CSD_sensorIndex);
                }
                else
                {
                    CSD_csv &= ~CSD_SW_STS_BUSY;
                }
            }
        }
        else
        {
            /* Do nothing */
        }
		/*  Place your Interrupt code here */
        /* `#START CSD_TWO_CHANNELS_IsrCH1_ISR_EXIT` */

        /* `#END` */
    }
#endif  /* (CSD_DESIGN_TYPE == CSD_TWO_CHANNELS_DESIGN) */


/* [] END OF FILE */
