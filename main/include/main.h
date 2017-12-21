using namespace std;

#include <driver/gpio.h>
#include <esp_log.h>
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

int inDangerzone;
bool buttonPressed;
int txInterval = 20;

// Lora
Ataslora* ataslora;
static osjob_t sendDataJob;

// GPS
Atasgps* atasgps;
array<double,3>  gpsLocation;
const int gpsRetryCount = 5;
int gpsTryCount = 0;
bool receivedGPSData = false;

// Sound
Atassound* atassound;

// Button Handling
//Atasbutton* atasbutton;

// Display
Atasdisplay* atasdisplay;