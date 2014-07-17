## PSoC5LP Low Power CapSense ##

This project implements low power CapSense on the PSoC5LP that runs on the CY8CKIT-050. The design uses two sensors (kit buttons P5_5, P5_6). It lights LED4 when a touch is detected. The scan interval is set on the SleepTimer component on the schematic. It uses manual tuning with firmware IDAC calibration to 83%. Very fast scan mode
is used with PRS disabled. #defines are used to disable sleeping for power measurement. Debug_Out is a debug pin on P0_0 used to scope the mode timing.
