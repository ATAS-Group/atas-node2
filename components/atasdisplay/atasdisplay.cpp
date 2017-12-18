#include "atasdisplay.h"

Atasdisplay::Atasdisplay(void) {
	io = new GxIO_SPI(SPI, DISPLAY_PIN_CS, DISPLAY_PIN_DC, DISPLAY_PIN_RESET);
	display =  new GxEPD_Class(*io, DISPLAY_PIN_RESET, DISPLAY_PIN_BUSY);
	display->init();
	display->setTextColor(GxEPD_BLACK);
}

void Atasdisplay::displayDashboard(){
	display->setFont(fontsans9);	
	display->fillScreen(GxEPD_WHITE);
	//display->drawBitmap(img_location, 10, 10, 40, 40, GxEPD_BLACK);
	display->setCursor(60, 20);
	display->println("Lat: 46.212134");
	display->setCursor(60, 45);
	display->println("Lng: 7.8932157");
	display->update();
}