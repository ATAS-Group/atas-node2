using namespace std;

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include "esp_system.h"
#include <esp_log.h>

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

TaskHandle_t TaskHandle_GPS;
TaskHandle_t TaskHandle_Alarm;
TaskHandle_t TaskHandle_Display;


int inDangerzone = 0;
// in seconds
int txInterval = 20;
bool manualAlarmActive = false;

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
Atasbutton* atasbutton;

// Display
Atasdisplay* atasdisplay;