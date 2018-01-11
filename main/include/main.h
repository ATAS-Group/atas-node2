using namespace std;

#include "esp_system.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <freertos/semphr.H>

#include <driver/gpio.h>
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include <unistd.h>
#include <stdio.h>
#include <math.h> 

// oss stream
#include <sstream>
#include <iostream> 

// Types
#include <string>
#include <array>

// Arduino
#include "Arduino.h"

// Atas Classes
#include "atasdisplay.h"
#include "ataslora.h"
#include "atasgps.h"
#include "atassound.h"
#include "atasbutton.h"

TaskHandle_t xDisplayHandler;
TaskHandle_t xGPSHandler;
TaskHandle_t xAlarmHandler;
TaskHandle_t xLoraHandler;

SemaphoreHandle_t xSPISemaphore = NULL;
 
int inDangerzone = 0;
// in seconds
int txInterval = 20;

// Lora
Ataslora* ataslora;
static osjob_t sendDataJob;

// GPS
Atasgps* atasgps;
double* gpsLocation = new double[3];
bool receivedGPSData = false;

// Sound
Atassound* atassound;

// Button Handling
bool manualAlarmActive = false;
static QueueHandle_t tsqueue;
Atasbutton* atasbutton;
// in seconds
int alarmTriggerTime = 300;

// Display
Atasdisplay* atasdisplay;