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
            break;
        case EV_JOINED:
			printf("EV_JOINED\n");
            printf("ataslora: joined\n");
				
			ataslora->setConnectionState(joined);	
            // Disable link check validation (automatically enabled
            // during join, but not supported by TTN at this time).
            LMIC_setLinkCheckMode(0);
			
			// Set Downlink RX2 to SF9
			LMIC.dn2Dr = DR_SF9;
			
			// Set Spreading Factor
			LMIC_setDrTxpow(DR_SF9, 14);
			
            break;
        case EV_RFU1:
            printf("EV_RFU1\n");
            break;
        case EV_JOIN_FAILED:
            printf("EV_JOIN_FAILED\n");
			ataslora->setConnectionState(failed);		
            break;
        case EV_REJOIN_FAILED:
            printf("EV_REJOIN_FAILED\n");
			ataslora->setConnectionState(failed);	
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
			//atasdisplay->displayLoraData();
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
			if((showError == false) && (isnan(gpsLocation[0]) || isnan(gpsLocation[1]))){
				// display error only once
				atasdisplay->displayGpsError();
			};
			showError = true;
			vTaskDelay(10000 / portTICK_RATE_MS);		
		}else{
			// only update data if new data arrived
			// TODO
			
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

void buttonIntTask(void *pvParameters)
{
    printf("Waiting for button press interrupt on gpio...\r\n");
    QueueHandle_t *tsqueue = (QueueHandle_t *)pvParameters;
	int buttonPressedTime = 0;
	int buttonReleasedTime = 0;
	
    uint32_t last = 0;
    while(1) {
        uint32_t button_ts;
        xQueueReceive(*tsqueue, &button_ts, portMAX_DELAY);
        button_ts *= portTICK_PERIOD_MS;
		
		// reduce bouncing
        if(last < button_ts - 200) {
			int level = gpio_get_level(GPIO_NUM_15);
			
			// Button down
			if(level == 1){
				printf("Buttonpressed\n");
				buttonPressedTime = button_ts;
			}
			// Button release
			else{
				printf("Button released\n");
				buttonReleasedTime = button_ts;
				
				// When Button was realesed, check the time the button was pressed
				if(buttonReleasedTime - buttonPressedTime > alarmTriggerTime){	
					manualAlarmActive = !manualAlarmActive;
					// Audio Signal for change
					atassound->enable();
					delay(100);
					atassound->mute();
				}	
			}	
			last = button_ts;	
        }
    }
}


void alarmHandler(void * parameter){
	while(1){
		// Manual Alarm on
		if(manualAlarmActive == true){
			printf("alarmHandler: Manual Alarm On\n");
			atasdisplay->displayManualAlarmIsOn();
			// mute soundalarm
			atassound->mute();
		}
		else if(inDangerzone > 0){
			printf("alarmHandler: inDangerzone: %i\n",inDangerzone);
			// show danger
			atasdisplay->displayAlarm(static_cast<Alarm>(inDangerzone));
			// play sound
			printf("Atassound: enable sound\n");
			atassound->enable();
		} 
		// no Danger
		else {
			printf("alarmHandler: no Alarm\n");
			
			// mute soundalarm
			atassound->mute();
			
			// show dashboard
			atasdisplay->displayDashboard();
			
			// show text on display
			Connectionstate conState = ataslora->getConnectionState();
			switch(conState){
		        case joined:
		            atasdisplay->displayLoraStatus("Joined");	
		            break;
				case failed:
					atasdisplay->displayLoraStatus("Failed");
					break;
				case joining:
					atasdisplay->displayLoraStatus("Joining");
					break;	
				default:
					atasdisplay->displayLoraStatus("Not Connected");
					break;
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
	
		printf("ataslora: get ready to send \n");
		
		// wait for next send
		delay(1000);
		
		// get semaphore
		if(xSemaphoreTake( xSPISemaphore, portMAX_DELAY ) == pdTRUE){						
			// run, until the next tx succeeds
			while(ataslora->getSendState() == send){
				os_runloop_once();
				delay(1);
			}	
			xSemaphoreGive(xSPISemaphore);
		}
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

void buttonCallback(void *args){
    uint32_t now = xTaskGetTickCountFromISR();
    xQueueSendToBackFromISR(tsqueue, &now, NULL);
}

void displayHandler(void * parameter){
	while(1){
		
		// check if we need to redraw the display	
		if(atasdisplay->getHasChanged() == true){
			printf("atasdisplay: CHANGED\n");
			
			if(xSemaphoreTake( xSPISemaphore, portMAX_DELAY) == pdTRUE){
				//change priotity so drawing is not disturbed
				vTaskPrioritySet(xDisplayHandler,4);
				// redraw
				printf("atasdisplay: semaphore taken\n");
				
				atasdisplay->updateDisplay();
				// make sure picture is all drawn
				printf("atasdisplay: wait for finish drawn\n");
		
				xSemaphoreGive(xSPISemaphore);
				
				printf("atasdisplay: drawing done\n");
				// reset priority
				vTaskPrioritySet(xDisplayHandler,1);
			}else{
				printf("atasdisplay: cannot get semaphore\n");
			}
		}else{
			printf("atasdisplay: NO CHANGE\n");
		}
		vTaskDelay(1000 / portTICK_RATE_MS);
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
	
	// Semaphore
	xSPISemaphore = xSemaphoreCreateMutex(); 
	
	if( xSPISemaphore != NULL)
	{
		// Tasks
		tsqueue = xQueueCreate(2, sizeof(uint32_t));
	
		xTaskCreate(&displayHandler,"displayHandler",10000,NULL,1,&xDisplayHandler);	
		xTaskCreate(&alarmHandler,"alarmHandler",10000,NULL,1,&xAlarmHandler);	
		xTaskCreate(&buttonIntTask, "buttonIntTask", 10000, &tsqueue, 1, NULL);
		xTaskCreate(&gpsHandler,"gpsHandler",10000,NULL,1,&xGPSHandler);
		xTaskCreate(&loraHandler,"loraHandler",10000,NULL,1,&xLoraHandler);
		vTaskStartScheduler();		
	 }else{
		 printf("semaphore creation failed\n");
	 }	
}