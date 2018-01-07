// Standar Lib
#include <SPI.h>

// Display, Waveshare 1.54 200x200
#include "GxEPD.h"
#include "GxGDEW0154Z04.h"
#include "GxIO.h"
#include "GxIO_SPI.h"

#include "images.h"

#include <array>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream> 

// Font
#include "FreeSans12pt7b.h"
#include "FreeSans9pt7b.h"

// Arduino, allowes GxEPD
#include "Arduino.h"


// Pin Definition
static const int DISPLAY_PIN_CS = 5; 		// orange
static const int DISPLAY_PIN_DC =  17; 		// green
static const int DISPLAY_PIN_RESET = 16; 	// white
static const int DISPLAY_PIN_BUSY = 4; 		// violett

// State of the Display
enum Alarm { avalanche=1, snow, landslide };
enum Displaystate {indangerzone,dashboard,manualalarm};

class Atasdisplay{
	private:
		Displaystate state;
		const GFXfont* fontsans9 = &FreeSans9pt7b;
		const GFXfont* fontsans12 = &FreeSans12pt7b;
		GxIO_SPI *io;
		GxEPD_Class *display;
		bool hasChanged = false;
   	public:
		bool getHasChanged();
		void updateDisplay();
		void displayDashboard();
		void displayGpsError();
		void displayLoraStatus(String status);
		void displayLoraData(int messageTimes[2]);
		void displayGpsData(double gpsLocation[3]);
		void displayAlarm(Alarm alarm);
		void displayManualAlarmIsOn();
		Atasdisplay();
};