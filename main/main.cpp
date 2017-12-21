#include "main.h"

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
            printf("ataslora: joined\n");
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
		printf("ataslora: tx complete\n");
			if (LMIC.txrxFlags & TXRX_ACK){printf("ataslora: received ack\n");}
		    if (LMIC.dataLen) {
				printf("ataslora: received downlink message\n");

				uint8_t payload = 0;
				for (int i = 0; i < LMIC.dataLen; i++) {
	        		payload = LMIC.frame[LMIC.dataBeg + i];
	   			}
				
				// set Dangerzone
				printf("ataslora: received data: %d\n", payload);
				inDangerzone = payload;
			}
			ataslora->setState(1);	
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
void hello(void *args){
	buttonPressed = !buttonPressed;
}

void gpsHandler(void * parameter){	
	while(1){
		// try to get data
		gpsLocation = atasgps->getLocation();
		
		if((gpsLocation[0] == 999) || (gpsLocation[1] == 999)){
			receivedGPSData = false;
			printf("atasgps: no valid data received yet, retry in 5s\n");
		}else{
			printf("atasgps: data received\n");
			receivedGPSData = true;
		}
		printf("Lat:%f\n",gpsLocation[0]);
		printf("Lng:%f\n",gpsLocation[1]);
		vTaskDelay(30000 / portTICK_RATE_MS);
	}
}


void alarmHandler(void * parameter){
	while(1){
		// in Danger
		if(inDangerzone > 0){
			// show danger
			atasdisplay->displayAlarm(static_cast<Alarm>(inDangerzone));
			// play sound
			if(atassound->getState() == false){
				atassound->enable();
			}
		}
		// no Danger
		else{
			printf("NO DANGER\n");
			atasdisplay->displayDashboard();
			// mute soundalarm
			if(atassound->getState() == true){
				atassound->mute();
			}
		}
		vTaskDelay(5000 / portTICK_RATE_MS);
	}
}

extern "C" void app_main()
{    
	// init Arduino subsystem
	initArduino();
	 
	printf("ATAS Node 2\n");
	
	atasdisplay = new Atasdisplay();
	ataslora = new Ataslora();
	atasgps = new Atasgps();
	atasbutton = new Atasbutton(&hello);
	atassound = new Atassound();
	
	xTaskCreate(&gpsHandler,"gpsHandler",10000,NULL,2,NULL); 
	xTaskCreate(&alarmHandler,"alarmHandler",10000,NULL,3,NULL); 
	
	// ataslora
	ataslora->init();
	
	while(1){
		ostringstream oss;

		// prepare data
		oss << gpsLocation[0] << "," << gpsLocation[1] << "," << buttonPressed << "," << inDangerzone;			
		string message = oss.str();

		// set data to be send over lora
		ataslora->setData(&sendDataJob, message);

		// enable ataslora to send data
		ataslora->setState(0);

		// run, until the next tx succeds
		while(ataslora->getState() == 0){
			os_runloop_once();
			delay(2);
		}
	}
}