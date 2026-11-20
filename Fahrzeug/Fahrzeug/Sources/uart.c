/* Serielles Interface */
/* Jumper J7 und müssen auf 2-3 stehen, d. h MCU TXD/TSD to RS232 Transceiver */
/* Der Virtuelle Serielle Port ist damit ausgeschaltet ! */
/* Jumper-Stellung ist notwendig, da ansonst RXD auf dauerhaft 5V gehalten wird */
/* J13 und J14 dürfen nicht auf default stehen (MCU LIN0TX/RS to Transceiver), auf LIN1 stört nicht */  

#include "MPC5606B.h"
#include "uart.h"
// #include <stdio.h>
// #include <string.h>

#ifdef _With_UART_

void ItoA( int z, char* Buffer, char append_null) /* append_null == 1 --> append '\0' byte to string */
{
  int i = 0;
  int j;
  char tmp;
  unsigned u;    // In u bearbeiten wir den Absolutbetrag von z.
  
    // ist die Zahl negativ?
    // gleich mal ein - hinterlassen und die Zahl positiv machen
    if( z < 0 ) {
      Buffer[0] = '-';
      Buffer++;
      // -INT_MIN ist idR. größer als INT_MAX und nicht mehr 
      // als int darstellbar! Man muss daher bei der Bildung 
      // des Absolutbetrages aufpassen.
      u = ( (unsigned)-(z+1) ) + 1; 
    }
    else { 
      u = (unsigned)z;
    }
    // die einzelnen Stellen der Zahl berechnen
    do {
      Buffer[i++] = (char) ('0' + u % 10);
      u /= 10;
    } while( u > 0 );

    // den String in sich spiegeln
    for( j = 0; j < i / 2; ++j ) {
      tmp = Buffer[j];
      Buffer[j] = Buffer[i-j-1];
      Buffer[i-j-1] = tmp;
    }
    if (append_null == 1) Buffer[i] = '\0';
}


/*******************************************************************************
* Global variables
*******************************************************************************/

/*******************************************************************************
* Constants and macros
*******************************************************************************/

/*******************************************************************************
* Local types
*******************************************************************************/

/*******************************************************************************
* Local function prototypes
*******************************************************************************/
static void Init_LINFlex_0(void);
static void TransmitData(const char * pBuf, const uint32_t cnt);
static int32_t ReceiveData(char * const pBuf);

/*******************************************************************************
* Local variables
*******************************************************************************/

/*******************************************************************************
* Local functions
*******************************************************************************/ 

/*******************************************************************************
Function Name : Init_LINFlex_0
Engineer      : Martin Kovar
Date          : Dec-09-2015
Parameters    : NONE
Modifies      : NONE
Returns       : NONE
Notes         : initializes MPC5607B's Init_LINFlex_0 module for UART mode
Issues        : expecting system clock is 64MHz
*******************************************************************************/
static void Init_LINFlex_0 (void)
{
	/* LINFlex memory map: S389 */
	/* enter INIT mode */
	/* LIN control register 1: LINCR1 S385 */
	LINFLEX_0.LINCR1.R = 0x0081; /* SLEEP=0, INIT=1 */
	
	/* wait for the INIT mode */  
	/* LIN status register (LINSR), S389 u. S390 */
	while (0x1000 != (LINFLEX_0.LINSR.R & 0xF000)) {}  
		
	/* configure pads, S50 bzw. S52 */
	/* SIUL memory map: S336 */
	/* Pad Configuration Registers: S344 */
	SIU.PCR[18].R = 0x0604;     /* Configure pad PB2 for AF1 func: LIN0TX */
	// SIU.PCR[19].R = 0x0100;     /* Configure pad PB3 for LIN0RX */	
	SIU.PCR[19].R = 0x0100;     /* Configure pad PB3 for LIN0RX */	
	/* entspricht SIU.PCR[19].B.IBE = 1 */
	

	/*
	 * For INPUT pads:
		— Select the feature location from PSMI register
		— Set the IBE bit in the appropriate PCR
	 * 
	 */
	// Tests:
	// SIU.PCR[19].B.ODE = 1; // 1 --> Pad configured for open drain: nur für Outputs! (S345)
	// SIU.PCR[19].B.SRC = 1; // 1 --> Pad is configured as medium or fast (depending on the pad)
	// SIU.PCR[19].B.WPE = 1; // 1 --> Weak pull device enabled for the pad
	// SIU.PCR[19].B.WPS = 1; // 1 --> Weak pull-up selectedrr
	// SIU.PCR[19].B.WPS = 0; // 0 --> Weak pull-down selected
	// SIU.PCR[19].R = 0x0103;   /* entspr. WPE=1 und WPS = 1 */
	
	
	// SIU.PCR[19].B.APC = 1; // bringt nix  // Analog Pad Control: enables the usage of the pad as analog input, S345
	
	/* configure for UART mode */
	/* UART mode control register (UARTCR), S398 */
	LINFLEX_0.UARTCR.R = 0x0001; /* set the UART bit first to be able to write the other bits */
	
	LINFLEX_0.UARTCR.R = 0x0033; /* 8bit data, no parity, Tx and Rx enabled, UART mode */
								 /* Transmit buffer size = 1 (TDFL = 0 */
								 /* Receive buffer size = 1 (RDFL = 0) */
	// LINFLEX_0.UARTCR.B.PCE = 1;  // activate parity check 
	
	/* configure baudrate 19200 */
	/* assuming 64 MHz peripheral set 1 clock (fsys below)*/
	/* LFDIV = fsys / (16 * desired baudrate)
	   LINIBRR = integer part of LFDIV
	   LINFBRR = 16 * fractional part of LFDIV (after decimal point)
	   
	   for instance:
	   LFDIV = 64e6/(16*19200) = 208.333
	   LINIBRR = 208
	   LINFBRR = 16*0.333 = 5
	*/
    #if defined(CORE_CLOCK_64MHz)		
		LINFLEX_0.LINFBRR.R = 5; /* vgl. Tab. 21-1, S381 */
		LINFLEX_0.LINIBRR.R = 208;
 
 	#elif defined(CORE_CLOCK_48MHz)
 		LINFLEX_0.LINFBRR.R = 4;
        LINFLEX_0.LINIBRR.R = 156;
 
 	#endif
 
		
	/* exit Initialization mode, enter NORMAL mode, vgl. Abschnitt 21.5.1, S. 382 */
	LINFLEX_0.LINCR1.R = 0x0080; /* INIT=0 */
}

/*******************************************************************************
Function Name : TransmitData
Engineer      : b05111
Date          : Apr-14-2011
Parameters    : pBuf - input string. won't be modified by the function
              : cnt  - input number of characters to be transmitted
Modifies      : NONE
Returns       : NONE
Notes         : Tx data on LINFlex_0. polled mode. 
Issues        : NONE 
*******************************************************************************/
static void TransmitData(const char * const pBuf, const uint32_t cnt) 
{
    uint32_t j; // Dummy variable
    
    for (j = 0; j < cnt; j++)
    {  // Loop for character string 
    	// Buffer data register LSB (BDRL): S407
   	    LINFLEX_0.BDRL.B.DATA0 = *(pBuf+j);    
   	      //write character to transmit buffer
   	    // UART mode status register (UARTSR): S399
   	    // Data Transmission Completed Flag
	    while (1 != LINFLEX_0.UARTSR.B.DTF) {}  
	      // Wait for data transmission completed flag
	    LINFLEX_0.UARTSR.R = 0x0002;
        // clear the DTF flag and not the other flags
    }
   
}

/*******************************************************************************
Function Name : ReceiveData
Engineer      : b05111
Date          : Apr-14-2011
Parameters    : pBuf - address of a char where the received char is written to
                       the address (pBuf) doesn't change in the function
Modifies      : NONE
Returns       : NONE
Notes         : Rx data on LINFlex_0. polled mode. 
Issues        : NONE 
*******************************************************************************/
static int32_t ReceiveData(char * const pBuf) 
{
    
    int32_t rx_data;
    
    /* wait for DRF */
	while (1 != LINFLEX_0.UARTSR.B.DRF) {}  /* Wait for data reception completed flag */
		
	/* wait for RMB */
	while (1 != LINFLEX_0.UARTSR.B.RMB) {}  /* Wait for Release Message Buffer */
	
	/* get the data */
	
	rx_data = (int32_t) LINFLEX_0.BDRM.R; // read whole register due to erratum e4897PS
    
	*pBuf = (uint8_t)rx_data; // take 
	
	/* clear the DRF and RMB flags by writing 1 to them */
	LINFLEX_0.UARTSR.R = 0x0204;
    
}

/*******************************************************************************
* Global functions
*******************************************************************************/ 

/*
Methods called by MW MSL libraries to perform console IO:
*/

UARTError InitializeUART(UARTBaudRate baudRate)
{	
#pragma unused(baudRate) /* verhindert Warnung bzgl. nicht genutzer Variablen */
	Init_LINFlex_0 ();  
	return kUARTNoError; 
}

UARTError ReadUARTN(void* bytes, unsigned long limit)
{
	int count;
	UARTError err; 

	for (count = 0, err = kUARTNoError; 
		count < limit && err == kUARTNoError;
		count++)
        {
		err = ReceiveData( (char *)bytes + count );
        }

	return err;
}

UARTError ReadUARTPoll(char* c) 
{
    int32_t rx_data;
    
    /* Buffer data register MSB,  S408*/ 
    rx_data = (int32_t) LINFLEX_0.BDRM.R; // read whole register due to erratum e4897PS
    
    if (LINFLEX_0.UARTSR.B.RMB == 0)   /* S394: Release Message Buffer */
        return  kUARTNoData;  // return no data  
    else 
    {
        LINFLEX_0.UARTSR.R = 0x0204;
        *c =(unsigned char) rx_data; // read byte of Data
        return kUARTNoError;  // return no error
    }  
}

UARTError ReadUARTString(const char* string)
{
	char* p = (char*) string;
	while(1)
	{
		ReadUARTPoll(p);
		if(*p == '\0') break;
		p++;
	}
}

UARTError WriteUARTString(const char * bytes)
{ 
	unsigned long cnt = 0;
	while( bytes[cnt] != '\0' && cnt < 32) cnt++;
	TransmitData(bytes,cnt);  
  	return kUARTNoError; 
}

UARTError WriteUARTN(const char * bytes, unsigned long length)
{ 
	TransmitData (bytes,length);  
  	return kUARTNoError; 
}

#endif


