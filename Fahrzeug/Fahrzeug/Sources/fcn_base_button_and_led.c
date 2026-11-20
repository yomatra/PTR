#include "MPC5606B.h"
#include "fcn_base_button_and_led.h"



void init_leds(void)
{
    for (uint8_t i = 0; i < 4; i++) {
        uint8_t pin = 68 + i;  // E4 bis E7

        SIU.PCR[pin].B.PA = 0;
        SIU.GPDO[pin].B.PDO = 1;
        SIU.PCR[pin].B.OBE = 1;
    }
}

void led_on(uint8_t nr)
{
    if (nr < 1 || nr > 4) return;

    SIU.GPDO[67 + nr].B.PDO = 0;
}

void led_off(uint8_t nr)
{
    if (nr < 1 || nr > 4) return;

    SIU.GPDO[67 + nr].B.PDO = 1;
}

void init_buttons(void)
{
    for (uint8_t i = 0; i < 4; i++) {
        uint8_t pin = 64 + i;  // E0 bis E3

        SIU.PCR[pin].B.PA = 0;
        SIU.PCR[pin].B.IBE = 1;
        SIU.PCR[pin].B.OBE = 0;
    }
}

uint8_t get_button(uint8_t nr)
{
    if (nr < 1 || nr > 4) return 0;

    // Annahme: gedrückt = LOW
    return SIU.GPDI[63 + nr].B.PDI == 0;
}
