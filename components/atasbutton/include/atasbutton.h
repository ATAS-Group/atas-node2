#include <stdio.h>
#include "driver/gpio.h"

#define BUTTON_GPIO GPIO_NUM_15

class Atasbutton
{
    private:
	public:
		Atasbutton(gpio_isr_t buttonCallback);
		bool getState();
};