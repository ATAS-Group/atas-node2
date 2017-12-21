#include "ataslora.h"

void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = LORA_PIN_CS,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = LORA_PIN_RESET,
	// DIO3 not used, FSK only
    .dio = {LORA_PIN_DIO0, LORA_PIN_DIO1, LMIC_UNUSED_PIN},
};

void Ataslora::setState(int newState){
    state = newState;
}


int Ataslora::getState(){
	return state;
}


void Ataslora::init(){
	// LMIC init
    printf("ataslora: OS Init\n");
	os_init();
	
    // Reset MAC state. Session and pending data transfers will be discarded.
    printf("ataslora: Lmic Reset\n");
	LMIC_reset();
}

void Ataslora::setData(osjob_t* job, std::string messageString) {
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        printf("ataslora: OP_TXRXPEND, not sending\n");
    } else{
		// convert string -> unsigned char *
		unsigned char* message = (unsigned char*)messageString.c_str();
        LMIC_setTxData2(1, message, messageString.length(), 0);
        printf("ataslora: packets queued\n");
	}
}