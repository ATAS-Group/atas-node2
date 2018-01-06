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
			ataslora->setConnectionState(joining);	
			atasdisplay->displayLoraStatus("Joining");
            break;
        case EV_JOINED:
            printf("ataslora: joined\n");
			atasdisplay->displayLoraStatus("Joined");	
			ataslora->setConnectionState(joined);	
            // Disable link check validation (automatically enabled
            // during join, but not supported by TTN at this time).
            LMIC_setLinkCheckMode(0);
            break;
        case EV_RFU1:
            printf("EV_RFU1\n");
            break;
        case EV_JOIN_FAILED:
            printf("EV_JOIN_FAILED\n");
			ataslora->setConnectionState(failed);	
			atasdisplay->displayLoraStatus("Join Failed");	
            break;
        case EV_REJOIN_FAILED:
            printf("EV_REJOIN_FAILED\n");
			ataslora->setConnectionState(failed);	
			atasdisplay->displayLoraStatus("Rejoin Failed");	
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
			ataslora->setSendState(nosend);	
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

void gpsHandler(void * parameter){	
	bool showError = false;
	
	while(1){
		// try to get data
		printf("atasgps: get data\n");
		gpsLocation = atasgps->getLocation();
		
		printf("%f, %f\n",gpsLocation[0],gpsLocation[1]);
		
		if(isnan(gpsLocation[0]) || isnan(gpsLocation[1])){
			printf("atasgps: no valid data received yet, retry in 5s\n");
			if(showError == false){atasdisplay->displayGpsError();};
			showError = true;
			vTaskDelay(5000 / portTICK_RATE_MS);
		}else{
			printf("atasgps: data received Lat: %f, Lng: %f\n",gpsLocation[0],gpsLocation[1]);		
			atasdisplay->displayGpsData(gpsLocation);
			showError = false;
			vTaskDelay(30000 / portTICK_RATE_MS);
		}	
	}
}

/*
void buttonHandler(void * parameter){
	int buttonIsPressedCounter = 0;
	
	while(1){
		if(atasbutton->getState() == true){
			buttonIsPressedCounter++;
		}else{
			buttonIsPressedCounter = 0;
		}		
		if(buttonIsPressedCounter > 30){
			atasdisplay->displayManualAlarmIsOn();
		}
		vTaskDelay(100 / portTICK_RATE_MS);
	}
}*/


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
		else {
			atasdisplay->displayDashboard();
			// mute soundalarm
			if(atassound->getState() == true){
				atassound->mute();
			}
		}
		vTaskDelay(20000 / portTICK_RATE_MS);
	}
}

extern "C" void app_main()
{    
	// init Arduino subsystem
	initArduino();
	 
	printf("ATAS Node 2\n");
	
	atasdisplay = new Atasdisplay();
	// wait for display init.
	vTaskDelay(2000 / portTICK_RATE_MS);
		
	ataslora = new Ataslora();
	atasgps = new Atasgps();
	//atasbutton = new Atasbutton();
	atassound = new Atassound();
	
	xTaskCreate(&gpsHandler,"gpsHandler",10000,NULL,2,NULL);
	//xTaskCreate(&alarmHandler,"alarmHandler",10000,NULL,2,NULL);
	
	// ataslora
	ataslora->init();
	
	while(1){
		ostringstream oss;

		// prepare data
		//oss << gpsLocation[0] << "," << gpsLocation[1] << "," << manualAlarmActive << "," << inDangerzone;			
		string message = oss.str();

		// set data to be send over lora
		ataslora->setData(&sendDataJob, message);

		// enable ataslora to send data
		ataslora->setSendState(send);

		// run, until the next tx succeds
		while(ataslora->getSendState() == send){
			os_runloop_once();
			atasdisplay->updateDisplay();
			vTaskDelay(20 / portTICK_RATE_MS);
		}
	}
}