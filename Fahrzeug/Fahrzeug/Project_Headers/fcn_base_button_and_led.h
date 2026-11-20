#ifndef FCN_BASE_BUTTON_AND_LED_H_
#define FCN_BASE_BUTTON_AND_LED_H_

#define _Loesung2_TasterUndLEDs_

/* Hinweis: Die folgenden Funktionen müssen in der Datei 
 * fcn_base_button_and_led.c implementiert werden. 
 */

// LED control
void init_leds(void);
void led_on(uint8_t nr);	 // nr={1,2,3,4}
void led_off(uint8_t nr);    

// push button control
void init_buttons(void);
uint8_t get_button(uint8_t nr);  // get status of button nr={1,2,3,4}

#endif
