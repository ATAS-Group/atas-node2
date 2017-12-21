#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "minmea.h"
#include "atasgps.h"

#define BUF_SIZE 1024
#define GPS_RX_PIN (26)
#define GPS_TX_PIN (27)

Atasgps::Atasgps(){
	
	 /* UART config */
    uart_config_t uart_config;
    uart_config.baud_rate = 9600;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
	uart_config.rx_flow_ctrl_thresh = 122,

	//Configure UART parameters
    uart_param_config(UART_NUM_2, &uart_config);
    
    //Set UART pins(TX: IO27, RX: IO26)
	uart_set_pin(UART_NUM_2,
		GPS_TX_PIN,
		GPS_RX_PIN,         
		UART_PIN_NO_CHANGE, // RTS
		UART_PIN_NO_CHANGE  // CTS
	); 
    //Install UART driver
    uart_driver_install(UART_NUM_2, BUF_SIZE * 2, 0, 0, NULL, 0);
}

char * Atasgps::readLine(){
	// store line
	static char line[256];
	int length;
	char *pointerOfLine = line; // line as content of *pointer...
	
	while(1){
		// read one byte/char
		length = uart_read_bytes(UART_NUM_2, (unsigned char *)pointerOfLine, 1, portMAX_DELAY);
		if (length == 1) {
			// end of line found
			if (*pointerOfLine == '\n') {
				pointerOfLine++;
				*pointerOfLine = 0;
				return line;
			}
			pointerOfLine++; // move to the next char
		} // End of read a character
	} // End of loop
} // End of readLine

array<double,3> Atasgps::getLocation(){
    int datareceived = 0;
	while(datareceived == 0){
		// read line from gps
		char *line = readLine();
		
		struct minmea_sentence_gga frameGga;
		switch(minmea_sentence_id(line, false)) {
			case MINMEA_SENTENCE_GGA:
	    		if (minmea_parse_gga(&frameGga, line)) {
					location = {minmea_tocoord(&frameGga.latitude), minmea_tocoord(&frameGga.longitude), minmea_tocoord(&frameGga.height)};
					// data received
					datareceived = 1;
	    		}	
				break;
	   		default:{
				break;
	   		}	
		 } 
	}
	return location; 
}