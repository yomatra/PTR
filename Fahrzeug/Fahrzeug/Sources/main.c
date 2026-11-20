#include "MPC5606B.h"
#include "fcn_base_camera.h"
#include "fcn_base_button_and_led.h"
#include "image_processing.h"
#include "IntcInterrupts.h"
#include "debug.h"
#include "uart.h"

/* ________________________________Function Declarations_________________________________________*/
void init(void);
void wait_until_end_of_cam_cycle(void);
int main(void);

/* ________________________________Functions_______________________________________________________*/
	
void init(void) {
	// disable watchdog, see example PLL_sysclk (disableWatchdog())
	SWT.SR.R = 0x0000C520;
	SWT.SR.R = 0x0000D928;
	SWT.CR.R = 0x8000010A;
	
	// Set PLL to make 64 MHz out of 8 MHz input, see example PLL_sysclk (initModesAndClock())
	CGM.FMPLL_CR.B.NDIV = 32; // multiply by 32
	CGM.FMPLL_CR.B.IDF = 1;  // divide by 2
	CGM.FMPLL_CR.B.ODF = 0;  // divide by 2  --> 64 MHz = 8 MHz * 32 / (2+2)
	
	// config RunMode 0, see example PLL_sysclk (initModesAndClock())
	ME.RUN[0].B.PDO = 0; 		// I/O output power-down control: controls the output power down of I/Os: 0: No automatic safe gating of I/Os used and pads power sequence driver is enabled
	ME.RUN[0].B.MVRON = 1; 		// Main voltage regulator is switched on
	ME.RUN[0].B.DFLAON = 3; 	// Data flash is in normal mode
	ME.RUN[0].B.CFLAON = 3; 	// Code flash is in normal mode
	ME.RUN[0].B.FMPLLON = 1; 	// frequency modulated phase locked loop control is switched on
	ME.RUN[0].B.FXOSCON = 1; 	// fast external crystal oscillator (8 MHz) control is switched on
	ME.RUN[0].B.FIRCON = 0; 	// fast internal RC oscillator (16 MHz) is switched off
	ME.RUN[0].B.SYSCLK = 4; 	// system clock = frequ. mod. PLL 
	
	ME.RUNPC[1].B.RUN0 = 1; 	// RUN Peripheral Configuration Register (p184) --> for run mode RUN0    
	
	// Peripheral Control Register (im Mode Entry Module) (p186)
	ME.PCTL[32].B.RUN_CFG = 1;  // #32 --> ADC0 Control (Tab 8-39, p159)   
#ifdef _With_UART_
	ME.PCTL[48].B.RUN_CFG = 1;  // #48 --> LINFlex0
#endif
	ME.PCTL[57].B.RUN_CFG = 1;  // #57 --> CTU 
	ME.PCTL[68].B.RUN_CFG = 1;  // #68 --> SIUL 
	ME.PCTL[72].B.RUN_CFG = 1;  // #72 --> eMIOS_0  
		
	// Mode Transition to enter RUN0 mode, see example PLL_sysclk (initModesAndClock())
	ME.MCTL.R = 0x40005AF0;
	ME.MCTL.R = 0x4000A50F;

	while (ME.GS.B.S_MTRANS) { // wait for mode change to finish
		asm volatile ("ori 0,0,0"); // NOP  (no operation)
	}
	
	while(ME.GS.B.S_CURRENTMODE != 4) { // Verify RUN0 is the current mode (not SAFE mode)
		asm volatile ("ori 0,0,0"); // NOP
	}
	// end of Mode Transition
}


void wait_until_end_of_cam_cycle(void) {
	// Wartet darauf, dass seit dem letzten Aufruf dieser Funktion 
	// <CYCLE> Zyklen vergangen sind (unabh. von der Rechenzeit dazwischen)
	while (EMIOS_0.CH[8].CCNTR.R > 1) {
		asm volatile ("ori 0,0,0"); // NOP 
	}
}


/* ___________________________________________MAIN____________________________________________________________*/

int main(void) {
	
	init();
	init_cam();
	init_si();
	init_leds();
	init_buttons();
#ifdef _With_UART_
	InitializeUART(kBaud19200);
	WriteUARTString("\n\rInitialization finished!\r\n"); 
#endif

	/* LOOP _________________________________________*/
	while (1) {
		wait_until_end_of_cam_cycle();
		generate_si_signal();

		for (uint8_t nr = 1; nr <= 4; nr++) {
			if (get_button(nr)) {
				led_on(nr);
			} else {
				led_off(nr);
			}
		}


		/*
		uint32_t val_triggered;
		get_ADC_B4_triggered(&val_triggered);
		val_triggered = val_triggered;
		*/
			
		// do image processing ...
		
		// close loop control ...
		
		// set motors and servo ...
		
	} // of while()
} // of main()

