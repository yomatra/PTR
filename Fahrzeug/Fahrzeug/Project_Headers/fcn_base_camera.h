#ifndef FCN_BASE_CAMERA_H_
#define FCN_BASE_CAMERA_H_

#include "MPC5606B.h"

#define CYCLE 10000 // cycle time in �s	
#define CAMLINE_LENGTH 128

void init_cam(void); 
void wait_until_end_of_cam_cycle(void);
void get_ADC_B4_triggered(uint32_t* val);
void init_si(void);
void generate_si_signal(void);
#endif // of FCN_BASE_CAMERA_H_

/* SI (digital output to the sensor: begins a scan/exposure),   --> orange
 * CLK (digital output to the sensor: latches SI and clocks the pixels out),  --> red 
 * AO (analog pixel input from the sensor: 0 � Vdd, or tri-stated if beyond pixel 128). --> yellow
 * The exposure time (also called �integration time�) is the time interval between SI pulses.
 * During each exposure, all the pixels need to be clocked out of the device to prepare it for the
 * next exposure. However, the exposure interval for each pixel begins and ends with the SI pulse, not with
 * the moment it�s clocked out, as with some other sensors. Therefore, all the pixels get exposed
 * simultaneously, and the acquired image represents the same interval in time for each of them.
 * 
 * ground --> black
 * Vcc --> brown
 */ 


