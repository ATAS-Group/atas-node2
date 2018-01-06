using namespace std;

#include "driver/uart.h"
#include "soc/uart_struct.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <array>

#include <math.h>

#include "minmea.h"

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

#define BUF_SIZE 1024
#define GPS_RX_PIN (27)
#define GPS_TX_PIN (26)


class Atasgps
{
	private:
		// in thousands, 5 = 10000
		const int decimalPlaces = 100000;		
		char *readLine();
    public:
        double* getLocation();
		Atasgps();
};