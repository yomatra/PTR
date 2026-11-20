
#ifndef DEBUG_H_
#define DEBUG_H_

//debug functions
void debug_camline_uint8(uint8_t camline[]);
void debug_camline_int8(int8_t camline[]);
void debug_position_of_edges(uint8_t position_of_edges[]);
void LED_show_cycle_time(uint32_t cycle_time);

void printf_pixels(const uint8_t cam_line[]); // Prints raw camera data to serial.
void printf_pixels_txt(const uint8_t cam_line[]); // Prints readable camera data to serial

#endif /* DEBUG_H_ */
