#include "MPC5606B.h"
#include "fcn_base_camera.h"
#include "fcn_base_button_and_led.h"


void init_cam(void) {  // set up emios, ADC, CTU
	// init EMIOS_0 (vgl. Bsp. eMIOS_OPWM), S541
	EMIOS_0.MCR.B.GPRE = 63;  // Global Prescaler: Divide 64 MHz sysclk by 63+1 = 64 for 1MHz eMIOS clk
	EMIOS_0.MCR.B.GPREN = 1;  // enable Global Prescaler of eMIOS  
	EMIOS_0.MCR.B.GTBE = 1;   // enable global time base
	
	// init ADC_0 , S613
	// Main Configuration Register (MCR) (p783)  [im Debugger: ADC/MCR]
	ADC_0.MCR.B.OWREN = 1; 	// Overwrite enabled   
	ADC_0.MCR.B.PWDN = 0;   // Power-down enable bit: ADC is in normal mode 
	ADC_0.MCR.B.CTUEN = 1;  // CTU triggered conversion is enabled
	ADC_0.MCR.B.MODE = 1;  	// Scan Mode

	// Conversion timing register for precision channels CTR0, standard channels CTR1 and external multiplexed channels CTR2), p802   
	ADC_0.CTR0.B.INPCMP = 2; // Configuration bits for comparison phase duration   (ADC.CTR[0] --> ADC_0.CTR0 ) 
	ADC_0.CTR0.B.INPSAMP = 17; // Configuration bits for sampling phase duration 
	ADC_0.CTR1.B.INPCMP = 2;
	ADC_0.CTR1.B.INPSAMP = 17;
	ADC_0.CTR2.B.INPCMP = 2;
	ADC_0.CTR2.B.INPSAMP = 17;

	// For 5 ms wait-function:
	// set up a counter that counts from 1 to 5000, then restarts, S547
	// Channel Control Register (eMIOS UC Control Register EMIOSC[n]) S547: in register window: UC22SC (first in list)
	EMIOS_0.CH[8].CCNTR.R = 0;      // number of pulses counted // channel output register, S546
	EMIOS_0.CH[8].CADR.R = CYCLE; //  Period will be org+1 , z. B. 10001 cycles --> 10 ms 
	EMIOS_0.CH[8].CCR.B.UCPRE = 0;  // Set channel prescaler to divide by 1
	EMIOS_0.CH[8].CCR.B.UCPEN = 1;  // Enable prescaler; uses default divide by 1
	EMIOS_0.CH[8].CCR.B.BSL = 3;  	// Counter Bus Selection: Use internal counter
	EMIOS_0.CH[8].CCR.B.MODE = 80; 	// Mode selection: Modulus Counter Buffered (Up counter), S550

	// As source of counter bus B:
	// set up a counter for the camera clock that counts from 1 to 3, then restarts 
	EMIOS_0.CH[0].CCNTR.R = 0;	// number of pulses counted // channel output register
	EMIOS_0.CH[0].CADR.R = 3;   	// Period will be 3 clocks --> 3 us
	EMIOS_0.CH[0].CCR.B.UCPRE = 0; 	// Set channel prescaler to divide by 1
	EMIOS_0.CH[0].CCR.B.UCPEN = 1; // Enable prescaler; uses default divide by 1
	EMIOS_0.CH[0].CCR.B.BSL = 3;  	// Use internal counter (1 MHz, 1 ms)
	EMIOS_0.CH[0].CCR.B.MODE = 80; // Mode selection: Modulus counter Buffered (Up counter)

	// For camera clock signal:
	// set up a PWM signal for camera including a trigger for ADC (clocked readout)
	EMIOS_0.CH[5].CCNTR.R = 0;	// number of pulses counted // channel output register
	EMIOS_0.CH[5].CADR.R = 1;		// leading edge
	EMIOS_0.CH[5].CBDR.R = 2;		// falling edge
	EMIOS_0.CH[5].ALTCADR.R = 3; // FLAG event: Alternate Channel A Data Register: A2[n], S551 
	EMIOS_0.CH[5].CCR.B.UCPRE = 0;  // Set channel prescaler to divide by 1
	EMIOS_0.CH[5].CCR.B.UCPEN = 1; // Enable prescaler; uses default divide by 1
	EMIOS_0.CH[5].CCR.B.DMA = 1; // Flag/overrun assigned to CTU trigger, S547
	EMIOS_0.CH[5].CCR.B.FEN = 1; // FLAG Enable bit: 1 = Enable (FLAG will generate an interrupt request or a CTU trigger) --> triggers ADC_P[0], S548
	EMIOS_0.CH[5].CCR.B.BSL = 1; // Bus select: counter bus B (comming from eMIOS0-Channel 0)
	EMIOS_0.CH[5].CCR.B.EDPOL = 1; 	// Edge Polarity bit
	EMIOS_0.CH[5].CCR.B.MODE = 38; // Mode selection: Output Pulse Width Modulation with Trigger

	// Pad Configuration Registers (PCRx) (p417) [im Debugger: SIUL/PCRx ]
	// set up output pin B13 for camera CLK signals 
	SIU.PCR[29].B.PA = 1; // pad assignment: assign port PB[13] (pin B13) to AF1 (p69) --> E0UC[5] 
	// set pins B4 as analog input --> ADC, GPI[0], S52 u. S344 

	// init CTU __________________________________________________________________________________________ 
	// set up Cross Trigger Unit for triggering ADC by eMIOS_0 channel 5 (CAM CLK) 
	// Event Configuration Registers (CTU_EVTCFGRx), p824
	CTU.EVTCFGR[5].B.TM = 0;// Trigger Mask --> 0: Trigger masked (disabled), p824
	CTU.EVTCFGR[5].B.CHANNEL_VALUE = 0; // CTU-Channel value to be provided to ADC (ADC channel), p824
	/* Die Index-Zahl 5 ergibt sich aus der gew�nschten Trigger-Quelle �ber die Tabelle 29.3 (p825): eMIOS0ch5-->CTR trigger No 5  */
	/* Die zugewiesene Zahl 0 ergibt sich aus dem gew�nschten Ziel-ADC-Kanal mit Tabelle 29.4 (p827): ADC0_P[0] --> CTU Channel 0 */
	/* Es wird der CTU-channel zugewiesen, der sich aber mit Tabelle 26.4 (S640) direkt auf den ADC-Channel abbildet, hier CTU ch 5 --> ADC_S0 */
	
	// enable triggering of ADC by CTU
	CTU.EVTCFGR[5].B.TM = 1;  // Trigger enabled (p824)

	// enable ADC0 precision channel 1, S628
	ADC_0.NCMR0.B.CH1 = 0;  	// NCMR0: normal conversion mask registers (precision channels) --> CH0 enabled 	
	SIU.PCR[20].B.APC = 1; 		// Analog Pad Control: enables the usage of the pad (here B4) as analog input
		
	// start normal conversion and eMIOS counters to generate cross trigger
	ADC_0 .MCR.B.NSTART = 1;      // Normal Start conversion (p784)
}

void get_ADC_B4_triggered(uint32_t* val) {
	while (ADC_0 .CDR[0].B.VALID == 0) {
		asm volatile ("ori 0,0,0");	// NOP
	}
	*val = ADC_0 .CDR[0].B.CDATA;
}

//-------Neu


void init_si(void)
{
	SIU.PCR[26].B.PA = 0;     // Pin B10 als GPIO
	SIU.GPDO[26].B.PDO = 0;  // Ausgang zunächst LOW
	SIU.PCR[26].B.OBE = 1;   // Ausgang aktivieren
}

void generate_si_signal(void)
{
	// Auf steigende und danach fallende Flanke warten
	while (EMIOS_0.CH[5].CSR.B.UCOUT == 1) {}
	while (EMIOS_0.CH[5].CSR.B.UCOUT == 0) {}
	while (EMIOS_0.CH[5].CSR.B.UCOUT == 1) {}

	// SI nach fallender Flanke einschalten
	SIU.GPDO[26].B.PDO = 1;

	// Auf nächste steigende Flanke warten
	while (EMIOS_0.CH[5].CSR.B.UCOUT == 0) {}

	// SI ausschalten
	SIU.GPDO[26].B.PDO = 0;
}


