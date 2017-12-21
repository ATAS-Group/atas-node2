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

int inDangerzone = 0;
int txInterval = 20;

// Lora
Ataslora* ataslora;
static osjob_t sendDataJob;

// GPS
Atasgps* atasgps;
array<double,3>  gpsLocation;
bool receivedGPSData = false;

// Sound
Atassound* atassound;

// Button Handling
Atasbutton* atasbutton;
bool buttonPressed;

// Display
Atasdisplay* atasdisplay;