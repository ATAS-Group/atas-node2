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
			
			// Set Spreading Factor
			LMIC_setDrTxpow(DR_SF12, 14);
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
	double* gpsLocationTemp = new double[3];
	
	while(1){
		// try to get data
		printf("atasgps: get data\n");
		gpsLocationTemp = atasgps->getLocation();
		
		if(isnan(gpsLocationTemp[0]) || isnan(gpsLocationTemp[1])){
			printf("atasgps: no valid data received yet, retry in 5s\n");
			if(showError == false){atasdisplay->displayGpsError();};
			showError = true;
			// fast repeat, 5sec
			vTaskDelay(5000 / portTICK_RATE_MS);
		}else{
			// only update data if new data arrived
			gpsLocation[0] = gpsLocationTemp[0];
			gpsLocation[1] = gpsLocationTemp[1];
			gpsLocation[2] = gpsLocationTemp[2];
			
			printf("atasgps: data received Lat: %f, Lng: %f, Alt: %f\n",gpsLocation[0],gpsLocation[1],gpsLocation[2]);		
			atasdisplay->displayGpsData(gpsLocation);
			showError = false;
			// update all 30sek
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


void displayHandler(void * parameter){
	while(1){
		atasdisplay->updateDisplay();
		vTaskDelay(10000 / portTICK_RATE_MS);
	}
}

void alarmHandler(void * parameter){
	while(1){
		// Manual Alarm has highest Priority
		if(manualAlarmActive == false){		
			// in Danger
			printf("inDangerzone: %i\n",inDangerzone);
			
			if(inDangerzone > 0){
				// show danger
				atasdisplay->displayAlarm(static_cast<Alarm>(inDangerzone));
				// play sound
				if(atassound->getState() == false){
					atassound->enable();
				}
			} 
			// Manual Alarm ative
		
			// no Danger
			else {
				atasdisplay->displayDashboard();
				// mute soundalarm
				if(atassound->getState() == true){
					atassound->mute();
				}
			}
		}
		vTaskDelay(2000 / portTICK_RATE_MS);
	}
}

extern "C" void app_main()
{    
	// init Arduino subsystem
	initArduino();
	 
	printf("ATAS Node 2\n");
	
	// atasgps: placeholder, 999 for no data, 0 and -1 not possible -> real coordinates
	gpsLocation[0] = 999;
	gpsLocation[1] = 999;
	
	atasdisplay = new Atasdisplay();
	// wait for display init.
	vTaskDelay(2000 / portTICK_RATE_MS);
		
	ataslora = new Ataslora();	
	atasgps = new Atasgps();
	//atasbutton = new Atasbutton();
	atassound = new Atassound();
	
	// Tasks
	xTaskCreate(&gpsHandler,"gpsHandler",10000,NULL,3,&TaskHandle_GPS);
	xTaskCreate(&alarmHandler,"alarmHandler",10000,NULL,2,&TaskHandle_Alarm);
	xTaskCreate(&displayHandler,"displayHandler",10000,NULL,2,&TaskHandle_Display);
	
	// ataslora
	ataslora->init();
		
	while(1){	
		ostringstream oss;
		oss.precision(5);
		
		// prepare data
		oss << fixed << gpsLocation[0] << "," << gpsLocation[1] << "," << manualAlarmActive << "," << inDangerzone;			
		string message = oss.str();	

		// set data to be send over lora
		ataslora->setData(&sendDataJob, message);

		// enable ataslora to send data
		ataslora->setSendState(send);

		// Supend Activities
		vTaskSuspend(TaskHandle_GPS);
		vTaskSuspend(TaskHandle_Alarm);
		vTaskSuspend(TaskHandle_Display);
			
		// run, until the next tx succeds
		while(ataslora->getSendState() == send){
			os_runloop_once();
			delay(1);
		}
		
		// time for other tasks to run, only if joined
		if(ataslora->getConnectionState() == joined){
			vTaskDelay(10000 / portTICK_RATE_MS);
		}
		
		// Resume Activities
		vTaskResume(TaskHandle_GPS);
		vTaskResume(TaskHandle_Alarm);
		vTaskResume(TaskHandle_Display);
		
	}
}