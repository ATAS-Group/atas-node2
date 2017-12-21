#include "atasdisplay.h"

Atasdisplay::Atasdisplay(void) {
	io = new GxIO_SPI(SPI, DISPLAY_PIN_CS, DISPLAY_PIN_DC, DISPLAY_PIN_RESET);
	display =  new GxEPD_Class(*io, DISPLAY_PIN_RESET, DISPLAY_PIN_BUSY);
	display->init();
	display->setTextColor(GxEPD_BLACK);
}


void Atasdisplay::displayDashboard(){
	if(state != dashboard){
		display->setFont(fontsans9);	
		display->fillScreen(GxEPD_WHITE);
		display->drawBitmap(img_location, 10, 10, 40, 40, GxEPD_BLACK);
		display->setCursor(60, 20);
		display->println("Lat: 46.212134");
		display->setCursor(60, 45);
		display->println("Lng: 7.8932157");
		display->drawLine(0,80,200,80,GxEPD_BLACK);
		display->drawBitmap(img_updownload, 10, 100, 40, 40, GxEPD_BLACK);
		display->setCursor(60, 110);
		display->println("01.12.17 15:20");
		display->setCursor(60, 135);
		display->println("01.12.17 15:31");
		display->update();
		state = dashboard;
	}
}

void Atasdisplay::displayManualAlarmIsOn(){	
	if(state != manualalarm){
		display->setFont(fontsans12);
		display->setCursor(48, 175);
		display->fillScreen(GxEPD_WHITE);
		display->println("Alarm ON");
		display->drawBitmap(img_alarmbell, 36, 16, 128, 128, GxEPD_BLACK);
		display->update();
		state = manualalarm;
	}
}

void Atasdisplay::displayAlarm(Alarm alarm){
	if(state != indangerzone){
		display->setFont(fontsans12);
		display->setCursor(48, 175);
		display->fillScreen(GxEPD_WHITE);
	
		switch(alarm) {
		    case avalanche : 
				display->drawBitmap(img_avalanche, 36, 16, 128, 128, GxEPD_BLACK);
				display->println("Avalanche");
				printf("Triggered Alarm: Avalanche\n");
				break;
		    case landslide : 
				display->drawBitmap(img_landslide, 36, 16, 128, 128, GxEPD_BLACK);
				display->println("Landslide");
				printf("Triggered Alarm: Landslide\n");
		    	break;
			case snow:
				display->drawBitmap(img_snowflake, 36, 16, 128, 128, GxEPD_BLACK);
				display->setCursor(42, 175);
				display->println("Snowstorm");
				printf("Triggered Alarm: Snowstorm\n");
				break;
			default:
				display->println("Error");
				printf("displayAlarm: error, no alarm defined");
				break;
		}
		display->update();
		state = indangerzone;
	}
}