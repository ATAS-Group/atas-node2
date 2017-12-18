#include <driver/gpio.h>
#include <esp_log.h>
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"

// Arduino, allowes GxEPD
#include "Arduino.h"

// LMIC
#include <lmic.h>
#include <hal.h>
#include <SPI.h>

// already declared in pins_arduino.h
//static const uint8_t MISO = 19; 	// blue
//static const uint8_t MOSI = 23; 	// blue
//static const uint8_t SCK = 18; 	// yellow
static const uint8_t PIN_CS = 5; 		// orange

//static const int PIN_DC =  17; 	// green
static const int PIN_RESET = 16; 	// white
//static const int PIN_BUSY = 4; 		// violett
static const int PIN_DIO0 = 32;
static const int PIN_DIO1 = 33;

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8]={ 0x4A, 0x40, 0x00, 0xF0, 0x7E, 0xD5, 0xB3, 0x70 };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8]= { 0x06, 0x58, 0x15, 0xC0, 0x06, 0xDE, 0x29, 0x00 };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.
static const u1_t PROGMEM APPKEY[16] = { 0xE8, 0xAD, 0x93, 0x8D, 0x8F, 0x77, 0x65, 0xBE, 0x11, 0xCF, 0x87, 0xEE, 0x03, 0xC8, 0x7F, 0xA5 };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

static uint8_t mydata[] = "Hello, world!";
static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 60;


// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = PIN_CS,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = PIN_RESET,
	// DIO3 not used, FSK only
    .dio = {PIN_DIO0, PIN_DIO1, LMIC_UNUSED_PIN},
};

void do_send(osjob_t* j){
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        printf("OP_TXRXPEND, not sending\n");
    } else {
        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);
        printf("Packet queued\n");
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            printf("EV_SCAN_TIMEOUT\n");
            break;
        case EV_BEACON_FOUND:
             printf("EV_BEACON_FOUND\n");
            break;
        case EV_BEACON_MISSED:
             printf("EV_BEACON_MISSED\n");
            break;
        case EV_BEACON_TRACKED:
             printf("EV_BEACON_TRACKED\n");
            break;
        case EV_JOINING:
             printf("EV_JOINING\n");
            break;
        case EV_JOINED:
             printf("EV_JOINED\n");

            // Disable link check validation (automatically enabled
            // during join, but not supported by TTN at this time).
            LMIC_setLinkCheckMode(0);
            break;
        case EV_RFU1:
            printf("EV_RFU1\n");
            break;
        case EV_JOIN_FAILED:
            printf("EV_JOIN_FAILED\n");
            break;
        case EV_REJOIN_FAILED:
            printf("EV_REJOIN_FAILED\n");
            break;
            break;
        case EV_TXCOMPLETE:
            printf("EV_TXCOMPLETE (includes waiting for RX windows)\n");
            if (LMIC.txrxFlags & TXRX_ACK)
              printf("Received ack\n");
            if (LMIC.dataLen) {
              printf("Received ");
              printf("%d",LMIC.dataLen);
              printf(" bytes of payload\n");
            }
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_LOST_TSYNC:
            printf("EV_LOST_TSYNC\n");
            break;
        case EV_RESET:
            printf("EV_RESET\n");
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            printf("EV_RXCOMPLETE\n");
            break;
        case EV_LINK_DEAD:
            printf("EV_LINK_DEAD\n");
            break;
        case EV_LINK_ALIVE:
            printf("EV_LINK_ALIVE\n");
            break;
         default:
            printf("Unknown event\n");
            break;
    }
}

extern "C" void app_main()
{    
	// init Arduino subsystem
	initArduino();
	 
	printf("ATAS Node 2 - Lora Example!\n");
	
	// LMIC init
    printf("OS Init\n");
	os_init();
	
    // Reset the MAC state. Session and pending data transfers will be discarded.
    printf("Lmic Reset\n");
	LMIC_reset();

    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
	
	while(1){
		os_runloop_once();
		delay(2);
	}
}

