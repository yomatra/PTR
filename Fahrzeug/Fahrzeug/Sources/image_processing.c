#include "MPC5606B.h"
#include "image_processing.h"
#include "fcn_base_camera.h"
#include "debug.h"

void sobel_filter(uint8_t camline[], int8_t derivative_line[]) 
{
	camline[0] = camline[0]; // avoid warnings
	derivative_line[0] = 0; // avoid warnings 
	//TO-DO
}


void edge_locations(int8_t derivative_line[], uint8_t edge_locations[], uint8_t offset) 
{
	edge_locations[0] = edge_locations[0]; // avoid warnings
	derivative_line[0] = 0; 
	offset = 0;  
	//TO-DO
}


uint8_t line_location (uint8_t edge_locations[], uint8_t line_info[])
{
	edge_locations[0] = edge_locations[0]; // avoid warnings
	line_info[0] = line_info[1] = line_info[2] = line_info[3] = 0;  
	//TO-DO
}


