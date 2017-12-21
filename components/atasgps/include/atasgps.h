using namespace std;

#include "driver/uart.h"
#include "soc/uart_struct.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <array>

#include "minmea.h"

#define BUF_SIZE 1024
#define GPS_RX_PIN (26)
#define GPS_TX_PIN (27)


class Atasgps
{
	private:
		const int decimalPlaces = 8;		
		array<double,3> location;
		char *readLine();
    public:
        array<double,3> getLocation();
		Atasgps();
};