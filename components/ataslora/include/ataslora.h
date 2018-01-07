// LMIC
#include <lmic.h>
#include <hal.h>
#include <SPI.h>
#include <string>
#include <stdio.h>

#include <driver/gpio.h>

// memcpy_P function
#include <pgmspace.h>

// already declared in pins_arduino.h
//static const uint8_t MISO = 19;
//static const uint8_t MOSI = 23; 	
//static const uint8_t SCK = 18; 	
static const uint8_t LORA_PIN_CS = 2; 		

//static const int PIN_DC =  17; 	
static const int LORA_PIN_RESET = 16; 	
//static const int PIN_BUSY = 4; 		
static const int LORA_PIN_DIO0 = 32;
static const int LORA_PIN_DIO1 = 33;


// app eui, ittle-endian format
static const u1_t PROGMEM APPEUI[8]={ 0x4A, 0x40, 0x00, 0xF0, 0x7E, 0xD5, 0xB3, 0x70 };

// device eui, little endian format
static const u1_t PROGMEM DEVEUI[8]= { 0x06, 0x58, 0x15, 0xC0, 0x06, 0xDE, 0x29, 0x00 };

// appkey, big endian
static const u1_t PROGMEM APPKEY[16] = { 0xE8, 0xAD, 0x93, 0x8D, 0x8F, 0x77, 0x65, 0xBE, 0x11, 0xCF, 0x87, 0xEE, 0x03, 0xC8, 0x7F, 0xA5 };

enum Connectionstate {joined,joining,failed,start};
enum Sendstate {send,nosend};

class Ataslora{
	private:
	Connectionstate connectionState = start;
	Sendstate sendState = send;
    public:
	void setConnectionState(Connectionstate state);
	Connectionstate getConnectionState();
	void setSendState(Sendstate state);
	Sendstate getSendState();
	void setData(osjob_t* job, std::string message);
	void init();
};