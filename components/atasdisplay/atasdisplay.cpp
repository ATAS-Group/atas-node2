#include "atasdisplay.h"


Atasdisplay::Atasdisplay(void) {
	io = new GxIO_SPI(SPI, DISPLAY_PIN_CS, DISPLAY_PIN_DC, DISPLAY_PIN_RESET);
	display =  new GxEPD_Class(*io, DISPLAY_PIN_RESET, DISPLAY_PIN_BUSY);
	display->init();
	display->setRotation(2);
	display->setTextColor(GxEPD_BLACK);
	displayDashboard();
}

void Atasdisplay::displayGpsData(double gpsLocation[3]){	
	if(state == dashboard){		
		// prepare
		// latitude
		std::ostringstream latStream;
		latStream.precision(5);
		latStream << std::fixed << "Lat: " << gpsLocation[0];
		std::string latString = latStream.str();
		char *lat = new char[latString.length() + 1]; 
		std::strcpy(lat, latString.c_str());
		
		// longitude
		std::ostringstream lngStream;
		lngStream.precision(5);
		lngStream << std::fixed << "Lng: " << gpsLocation[1];
		std::string lngString = lngStream.str();
		char *lng = new char[lngString.length() + 1]; 
		std::strcpy(lng, lngString.c_str());
		
		// altitude
		std::ostringstream altitudeStream;
		altitudeStream << "Alt: " << int(gpsLocation[2]) << "m";
		std::string altitudeString = altitudeStream.str();
		char *alt = new char[altitudeString.length() + 1]; 
		std::strcpy(alt, altitudeString.c_str());
		
		// show
		display->fillRect(60, 0, 140, 99, GxEPD_WHITE);
		display->setCursor(60, 35);
		display->println(lat);
		display->setCursor(60, 55);
		display->println(lng);
		display->setCursor(60, 75);
		display->println(alt);
		hasChanged = true;
	}
}

bool Atasdisplay::getHasChanged(){
	return hasChanged;
}

void Atasdisplay::displayGpsError(){
	if(state == dashboard){
		printf("show gps error\n");
		display->fillRect(60, 20, 140, 50, GxEPD_WHITE);
		display->setCursor(60, 55);
		display->println("No Data yet");
		hasChanged = true;
	}
}

void Atasdisplay::displayLoraStatus(String status){
	if(state == dashboard){
		display->fillRect(60, 101, 140, 99, GxEPD_WHITE);
		display->setCursor(60, 155);
		display->println(status);
		hasChanged = true;
	}
}

void Atasdisplay::displayLoraData(int messageTimes[2]){
	
}


void Atasdisplay::updateDisplay(){
	if(hasChanged == true){
		display->update();
		hasChanged = false;
	}
}



void Atasdisplay::displayDashboard(){
	if(state != dashboard){
		display->setFont(fontsans9);	
		display->fillScreen(GxEPD_WHITE);
		display->drawBitmap(img_location, 10, 30, 40, 40, GxEPD_BLACK);
		display->drawLine(0,100,200,100,GxEPD_BLACK);
		display->drawBitmap(img_updownload, 10, 130, 40, 40, GxEPD_BLACK);
		// gps 
		display->setCursor(60, 55);
		display->println("No Data yet");
		// lora
		display->setCursor(60, 155);
		display->println("Idle");
		
		hasChanged = true;
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
		state = manualalarm;
		hasChanged = true;
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
		hasChanged = true;
		state = indangerzone;
	}
}