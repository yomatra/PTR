// main.c - PLL example for MPC56xxB */
// Description:  Set sysclk to FMPLL0 running at 64 MHz & enable CLKOUT */
#include "MPC5606B.h"

void initModesAndClock(void);
void initPeriClkGen(void);
void disableWatchdog(void);
void initEMIOS_0(void);
void initEMIOS_0ch23(void);
void initEMIOS_0ch21(void);
void initEMIOS_0ch22(void);

vuint32_t i = 0;                      


void initModesAndClock(void) {
	ME.MER.R = 0x0000001D;   // Mode Enable Register ME_ME (p169) --> Enable DRUN, RUN0, SAFE, RESET modes  

	CGM.FMPLL_CR.R = 0x02400100;    // FMPLL Control Register CR (p123) --> 8 MHz xtal: Set PLL0 to 64 MHz    
	
	ME.RUN[0].R = 0x001F0074;    // (Run) Mode Configuration ME_RUN0_MC (p179)--> 16MHz IRCON, OSC0ON, PLL0ON,syclk=PLL0 
	
	ME.RUNPC[1].R = 0x00000010; 	 // RUN Peripheral Configuration Register (p184) --> for run mode RUN0    

	// Peripheral Control Register (im Mode Entry Module) (p186)
	// These bits associate a configuration as defined in the ME_RUN_PC0..7 registers to the peripheral
	ME.PCTL[68].R = 0x01;  //  #68 --> SIUL Control  (p160)                   
	ME.PCTL[72].R = 0x01;   // #72 --> eMIOS0 Control (p160)
	/* 
	 	ME.PCTL[68].B.RUBCFG = 1;  // ME_PCTL68: SIUL Control: select ME_RUN_PC1 configuration 
		ME.PCTL[72].B.RUBCFG = 1;  // ME_PCTL72; eMIOS0 Control: select ME_RUN_PC1 configuration  
	*/
	
	// Mode Transition to enter RUN0 mode by setting key and inverted key consecutively    
	ME.MCTL.R = 0x40005AF0;    // Mode Control Register (p168): Enter RUN0 Mode & Key   
	ME.MCTL.R = 0x4000A50F;         // Enter RUN0 Mode & Inverted Key (Read access will always return inverted key)   
	while (ME.GS.B.S_MTRANS) {}     	// Wait for mode transition to complete     
	while(ME.GS.B.S_CURRENTMODE != 4) {} // Verify RUN0 is the current mode 
}

void initPeriClkGen(void) {
	// System Clock Divider Configuration Register (CGM_SC_DC0..2) (p148)
	CGM.SC_DC2.R = 0x80;      // Enable Devider 0 (DE0) and set Divider 0 Division Value to 1 (DIV0)
	/* 
		CGM_SC_DC2[2].B.DE0 = 1; // Enable system clock divider (DE0) 
		CGM_SC_DC2[2].B.DIV0 = 0; // Set system clock divider to 0+1 = 1 (DIV0) 
	*/
}

void disableWatchdog(void) {    // Software Watchdog Timer is turned on after reset!
  // SWT Service Register (SWT_SR) S962 [im Debugger: SWT/SR ]
  SWT.SR.R = 0x0000c520;     // Write keys to clear soft lock bit    // S962, Table 33-6
  SWT.SR.R = 0x0000d928; 
  // Clear watchdog enable [im Debugger: SWT/CR ]
  SWT.CR.R = 0x8000010A;   // SWT Control Register (SWT_CR) (p959)      
}        

void initEMIOS_0(void) { 		
  // eMIOS Module Configuration Register (EMIOSMCR) (p705) [im Debugger: eMIOS0/MCR ]
  EMIOS_0.MCR.B.GPRE= 63;   /* Global Prescaler (p706): Divide 64 MHz sysclk by 63+1 = 64 for 1 MHz eMIOS clk */

  EMIOS_0.MCR.B.GPREN = 1;		// Enable Global Prescaler of eMIOS (p706)
  EMIOS_0.MCR.B.GTBE = 1;   	// Enable global time base 
  EMIOS_0.MCR.B.FRZ = 1;    	// Enable stopping channels when in debug mode 
}

void initEMIOS_0ch23(void) {     // EMIOS 0 CH 23: Modulus Up Counter 
	// compare Unified Channel memory map: S704-705:  
	// see: eMIOS channel to pin mapping: Tab. 27-1, S694 
	// see: Channel configuration (counter bus): Fig. 27-7, S703 
	
	// eMIOS UC A Register (EMIOSA[n]) (p708), Register: eMIOS0/UC23A
	EMIOS_0.CH[23].CADR.R = 63999;      /* 64000 Match value = 999 --> period will be 999+1 = 1000 clocks (1 ms, 1 kHz)  */
	
	// eMIOS UC Control Register (EMKIOSC[n]) (p710) Register: eMIOS0/UC23SC  
	EMIOS_0.CH[23].CCR.B.MODE = 0x50; // select Modus: Modulus Counter Buffered (MCB) (Up counter) = 101 0000b (cmp. Tab 27-21, S714)
	EMIOS_0.CH[23].CCR.B.BSL = 0x3;	  // Counter Bus Selection: Use internal counter (cmp. Tab 27-20, S713-S714)
	EMIOS_0.CH[23].CCR.B.UCPRE=2;	  // ÄNDERUNG !!Set channel prescaler to divide by 1 (cmp. Tab. 27-18, S714)
	EMIOS_0.CH[23].CCR.B.UCPEN = 1;   // Enable prescaler; uses default divide by 1
	EMIOS_0.CH[23].CCR.B.FREN = 1; 	  // Freeze channel counting when in debug mode

}

void initEMIOS_0ch21(void) {        // EMIOS 0 CH 21: Output Pulse Width Modulation 
	EMIOS_0.CH[21].CADR.R = 99;      // ÄNDERUNG 99 und 999 Abstand beträgt 900eMIOS UCA Register (p708): Leading edge when channel counter bus=250
	EMIOS_0.CH[21].CBDR.R = 999;      // eMIOS UCB Register (p709): Trailing edge when channel counter bus=500
 
    // eMIOS UC Control Register (EMKIOSC[n]) (p710) Register: eMIOS0/UC21SC  
	EMIOS_0.CH[21].CCR.B.BSL = 0x0;	// Counter Bus Selection: Use internal counter (cmp. Tab 27-20, S713-S714): Use counter bus A (default)
	EMIOS_0.CH[21].CCR.B.EDPOL = 1;	// Edge Polarity Bit: 1 --> A match on comparator A sets the output flip-flop, while a match on comparator B clears it
	EMIOS_0.CH[21].CCR.B.MODE = 0x60; // Mode is OPWM Buffered (cmp. Tab 27-21, S714)
	
	// see: eMIOS_0 channel to pin mapping (Tab. 27-1, S694) 
	// see: Functional port pin description (Tab. 4-3, S69)
	
	// Pad Configuration Registers (PCRx) (p417) 
	SIU.PCR[69].B.PA = 1; // pad assignment: assign port PE[5] (pin E5) to Alt Func AF0 (p69)
}

void initEMIOS_0ch22(void) {         // EMIOS 0 CH 22: Output Pulse Width Modulation 
	EMIOS_0.CH[22].CADR.R = 500;      // eMIOS UCA Register (p708): Leading edge when channel counter bus=500 (Register UC22A)   
	EMIOS_0.CH[22].CBDR.R = 999;      // eMIOS UCB Register (p709): Trailing edge when channel counter bus=999 (Register UC22B)

	// Hinweis: LED ist in der Zeitperiode zw. A und B aus !!!

	// eMIOS UC Control Register (EMKIOSC[n]) (p710) Register: eMIOS0/UC22SC  
	EMIOS_0.CH[22].CCR.B.BSL = 0x0;	// Counter Bus Selection: Use internal counter (cmp. Tab 27-20, S713-S714): Use counter bus A (default)
	EMIOS_0.CH[22].CCR.B.EDPOL = 1;	// Edge Polarity Bit: 1 --> A match on comparator A sets the output flip-flop, while a match on comparator B clears it
	EMIOS_0.CH[22].CCR.B.MODE = 0x60; // Mode is OPWM Buffered (cmp. Tab 27-21, S714)

	// Pad Configuration Registers (PCRx) (p417) 
	SIU.PCR[70].B.PA = 1; // pad assignment: assign port PE[6] (pin E6) to Alt Func AF0 (p69)
}

void main (void) {	
	volatile uint32_t i = 0; 

	initModesAndClock(); 
	initPeriClkGen();  	// Initialize peripheral clock generation 
	disableWatchdog();  
	initEMIOS_0();     	// Initialize eMIOS 0 to provide 1 MHz clock to channels 
	initEMIOS_0ch23(); 	// Initialize eMIOS 0 channel 23 as modulus counter --> on Counter Bus_A
	initEMIOS_0ch21(); 	// Initialize eMIOS 0 channel 0 as OPWM, using channel 23 as time base 
	initEMIOS_0ch22(); 	// Initialize eMIOS 0 channel 1 as OPWM, using channel 23 as time base 

	
	while(1) { 
		for (uint32_t duty = 1; duty < 100; duty++) {
			EMIOS_0.CH[21].CADR.R = 0;
			EMIOS_0.CH[21].CBDR.R = duty * 10;

			EMIOS_0.CH[22].CADR.R = 0;
			EMIOS_0.CH[22].CBDR.R = (100 - duty) * 10;

			for (volatile uint32_t j = 0; j < 100000; j++) {
				// Nur zur Verlangsamung
			}
		}	}
}



