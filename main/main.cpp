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
				
				// in Danger
				if(inDangerzone > 0){
					// show danger
					// -1 to create a valid Alarm enum
					atasdisplay->displayAlarm(static_cast<Alarm>(inDangerzone));
					// play sound
					//atassound->enable();
					printf("PLAY SOUND\n");
				}
				// no Danger
				else{
					atasdisplay->displayDashboard();
					// mute soundalarm
					/*if(atassound->getState() == true){
		            	atassound->mute();
					}*/
				}
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

extern "C" void app_main()
{    
	// init Arduino subsystem
	initArduino();
	 
	printf("ATAS Node 2\n");
	
	atasdisplay = new Atasdisplay();
	ataslora = new Ataslora();
	atasgps = new Atasgps();
	
	// start, show dashboard
	atasdisplay->displayDashboard();
	
	// init
	ataslora->init();
	
	while(1){
			
		// ***** Button *****
		// get Button state
		//buttonPressed =  atasbutton->getState()
		
		
		
		
		
		// ***** Button *****			
		

		// handle sound
		/*if(inDangerzone == 0){
			if(atassound->getState() == true){
                                atassound->mute();
    			}
                }
                    else if(inDangerzone == 1){
                            printf("PLAY SOUND\n");
			//atassound->enable();
                    }*/

		// **** GPS ***** 				
		// get gps data, max try -> gpsRetryCount
		gpsTryCount = 0;
		while((isnan(gpsLocation[0])) || (isnan(gpsLocation[1])) || (gpsTryCount < gpsRetryCount)){
			// get GPS Data
			// 0, latitude | 1, longitude
			gpsLocation = atasgps->getLocation();
			gpsTryCount++;
		}
		
		if((isnan(gpsLocation[0])) || (isnan(gpsLocation[1]))){
			receivedGPSData = false;
			printf("atasgps: no valid data received yet");
		}else{
			printf("atasgps: data received");
			receivedGPSData = true;
		}
			
		// ***** Lora *****
		// build string 
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
		}

		sleep(txInterval);
	}
}