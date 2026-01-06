/*
  WifiManager.h - Managing Wifi and OTA
  
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

#ifndef _DPSOFTWARE_WIFI_MANAGER_H
#define _DPSOFTWARE_WIFI_MANAGER_H

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_ARCH_ESP32)

#include <WiFi.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <esp_task_wdt.h>

#endif

#include <ArduinoOTA.h>
#include <FS.h>
#include <LittleFS.h>
#include "Helpers.h"
#include "Secrets.h"
#include "Configuration.h"

//Establishing Local server at port 80 whenever required
#if defined(ESP8266)
extern ESP8266WebServer server;
#elif defined(ARDUINO_ARCH_ESP32)
extern WebServer server;
#endif
// WiFi Client
extern WiFiClient espClient;
// WebServer content
extern String content;
// WebServer status code
extern int statusCode;
// WebServer HTML frontend
extern String htmlString;

#ifdef WLED_DEBUG_IMPROV
#define DIMPROV_PRINT(x) Serial.print(x)
#define DIMPROV_PRINTLN(x) Serial.println(x)
#define DIMPROV_PRINTF(x...) Serial.printf(x)
#else
#define DIMPROV_PRINT(x)
#define DIMPROV_PRINTLN(x)
#define DIMPROV_PRINTF(x...)
#endif
#define IMPROV_VERSION 1

[[maybe_unused]] void parseWiFiCommand(char *rpcData);

enum ImprovPacketType {
    Current_State = 0x01,
    Error_State = 0x02,
    RPC_Command = 0x03,
    RPC_Response = 0x04
};

enum ImprovPacketByte {
    Version = 6,
    PacketType = 7,
    Length = 8,
    RPC_CommandType = 9
};

enum ImprovRPCType {
    Command_Wifi = 0x01,
    Request_State = 0x02,
    Request_Info = 0x03
};

extern byte improvActive; //0: no improv packet received, 1: improv active, 2: provisioning
extern byte improvError;
extern char serverDescription[33];
extern char cmDNS[33];
extern char clientSSID[33];
extern char clientPass[65];
// TODO
static uint8_t improvIndex = 0;
static uint8_t improvLength = 0;
static uint8_t improvChecksum = 0;
static uint8_t improvPacketType = 0;
static uint8_t improvRpcType = 0;
static char improvRpcData[128];
// TODO

class WifiManager {

private:
    Helpers helper;

    static void createWebServer();

    static void setupAP();

    static void launchWeb();

public:
    void setupWiFi(void (*manageDisconnections)(), void (*manageHardwareButton)());

    void reconnectToWiFi(void (*manageDisconnections)(), void (*manageHardwareButton)());

    static void setupOTAUpload();

    static int getQuality();

    static bool isWifiConfigured(); // check if wifi is correctly configured
    static void launchWebServerForOTAConfig(); // if no ssid available, launch web server to get config params via browser
    static void launchWebServerCustom(void (*listener)()); // if no ssid available, launch web server to get config params via browser
    void manageImprovWifi(); // if no ssid available, launch web server to get config params via browser
    void handleImprovPacket();

    void sendImprovInfoResponse();

    void parseWiFiCommand(char *rpcData);

    void sendImprovRPCResponse(byte commandId);

    void sendImprovRPCResponse(byte commandId, bool forceConnection);

    void sendImprovStateResponse(uint8_t state, bool error);

    static bool isConnected(); // return true if wifi is connected

    void setTxPower() const;
};

#endif