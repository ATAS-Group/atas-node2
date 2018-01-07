#include "atasbutton.h"

Atasbutton::Atasbutton(gpio_isr_t buttonCallback){
  	//Configure button
    gpio_config_t btn_config;
    btn_config.intr_type = GPIO_INTR_ANYEDGE;    //Enable interrupt on both rising and falling edges
    btn_config.mode = GPIO_MODE_INPUT;           //Set as Input
    btn_config.pin_bit_mask = (1 << BUTTON_GPIO); //Bitmask
    btn_config.pull_up_en = GPIO_PULLUP_DISABLE;    //Disable pullup
    btn_config.pull_down_en = GPIO_PULLDOWN_ENABLE; //Enable pulldown
    gpio_config(&btn_config);
    printf("Button configured\n");
 	//Configure interrupt and add handler
    gpio_install_isr_service(0);                  //Start Interrupt Service Routine service
    gpio_isr_handler_add(BUTTON_GPIO, buttonCallback, NULL); //Add handler of interrupt
    printf("atasbutton: Interrupt configured\n");
}

bool Atasbutton::getState()
{
  	return gpio_get_level(BUTTON_GPIO);
}