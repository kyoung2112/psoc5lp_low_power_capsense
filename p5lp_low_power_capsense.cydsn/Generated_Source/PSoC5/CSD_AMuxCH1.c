/*******************************************************************************
* File Name: CSD_AMuxCH1.c
* Version 3.40
*
*  Description:
*    This file contains all functions required for the analog multiplexer
*    CapSense_CSD_AMux User Module.
*
*   Note:
*
*******************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#include "CSD_AMuxCH1.h"

uint8 CSD_AMuxCH1_initVar = 0u;
uint8 CSD_AMuxCH1_lastChannel = CSD_AMuxCH1_NULL_CHANNEL;


/*******************************************************************************
* Function Name: CSD_AMuxCH1_Start
********************************************************************************
* Summary:
*  Disconnect all channels.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void CSD_AMuxCH1_Start(void)
{
    CSD_AMuxCH1_DisconnectAll();
    CSD_AMuxCH1_initVar = 1u;
}


/*******************************************************************************
* Function Name: CSD_AMuxCH1_Init
********************************************************************************
* Summary:
*  Disconnect all channels.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void CSD_AMuxCH1_Init(void)
{
    CSD_AMuxCH1_DisconnectAll();
}


/*******************************************************************************
* Function Name: CSD_AMuxCH1_Stop
********************************************************************************
* Summary:
*  Disconnect all channels.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void CSD_AMuxCH1_Stop(void)
{
    CSD_AMuxCH1_DisconnectAll();
}


/*******************************************************************************
* Function Name: CSD_AMuxCH1_Select
********************************************************************************
* Summary:
*  This functions first disconnects all channels then connects the given
*  channel.
*
* Parameters:
*  channel:  The channel to connect to the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void CSD_AMuxCH1_Select(uint8 channel) CYREENTRANT
{
    CSD_AMuxCH1_DisconnectAll();        /* Disconnect all previous connections */
    CSD_AMuxCH1_Connect(channel);       /* Make the given selection */
    CSD_AMuxCH1_lastChannel = channel;  /* Update last channel */
}


/*******************************************************************************
* Function Name: CSD_AMuxCH1_FastSelect
********************************************************************************
* Summary:
*  This function first disconnects the last connection made with FastSelect or
*  Select, then connects the given channel. The FastSelect function is similar
*  to the Select function, except it is faster since it only disconnects the
*  last channel selected rather than all channels.
*
* Parameters:
*  channel:  The channel to connect to the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void CSD_AMuxCH1_FastSelect(uint8 channel) CYREENTRANT
{
    /* Disconnect the last valid channel */
    if( CSD_AMuxCH1_lastChannel != CSD_AMuxCH1_NULL_CHANNEL)   /* Update last channel */
    {
        CSD_AMuxCH1_Disconnect(CSD_AMuxCH1_lastChannel);
    }

    /* Make the new channel connection */
    CSD_AMuxCH1_Connect(channel);
    CSD_AMuxCH1_lastChannel = channel;   /* Update last channel */
}


#if(CSD_AMuxCH1_MUXTYPE == CSD_AMuxCH1_MUX_DIFF)
    /*******************************************************************************
    * Function Name: CSD_AMuxCH1_Connect
    ********************************************************************************
    * Summary:
    *  This function connects the given channel without affecting other connections.
    *
    * Parameters:
    *  channel:  The channel to connect to the common terminal.
    *
    * Return:
    *  void
    *
    *******************************************************************************/
    void CSD_AMuxCH1_Connect(uint8 channel) CYREENTRANT
    {
        CSD_AMuxCH1_CYAMUXSIDE_A_Set(channel);
        CSD_AMuxCH1_CYAMUXSIDE_B_Set(channel);
    }
    
    
    /*******************************************************************************
    * Function Name: CSD_AMuxCH1_Disconnect
    ********************************************************************************
    * Summary:
    *  This function disconnects the given channel from the common or output
    *  terminal without affecting other connections.
    *
    * Parameters:
    *  channel:  The channel to disconnect from the common terminal.
    *
    * Return:
    *  void
    *
    *******************************************************************************/
    void CSD_AMuxCH1_Disconnect(uint8 channel) CYREENTRANT
    {
        CSD_AMuxCH1_CYAMUXSIDE_A_Unset(channel);
        CSD_AMuxCH1_CYAMUXSIDE_B_Unset(channel);
    }
    
#endif  /* End (CSD_AMuxCH1_MUXTYPE == CSD_AMuxCH1_MUX_DIFF) */


/*******************************************************************************
* Function Name: CSD_AMuxCH1_DisconnectAll
********************************************************************************
* Summary:
*  This function disconnects all channels.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void CSD_AMuxCH1_DisconnectAll(void) CYREENTRANT
{
    uint8 chan;

    #if(CSD_AMuxCH1_MUXTYPE == CSD_AMuxCH1_MUX_SINGLE)
        for(chan = 0; chan < CSD_AMuxCH1_CHANNELS ; chan++)
        {
            CSD_AMuxCH1_Unset(chan);
        }
    #else
        for(chan = 0; chan < CSD_AMuxCH1_CHANNELS ; chan++)
        {
            CSD_AMuxCH1_CYAMUXSIDE_A_Unset(chan);
            CSD_AMuxCH1_CYAMUXSIDE_B_Unset(chan);
        }
    #endif  /* End (CSD_AMuxCH1_MUXTYPE == CSD_AMuxCH1_MUX_SINGLE) */
}


/* [] END OF FILE */
