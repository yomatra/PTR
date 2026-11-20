// main.c - PLL example for MPC56xxB */
// Description:  Set sysclk to FMPLL0 running at 64 MHz & enable CLKOUT */
#include "MPC5606B.h"

void initModesAndClock(void);
void initOutputClock(void);
void disableWatchdog(void);

// original: Frequenz 4 MHz, Periodenl�nge 250 ns, am Pin A0

void initModesAndClock(void) {
	ME.MER.R = 0x0000001D;   // Mode Enable Register ME_ME (p169) --> Enable DRUN, RUN0, SAFE, RESET modes  
	/* S169 im Reference Manual (MPC5607BRM.pdf) [pdf-Seite 180, im Debugger: Mode Entry Module MC_ME/ME] 
	    ME.MER.R = 0x0000001D entspricht den folgenden Einzelzuweisungen (ungleich 0):
		ME.MER.B.RESET = 1;
		ME.MER.B.SAFE = 1;
		ME.MER.B.DRUN = 1;
		ME.MER.B.RUN0 = 1;
	*/
  
	CGM.FMPLL_CR.R = 0x02400100;    // FMPLL Control Register CR (p123) --> 8 MHz xtal: Set PLL0 to 64 MHz    
	/*  [im Debugger: Frequency-Modulated Phase-Locked Loop FMPLL/CR  (Achtung: einige Bits werden von der HW gesetzt!) ]
		CGM.FMPLL_CR.B.PLL_FAIL_FLAG = 0; // set by hardware whenever a loss of lock event occurs while FMPLL, cleared by software writing �1�. 
		CGM.FMPLL_CR.B.PLL_FAIL_MASK = 0; // pll_fail not masked  
		CGM.FMPLL_CR.B.S_LOCK = 0; // set "FMPLL unlocked", This bit is an indication of whether the FMPLL has acquired lock 
		CGM.FMPLL_CR.B.I_LOCK = 0; // set by hardware whenever there is a lock/unlock event  
		CGM.FMPLL_CR.B.UNLOCK_ONCE = 0; // This bit is a sticking indication of FMPLL loss of lock condition  
		CGM.FMPLL_CR.B.EN_PLL_SW = 1; // Progressive clock switching enabled  
		// Setzen der Faktoren:    --> Formel:  Frequ_PLL = Frequ_ExtOsz * NDIV / (IDF*ODF) = 8 MHz * NDIV / (IDF*ODF)
		CGM.FMPLL_CR.B.NDIV = 64; // multiply by 64  
		CGM.FMPLL_CR.B.ODF = 2;   // divide by 8  
		CGM.FMPLL_CR.B.IDF = 0;   // divide by 1  
	*/

	ME.RUN[0].R = 0x001F0074;    // (Run) Mode Configuration ME_RUN0_MC (p179)--> 16MHz IRCON, OSC0ON, PLL0ON,syclk=PLL0 
	/*  [im Debugger: Mode Entry Module MC_ME/RUN0MC ]
		ME.RUN[0].B.SYSCLK = 4; // select FMPLL for system clock   
		ME.RUN[0].B.FIRCON = 1; // fast internal RC oscillator (16 MHz) is switched on  
		ME.RUN[0].B.FXOSC0ON = 1; // fast external crystal oscillator (8 MHz) is switched on   
		ME.RUN[0].B.FMPLLON = 1; // frequency modulated phase locked loop is switched on   
		ME.RUN[0].B.CFLAON = 3; // Code flash is in normal mode   
		ME.RUN[0].B.DFLAON = 3; // Data flash is in normal mode   
		ME.RUN[0].B.MVRON = 1;  // Main voltage regulator is switched on   
	*/
        
	ME.RUNPC[1].R = 0x00000010; 	 // RUN Peripheral Configuration Register (p184) --> for run mode RUN0    
	/*  [im Debugger:Mode Entry Module MC_ME/RUNPC1]
	    These registers configure eight different types of peripheral behavior during run modes
		ME.RUNPC[1].B.RUN0 = 1;  // Peripheral is active (during RUN0) 
								 // all other peripherals are frozen with clock gated
	*/
	
	ME.PCTL[68].R = 0x01;  // Peripheral Control Register (im Mode Entry Module) (p186) (hier: 68-->SIUL)                    
	/*  [ im Debugger:  MC_ME/PCTL68 ]
	    These bits associate a configuration as defined in the ME_RUN_PC0..7 registers to the peripheral
		ME.PCTL[68].B.RUBCFG = 1;  //selects ME_RUN_PC1    
	*/
	
	// Mode Transition to enter RUN0 mode by setting key and inverted key consecutively    
	ME.MCTL.R = 0x40005AF0;    // Mode Control Register (p168): Enter RUN0 Mode & Key   
	/*  [im Debugger: MC_ME/MCTL   (Achtung: Bits werden auch von der HW gesetzt) ]
		ME.MCTL.B.TARGET_MODE = 4; // select RUN0   
		ME.MCTL.B.KEY = 0x5AF0 // Control key � These bits enable write access to this register.   
	*/
	ME.MCTL.R = 0x4000A50F;         // Enter RUN0 Mode & Inverted Key (Read access will always return inverted key)   
	
	// Wait for mode transition to complete   
	while (ME.GS.B.S_MTRANS) {}  // Global Status Register (p166)   
	while(ME.GS.B.S_CURRENTMODE != 4) {} // Verify RUN0 is the current mode (not SAFE mode)   
	/*  [im Debugger: ME_GS   &  MC_ME/GS ]
		ME.GS.B.S_MTRANS == 1 --> Mode transition is ongoing
		ME.GS.B.S_CURRENTMODE == 4 --> RUN0 is current mode
	*/
}

void disableWatchdog(void) {    // Software Watchdog Timer is turned on after reset!
  // SWT Service Register (SWT_SR) S962 [im Debugger: SWT/SR ]
  SWT.SR.R = 0x0000c520;     // Write keys to clear soft lock bit    // S962, Table 33-6
  SWT.SR.R = 0x0000d928; 
  // Clear watchdog enable [im Debugger: SWT/CR ]
  SWT.CR.R = 0x8000010A;   // SWT Control Register (SWT_CR) (p959)      
  /*  
	  SWT.CR.B.MAP0 = 1; // Master Access Protection for Master 0 --> Access for the master is enabled   
	  SWT.CR.B.RIA = 1;  // Invalid access to the SWT causes a system reset if WEN=1   
	  SWT.CR.B.FRZ = 1;  // Debug Mode Control: SWT counter (watch dog timer) is stopped in debug mode   
	  SWT.CR.B.CSL = 1;  // Clock Selection: Oscillator clock   
	  SWT.CR.B.FRZ = 1;  // Debug Mode Control: SWT counter (watch dog timer) is stopped in debug mode   
	  SWT.CR.B.WEN = 0;  //  SWT is disabled   
  */
}        

void initOutputClock(void) {
	// [im Debugger: Clock Generation Module MC_CGM ]
	CGM.OC_EN.B.EN = 1; 	       // Ouput Clock Enable Register (p146): Output Clock enabled [im Debugger: MC_CGM/OCEN ]
	
	// Output Clock Division Select Register (p146): [ MC_CGM/OCDSSC ]
	CGM.OCDS_SC.B.SELDIV = 1;      // Output Clock divided by 4 ÄNDERUNG (1 = /2)  [ MC_CGM/OCDSSC ]
	CGM.OCDS_SC.B.SELCTL = 1;      // Output clock select 16 MHz int RC osc --> CLKOUT = 16 MHz IRC/4 = 4MHz , 0.25 �s   
	
	// Pad Configuration Registers (PCRx) (p417)  [im Debugger: System Integration Unit Lite  SIUL/PCR0 ]
	SIU.PCR[0].B.PA = 2; // pad assignment: assign port PS[0] (pin A0) to Alt Func AF2 --> CLKOUT (compare Port Map on page 69)
}


void main (void) {	
	vuint32_t i = 0;	 
	
	initModesAndClock(); 
	initOutputClock();   // Initialize Output Clock to 16 M, XOSC, then PLL     
	disableWatchdog();   
	
	while (1) { 
		i++;
	}
}


