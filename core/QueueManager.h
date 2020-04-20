#ifndef _DPSOFTWARE_QUEUE_MANAGER_H
#define _DPSOFTWARE_QUEUE_MANAGER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "Helpers.h"
#include "../../include/Secrets.h"
#include "../../include/Configuration.h"


extern PubSubClient mqttClient;
 
class QueueManager {
  
  public:

    void setupMQTTQueue(void (*callback)(char*, byte*, unsigned int));
    void mqttReconnect(void (*manageDisconnectionFunction)(), void (*manageQueueSubscription)(), void (*manageHardwareButton)());

};

#endif