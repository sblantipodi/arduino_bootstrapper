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
  
  queueManager.queueLoop(manageDisconnections, manageQueueSubscription, manageHardwareButton);

}

/********************************** SEND A SIMPLE MESSAGE ON THE QUEUE **********************************/
void BootstrapManager::publish(const char *topic, const char *payload, boolean retained) {

  if (DEBUG_QUEUE_MSG) {
    Serial.print(F("QUEUE MSG SENT [")); Serial.print(topic); Serial.println(F("] "));
    Serial.println(payload);
  }
  queueManager.publish(topic, payload, retained); 

}

/********************************** SEND A JSON MESSAGE ON THE QUEUE **********************************/
void BootstrapManager::publish(const char *topic, JsonObject objectToSend, boolean retained) {

  char buffer[measureJson(objectToSend) + 1];
  serializeJson(objectToSend, buffer, sizeof(buffer));

  queueManager.publish(topic, buffer, retained);

  if (DEBUG_QUEUE_MSG) {
    Serial.print(F("QUEUE MSG SENT [")); Serial.print(topic); Serial.println(F("] "));
    serializeJsonPretty(objectToSend, Serial); Serial.println();
  }

}

/********************************** SUBSCRIBE TO A QUEUE TOPIC **********************************/
void BootstrapManager::subscribe(const char *topic) {
  
  queueManager.subscribe(topic);
  
  if (DEBUG_QUEUE_MSG) {
    Serial.print(F("TOPIC SUBSCRIBED [")); Serial.print(topic); Serial.println(F("] "));  
  }

}

/********************************** PRINT THE MESSAGE ARRIVING FROM THE QUEUE **********************************/
StaticJsonDocument<BUFFER_SIZE> BootstrapManager::parseQueueMsg(char* topic, byte* payload, unsigned int length) {
  
  // JsonDocument jsonDoc;
  
  if (DEBUG_QUEUE_MSG) {
    Serial.print(F("QUEUE MSG ARRIVED [")); Serial.print(topic); Serial.println(F("] "));
  }

  char message[length + 1];
  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';

  DeserializationError error = deserializeJson(jsonDoc, payload);

  // non json msg
  if (error) {    
    JsonObject root = jsonDoc.to<JsonObject>();
    root[VALUE] = message;
    if (DEBUG_QUEUE_MSG) {
      String msg = root[VALUE];
      Serial.println(msg);
    }  
    return jsonDoc;
  } else { // return json doc
    if (DEBUG_QUEUE_MSG) {
      serializeJsonPretty(jsonDoc, Serial); Serial.println();
    }
    return jsonDoc;
  }

}

// return a new json object instance
JsonObject BootstrapManager::getJsonObject() {

  // JsonDocument jsonDoc;
  return jsonDoc.to<JsonObject>();
  
}

// Blink LED_BUILTIN without bloking delay
void BootstrapManager::nonBlokingBlink() {

  long currentMillis = millis();
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

// Print or display microcontroller infos
void BootstrapManager::getMicrocontrollerInfo() {

  // Helpers classes
  Helpers helper;

  helper.smartPrint(F("Wifi: ")); helper.smartPrint(wifiManager.getQuality()); helper.smartPrintln(F("%")); 
  helper.smartPrint(F("Heap: ")); helper.smartPrint(ESP.getFreeHeap()/1024); helper.smartPrintln(F(" KB")); 
  helper.smartPrint(F("Free Flash: ")); helper.smartPrint(ESP.getFreeSketchSpace()/1024); helper.smartPrintln(F(" KB")); 
  helper.smartPrint(F("Frequency: ")); helper.smartPrint(ESP.getCpuFreqMHz()); helper.smartPrintln(F("MHz")); 

  helper.smartPrint(F("Flash: ")); helper.smartPrint(ESP.getFlashChipSize()/1024); helper.smartPrintln(F(" KB")); 
  helper.smartPrint(F("Sketch: ")); helper.smartPrint(ESP.getSketchSize()/1024); helper.smartPrintln(F(" KB")); 
  helper.smartPrint(F("IP: ")); helper.smartPrintln(IP);
  helper.smartPrintln(F("MAC: ")); helper.smartPrintln(WiFi.macAddress());
  helper.smartPrint(F("SDK: ")); helper.smartPrintln(ESP.getSdkVersion());
  helper.smartPrint(F("Arduino Core: ")); helper.smartPrintln(ESP.getCoreVersion());
  helper.smartPrintln(F("Last Boot: ")); helper.smartPrintln(lastBoot);
  helper.smartPrintln(F("Last WiFi connection:")); helper.smartPrintln(lastWIFiConnection);
  helper.smartPrintln(F("Last MQTT connection:")); helper.smartPrintln(lastMQTTConnection);

}

// Draw screensaver useful for OLED displays
void BootstrapManager::drawScreenSaver(String txt) {

  if (screenSaverTriggered) {
    display.clearDisplay();
    for (int i = 0; i < 50; i++) {
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(5,17);
      display.fillRect(0, 0, display.width(), display.height(), i%2 != 0 ? WHITE : BLACK);
      display.setTextColor(i%2 == 0 ? WHITE : BLACK);
      display.drawRoundRect(0, 0, display.width()-1, display.height()-1, display.height()/4, i%2 == 0 ? WHITE : BLACK);
      display.println(txt);
      display.display();
    }
    display.setTextColor(WHITE);
    screenSaverTriggered = false;
  }

}

void BootstrapManager::drawInfoPage(String softwareVersion, String author) {

  yoffset -= 1;
  // add/remove 8 pixel for every line yoffset <= -209, if you want to add a line yoffset <= -217
  if (yoffset <= -209) {
    yoffset = SCREEN_HEIGHT + 6;
    lastPageScrollTriggered = true;
  }
  int effectiveOffset = (yoffset >= 0 && !lastPageScrollTriggered) ? 0 : yoffset;

  if (haVersion[0] != '\0') {
    display.drawBitmap((display.width()-HABIGLOGOW)-1, effectiveOffset+5, HABIGLOGO, HABIGLOGOW, HABIGLOGOH, 1);
  }  
  display.setCursor(0, effectiveOffset);
  display.setTextSize(1);
  display.print(WIFI_DEVICE_NAME); display.print(F(" "));
  display.println(softwareVersion);
  display.println("by " + author);
  display.println(F(""));
  
  if (haVersion[0] != '\0') {
    display.print(F("HA: ")); display.print(F("(")); display.print(haVersion); display.println(F(")"));
  }

  getMicrocontrollerInfo();

  // add/remove 8 pixel for every line effectiveOffset+175, if you want to add a line effectiveOffset+183
  display.drawBitmap((((display.width()/2)-(ARDUINOLOGOW/2))), effectiveOffset+175, ARDUINOLOGO, ARDUINOLOGOW, ARDUINOLOGOH, 1);

}

void BootstrapManager::sendState(const char *topic, JsonObject objectToSend, String version) {

  objectToSend["Whoami"] = WIFI_DEVICE_NAME;  
  objectToSend["IP"] = IP;
  objectToSend["MAC"] = MAC;
  objectToSend["ver"] = version;
  objectToSend["time"] = timedate;
  objectToSend["wifi"] = wifiManager.getQuality();

  // publish state only if it has received time from HA
  if (timedate != OFF_CMD) {
    // This topic should be retained, we don't want unknown values on battery voltage or wifi signal
    publish(topic, objectToSend, true);
  }

}