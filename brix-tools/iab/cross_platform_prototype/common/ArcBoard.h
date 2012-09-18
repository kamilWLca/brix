/*
 ****************************************************************************
 * (C) Copyright, Arcom Control Systems Inc.                                $
 *                                                                          $
 * $Workfile:: ArcBoard.h                                                   $
 * $Revision:: 137                                                          $
 * $Author:: admin                                                          $
 * $Modtime:: 1/22/97 8:29a                                                 $
 * $Archive:: /NTDriver/ArcBoard.Dll/ArcBoard.h                             $
 * $NoKeywords::                                                            $
 *                                                                          $
 ****************************************************************************
 */

// Include this file only once
#ifndef  __ARCBOARD_H_
#define  __ARCBOARD_H_

// Include Files
#include <windows.h>

typedef struct _SERIAL_PARAMS
{
    ULONG       BaseAddress;
    ULONG       PortNumber;
    ULONG       Baud;
    UCHAR       Parity;     // 'e', 'o', or 'n'
    UCHAR       DataBits;   // 7 or 8
    UCHAR       StopBits;   // 1 or 2
    UCHAR       Reserved;
} SERIAL_PARAMS,*PSERIAL_PARAMS;

//***************************************************************************
//************************* Global Constants ********************************
//***************************************************************************
// Version
#define  AN_VERSION           0                //Version Number
#define  AN_RELEASE           1                //Release Number   

// LED control
#define  AN_LED_OFF          (UCHAR) 0x00      //set LED OFF
#define  AN_LED_ON           (UCHAR) 0x01      //set LED ON

// Digital output control
#define  AN_DISABLE          (UCHAR) 0x00      //disbale Output groups
#define  AN_ENABLE           (UCHAR) 0x01      //enable Output groups
 
// Digital I/O groups
#define  AN_GROUP_0          (UCHAR) 0x00      //digital group 0
#define  AN_GROUP_1          (UCHAR) 0x01      //digital group 1
#define  AN_GROUP_2          (UCHAR) 0x02      //digital group 2
#define  AN_GROUP_3          (UCHAR) 0x03      //digital group 3
#define  AN_GROUP_4          (UCHAR) 0x04      //digital group 4

// Counters
#define  AN_COUNTER0         (UCHAR) 0x00      //counter no 0
#define  AN_COUNTER1         (UCHAR) 0x01      //counter no 1 
#define  AN_COUNTER2         (UCHAR) 0x02      //counter no 2
#define  AN_COUNTER3         (UCHAR) 0x03      //counter no 3
#define  AN_COUNTER4         (UCHAR) 0x04      //counter no 4 
#define  AN_COUNTER5         (UCHAR) 0x05      //counter no 5
#define  AN_COUNTER6         (UCHAR) 0x06      //counter no 6
#define  AN_COUNTER7         (UCHAR) 0x07      //counter no 7 
#define  AN_COUNTER8         (UCHAR) 0x08      //counter no 8

// Counter modes
#define  AN_CTC_MODE0        (UCHAR) 0x00      //Interrupt on Terminal Count
#define  AN_CTC_MODE1        (UCHAR) 0X02      //Hardware Retriggerable One-Shot
#define  AN_CTC_MODE2        (UCHAR) 0X04      //Rate Generator
#define  AN_CTC_MODE3        (UCHAR) 0X06      //Square Wave Mode
#define  AN_CTC_MODE4        (UCHAR) 0X08      //Software Triggered Strobe
#define  AN_CTC_MODE5        (UCHAR) 0X0A      //Hardware Triggered Strobe

// PCADADIO Dig-IO Nibble configuration
#define  AN_NIB_OUTPUT       (UCHAR) 0X00      //Nibble if for Digital Output
#define  AN_NIB_INPUT        (UCHAR) 0X01      //Nibble is for Digital Input

// PCAD12-16H AI gain ranges
#define  AN_GAIN_TEN         (UCHAR) 0x00      //10 volt range
#define  AN_GAIN_FIVE        (UCHAR) 0x01      //5 volt range
#define  AN_GAIN_ONE         (UCHAR) 0x02      //1 volt range
#define  AN_GAIN_HALF        (UCHAR) 0x03      //0.5 volt range

//PCSYSCON - constants
#define  MASK_BUZZER_CNTRL   (UCHAR) 0x07		// Mask to control what system faults will activate the buzzer
#define  MASK_RELAY_CNTRL    (UCHAR) 0x06		// Mask to control what system faults will activate the relay
#define  MASK_INT_CNTRL      (UCHAR) 0x05		// Mask to control what system faults will activate the interrupt
#define	 TWO_SECONDS	     (UCHAR) 0x00	    // Set watchdog for 2 second timer
#define	 TEN_SECONDS		 (UCHAR) 0x01		// Set watchdog for 10 second timer
#define	 ALARM_POWER_FAIL	 (UCHAR) 0x10    	// Alarm - +/-12V or 5V falls below DAC value
#define	 ALARM_OPT2_ERROR    (UCHAR) 0x08		// Alarm - Optional error input line
#define	 ALARM_OPT1_ERROR	 (UCHAR) 0x04		// Alarm - Optional error input line
#define	 ALARM_WDOG_ERROR    (UCHAR) 0x02		// Alarm - Watchdog timeout error
#define	 ALARM_OVER_TEMP     (UCHAR) 0x01		// Alarm - Over temperature fault



//***************************************************************************
//************************* Error Codes *************************************
//***************************************************************************
// General error codes
#define  AN_SUCCESS                  (ULONG) 0xFFFFFFFF //Success
#define  AN_ERR_EXCEPTION            (ULONG) 101      //An internal exception occured  
#define  AN_ERR_KERNELDRIVER_FAILED  (ULONG) 102
#define  AN_ERR_REGISTRYREAD_FAILED  (ULONG) 103      //Could not read info from registry
#define  AN_ERR_INVALID_BASEADDRESS  (ULONG) 104      //an invalid base address was specified
#define  AN_ERR_NO_BOARD_AT_ADDRESS  (ULONG) 105      //Specified board is not at the base address

// Error codes - DIO
#define  AN_ERR_BAD_DIO_GROUPNO      (ULONG) 201      //bad digital i/o group number
#define  AN_ERR_BAD_DIO_BITNO        (ULONG) 202      //bad digital i/o bit number
#define  AN_ERR_BAD_COUNTERNO        (ULONG) 203      //bad counter number
#define  AN_ERR_BAD_COUNTERMODE      (ULONG) 203      //bad counter mode

// Error codes - ADC-DAC
#define  AN_ERR_BAD_ANACHANNEL       (ULONG) 301      //bad channel number
#define  AN_ERR_BAD_ANAGAIN          (ULONG) 302      //bad gain type

// Error codes - PCADADAIO
#define  AN_ERR_BAD_DIGIOCONFIG      (ULONG) 401      //bad nibble configuration

//Errors - PCSYSCON
#define  AN_ERR_BAD_TIMEOUTVAL       (ULONG) 501      //bad timeout specified 
#define  AN_ERR_BAD_WATCHDOG         (ULONG) 502      //tried to reinit Watchdog before initializing it
#define  AN_ERR_BAD_MASKNO           (ULONG) 503      //bad Mask Number specified
#define  AN_ERR_BAD_MASKVALUE        (ULONG) 504      //bad value specified for mask
#define  AN_ERR_BAD_COMPARATOR_NO    (ULONG) 505      //bad comparator no. specified (only 0-3 valid)



//****************************************************************************
//*********************** Function Prototypes ********************************
//****************************************************************************
#define CCONV __stdcall 
# ifdef __cplusplus
extern "C" {
# endif /* __cplusplus */
//****************************************************************************

    
// Common functions
ULONG CCONV  ARCBOARD_Init (VOID);
ULONG CCONV  ARCBOARD_Close (VOID);


// LowLevel functions
ULONG CCONV  anOutputB (ULONG ulBase, UCHAR cValue);
ULONG CCONV  anInputB (ULONG ulBase, PUCHAR pcValue);
ULONG CCONV  anOutputW (ULONG ulBase, USHORT unValue);
ULONG CCONV  anInputW (ULONG ulBase, PUSHORT punValue);
ULONG CCONV  anBoardOutputB (ULONG ulBase, USHORT unOffset, UCHAR cValue);
ULONG CCONV  anBoardInputB (ULONG ulBase, USHORT unOffset, PUCHAR pcValue);

ULONG CCONV  anPcSer4OutB(ULONG ulBase, ULONG ulComPort, UCHAR cValue) ;
ULONG CCONV  anPcSer4InB(ULONG ulBase, ULONG ulComPort, PUCHAR pcValue, PULONG ulReturn) ;
ULONG CCONV  anPcSer4Init(PSERIAL_PARAMS pSP); // ULONG ulComPort);
ULONG CCONV  anPcSer4StatusInformation(
                        ULONG ulBase, 
                        ULONG ulComPort, 
                        ULONG bSetDtr,
                        ULONG bNewDtrState,
                        ULONG bSetRts,
                        ULONG bNewRtsState,
                        ULONG *RingBufferSize,
                        ULONG *ReceiveCount,
                        ULONG *TransmitCount,
                        ULONG *CtsState,
                        ULONG *CdState);
ULONG CCONV anPcSer4AllSent(ULONG ulBase, ULONG ulComPort, ULONG *pAllSent);
ULONG CCONV anPcSer4Flush(
                        ULONG ulBase, 
                        ULONG ulComPort, 
                        ULONG bFlushReceive,
                        ULONG bFlushTransmit) ;


// PCIB40 functions 
ULONG CCONV  PCIB40_Init (ULONG ulBase);
ULONG CCONV  PCIB40_UserLED (ULONG ulBase, UCHAR cAction);
ULONG CCONV  PCIB40_OutputsEnable (ULONG ulBase, UCHAR cAction);
ULONG CCONV  PCIB40_DigGroupRead (ULONG ulBase,  UCHAR cGroup, PUCHAR pcValue);
ULONG CCONV  PCIB40_DigGroupWrite (ULONG ulBase,  UCHAR cGroup, UCHAR cValue);
ULONG CCONV  PCIB40_DigBitRead (ULONG ulBase, UCHAR cBitNo, PUCHAR pcValue);
ULONG CCONV  PCIB40_DigBitWrite (ULONG ulBase, UCHAR cBitNo, UCHAR cValue);
ULONG CCONV  PCIB40_CounterInit (ULONG ulBase, UCHAR cCounterNo, UCHAR cMode, USHORT unNewCount);
ULONG CCONV  PCIB40_CounterRead (ULONG ulBase, UCHAR cCounterNo, PUSHORT punCount);


// PCO24 functions
ULONG CCONV  PCO24_Init (ULONG ulBase);
ULONG CCONV  PCO24_UserLED (ULONG ulBase, UCHAR cAction);
ULONG CCONV  PCO24_OutputsEnable (ULONG ulBase, UCHAR cAction);
ULONG CCONV  PCO24_DigGroupRead (ULONG ulBase,  UCHAR cGroup, PUCHAR pcValue);
ULONG CCONV  PCO24_DigGroupWrite (ULONG ulBase,  UCHAR cGroup, UCHAR cValue);
ULONG CCONV  PCO24_DigBitRead (ULONG ulBase, UCHAR cBitNo, PUCHAR pcValue);
ULONG CCONV  PCO24_DigBitWrite (ULONG ulBase, UCHAR cBitNo, UCHAR cValue);


// PCI24 functions
ULONG CCONV  PCI24_Init (ULONG ulBase);
ULONG CCONV  PCI24_UserLED (ULONG ulBase, UCHAR cAction);
ULONG CCONV  PCI24_DigGroupRead (ULONG ulBase,  UCHAR cGroup, PUCHAR pcValue);
ULONG CCONV  PCI24_DigBitRead (ULONG ulBase, UCHAR cBitNo, PUCHAR pcValue);
ULONG CCONV  PCI24_CounterInit (ULONG ulBase, UCHAR cCounterNo, UCHAR cMode, USHORT unNewCount);
ULONG CCONV  PCI24_CounterRead (ULONG ulBase, UCHAR cCounterNo, PUSHORT punCount);


// PCRELAY8/IN8 functions
ULONG CCONV  PCRELAY8_Init (ULONG ulBase);
ULONG CCONV  PCRELAY8_RelaysEnable (ULONG ulBase, UCHAR cAction);
ULONG CCONV  PCRELAY8_RelayGroupRead (ULONG ulBase, PUCHAR pcValue);
ULONG CCONV  PCRELAY8_RelayGroupWrite (ULONG ulBase, UCHAR cValue);
ULONG CCONV  PCRELAY8_OptoInputGroupRead (ULONG ulBase, PUCHAR pcValue);
ULONG CCONV  PCRELAY8_RelayRead (ULONG ulBase, UCHAR cRelayNo, PUCHAR pcValue);
ULONG CCONV  PCRELAY8_RelayWrite (ULONG ulBase, UCHAR cRelayNo, UCHAR cValue);
ULONG CCONV  PCRELAY8_OptoInputRead (ULONG ulBase, UCHAR cInputNo, PUCHAR pcValue);


// PCIB32 functions
ULONG CCONV  PCIB32_Init (ULONG ulBase);
ULONG CCONV  PCIB32_UserLED (ULONG ulBase, UCHAR cAction);
ULONG CCONV  PCIB32_OutputsEnable (ULONG ulBase, UCHAR cAction);
ULONG CCONV  PCIB32_DigGroupRead (ULONG ulBase, UCHAR cGroup, PUCHAR pcValue);
ULONG CCONV  PCIB32_DigGroupWrite (ULONG ulBase, UCHAR cGroup, UCHAR cValue);
ULONG CCONV  PCIB32_DigBitRead (ULONG ulBase, UCHAR cBitNo, PUCHAR pcValue);
ULONG CCONV  PCIB32_DigBitWrite (ULONG ulBase, UCHAR cBitNo, UCHAR cValue);

// PCIO8 functions
ULONG CCONV  PCIO8_Init (ULONG ulBase);
ULONG CCONV  PCIO8_OutputsEnable (ULONG ulBase, UCHAR cAction);
ULONG CCONV  PCIO8_DigOutputGroupRead (ULONG ulBase, PUCHAR pcValue);
ULONG CCONV  PCIO8_DigOutputGroupWrite (ULONG ulBase,  UCHAR cValue);
ULONG CCONV  PCIO8_DigOutputBitRead (ULONG ulBase, UCHAR cBitNo, PUCHAR pcValue);
ULONG CCONV  PCIO8_DigOutputBitWrite (ULONG ulBase, UCHAR cBitNo, UCHAR cValue);
ULONG CCONV  PCIO8_DigInputGroupRead (ULONG ulBase, PUCHAR pcValue);
ULONG CCONV  PCIO8_DigInputBitRead (ULONG ulBase, UCHAR cBitNo, PUCHAR pcValue);


// PCAD1216H functions
ULONG CCONV  PCAD1216_Init (ULONG ulBase);
ULONG CCONV  PCAD1216_UserLED (ULONG ulBase, UCHAR cAction);
ULONG CCONV  PCAD1216_SelectChannel (LONG ulBase, UCHAR cExpAdr, UCHAR cChannel);
ULONG CCONV  PCAD1216_SelectGain (LONG ulBase, UCHAR cGain);
ULONG CCONV  PCAD1216_StartConversion (LONG ulBase);
ULONG CCONV  PCAD1216_GetConversionStatus (LONG ulBase, PUCHAR pcStatus);
ULONG CCONV  PCAD1216_ReadData (LONG ulBase, PUSHORT punData);
ULONG CCONV  PCAD1216_ReadEasy (LONG ulBase, UCHAR cExpAdr, UCHAR cChannel, PUSHORT punData);
ULONG CCONV  PCAD1216_CounterInit (ULONG ulBase, UCHAR cCounterNo, UCHAR cMode, USHORT unNewCount);
ULONG CCONV  PCAD1216_CounterRead (ULONG ulBase, UCHAR cCounterNo, PUSHORT punCount);


// PCDAC12-4 functions
ULONG CCONV  PCDAC124_Init (ULONG ulBase);
ULONG CCONV  PCDAC124_UserLED (ULONG ulBase, UCHAR cAction);
ULONG CCONV  PCDAC124_Write (LONG ulBase, UCHAR cChannel, USHORT unValue);


// PCADADIO functions
ULONG CCONV  PCADADIO_Init (ULONG ulBase);
ULONG CCONV  PCADADIO_UserLED (ULONG ulBase, UCHAR cAction);
ULONG CCONV  PCADADIO_AnalogWrite (ULONG ulBase, UCHAR cChannel, USHORT unValue);
ULONG CCONV  PCADADIO_DigIoConfig (ULONG ulBase, UCHAR cNib0, UCHAR cNib1, UCHAR cNib2, UCHAR cNib3);
ULONG CCONV  PCADADIO_DigGroupRead (ULONG ulBase, UCHAR cGroup, PUCHAR pcValue);
ULONG CCONV  PCADADIO_DigGroupWrite (ULONG ulBase, UCHAR cGroup, UCHAR cValue);
ULONG CCONV  PCADADIO_DigBitRead (ULONG ulBase, UCHAR cBitNo, PUCHAR pcValue);
ULONG CCONV  PCADADIO_DigBitWrite (ULONG ulBase, UCHAR cBitNo, UCHAR cValue);
ULONG CCONV  PCADADIO_CounterInit (ULONG ulBase, UCHAR cCounterNo, UCHAR cMode, USHORT unNewCount);
ULONG CCONV  PCADADIO_CounterRead (ULONG ulBase, UCHAR cCounterNo, PUSHORT punCount);
ULONG CCONV  PCADADIO_CounterInterruptRead (ULONG ulBase, PUCHAR pcIntr);
ULONG CCONV  PCADADIO_CounterInterruptClear (ULONG ulBase);
ULONG CCONV  PCADADIO_AnalogSelectChannel(ULONG ulBase, UCHAR cChannel);
ULONG CCONV  PCADADIO_AnalogStartConversion (ULONG ulBase);
ULONG CCONV  PCADADIO_AnalogGetConversionStatus (ULONG ulBase, PUCHAR pcStatus);
ULONG CCONV  PCADADIO_AnalogReadData (ULONG ulBase, PUSHORT punData);
ULONG CCONV  PCADADIO_AnalogReadEasy (ULONG ulBase, UCHAR cChannel, PUSHORT punData);


// PCPIC functions
ULONG CCONV  PCPIC_Init (ULONG ulBase);
ULONG CCONV  PCPIC_UserLED (ULONG ulBase, UCHAR cAction);
ULONG CCONV  PCPIC_BoardSetup (ULONG ulBase, UCHAR cGroup1to4, UCHAR cGroup5to8, UCHAR cMode);
ULONG CCONV  PCPIC_CounterInit (ULONG ulBase, UCHAR cCounterNo, UCHAR cMode, USHORT unNewCount);
ULONG CCONV  PCPIC_CounterRead (ULONG ulBase, UCHAR cCounterNo, PUSHORT punCount);
ULONG CCONV  PCPIC_TriggerTimeBase (ULONG ulBase);
ULONG CCONV  PCPIC_InterruptRead (ULONG ulBase, PUCHAR pcValue);
ULONG CCONV  PCPIC_InterruptClear (ULONG ulBase);
ULONG CCONV  PCPIC_DigOutputGroupWrite (ULONG ulBase,  UCHAR cValue);
ULONG CCONV  PCPIC_DigInputGroupRead (ULONG ulBase, UCHAR cGroup, PUCHAR pcValue);
ULONG CCONV  PCPIC_DigInputBitRead (ULONG ulBase, UCHAR cBitNo, PUCHAR pcValue);


// PCSYSCON functions
ULONG CCONV  PCSYSCON_Init (ULONG ulBase);
ULONG CCONV  PCSYSCON_UserLED (ULONG ulBase, UCHAR cAction);
ULONG CCONV  PCSYSCON_ReadStatus (ULONG ulBase, PUCHAR pcValue);
ULONG CCONV  PCSYSCON_WatchdogSet (ULONG ulBase, UCHAR cTimeout);
ULONG CCONV  PCSYSCON_WatchdogReset (ULONG ulBase);
ULONG CCONV  PCSYSCON_MaskRead (ULONG ulBase, UCHAR cMaskNo, PUCHAR pcValue);
ULONG CCONV  PCSYSCON_MaskWrite (ULONG ulBase, UCHAR cMaskNo, UCHAR cMaskValue);
ULONG CCONV  PCSYSCON_SetComparator (ULONG ulBase, UCHAR cComparatorNo, UCHAR cValue);
ULONG CCONV  PCSYSCON_DigOutputGroupWrite (ULONG ulBase,  UCHAR cValue);
ULONG CCONV  PCSYSCON_DigInputGroupRead (ULONG ulBase, PUCHAR pcValue);
ULONG CCONV  PCSYSCON_DigInputBitRead (ULONG ulBase, UCHAR cBitNo, PUCHAR pcValue);


// PCSER4 functions
ULONG CCONV   PCSER4_Init (ULONG ulBase);
ULONG CCONV   PCSER4_UserLED (ULONG ulBase, UCHAR cAction);
ULONG CCONV   PCSER4_Open(ULONG ulBase, ULONG ulComPort, ULONG unBaud, UCHAR cParity, UCHAR cDatabits, UCHAR cStopbits);
ULONG CCONV   PCSER4_Close(ULONG ulBase, ULONG ulComPort);
ULONG CCONV   PCSER4_PutC(ULONG ulBase, ULONG ulComPort, UCHAR cValue);
ULONG CCONV   PCSER4_GetC(ULONG ulBase, ULONG ulComPort, PUCHAR pcValue,PULONG ulRetLen);
ULONG CCONV   PCSER4_SetStatusDTR(ULONG ulBase, ULONG ulComPort, ULONG bNewDtrState);
ULONG CCONV   PCSER4_SetStatusRTS(ULONG ulBase, ULONG ulComPort, ULONG bNewRtsState);
ULONG CCONV   PCSER4_GetStatusCTS(ULONG ulBase, ULONG ulComPort, ULONG *CtsState);
ULONG CCONV   PCSER4_GetStatusDCD(ULONG ulBase, ULONG ulComPort, ULONG *CdState);

/*********************************************************************************
// Function:    PCSER4_AllSent()
// Description
//   Checks the ALL SENT status of the passed serial *  communications 
//   channel. This bit is an indications that ALL bytes
//   have been emptied from the UARTS FIFO buffers as well as all data
//   shifted out of the transmit shift registers. For application programs
//   where the RTS/CTS handshaking signals are manipulated from within the
//   application, this function can be used to check for comfirmation that
//   all data has been shifted out of the UART.
//
//   The allsent status is returned in the pAllSent parameter
//      0  if the ALL SENT status is FALSE.
//      !0 if if ALL SENT is TRUE.
/ Returns:     AN_SUCCESS if no error encountered
/********************************************************************************/
ULONG CCONV   PCSER4_AllSent(ULONG ulBase, ULONG ulComPort, ULONG *pAllSent);

ULONG CCONV   PCSER4_GetTXBuffCount(
                    ULONG   ulBase, 
                    ULONG   ulComPort,
                    ULONG   *RingBufferSize,
                    ULONG   *TransmitCount);
ULONG CCONV   PCSER4_GetRXBuffCount(
                    ULONG   ulBase, 
                    ULONG   ulComPort,
                    ULONG   *RingBufferSize,
                    ULONG   *ReceiveCount);
ULONG CCONV   PCSER4_RXFlush(ULONG   ulBase,  ULONG   ulComPort);
ULONG CCONV   PCSER4_TXFlush(ULONG   ulBase,  ULONG   ulComPort);

// PC/104Multi-IO functions
ULONG CCONV  PC104M_SelectChannel (LONG ulBase, UCHAR cDifferential, UCHAR cChannel);
ULONG CCONV  PC104M_GetConversionStatus (LONG ulBase, PUCHAR pcStatus);
ULONG CCONV  PC104M_ReadData (LONG ulBase, PUSHORT punData);
ULONG CCONV  PC104M_ReadEasy (LONG ulBase, UCHAR cDifferential, UCHAR cChannel, PUSHORT punData);
ULONG CCONV  PC104M_AnalogWrite (ULONG ulBase, UCHAR cChannel, USHORT unValue);
ULONG CCONV  PC104M_DigBitRead (ULONG ulBase, UCHAR cBitNo, PUCHAR pcValue);




//****************************************************************************
# ifdef __cplusplus
}
# endif /* __cplusplus */
//****************************************************************************
//****************************************************************************
//****************************************************************************

#endif /* __ARCBOARD_H_ */

//eof//
   
