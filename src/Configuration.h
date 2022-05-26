/*
  Configuration.h - Config header
  
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

#ifndef _DPSOFTWARE_CONFIG_H
#define _DPSOFTWARE_CONFIG_H

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#elif defined(ESP32)
#include <WiFi.h>
  #include <ESPmDNS.h>
  #include <HTTPClient.h>
  #include <WebServer.h>
#endif

#ifndef AUTHOR
#define AUTHOR "DPsoftware"
#endif

// Serial rate for debug
#ifndef SERIAL_RATE
#define SERIAL_RATE 115200
#endif

#ifndef DEBUG_QUEUE_MSG
#define DEBUG_QUEUE_MSG false
#endif

// Specify if you want to use a display or only Serial
#ifndef DISPLAY_ENABLED
#define DISPLAY_ENABLED false
#endif

#if (DISPLAY_ENABLED)
#include <Adafruit_SSD1306.h>
// Display specs
#define OLED_RESET LED_BUILTIN // Pin used for integrated D1 Mini blue LED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
extern Adafruit_SSD1306 display;
#endif

// Values greater then 0 enables Improv for that milliseconds period
#ifndef IMPROV_ENABLED
#define IMPROV_ENABLED 0
#endif

// SENSORNAME will be used as device network name
#ifndef WIFI_DEVICE_NAME
#define WIFI_DEVICE_NAME "ArduinoBootstrapper"
#endif
const char* const DEVICE_NAME = WIFI_DEVICE_NAME;

// Port for the OTA firmware uplaod
#ifndef MICROCONTROLLER_OTA_PORT
#define MICROCONTROLLER_OTA_PORT 8199
#endif
const int OTA_PORT = MICROCONTROLLER_OTA_PORT;

// Set wifi power in dbm range 0/0.25, set to 0 to reduce PIR false positive due to wifi power, 0 low, 20.5 max.
#ifndef WIFI_SIGNAL_STRENGTH
#define WIFI_SIGNAL_STRENGTH 0
#endif
const double WIFI_POWER = WIFI_SIGNAL_STRENGTH;

// GATEWAY IP
#ifndef GATEWAY_IP
#define GATEWAY_IP "192.168.1.1"   // enter your gateway IP address
#endif
const char* const IP_GATEWAY = GATEWAY_IP;

// SUBNET IP
#ifndef SUBNET_IP
#define SUBNET_IP "255.255.255.0"
#endif
const char* const IP_SUBNET = SUBNET_IP;

// DNS IP
#ifndef DNS_IP
#define DNS_IP "8.8.8.8"    // enter a DNS server IP if you need domain name access and static IP is selected
// example: 8.8.8.8 for Google DNS server or use the DNS server provided by your ISP (see your gateway configuration)
#endif
const char* const IP_DNS = DNS_IP;

// STATIC IP FOR THE MICROCONTROLLER
#ifndef MICROCONTROLLER_IP
#define MICROCONTROLLER_IP "192.168.1.99"  // enter a valid IP address for static IP or "DHCP" for dynamic IP
#endif
const char* const IP_MICROCONTROLLER = MICROCONTROLLER_IP;

// MQTT server IP
#ifndef MQTT_SERVER_IP
#define MQTT_SERVER_IP "192.168.1.3"
#endif
const char* const MQTT_SERVER = MQTT_SERVER_IP;

// MQTT server port
#ifndef MQTT_SERVER_PORT
#define MQTT_SERVER_PORT "1883"
#endif
const char* const MQTT_PORT = MQTT_SERVER_PORT;

// Maximum number of reconnection (WiFi/MQTT) attemp before powering off peripherals
#ifndef MAX_RECONNECT
#define MAX_RECONNECT 500
#endif

// Maximum JSON Object Size
#ifndef MAX_JSON_OBJECT_SIZE
#define MAX_JSON_OBJECT_SIZE 50
#endif

// Maximum JSON Object Size
#ifndef SMALL_JSON_OBJECT_SIZE
#define SMALL_JSON_OBJECT_SIZE 50
#endif

// Maximum JSON Object Size
#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 1024
#endif

// MQTT Keep Alive
#ifndef MQTT_KEEP_ALIVE
#define MQTT_KEEP_ALIVE 60
#endif

// MQTT Last Will
#ifndef MQTT_USE_LAST_WILL
#define MQTT_WILL_TOPIC 0
#define MQTT_WILL_PAYLOAD 0
#endif

#ifndef MQTT_WILL_QOS
#define MQTT_WILL_QOS 1
#endif

#ifndef MQTT_WILL_RETAIN
#define MQTT_WILL_RETAIN 0
#endif

#ifndef MQTT_CLEAN_SESSION
#define MQTT_CLEAN_SESSION 1
#endif

// Additional param that can be used for general purpose use
#ifndef ADDITIONAL_PARAM_TEXT
#define ADDITIONAL_PARAM_TEXT "ADDITIONAL PARAM"
#endif

// Additional param that can be used for general purpose use
#ifndef ADDITIONAL_PARAM
#define ADDITIONAL_PARAM "none"
#endif
const char* const PARAM_ADDITIONAL = ADDITIONAL_PARAM;

#endif