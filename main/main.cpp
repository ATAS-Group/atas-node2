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
			
			// Set Downlink RX2 to SF9
			LMIC.dn2Dr = DR_SF9;
			
			// Set Spreading Factor
			LMIC_setDrTxpow(DR_SF7, 14);
			
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
			// run os_runloop , 2s longer.
			delay(2000);
			// stop lora sending
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
	// atasgps: placeholder, 999 for no data, 0 and -1 not possible -> real coordinates
	gpsLocation[0] = 999;
	gpsLocation[1] = 999;
	
	bool showError = false;
	double* gpsLocationTemp = new double[3];
	
	while(1){
		// try to get data
		printf("atasgps: get data\n");
		gpsLocationTemp = atasgps->getLocation();
	
		if(isnan(gpsLocationTemp[0]) || isnan(gpsLocationTemp[1])){
			printf("atasgps: no valid data received yet, retry in 5s\n");
			if(showError == false){
				atasdisplay->displayGpsError();
			};
			showError = true;
			vTaskDelay(10000 / portTICK_RATE_MS);		
		}else{
			// only update data if new data arrived
			gpsLocation[0] = gpsLocationTemp[0];
			gpsLocation[1] = gpsLocationTemp[1];
			gpsLocation[2] = gpsLocationTemp[2];
		
			printf("atasgps: data received Lat: %f, Lng: %f, Alt: %f\n",gpsLocation[0],gpsLocation[1],gpsLocation[2]);		
			atasdisplay->displayGpsData(gpsLocation);
			showError = false;
			vTaskDelay(30000 / portTICK_RATE_MS);		
		}
	}
}


void buttonHandler(){
	int buttonIsPressedCounter = 0;
	
	while(1){
		
		vTaskDelay(100 / portTICK_RATE_MS);
	}
}

void buttonIntTask(void *pvParameters)
{
    printf("Waiting for button press interrupt on gpio...\r\n");
    QueueHandle_t *tsqueue = (QueueHandle_t *)pvParameters;
	
    uint32_t last = 0;
    while(1) {
        uint32_t button_ts;
        xQueueReceive(*tsqueue, &button_ts, portMAX_DELAY);
        button_ts *= portTICK_PERIOD_MS;
		
		// reduce "prellen"
        if(last < button_ts-200) {
			printf("Button interrupt last fired at %dms\r\n", last);
            printf("Button interrupt fired at %dms\r\n", button_ts);
			// presse for long enouth
			if(button_ts - last > (alarmTriggerTime * 1000)){
				printf("atasbutton: toggle Alamr\n");
				
				manualAlarmActive = !manualAlarmActive;
				
				if(manualAlarmActive == true){
					atasdisplay->displayManualAlarmIsOn();
				}else{
					// 
				}	
			}	
			last = button_ts;	
        }
    }
}


void alarmHandler(void * parameter){
	while(1){
		// in Danger
		printf("inDangerzone: %i\n",inDangerzone);
	
		if(inDangerzone > 0 && manualAlarmActive == false){
			// show danger
			atasdisplay->displayAlarm(static_cast<Alarm>(inDangerzone));
			// play sound
			if(atassound->getState() == false){
				printf("Atassounf: enable sound\n");
				//atassound->enable();
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
		vTaskDelay(2000 / portTICK_RATE_MS);
	}	
}

void loraHandler(void * parameter){
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
	
		// Disable Activities
		vTaskSuspend( xDisplayHandler );
		vTaskSuspend( xGPSHandler );
		vTaskSuspend( xAlarmHandler );
	
		// run, until the next tx succeds
		while(ataslora->getSendState() == send){
			os_runloop_once();
			delay(1);
		}	
		
		vTaskResume( xDisplayHandler );
		vTaskResume( xGPSHandler );
		vTaskResume( xAlarmHandler );
		
		vTaskDelay(txInterval * 1000 / portTICK_RATE_MS);
	}
}

void buttonCallback(void *args){
    uint32_t now = xTaskGetTickCountFromISR();
    xQueueSendToBackFromISR(tsqueue, &now, NULL);
}

void displayHandler(void * parameter){
	while(1){
		// check if we need to redraw the display	
		if(atasdisplay->getHasChanged()==true){
			// redraw
			atasdisplay->updateDisplay();
		}
		vTaskDelay(2000 / portTICK_RATE_MS);
	}
}


extern "C" void app_main()
{    
	// init Arduino subsystem
	initArduino();
	 
	printf("ATAS Node 2\n");
	
	// Init Components
	atasdisplay = new Atasdisplay();	
	ataslora = new Ataslora();	
	atasgps = new Atasgps();
	atasbutton = new Atasbutton(&buttonCallback);
	atassound = new Atassound();
	
	// Tasks
	tsqueue = xQueueCreate(2, sizeof(uint32_t));
	xTaskCreate(&displayHandler,"displayHandler",10000,NULL,1,&xDisplayHandler);
	xTaskCreate(&buttonIntTask, "buttonIntTask", 10000, &tsqueue, 2, NULL);
	xTaskCreate(&gpsHandler,"gpsHandler",10000,NULL,1,&xGPSHandler);
	xTaskCreate(&alarmHandler,"alarmHandler",10000,NULL,1,&xAlarmHandler);
	
	// wait a bit until start lora communication
	delay(8000);
	xTaskCreate(&loraHandler,"loraHandler",10000,NULL,2000,NULL);			
}