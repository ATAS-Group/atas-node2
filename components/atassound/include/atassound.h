#include <stdio.h>
#include "driver/ledc.h"

#define PWM_PIN 21
#define PWM_BITS LEDC_TIMER_10_BIT
#define PWM_FREQ 1000
#define PWM_TIMER LEDC_TIMER_0
#define PWM_MODE LEDC_HIGH_SPEED_MODE
#define PWM_CHANNEL LEDC_CHANNEL_0
#define PWM_LOW 0
#define PWM_HIGH 512

class Atassound
{
    private:
		ledc_timer_config_t timer_conf;
		bool isenabled = false;
	public:
		Atassound();
        void enable();
        void mute();
		bool getState();
};