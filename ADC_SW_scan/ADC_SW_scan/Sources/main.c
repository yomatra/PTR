// main.c - PLL example for MPC56xxB */
#include "MPC5606B.h"

void initModesAndClock(void);
void initPeriClkGen(void);
void disableWatchdog(void);
void initADC(void);


uint32_t Result[3];             // ADC conversion results 
uint16_t ResultInMv[3];         // ADC conversion results in mv 
	
void initModesAndClock(void) {
	ME.MER.R = 0x0000001D;   // Mode Enable Register ME_ME (p169) --> Enable DRUN, RUN0, SAFE, RESET modes  

	CGM.FMPLL_CR.R = 0x02400100;    // FMPLL Control Register CR (p123) --> 8 MHz xtal: Set PLL0 to 64 MHz    
	
	ME.RUN[0].R = 0x001F0074;    // (Run) Mode Configuration ME_RUN0_MC (p179)--> 16MHz IRCON, OSC0ON, PLL0ON,syclk=PLL0 
	
	ME.RUNPC[1].R = 0x00000010; 	 // RUN Peripheral Configuration Register (p184) --> for run mode RUN0    
	
	// Peripheral Control Register (im Mode Entry Module) (p186)
	// These bits associate a configuration as defined in the ME_RUN_PC0..7 registers to the peripheral
	ME.PCTL[32].R = 0x01; 	            // #32 --> ADC0 Control (Tab 8-39, p159) 
	ME.PCTL[68].R = 0x01;               // #68 --> SIUL Control (p160)     
	
	
	// Mode Transition to enter RUN0 mode by setting key and inverted key consecutively, siehe Beispiel PLL_sysclk
	ME.MCTL.R = 0x40005AF0;         	// Enter RUN0 Mode & Key 
	ME.MCTL.R = 0x4000A50F;         	// Enter RUN0 Mode & Inverted Key
	while (ME.GS.B.S_MTRANS) {}     	// Wait for mode transition to complete     
	while(ME.GS.B.S_CURRENTMODE != 4) {} // Verify RUN0 is the current mode 
}

void initPeriClkGen(void) {
	CGM.SC_DC2.R = 0x80;      // Enable Devider 0 (DE0) and set Divider 0 Division Value to 1 (DIV0)
}
void disableWatchdog(void) {    // Software Watchdog Timer is turned on after reset!
  // SWT Service Register (SWT_SR) S962 [im Debugger: SWT/SR ]
  SWT.SR.R = 0x0000c520;     // Write keys to clear soft lock bit    // S962, Table 33-6
  SWT.SR.R = 0x0000d928; 
  // Clear watchdog enable [im Debugger: SWT/CR ]
  SWT.CR.R = 0x8000010A;   // SWT Control Register (SWT_CR) (p959)      
}        

void initADC(void) {
	// Assign ports for ADC
	// Pad Configuration Registers (PCRx) (p417) [im Debugger: SIUL/PCRx ]
	SIU.PCR[24].B.APC = 1;  // Analog Pad Control: This bit enables the usage of the pad PB[8] as analog input ADC0_S[1] (p63)
	SIU.PCR[25].B.APC = 1;  // Analog Pad Control: This bit enables the usage of the pad PB[9] as analog input ADC0_S[2] (p63)
	SIU.PCR[26].B.APC = 1; // B10 Analogeingang
	//SIU.PCR[24].R = 0x2000;         // Initialize PB[8] as ANS0, [im Debugger: Peripheral Control Register im Mode Entry Module: MC_ME/PCTL24 ]
	
	// Initialize ADC0 for scan mode
	ADC_0.MCR.R = 0x20000000;  // Main Configuration Register (MCR) (p783)  [im Debugger: ADC/MCR]   
	/*
		ADC.MCR.MODE = 1;  			// Register Mode: scan mode (p784)
		ADC.MCR.JTRGEN = 0;  		// Register Injection external trigger enable: disabled 
		ADC.MCR.PWDN = 0;  			// Register Power-down enable:  0-->ADC is in normal mode 
		ADC.MCR.CTUEN = 0; 			// Register Cross trigger unit conversion enable: CTU triggered conversion disabled 
	*/
	
	// Normal Conversion Mask Registers (NCMR[0..2] (p803)
	ADC_0.NCMR1.R = 3;  // Sampling is enabled for channel 32 and 33 
	
	// Conversion Timing Register 0 (p802) [ im Debugger: ADC/CTR1 ]
		ADC_0.CTR1.R = 0x00008606;      // set conversion times for 32MHz ADClock  
	/* Conversion timing register for normal sampling channels 32-47 (CTR[1])  // see p802
		ADC.CTR[1].B.INPLATCH = 1;  // Configuration bit for latching phase duration  
		ADC.CTR[1].B.INPCMP = 3;  	// Configuration bits for comparison phase duration 
		ADC.CTR[1].B.INPSAMP = 6; 	// Configuration bits for sampling phase duration 
	*/
	
	ADC_0.MCR.B.NSTART=1;           // Main Configuration Register (MCR): Start normal conversions for ADC0 (p784)
}





//---------------Neue


// LED 1–4 liegen an E4–E7, PCR/GPDO 68–71.
// LEDs sind active-low: 0 = an, 1 = aus.

void init_leds(void)
{
	for (uint8_t i = 0; i < 4; i++) {
		uint8_t pin = 68 + i;

		SIU.PCR[pin].B.PA = 0;    // GPIO-Funktion
		SIU.GPDO[pin].B.PDO = 1; // LED zunächst aus
		SIU.PCR[pin].B.OBE = 1;  // Ausgang aktivieren
	}
}

void led_on(uint8_t nr)
{
	if (nr < 1 || nr > 4)
		return;

	SIU.GPDO[67 + nr].B.PDO = 0;
}

void led_off(uint8_t nr)
{
	if (nr < 1 || nr > 4)
		return;

	SIU.GPDO[67 + nr].B.PDO = 1;
}

void show_voltage(uint16_t millivolt)
{
	for (uint8_t nr = 1; nr <= 4; nr++) {
		if (millivolt >= nr * 1000U) {
			led_on(nr);
		} else {
			led_off(nr);
		}
	}
}


void main (void) {	
	uint8_t i = 0;	 
	
	initModesAndClock(); 
	initPeriClkGen();         		
	disableWatchdog();
	initADC();
	init_leds();
	
 
	while (1) { 
		// read data from Channel Data Register (CDR[0..95], p809)
		while (ADC_0.CDR[33].B.VALID != 1) {};              // Wait for last scan to complete 
		Result[0]= ADC_0.CDR[32].B.CDATA;                   // Read ADC0_S[1] conversion result data  
		Result[1]= ADC_0.CDR[33].B.CDATA;    // Read ADC0_S[2] conversion result data
		Result[2]= ADC_0.CDR[34].B.CDATA;
		ResultInMv[0] = (uint16_t) (5000*Result[0]/0x3FF);  // Converted result in mV 
		ResultInMv[1] = (uint16_t) (5000*Result[1]/0x3FF);  // Converted result in mV 
		ResultInMv[2] = (uint16_t) (5000*Result[2]/0x3FF);


		show_voltage(ResultInMv[2]);
		i++;
	}	
}

