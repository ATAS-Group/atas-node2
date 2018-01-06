#include <stdio.h>
#include "driver/gpio.h"

#define BUTTON_GPIO GPIO_NUM_15

class Atasbutton
{
    private:
		bool buttonIsPressed = false;
	public:
		Atasbutton();
		bool getState();
		void buttonCallback(void *args);
};