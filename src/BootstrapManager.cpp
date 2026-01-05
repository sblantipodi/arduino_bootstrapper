/*
  BoostrapManager.cpp - Main file for bootstrapping arduino projects
  
  Copyright © 2020 - 2026  Davide Perini
  
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

void BootstrapManager::littleFsInit() {
#if defined(ESP8266)
  if (!LittleFS.begin()) {
#elif defined(ARDUINO_ARCH_ESP32)
  if (!LittleFS.begin(true)) {
#endif
    Serial.println("LittleFS mount failed");
    return;
  }
}

/********************************** BOOTSTRAP FUNCTIONS FOR SETUP() *****************************************/
void BootstrapManager::bootstrapSetup(void (*manageDisconnections)(), void (*manageHardwareButton)(),
                                      void (*callback)(char *, byte *, unsigned int)) {
  littleFsInit();
  if (isWifiConfigured() && !forceWebServer) {
    isConfigFileOk = true;
    // Initialize Wifi manager
    wifiManager.setupWiFi(manageDisconnections, manageHardwareButton);
    // Initialize Queue Manager
    if (mqttIP.length() > 0) {
      QueueManager::setupMQTTQueue(callback);
    } else {
      Serial.println(F("Skip MQTT connection."));
    }
    // Initialize OTA manager
    WifiManager::setupOTAUpload();
  } else {
    isConfigFileOk = false;
    launchWebServerForOTAConfig();
  }
#if defined(ARDUINO_ARCH_ESP32)
  esp_task_wdt_config_t twdt_config = {
    .timeout_ms = 5000,
    .trigger_panic = true,
  };
  esp_task_wdt_init(&twdt_config); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
#endif
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
  Serial.setTxTimeoutMs(0);
#endif
}

#if defined(ARDUINO_ARCH_ESP32)
void eth_event(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH Started");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      ethConnected = true;
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      microcontrollerIP = ETH.localIP().toString();
      ethConnected = true;
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      ethConnected = false;
      microcontrollerIP = F("0.0.0.0");
      currentWiFiIp = IPAddress(0, 0, 0, 0);
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      ethConnected = false;
      microcontrollerIP = F("0.0.0.0");
      currentWiFiIp = IPAddress(0, 0, 0, 0);
      break;
    default:
      break;
  }
}
#endif

/********************************** BOOTSTRAP FUNCTIONS FOR SETUP() *****************************************/
void BootstrapManager::bootstrapSetup(void (*manageDisconnections)(), void (*manageHardwareButton)(),
                                      void (*callback)(char *, byte *, unsigned int), bool waitImprov,
                                      void (*listener)()) {
  littleFsInit();
  if (isWifiConfigured() && !forceWebServer && (ethd == 0 || ethd == -1)) {
    isConfigFileOk = true;
    // Initialize Wifi manager
    wifiManager.setupWiFi(manageDisconnections, manageHardwareButton);
    initMqttOta(callback);
  } else if (ethd > 0) {
#if defined(ARDUINO_ARCH_ESP32)
    isConfigFileOk = true;
    ETH.setHostname(Helpers::string2char(deviceName));
    WiFi.onEvent(eth_event);
    EthManager::connectToEthernet(ethd, mosi, miso, sclk, cs);
    wifiManager.setupWiFi(manageDisconnections, manageHardwareButton);
    initMqttOta(callback);
#endif
  } else {
    isConfigFileOk = false;
    launchWebServerCustom(waitImprov, listener);
  }
#if defined(ARDUINO_ARCH_ESP32)
  esp_task_wdt_config_t twdt_config = {
    .timeout_ms = 3000, // TODO 3000
    .trigger_panic = true,
  };
  esp_task_wdt_init(&twdt_config); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
#endif
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
  Serial.setTxTimeoutMs(0);
#endif
}

void BootstrapManager::initMqttOta(void (*callback)(char *, byte *, unsigned int)) {
  // Initialize Queue Manager
  if (mqttIP.length() > 0) {
    QueueManager::setupMQTTQueue(callback);
  } else {
    Serial.println(F("Skip MQTT connection."));
  }
  // Initialize OTA manager
  WifiManager::setupOTAUpload();
}

/********************************** BOOTSTRAP FUNCTIONS FOR LOOP() *****************************************/
bool rcpResponseSent = false;
void BootstrapManager::bootstrapLoop(void (*manageDisconnections)(), void (*manageQueueSubscription)(), void (*manageHardwareButton)()) {
#if defined(ARDUINO_ARCH_ESP32)
  if (millis() - lastMillisForWdt >= 3000) {
    lastMillisForWdt = millis();
    esp_task_wdt_reset();
  }
#endif
#if (IMPROV_ENABLED > 0)
  if (!rcpResponseSent && WifiManager::isConnected()) {
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
void BootstrapManager::setMQTTWill(const char *topic, const char *payload, const int qos, boolean retain,
                                   boolean cleanSession) {
  QueueManager::setMQTTWill(topic, payload, qos, retain, cleanSession);
}

/********************************** SEND A SIMPLE MESSAGE ON THE QUEUE **********************************/
void BootstrapManager::publish(const char *topic, const char *payload, boolean retained) {
  if (DEBUG_QUEUE_MSG) {
    Serial.print(F("QUEUE MSG SENT ["));
    Serial.print(topic);
    Serial.println(F("] "));
    Serial.println(payload);
  }
  QueueManager::publish(topic, payload, retained);
}

/********************************** SEND A JSON MESSAGE ON THE QUEUE **********************************/
void BootstrapManager::publish(const char *topic, JsonObject objectToSend, boolean retained) {
  char buffer[measureJson(objectToSend) + 1];
  serializeJson(objectToSend, buffer, sizeof(buffer));
  QueueManager::publish(topic, buffer, retained);
  if (DEBUG_QUEUE_MSG) {
    Serial.print(F("QUEUE MSG SENT ["));
    Serial.print(topic);
    Serial.println(F("] "));
    serializeJsonPretty(objectToSend, Serial);
    Serial.println();
  }
}

/********************************** SUBSCRIBE TO A QUEUE TOPIC **********************************/
void BootstrapManager::unsubscribe(const char *topic) {
  QueueManager::unsubscribe(topic);
  if (DEBUG_QUEUE_MSG) {
    Serial.print(F("TOPIC SUBSCRIBED ["));
    Serial.print(topic);
    Serial.println(F("] "));
  }
}

/********************************** SUBSCRIBE TO A QUEUE TOPIC **********************************/
void BootstrapManager::subscribe(const char *topic) {
  QueueManager::subscribe(topic);
  if (DEBUG_QUEUE_MSG) {
    Serial.print(F("TOPIC SUBSCRIBED ["));
    Serial.print(topic);
    Serial.println(F("] "));
  }
}

/********************************** SUBSCRIBE TO A QUEUE TOPIC **********************************/
void BootstrapManager::subscribe(const char *topic, uint8_t qos) {
  QueueManager::subscribe(topic, qos);
  if (DEBUG_QUEUE_MSG) {
    Serial.print(F("TOPIC SUBSCRIBED ["));
    Serial.print(topic);
    Serial.println(F("] "));
  }
}

/********************************** PRINT THE MESSAGE ARRIVING FROM THE QUEUE **********************************/
JsonDocument BootstrapManager::parseQueueMsg(char *topic, byte *payload, unsigned int length) {
  if (DEBUG_QUEUE_MSG) {
    Serial.print(F("QUEUE MSG ARRIVED ["));
    Serial.print(topic);
    Serial.println(F("] "));
  }
  char message[length + 1];
  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char) payload[i];
  }
  message[length] = '\0';
  DeserializationError error = deserializeJson(jsonDoc, (const byte *) payload, length);
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
      serializeJsonPretty(jsonDoc, Serial);
      Serial.println();
    }
    return jsonDoc;
  }
}

/********************************** PRINT THE MESSAGE ARRIVING FROM HTTP **********************************/
JsonDocument BootstrapManager::parseHttpMsg(String payload, unsigned int length) {
  char message[length + 1];
  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char) payload[i];
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
      serializeJsonPretty(jsonDoc, Serial);
      Serial.println();
    }
    return jsonDoc;
  }
}

// return a new json object instance
JsonObject BootstrapManager::getJsonObject() {
  return jsonDoc.to<JsonObject>();
}

// Blink LED_BUILTIN without bloking delay
[[maybe_unused]] void BootstrapManager::nonBlokingBlink() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval && ledTriggered) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    // blink led
#if defined(LED_BUILTIN)
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
#endif
    blinkCounter++;
    if (blinkCounter >= blinkTimes) {
      blinkCounter = 0;
      ledTriggered = false;
#if defined(LED_BUILTIN)
      digitalWrite(LED_BUILTIN, HIGH);
#endif
    }
  }
}

// Print or display microcontroller infos
void BootstrapManager::getMicrocontrollerInfo() {
  Helpers::smartPrint(F("Wifi: "));
  Helpers::smartPrint(WifiManager::getQuality());
  Helpers::smartPrintln(F("%"));
#if defined(ESP8266)
  Helpers::smartPrint(F("Heap: ")); Helpers::smartPrint(EspClass::getFreeHeap()/1024); Helpers::smartPrintln(F(" KB"));
  Helpers::smartPrint(F("Free Flash: ")); Helpers::smartPrint(EspClass::getFreeSketchSpace()/1024); Helpers::smartPrintln(F(" KB"));
  Helpers::smartPrint(F("Frequency: ")); Helpers::smartPrint(EspClass::getCpuFreqMHz()); Helpers::smartPrintln(F("MHz"));
  Helpers::smartPrint(F("Flash: ")); Helpers::smartPrint(EspClass::getFlashChipSize()/1024); Helpers::smartPrintln(F(" KB"));
  Helpers::smartPrint(F("Sketch: ")); Helpers::smartPrint(EspClass::getSketchSize()/1024); Helpers::smartPrintln(F(" KB"));
  Helpers::smartPrint(F("SDK: ")); Helpers::smartPrintln(EspClass::getSdkVersion());
#elif defined(ARDUINO_ARCH_ESP32)
  Helpers::smartPrint(F("Heap: "));
  Helpers::smartPrint(ESP.getFreeHeap() / 1024);
  Helpers::smartPrintln(F(" KB"));
  Helpers::smartPrint(F("Free Flash: "));
  Helpers::smartPrint(ESP.getFreeSketchSpace() / 1024);
  Helpers::smartPrintln(F(" KB"));
  Helpers::smartPrint(F("Frequency: "));
  Helpers::smartPrint(ESP.getCpuFreqMHz());
  Helpers::smartPrintln(F("MHz"));
  Helpers::smartPrint(F("Flash: "));
  Helpers::smartPrint(ESP.getFlashChipSize() / 1024);
  Helpers::smartPrintln(F(" KB"));
  Helpers::smartPrint(F("Sketch: "));
  Helpers::smartPrint(ESP.getSketchSize() / 1024);
  Helpers::smartPrintln(F(" KB"));
  Helpers::smartPrint(F("SDK: "));
  Helpers::smartPrintln(ESP.getSdkVersion());
#endif
  Helpers::smartPrintln(F("MAC: "));
  Helpers::smartPrintln(WiFi.macAddress());
  Helpers::smartPrint(F("IP: "));
  Helpers::smartPrintln(microcontrollerIP);
  // helper.smartPrint(F("Arduino Core: ")); helper.smartPrintln(ESP.getCoreVersion());
  Helpers::smartPrintln(F("Last Boot: "));
  Helpers::smartPrintln(lastBoot);
  Helpers::smartPrintln(F("Last WiFi connection:"));
  Helpers::smartPrintln(lastWIFiConnection);
  Helpers::smartPrintln(F("Last MQTT connection:"));
  Helpers::smartPrintln(lastMQTTConnection);
}

// Draw screensaver useful for OLED displays
[[maybe_unused]] void BootstrapManager::drawScreenSaver(const String &txt) {
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
[[maybe_unused]] void BootstrapManager::drawInfoPage(const String &softwareVersion, const String &author) {
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
[[maybe_unused]] void BootstrapManager::sendState(const char *topic, JsonObject objectToSend, const String &version) {
  objectToSend["Whoami"] = deviceName;
  objectToSend["IP"] = microcontrollerIP;
  objectToSend["MAC"] = MAC;
  objectToSend["ver"] = version;
  objectToSend["time"] = timedate;
  objectToSend["wifi"] = WifiManager::getQuality();

  // publish state only if it has received time from HA
  if (timedate != OFF_CMD) {
    // This topic should be retained, we don't want unknown values on battery voltage or wifi signal
    publish(topic, objectToSend, true);
  }
}

// write json file to storage
void BootstrapManager::writeToLittleFS(const JsonDocument &jDoc, const String &filenameToUse) {
  File jsonFile = LittleFS.open("/" + filenameToUse, FILE_WRITE);
  if (!jsonFile) {
    Helpers::smartPrintln("Failed to open [" + filenameToUse + "] file for writing");
  } else {
    serializeJsonPretty(jDoc, Serial);
    serializeJson(jDoc, jsonFile);
    jsonFile.close();
    Helpers::smartPrintln("[" + filenameToUse + "] written correctly");
  }
}

// read json file from storage
JsonDocument BootstrapManager::readLittleFS(const String &filenameToUse) {
  // Helpers classes
  Helpers helper;
#if (DISPLAY_ENABLED)
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
#endif
  Helpers::smartPrintln(F("Mounting LittleFS..."));
  helper.smartDisplay();
  File jsonFile = LittleFS.open("/" + filenameToUse, FILE_READ);
  if (!jsonFile) {
    Helpers::smartPrintln("Failed to open [" + filenameToUse + "] file");
    helper.smartDisplay();
  }
  size_t size = jsonFile.size();
  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);
  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  jsonFile.readBytes(buf.get(), size);
  JsonDocument jsonDoc;
  auto error = deserializeJson(jsonDoc, buf.get());
  if (filenameToUse != "setup.json") serializeJsonPretty(jsonDoc, Serial);
  jsonFile.close();
  if (error) {
    Helpers::smartPrintln("Failed to parse [" + filenameToUse + "] file");
    helper.smartDisplay(DELAY_2000);
  } else {
    Helpers::smartPrintln("[" + filenameToUse + "]\nJSON parsed");
    helper.smartDisplay(DELAY_2000);
    return jsonDoc;
  }
  helper.smartDisplay(DELAY_2000);
  return jsonDoc;
}

String BootstrapManager::readValueFromFile(const String &filenameToUse, const String &paramName) {
  String returnStr = "";
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
  }
  File jsonFile = LittleFS.open("/" + filenameToUse, FILE_READ);
  if (!jsonFile) {
    Helpers::smartPrintln("Failed to open [" + filenameToUse + "] file");
    helper.smartDisplay();
  }
  size_t size = jsonFile.size();
  std::unique_ptr<char[]> buf(new char[size]);
  jsonFile.readBytes(buf.get(), size);
  JsonDocument jDoc;
  auto error = deserializeJson(jDoc, buf.get());
  serializeJsonPretty(jDoc, Serial);
  JsonVariant answer = jDoc[paramName];
  if (answer.is<const char *>()) {
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
    if (WifiManager::isWifiConfigured()) {
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
        JsonDocument mydoc = readLittleFS(F("setup.json"));
        if (mydoc[F("qsid")].is<JsonVariant>()) {
            Serial.println(F("Storage OK, restoring WiFi and MQTT config."));
            microcontrollerIP = Helpers::getValue(mydoc["microcontrollerIP"]);
            qsid = Helpers::getValue(mydoc[F("qsid")]);
            qpass = Helpers::getValue(mydoc[F("qpass")]);
            OTApass = Helpers::getValue(mydoc[F("OTApass")]);
            if (OTApass.isEmpty()) {
              OTApass = OTAPASSWORD;
            }
            mqttIP = Helpers::getValue(mydoc[F("mqttIP")]);
            mqttPort = Helpers::getValue(mydoc[F("mqttPort")]);
            mqttuser = Helpers::getValue(mydoc[F("mqttuser")]);
            mqttpass = Helpers::getValue(mydoc[F("mqttpass")]);
            additionalParam = Helpers::getValue(mydoc[F("additionalParam")]);
            deviceName = Helpers::getValue(mydoc[F("deviceName")]);
            ethd = mydoc[F("ethd")].as<int8_t>();
#if defined(ARDUINO_ARCH_ESP32)
            if (ethd == spiStartIdx) {
              mosi = mydoc[F("mosi")].as<int8_t>();
              miso = mydoc[F("miso")].as<int8_t>();
              sclk = mydoc[F("sclk")].as<int8_t>();
              cs = mydoc[F("cs")].as<int8_t>();
            }
#endif
#if defined(ESP8266)
            ethd = -1;
#endif
            return true;
        } else {
            Serial.println(F("No setup file"));
        }
    }
    return false;
}

// if no ssid available, launch web server to get config params via browser
void BootstrapManager::launchWebServerForOTAConfig() {
  WifiManager::launchWebServerForOTAConfig();
#if (IMPROV_ENABLED > 0)
  manageImprov();
#endif
}

void BootstrapManager::manageImprov() {
  unsigned long timeNowStatus = 0;
  // If WiFi is not configured, handle improv packet for 15 seconds, then switch to settinigs managed by web server
  WiFi.disconnect();
  while ((WiFi.localIP()[0] == 0 && WiFi.status() != WL_CONNECTED) || improvePacketReceived) {
    wifiManager.manageImprovWifi();
  }
}

void BootstrapManager::launchWebServerCustom(bool waitImprov, void (*listener)()) {
  WifiManager::launchWebServerCustom(listener);
#if (IMPROV_ENABLED > 0)
  if (waitImprov) {
    manageImprov();
  }
#endif
}

// get the wifi quality
int BootstrapManager::getWifiQuality() {
  return WifiManager::getQuality();
}