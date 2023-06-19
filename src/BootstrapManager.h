/*
  BoostrapManager.cpp - Main header for bootstrapping arduino projects
  
  Copyright © 2020 - 2023  Davide Perini
  
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

#ifndef _DPSOFTWARE_BOOTSTRAP_MANAGER_H
#define _DPSOFTWARE_BOOTSTRAP_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <FS.h>
#include <LittleFS.h>
#include "Configuration.h"
#include "Helpers.h"
#include "WifiManager.h"
#include "QueueManager.h"
#if defined(ARDUINO_ARCH_ESP32)
#include <esp_task_wdt.h>
#endif

class BootstrapManager {

private:
    WifiManager wifiManager; // WifiManager classes for Wifi management
    QueueManager queueManager; // QueueManager classes for MQTT queue management
    Helpers helper;
#if defined(ARDUINO_ARCH_ESP32)
    unsigned long lastMillisForWdt = millis();
#endif

public:
    StaticJsonDocument<BUFFER_SIZE> jsonDoc;
    StaticJsonDocument<BUFFER_SIZE_MAX_SIZE> jsonDocBigSize;
    // using JsonDocument = StaticJsonDocument<BUFFER_SIZE>;
    StaticJsonDocument<BUFFER_SIZE> parseQueueMsg(char* topic, byte* payload, unsigned int length); // print the message arriving from the queue
    StaticJsonDocument<BUFFER_SIZE> parseHttpMsg(String payload, unsigned int length); // print the message arriving from HTTP
    void littleFsInit();
    void bootstrapSetup(void (*manageDisconnectionFunction)(), void (*manageHardwareButton)(), void (*callback)(char*, byte*, unsigned int)); // bootstrap setup()
    void bootstrapSetup(void (*manageDisconnectionFunction)(), void (*manageHardwareButton)(), void (*callback)(char*, byte*, unsigned int), bool waitImprov, void (*listener)()); // bootstrap setup()
    void bootstrapLoop(void (*manageDisconnectionFunction)(), void (*manageQueueSubscription)(), void (*manageHardwareButton)()); // bootstrap loop()
    static void setMQTTWill(const char *topic, const char *payload, int qos, boolean retain, boolean cleanSession); // set the last will parameters for mqtt
    static void publish(const char *topic, const char *payload, boolean retained); // send a message on the queue
    static void publish(const char *topic, JsonObject objectToSend, boolean retained); // send a message on the queue
    static void unsubscribe(const char *topic); // unsubscribe to a queue topic
    static void subscribe(const char *topic); // subscribe to a queue topic
    static void subscribe(const char *topic, uint8_t qos); // subscribe to a queue topic with qos 0 or 1
    JsonObject getJsonObject(); // return a new json object instance
    [[maybe_unused]] static void nonBlokingBlink(); // blink default LED when sending data to the queue
    [[maybe_unused]] static void getMicrocontrollerInfo(); // print or display microcontroller's info
    [[maybe_unused]] void drawInfoPage(const String& softwareVersion, const String& author); // draw a page with all the microcontroller's info
    [[maybe_unused]] void drawScreenSaver(const String& txt); // useful for OLED displays
    [[maybe_unused]] static void sendState(const char *topic, JsonObject objectToSend, const String& version); // send microcontroller's info on the queue
    [[maybe_unused]] static void writeToLittleFS(const DynamicJsonDocument& jDoc, const String& filenameToUse); // write json file to storage
    [[maybe_unused]] static StaticJsonDocument<BUFFER_SIZE> readLittleFS(const String& filenameToUse); // read json file from storage
    [[maybe_unused]] String readValueFromFile(const String& filenameToUse, const String& paramName); // read a param from a json file
    static bool isWifiConfigured(); // check if wifi is correctly configured
    void launchWebServerForOTAConfig(); // if no ssid available, launch web server to get config params via browser
    void launchWebServerCustom(bool waitImprov, void (*listener)()); // if no ssid available, launch web server to get config params via browser
    static int getWifiQuality(); // get the wifi quality
    void manageImprov();
};

#endif