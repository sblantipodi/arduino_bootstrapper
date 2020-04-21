/*
  BoostrapManager.cpp - Main file for bootstrapping arduino projects
  
  Copyright (C) 2020  Davide Perini
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of 
  this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
  copies of the Software, and to permit persons to whom the Software is 
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in 
  all copies or substantial portions of the Software.
  
  You should have received a copy of the MIT License along with this program.  
  If not, see <https://opensource.org/licenses/MIT/>.
*/

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

// Blink LED_BUILTIN without bloking delay
void BootstrapManager::nonBlokingBlink() {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval && ledTriggered) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    // blink led
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    blinkCounter++;
    if (blinkCounter >= blinkTimes) {
      blinkCounter = 0;
      ledTriggered = false;
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }  

}

void BootstrapManager::printInfo() {

  // Helpers classes
  Helpers helper;
  
  helper.smartPrint(F("Wifi: ")); helper.smartPrint(wifiManager.getQuality()); helper.smartPrintln(F("%")); 
  helper.smartPrint(F("Heap: ")); helper.smartPrint(ESP.getFreeHeap()/1024); helper.smartPrintln(F(" KB")); 
  helper.smartPrint(F("Free Flash: ")); helper.smartPrint(ESP.getFreeSketchSpace()/1024); helper.smartPrintln(F(" KB")); 
  helper.smartPrint(F("Frequency: ")); helper.smartPrint(ESP.getCpuFreqMHz()); helper.smartPrintln(F("MHz")); 

  helper.smartPrint(F("Flash: ")); helper.smartPrint(ESP.getFlashChipSize()/1024); helper.smartPrintln(F(" KB")); 
  helper.smartPrint(F("Sketch: ")); helper.smartPrint(ESP.getSketchSize()/1024); helper.smartPrintln(F(" KB")); 
  helper.smartPrint(F("IP: ")); helper.smartPrintln(WiFi.localIP());
  helper.smartPrintln(F("MAC: ")); helper.smartPrintln(WiFi.macAddress());
  helper.smartPrint(F("SDK: ")); helper.smartPrintln(ESP.getSdkVersion());
  helper.smartPrint(F("Arduino Core: ")); helper.smartPrintln(ESP.getCoreVersion());
  helper.smartPrintln(F("Last Boot: ")); helper.smartPrintln(lastBoot);
  helper.smartPrintln(F("Last WiFi connection:")); helper.smartPrintln(lastWIFiConnection);
  helper.smartPrintln(F("Last MQTT connection:")); helper.smartPrintln(lastMQTTConnection);

}