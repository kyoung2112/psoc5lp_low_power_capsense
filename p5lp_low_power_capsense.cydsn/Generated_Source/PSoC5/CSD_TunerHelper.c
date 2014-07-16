/*******************************************************************************
* File Name: CSD_TunerHelper.c
* Version 3.40
*
* Description:
*  This file provides the source code of Tuner helper APIs for the CapSense CSD 
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

#include "CSD_TunerHelper.h"

#if (CSD_TUNER_API_GENERATE)
    volatile CSD_MAILBOXES CSD_mailboxesComm;
#endif  /* (CSD_TUNER_API_GENERATE) */


/*******************************************************************************
* Function Name: CSD_TunerStart
********************************************************************************
*
* Summary:
*  Initializes CapSense CSD component and EzI2C communication componenet to use
*  mailbox data structure for communication with Tuner GUI.
*  Start the scanning, after initialization complete.
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
* Note:
*  All widgets are enabled by default except proximity widgets. Proximity widgets 
*  must be manually enabled as their long scan time is incompatible 
*  with the fast response required of other widget types. 
*
*******************************************************************************/
void CSD_TunerStart(void) 
{
    #if (CSD_TUNER_API_GENERATE)
        
        /* Init mbx and quick check */
        CSD_InitMailbox(&CSD_mailboxesComm.csdMailbox);
        CSD_mailboxesComm.numMailBoxes = CSD_DEFAULT_MAILBOXES_NUMBER;
        
        /* Start CapSense and baselines */
        CSD_Start();
        
        /* Initialize baselines */ 
        CSD_InitializeAllBaselines();
        CSD_InitializeAllBaselines();
        
        /* Start EzI2C, clears buf pointers */
        EZI2C_Start();
        
        /* Setup EzI2C buffers */
        EZI2C_SetBuffer1(sizeof(CSD_mailboxesComm), sizeof(CSD_mailboxesComm),
                                        (void *) &CSD_mailboxesComm);
        
        /* Starts scan all enabled sensors */
        CSD_ScanEnabledWidgets();
    
    #endif  /* (CSD_TUNER_API_GENERATE) */
}


/*******************************************************************************
* Function Name: CSD_TunerComm
********************************************************************************
*
* Summary:
*  This function is blocking. It waits till scaning loop is completed and apply
*  new parameters from Tuner GUI if available (manual tuning mode only). Updates
*  enabled baselines and state of widgets. Waits while Tuner GUI reports that 
*  content of mailbox could be modified. Then loads the report data into outbox 
*  and sets the busy flag. Starts new scanning loop.
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
void CSD_TunerComm(void) 
{
    #if (CSD_TUNER_API_GENERATE)
        if (0u == CSD_IsBusy())
        {   
            /* Apply new settings */
            #if (CSD_TUNING_METHOD == CSD_MANUAL_TUNING)
                CSD_ReadMessage(&CSD_mailboxesComm.csdMailbox);
            #endif  /* (CSD_TUNING_METHOD == CSD_MANUAL_TUNING) */

            /* Update all baselines and process all widgets */
            CSD_UpdateEnabledBaselines();
            CSD_ProcessAllWidgets(&CSD_mailboxesComm.csdMailbox.outbox);
            CSD_PostMessage(&CSD_mailboxesComm.csdMailbox);

            /* Enable EZI2C interrupts, after scan complete */
            EZI2C_EnableInt();

            while((CSD_mailboxesComm.csdMailbox.type != CSD_TYPE_ID) || \
                  ((EZI2C_GetActivity() & EZI2C_STATUS_BUSY) != 0u)){}
            
            /* Disable EZI2C interrupts, while scanning */
            EZI2C_DisableInt();
            
            /* Start scan all sensors */
            CSD_ScanEnabledWidgets();
        }
    #endif /* (CSD_TUNER_API_GENERATE) */
}


#if (CSD_TUNER_API_GENERATE)
    /*******************************************************************************
    * Function Name: CSD_ProcessAllWidgets
    ********************************************************************************
    *
    * Summary:
    *  Call required functions to update all widgets state:
    *   - CSD_GetCentroidPos() - calls only if linear sliders 
    *     available.
    *   - CSD_GetRadialCentroidPos() - calls only if radial slider 
    *     available.
    *   - CSD_GetTouchCentroidPos() - calls only if touch pad slider 
    *     available.
    *   - CSD_CheckIsAnyWidgetActive();
    *  The results of opeartions are copied to OUTBOX.
    *   
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    * Global Variables:
    *  CSD_OUTBOX outbox - structure which is used as ouput 
    *  buffer for report data to Tuner GUI.
    *  Update fields:
    *    - position[];
    *    - OnMask[];
    *
    * Reentrant:
    *  No
    *
    *******************************************************************************/
    void CSD_ProcessAllWidgets(volatile CSD_OUTBOX *outbox)
	                                        
    {
        uint8 i = 0u;
		#if (CSD_TOTAL_MATRIX_BUTTONS_COUNT)
        	volatile uint8 *mbPositionAddr;
		#endif /* (CSD_TOTAL_MATRIX_BUTTONS_COUNT) */

        #if (CSD_TOTAL_TOUCH_PADS_COUNT)
            uint16 pos[2];
        #endif  /* (CSD_TOTAL_TOUCH_PADS_COUNT) */
        
        #if ( (CSD_TOTAL_RADIAL_SLIDERS_COUNT) || (CSD_TOTAL_TOUCH_PADS_COUNT) || \
              (CSD_TOTAL_MATRIX_BUTTONS_COUNT) )
            uint8 widgetCnt;
        #endif  /* ((CSD_TOTAL_RADIAL_SLIDERS_COUNT) || (CSD_TOTAL_TOUCH_PADS_COUNT)) || 
                *   (CSD_TOTAL_MATRIX_BUTTONS_COUNT)
                */
        
        /* Calculate widget with centroids */
        #if (CSD_TOTAL_LINEAR_SLIDERS_COUNT)
            for(; i < CSD_TOTAL_LINEAR_SLIDERS_COUNT; i++)
            {
                outbox->position[i] = CSD_SWAP_ENDIAN16(CSD_GetCentroidPos(i));
            }
        #endif /* (CSD_TOTAL_LINEAR_SLIDERS_COUNT) */
        
        #if (CSD_TOTAL_RADIAL_SLIDERS_COUNT)
            widgetCnt = i;
            for(; i < (widgetCnt + CSD_TOTAL_RADIAL_SLIDERS_COUNT); i++)
            {
                outbox->position[i] = CSD_SWAP_ENDIAN16(CSD_GetRadialCentroidPos(i));
            }
        #endif /* (CSD_TOTAL_RADIAL_SLIDERS_COUNT) */
        
        #if (CSD_TOTAL_TOUCH_PADS_COUNT)
            widgetCnt = i;
            for(; i < (widgetCnt + (CSD_TOTAL_TOUCH_PADS_COUNT * 2u)); i = (i+2u))
            {
                if(CSD_GetTouchCentroidPos(i, pos) == 0u)
                {
                    outbox->position[i] = 0xFFFFu;
                    outbox->position[i+1u] = 0xFFFFu;
                }
                else
                {
                    outbox->position[i] = CSD_SWAP_ENDIAN16( (uint16) pos[0u]);
                    outbox->position[i+1u] = CSD_SWAP_ENDIAN16( (uint16) pos[1u]);
                }
            }
        #endif /* (CSD_TOTAL_TOUCH_PADS_COUNT) */

        #if (CSD_TOTAL_MATRIX_BUTTONS_COUNT)
            i += CSD_TOTAL_BUTTONS_COUNT;
            widgetCnt = 0u;
            for(; widgetCnt < (CSD_TOTAL_MATRIX_BUTTONS_COUNT * 2u); widgetCnt += 2u)
            {
                mbPositionAddr = &outbox->mbPosition[widgetCnt];
                if(CSD_GetMatrixButtonPos(i, ((uint8*) mbPositionAddr)) == 0u)
                {
                    outbox->mbPosition[widgetCnt] = 0xFFu;
                    outbox->mbPosition[widgetCnt+1u] = 0xFFu;
                }
                i += 2u;
            }
        #endif /* (CSD_TOTAL_MATRIX_BUTTONS_COUNT) */

        /* Update On/Off State */
        (void)CSD_CheckIsAnyWidgetActive();

        /* Copy OnMask */
        for(i = 0u; i < CSD_TOTAL_SENSOR_MASK_COUNT; i++)
        {
            outbox->onMask[i]  = CSD_sensorOnMask[i];
        }
    }
#endif /* (CSD_TUNER_API_GENERATE) */


/* [] END OF FILE */
