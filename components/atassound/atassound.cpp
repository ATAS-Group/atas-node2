#include "atassound.h"

Atassound::Atassound(){
	ledc_timer_config_t timer;
		timer.duty_resolution = PWM_BITS;
		timer.freq_hz = PWM_FREQ;
		timer.speed_mode = PWM_MODE;
		timer.timer_num = PWM_TIMER;

	ledc_timer_config(&timer);

	ledc_channel_config_t channel;
		channel.channel = PWM_CHANNEL;
		channel.duty = 0;
		channel.gpio_num = PWM_PIN;
		channel.speed_mode = PWM_MODE;
		channel.timer_sel = PWM_TIMER;	
		
	ledc_channel_config(&channel);
}


void Atassound::mute(){
	ledc_set_duty(PWM_MODE, PWM_CHANNEL, PWM_LOW);
	ledc_update_duty(PWM_MODE, PWM_CHANNEL);
	isenabled = false;
	printf("Atassound: mute\n");
}

void Atassound::enable(){
	ledc_set_duty(PWM_MODE, PWM_CHANNEL, PWM_HIGH);
	ledc_update_duty(PWM_MODE, PWM_CHANNEL);
	isenabled = true;
	printf("Atassound: enable\n");
}

bool Atassound::getState(){
	return isenabled;
}	