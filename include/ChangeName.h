#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Version.h>
#include "Configuration.h"
#include "../arduino_bootstrapper/core/BootstrapManager.h"


/****************** BOOTSTRAP MANAGER ******************/
BootstrapManager bootstrapManager;


/**************************** PIN DEFINITIONS **************************************************/
// Define your pin here


/************* MQTT TOPICS **************************/
const char* smartostat_sensor_state_topic = "tele/smartostat/SENSOR";


/********************************** FUNCTION DECLARATION (NEEDED BY PLATFORMIO WHILE COMPILING CPP FILES) *****************************************/
void callback(char* topic, byte* payload, unsigned int length);
void manageDisconnections();
void manageQueueSubscription();
void manageHardwareButton();
