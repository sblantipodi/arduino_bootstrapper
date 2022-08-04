/*
  BoostrapManager.cpp - Main file for bootstrapping arduino projects
  
  Copyright (C) 2020 - 2022  Davide Perini
  
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

#if defined(ESP8266)
  if (!LittleFS.begin()) {
    LittleFS.format();
#elif defined(ESP32)
  if (!LittleFS.begin(true, "data", 10)) {
#endif
    Serial.println("LittleFS mount failed");
    return;
  }

  if (isWifiConfigured() && !forceWebServer) {
    isConfigFileOk = true;
    // Initialize Wifi manager
    wifiManager.setupWiFi(manageDisconnections, manageHardwareButton);
    // Initialize Queue Manager
    if (mqttIP.length() > 0) {
      queueManager.setupMQTTQueue(callback);
    } else {
      Serial.println(F("Skip MQTT connection."));
    }
    // Initialize OTA manager
    wifiManager.setupOTAUpload();
  } else {
    isConfigFileOk = false;
    launchWebServerForOTAConfig();
  }

}

/********************************** BOOTSTRAP FUNCTIONS FOR LOOP() *****************************************/
bool rcpResponseSent = false;
void BootstrapManager::bootstrapLoop(void (*manageDisconnections)(), void (*manageQueueSubscription)(), void (*manageHardwareButton)()) {

#if (IMPROV_ENABLED > 0)
  if (!rcpResponseSent && wifiManager.isConnected()) {
    rcpResponseSent = true;
    wifiManager.sendImprovRPCResponse(0x01, true);
  }
  if (!temporaryDisableImprove) {
    wifiManager.handleImprovPacket();
  }
#endif
  wifiManager.reconnectToWiFi(manageDisconnections, manageHardwareButton);
  ArduinoOTA.handle();
  if (mqttIP.length() > 0) {
    queueManager.queueLoop(manageDisconnections, manageQueueSubscription, manageHardwareButton);
  }

}

/********************************** SET LAST WILL PARAMETERS IN THE Q MANAGER **********************************/
void BootstrapManager::setMQTTWill(const char *topic, const char *payload, const int qos, boolean retain, boolean cleanSession){
  queueManager.setMQTTWill(topic, payload, qos, retain, cleanSession);
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
void BootstrapManager::unsubscribe(const char *topic) {

  queueManager.unsubscribe(topic);

  if (DEBUG_QUEUE_MSG) {
    Serial.print(F("TOPIC SUBSCRIBED [")); Serial.print(topic); Serial.println(F("] "));
  }

}

/********************************** SUBSCRIBE TO A QUEUE TOPIC **********************************/
void BootstrapManager::subscribe(const char *topic) {

  queueManager.subscribe(topic);

  if (DEBUG_QUEUE_MSG) {
    Serial.print(F("TOPIC SUBSCRIBED [")); Serial.print(topic); Serial.println(F("] "));
  }

}

/********************************** SUBSCRIBE TO A QUEUE TOPIC **********************************/
void BootstrapManager::subscribe(const char *topic, uint8_t qos) {

  queueManager.subscribe(topic, qos);

  if (DEBUG_QUEUE_MSG) {
    Serial.print(F("TOPIC SUBSCRIBED [")); Serial.print(topic); Serial.println(F("] "));
  }

}

/********************************** PRINT THE MESSAGE ARRIVING FROM THE QUEUE **********************************/
StaticJsonDocument<BUFFER_SIZE> BootstrapManager::parseQueueMsg(char* topic, byte* payload, unsigned int length) {

  if (DEBUG_QUEUE_MSG) {
    Serial.print(F("QUEUE MSG ARRIVED [")); Serial.print(topic); Serial.println(F("] "));
  }

  char message[length + 1];
  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';

  DeserializationError error = deserializeJson(jsonDoc, (const byte*) payload, length);

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

/********************************** PRINT THE MESSAGE ARRIVING FROM HTTP **********************************/
StaticJsonDocument<BUFFER_SIZE> BootstrapManager::parseHttpMsg(String payload, unsigned int length) {

  char message[length + 1];
  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';

  DeserializationError error = deserializeJson(jsonDoc, payload.c_str(), length);

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

  Helpers helper;
  helper.smartPrint(F("Wifi: ")); helper.smartPrint(wifiManager.getQuality()); helper.smartPrintln(F("%"));
  helper.smartPrint(F("Heap: ")); helper.smartPrint(ESP.getFreeHeap()/1024); helper.smartPrintln(F(" KB"));
  helper.smartPrint(F("Free Flash: ")); helper.smartPrint(ESP.getFreeSketchSpace()/1024); helper.smartPrintln(F(" KB"));
  helper.smartPrint(F("Frequency: ")); helper.smartPrint(ESP.getCpuFreqMHz()); helper.smartPrintln(F("MHz"));

  helper.smartPrint(F("Flash: ")); helper.smartPrint(ESP.getFlashChipSize()/1024); helper.smartPrintln(F(" KB"));
  helper.smartPrint(F("Sketch: ")); helper.smartPrint(ESP.getSketchSize()/1024); helper.smartPrintln(F(" KB"));
  helper.smartPrint(F("IP: ")); helper.smartPrintln(microcontrollerIP);
  helper.smartPrintln(F("MAC: ")); helper.smartPrintln(WiFi.macAddress());
  helper.smartPrint(F("SDK: ")); helper.smartPrintln(ESP.getSdkVersion());
  // helper.smartPrint(F("Arduino Core: ")); helper.smartPrintln(ESP.getCoreVersion());
  helper.smartPrintln(F("Last Boot: ")); helper.smartPrintln(lastBoot);
  helper.smartPrintln(F("Last WiFi connection:")); helper.smartPrintln(lastWIFiConnection);
  helper.smartPrintln(F("Last MQTT connection:")); helper.smartPrintln(lastMQTTConnection);

}

// Draw screensaver useful for OLED displays
void BootstrapManager::drawScreenSaver(String txt) {
#if (DISPLAY_ENABLED)
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
#endif
}

// draw some infos about your controller
void BootstrapManager::drawInfoPage(String softwareVersion, String author) {
#if (DISPLAY_ENABLED)
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
  display.print(deviceName); display.print(F(" "));
  display.println(softwareVersion);
  display.println("by " + author);
  display.println(F(""));

  if (haVersion[0] != '\0') {
    display.print(F("HA: ")); display.print(F("(")); display.print(haVersion); display.println(F(")"));
  }

  getMicrocontrollerInfo();

  // add/remove 8 pixel for every line effectiveOffset+175, if you want to add a line effectiveOffset+183
  display.drawBitmap((((display.width()/2)-(ARDUINOLOGOW/2))), effectiveOffset+175, ARDUINOLOGO, ARDUINOLOGOW, ARDUINOLOGOH, 1);
#endif
}

// send the state of your controller to the mqtt queue
void BootstrapManager::sendState(const char *topic, JsonObject objectToSend, String version) {

  objectToSend["Whoami"] = deviceName;
  objectToSend["IP"] = microcontrollerIP;
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

// write json file to storage
void BootstrapManager::writeToLittleFS(DynamicJsonDocument jsonDoc, String filename) {

  File jsonFile = LittleFS.open("/" + filename, FILE_WRITE);
  if (!jsonFile) {
    helper.smartPrintln("Failed to open [" + filename + "] file for writing");
  } else {
    serializeJsonPretty(jsonDoc, Serial);
    serializeJson(jsonDoc, jsonFile);
    jsonFile.close();
    helper.smartPrintln("[" + filename + "] written correctly");
  }

}

// read json file from storage
DynamicJsonDocument BootstrapManager::readLittleFS(String filename) {

  // Helpers classes
  Helpers helper;

  #if (DISPLAY_ENABLED) 
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
  #endif
  helper.smartPrintln(F("Mounting LittleFS..."));
  helper.smartDisplay();

  File jsonFile = LittleFS.open("/" + filename, FILE_READ);

  if (!jsonFile) {
    helper.smartPrintln("Failed to open [" + filename + "] file");
    helper.smartDisplay();
  }

  size_t size = jsonFile.size();
  if (size > 1024) {
    helper.smartPrintln("[" + filename + "] file size is too large");
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  jsonFile.readBytes(buf.get(), size);

  DynamicJsonDocument jsonDoc(1024);
  auto error = deserializeJson(jsonDoc, buf.get());
  if (filename != "setup.json") serializeJsonPretty(jsonDoc, Serial);
  jsonFile.close();
  if (error) {
    helper.smartPrintln("Failed to parse [" + filename + "] file");
    helper.smartDisplay(DELAY_2000);
  } else {
    helper.smartPrintln("[" + filename + "]\nJSON parsed");
    helper.smartDisplay(DELAY_2000);
    return jsonDoc;
  }

  helper.smartDisplay(DELAY_2000);
  return jsonDoc;

}

String BootstrapManager::readValueFromFile(String filename, String paramName) {
  String returnStr = "";
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
  }
  File jsonFile = LittleFS.open("/" + filename, FILE_READ);
  if (!jsonFile) {
    helper.smartPrintln("Failed to open [" + filename + "] file");
    helper.smartDisplay();
  }
  size_t size = jsonFile.size();
  std::unique_ptr<char[]> buf(new char[size]);
  jsonFile.readBytes(buf.get(), size);
  DynamicJsonDocument jsonDoc(1024);
  auto error = deserializeJson(jsonDoc, buf.get());
  serializeJsonPretty(jsonDoc, Serial);
  JsonVariant answer = jsonDoc[paramName];
  if (answer.is<char*>()) {
    returnStr = answer.as<String>();
  } else {
    auto returnVal = answer.as<int>();
    returnStr = String(returnVal);
  }
  jsonFile.close();
  if (error) {
    returnStr = "";
  }
  return returnStr;
}

// check if wifi is correctly configured
bool BootstrapManager::isWifiConfigured() {

  if (wifiManager.isWifiConfigured()) {
    deviceName = DEVICE_NAME;
    microcontrollerIP = IP_MICROCONTROLLER;
    qsid = SSID;
    qpass = PASSWORD;
    OTApass = OTAPASSWORD;
    mqttIP = MQTT_SERVER;
    mqttPort = MQTT_PORT;
    mqttuser = MQTT_USERNAME;
    mqttpass = MQTT_PASSWORD;
    additionalParam = PARAM_ADDITIONAL;
    return true;
  } else {
    DynamicJsonDocument mydoc = readLittleFS("setup.json");
    if (mydoc.containsKey("qsid")) {
      Serial.println("Storage OK, restoring WiFi and MQTT config.");
      deviceName = helper.getValue(mydoc["deviceName"]);
      microcontrollerIP = helper.getValue(mydoc["microcontrollerIP"]);
      qsid = helper.getValue(mydoc["qsid"]);
      qpass = helper.getValue(mydoc["qpass"]);
      OTApass = helper.getValue(mydoc["OTApass"]);
      mqttIP = helper.getValue(mydoc["mqttIP"]);
      mqttPort = helper.getValue(mydoc["mqttPort"]);
      mqttuser = helper.getValue(mydoc["mqttuser"]);
      mqttpass = helper.getValue(mydoc["mqttpass"]);
      additionalParam = helper.getValue(mydoc["additionalParam"]);
      return true;
    } else {
      Serial.println("No setup file");
    }
  }
  return false;

}

// if no ssid available, launch web server to get config params via browser
void BootstrapManager::launchWebServerForOTAConfig() {

#if (IMPROV_ENABLED > 0)
  unsigned long timeNowStatus = 0;
  bool switchToWebServer = false;
  // If WiFi is not configured, handle improv packet for 15 seconds, then switch to settinigs managed by web server
  WiFi.disconnect();
  while (((WiFi.localIP()[0] == 0 && WiFi.status() != WL_CONNECTED) && !switchToWebServer) || improvePacketReceived) {
    if(millis() > timeNowStatus + IMPROV_ENABLED) {
      timeNowStatus = millis();
      switchToWebServer = true;
    }
    wifiManager.manageImprovWifi();
  }
#endif
  return wifiManager.launchWebServerForOTAConfig();

}

// get the wifi quality
int BootstrapManager::getWifiQuality() {

  return wifiManager.getQuality();

}