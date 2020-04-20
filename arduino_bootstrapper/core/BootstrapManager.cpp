#include "BootstrapManager.h"


// WifiManager class for Wifi management
WifiManager wifiManager;
// QueueManager class for MQTT queue management
QueueManager queueManager;

/********************************** BOOTSTRAP FUNCTIONS FOR SETUP() *****************************************/
void BootstrapManager::bootstrapSetup(void (*manageDisconnections)(), void (*manageHardwareButton)(), void (*callback)(char*, byte*, unsigned int)) {
  // Initialize Wifi manager
  wifiManager.setupWiFi(manageDisconnections, manageHardwareButton);
  // Initialize Queue Manager
  queueManager.setupMQTTQueue(callback);
  // Initialize OTA manager
  wifiManager.setupOTAUpload();
}

/********************************** BOOTSTRAP FUNCTIONS FOR LOOP() *****************************************/
void BootstrapManager::bootstrapLoop(void (*manageDisconnections)(), void (*manageQueueSubscription)(), void (*manageHardwareButton)()) {
  
  if (WiFi.status() != WL_CONNECTED) {
    delay(1);
    Serial.print(F("WIFI Disconnected. Attempting reconnection."));
    wifiManager.setupWiFi(manageDisconnections, manageHardwareButton);
    return;
  }
  
  ArduinoOTA.handle();

  if (!mqttClient.connected()) {
    queueManager.mqttReconnect(manageDisconnections, manageQueueSubscription, manageHardwareButton);
  }

  mqttClient.loop();

}