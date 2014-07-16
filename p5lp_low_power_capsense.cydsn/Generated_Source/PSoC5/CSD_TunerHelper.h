/*******************************************************************************
* File Name: CSD_TunerHelper.h
* Version 3.40
*
* Description:
*  This file provides constants and structure declarations for the tunner hepl
*  APIs for CapSense CSD component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CAPSENSE_CSD_TUNERHELPER_CSD_H)
#define CY_CAPSENSE_CSD_TUNERHELPER_CSD_H

#include "CSD.h"
#include "CSD_CSHL.h"
#if (CSD_TUNER_API_GENERATE)
    #include "CSD_MBX.h"
    #include "EZI2C.h"
#endif /* (CSD_TUNER_API_GENERATE) */


/***************************************
*     Constants for mailboxes
***************************************/

#define CSD_DEFAULT_MAILBOXES_NUMBER   (1u)


/***************************************
*        Function Prototypes
***************************************/

void CSD_TunerStart(void) ;
void CSD_TunerComm(void) ;

#if (CSD_TUNER_API_GENERATE)
    CSD_NO_STRICT_VOLATILE void CSD_ProcessAllWidgets(volatile CSD_OUTBOX *outbox)
	                                        					;

    extern volatile CSD_MAILBOXES CSD_mailboxesComm;
#endif /* (CSD_TUNER_API_GENERATE) */

#endif  /* (CY_CAPSENSE_CSD_TUNERHELPER_CSD_H)*/


/* [] END OF FILE */
