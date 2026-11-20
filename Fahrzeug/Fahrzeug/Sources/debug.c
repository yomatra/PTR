#include "MPC5606B.h"
#include "debug.h"
#include "uart.h"
#include "image_processing.h"
#include "fcn_base_button_and_led.h"


void debug_camline_uint8(uint8_t camline[]){
	int debug[CAMLINE_LENGTH];
	for (int i = 0; i < CAMLINE_LENGTH; i++){
		debug[i] = (int)camline[i];
	}
	int done = 1;
}


void debug_camline_int8(int8_t camline[]){
	int debug[CAMLINE_LENGTH];
	for (int i = 0; i < CAMLINE_LENGTH; i++){
		debug[i] = (int)camline[i];
	}
	int done = 1;
}



void printf_pixels(const uint8_t cam_line[]) {
#ifdef _With_UART_
	static int counter = 0;

	uint16_t i;
	struct {
		uint8_t header[3u];
		//  uint8_t data[CAMLINE_LENGTH * 4u];
		uint8_t data[CAMLINE_LENGTH];
		uint8_t trailer[4u];
	} message;
	uint16_t messageLength;

	counter++;

	/* 5ms * 200 = 1 second */
	if (counter == 100) {
		counter = 0;

		messageLength = sizeof(message);

		message.header[0u] = 'P';
		message.header[1u] = 'i';
		message.header[2u] = 'x';

		/* Initialise buffer */
		for (i = 0; i < messageLength; i++) {
			//  message.data[i] = ' ';
			message.data[i] = 0u;
		}

		for (i = 0; i < CAMLINE_LENGTH; i++) {
			//  ItoA(cam_line[i], (char*) (message.data + i * 4u), 0u);
			message.data[i] = cam_line[i];
		}

		message.trailer[0u] = 'e';
		message.trailer[1u] = 'l';
		message.trailer[2u] = 's';
		message.trailer[3u] = '\0';

		WriteUARTN((char*) &message, messageLength);

	}
#else /* _With_UART_ */
	uint8_t tmp = cam_line[0];  // avoid warnings
#endif /* _With_UART_ */
}


void printf_pixels_txt(const uint8_t cam_line[])
{
#ifdef _With_UART_
	static int counter = 0;
	counter++;
	/* 5ms * 200 = 1 second */
	if(counter%200 == 0) // only every second
	{
		const uint16_t size = 6*128;
		const uint16_t max_idx = size-1;
		static char px[size];
		for(int i = 0; i < max_idx; i++) px[i] = ' ';
		px[max_idx] = '\0';

		WriteUARTString("\n\rPixel:\n\r");

		for(int i = 0; i < CAMLINE_LENGTH; i++) {
			ItoA(cam_line[i],px+i*6,0);
		}
		WriteUARTN(px,max_idx/4);
		WriteUARTN(px+max_idx/4,max_idx/4);
		WriteUARTN(px+max_idx/2,max_idx/4);
		WriteUARTN(px+max_idx*3/4,max_idx/4);
	}
#else /* _With_UART_ */
	uint8_t tmp = cam_line[0];  // avoid warnings
#endif /* _With_UART_ */
}
