#ifndef _DPSOFTWARE_WIFI_MANAGER_H
#define _DPSOFTWARE_WIFI_MANAGER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "Helpers.h"
#include "../../include/Secrets.h"
#include "../../include/Configuration.h"


namespace WifiConfig {
  // DNS address for the microcontroller:
  const IPAddress mydns = IP_GATEWAY;
  // GATEWAY address for the microcontroller:
  const IPAddress mygateway = IP_GATEWAY;
  // Static IP address for the microcontroller:
  const IPAddress arduinoip = IP_MICROCONTROLLER;
}

class WifiManager {

  public:
    void setupWiFi(void (*manageDisconnectionFunction)(), void (*manageHardwareButton)());
    void setupOTAUpload();

};

#endif