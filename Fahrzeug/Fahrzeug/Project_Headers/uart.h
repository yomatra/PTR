/*  MSL  Version 4.0  1998 August 10  */

/*  $Date: 2007/01/29 21:29:28 $ 
 *  $Revision: 1.5.2.1 $ 
 *  $NoKeywords: $ 
 *
 *		Copyright © 1995-2007 Freescale Corporation.
 *		All rights reserved.
 * 
 *
 *	FILE
 *
 *		UART.h
 *
 *	DESCRIPTION
 *
 *		Abstract interface to serial device (UART) for MetroTRK.
 *		For maximum portability, do not change this file.
 *
 *
 */
 
/****************************************************************************/

#ifndef uart_h_included
#define uart_h_included

#define CORE_CLOCK_64MHz
//#define CORE_CLOCK_48MHz

#define _MSL_CONSOLE_SUPPORT 1

#if !_MSL_CONSOLE_SUPPORT
#error _MSL_CONSOLE_SUPPORT must not be defined to 1.
#endif


#define _With_UART_

/****************************************************************************/

typedef int UARTError;

enum {
	kUARTNoError = 0,
	kUARTUnknownBaudRate,
	kUARTConfigurationError,
	kUARTBufferOverflow,				/* specified buffer was too small */
	kUARTNoData							/* no data available from polling */
};

/****************************************************************************/

typedef enum {
	kBaudHWSet = -1,					/* use HW settings such as DIP switches */
	kBaud300 = 300,						/* valid baud rates */
	kBaud600 = 600,
	kBaud1200 = 1200,
	kBaud1800 = 1800,
	kBaud2000 = 2000,
	kBaud2400 = 2400,
	kBaud3600 = 3600,
	kBaud4800 = 4800,
	kBaud7200 = 7200,
	kBaud9600 = 9600,
	kBaud19200 = 19200,
	kBaud38400 = 38400,
	kBaud57600 = 57600,
	kBaud115200 = 115200,
	kBaud230400 = 230400
} UARTBaudRate;


void ItoA( int z, char* Buffer, char append_null);  /* append_null == 1 --> append '\0' byte to string */


/****************************************************************************/

// #define TestMotorsViaUART

UARTError InitializeUART(UARTBaudRate baudRate);

UARTError ReadUARTPoll(char* c);
UARTError ReadUARTN(void* bytes, unsigned long length);
// UARTError ReadUART1(char* c);
// UARTError ReadUARTString(char* string, unsigned long limit, char terminateChar);
UARTError ReadUARTString(const char* string);

// UARTError WriteUART1(char c);
UARTError WriteUARTString(const char* bytes);
UARTError WriteUARTN(const char* bytes, unsigned long length);



#endif /* uart_h_included */

