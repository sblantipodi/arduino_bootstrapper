#ifndef _DPSOFTWARE_BOOTSTRAP_MANAGER_H
#define _DPSOFTWARE_BOOTSTRAP_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "../../include/Configuration.h"
#include "Helpers.h"
#include "WifiManager.h"
#include "QueueManager.h"

// Maximum JSON Object Size
const int BUFFER_SIZE = JSON_OBJECT_SIZE(MAX_JSON_OBJECT_SIZE);
 
class BootstrapManager {
  
  public:
    void bootstrapSetup(void (*manageDisconnectionFunction)(), void (*manageHardwareButton)(), void (*callback)(char*, byte*, unsigned int));
    void bootstrapLoop(void (*manageDisconnectionFunction)(), void (*manageQueueSubscription)(), void (*manageHardwareButton)());

};

#endif