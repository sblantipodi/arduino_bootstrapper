/*
  BoostrapManager.cpp - Main header for bootstrapping arduino projects
  
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

#ifndef _DPSOFTWARE_BOOTSTRAP_MANAGER_H
#define _DPSOFTWARE_BOOTSTRAP_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <FS.h>
#if defined(ESP8266)
  #include <LittleFS.h>
#elif defined(ESP32)
  #include "SPIFFS.h"
#endif 
#include "Configuration.h"
#include "Helpers.h"
#include "WifiManager.h"
#include "QueueManager.h"


class BootstrapManager {

  private:
    WifiManager wifiManager; // WifiManager classes for Wifi management
    QueueManager queueManager; // QueueManager classes for MQTT queue management
    Helpers helper;

  public:
    StaticJsonDocument<BUFFER_SIZE> jsonDoc;
    StaticJsonDocument<BUFFER_SIZE_MAX_SIZE> jsonDocBigSize;
    // using JsonDocument = StaticJsonDocument<BUFFER_SIZE>;
    StaticJsonDocument<BUFFER_SIZE> parseQueueMsg(char* topic, byte* payload, unsigned int length); // print the message arriving from the queue
    StaticJsonDocument<BUFFER_SIZE> parseHttpMsg(String payload, unsigned int length); // print the message arriving from HTTP
    void bootstrapSetup(void (*manageDisconnectionFunction)(), void (*manageHardwareButton)(), void (*callback)(char*, byte*, unsigned int)); // bootstrap setup()
    void bootstrapLoop(void (*manageDisconnectionFunction)(), void (*manageQueueSubscription)(), void (*manageHardwareButton)()); // bootstrap loop()
    void publish(const char *topic, const char *payload, boolean retained); // send a message on the queue
    void publish(const char *topic, JsonObject objectToSend, boolean retained); // send a message on the queue
    void unsubscribe(const char *topic); // unsubscribe to a queue topic
    void subscribe(const char *topic); // subscribe to a queue topic
    void subscribe(const char *topic, uint8_t qos); // subscribe to a queue topic with qos 0 or 1
    JsonObject getJsonObject(); // return a new json object instance
    void nonBlokingBlink(); // blink default LED when sending data to the queue
    void getMicrocontrollerInfo(); // print or display microcontroller's info
    void drawInfoPage(String softwareVersion, String author); // draw a page with all the microcontroller's info
    void drawScreenSaver(String txt); // useful for OLED displays
    void sendState(const char *topic, JsonObject objectToSend, String version); // send microcontroller's info on the queue 
    #if defined(ESP8266)
      void writeToLittleFS(DynamicJsonDocument jsonDoc, String filename); // write json file to storage
      DynamicJsonDocument readLittleFS(String filename); // read json file from storage
    #elif defined(ESP32)
      void writeToSPIFFS(DynamicJsonDocument jsonDoc, String filename); // write json file to storage
      DynamicJsonDocument readSPIFFS(String filename); // read json file from storage
    #endif
    String readValueFromFile(String filename, String paramName); // read a param from a json file
    bool isWifiConfigured(); // check if wifi is correctly configured
    void launchWebServerForOTAConfig(); // if no ssid available, launch web server to get config params via browser
    int getWifiQuality(); // get the wifi quality

};

#endif