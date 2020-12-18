/*
  Helpers.h - Helper classes
  
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

#ifndef _DPSOFTWARE_HELPERS_UTILS_H
#define _DPSOFTWARE_HELPERS_UTILS_H

#include <ArduinoJson.h>
#include "Configuration.h"


// Maximum JSON Object Size
const int BUFFER_SIZE = JSON_OBJECT_SIZE(MAX_JSON_OBJECT_SIZE);

extern bool isConfigFileOk;
extern String lastMQTTConnection;
extern String lastWIFiConnection;
extern String lastBoot;

extern bool screenSaverTriggered;

extern bool lastPageScrollTriggered;
extern int yoffset;

extern String haVersion;
extern String MAC;
extern String deviceName;
extern String microcontrollerIP;
extern String qsid;
extern String qpass;
extern String OTApass;
extern String mqttIP;
extern String mqttPort;
extern String mqttuser;
extern String mqttpass;
extern String additionalParam;

// Blink LED vars
extern long previousMillis;     // will store last time LED was updated
extern const long interval;           // interval at which to blink (milliseconds)
extern bool ledTriggered;
extern int blinkCounter;
extern const int blinkTimes; // 6 equals to 3 blink on and 3 off

extern String timedate;
extern String date;
extern String currentime;
extern String ERROR;

extern int wifiReconnectAttemp;
extern int mqttReconnectAttemp;
extern bool fastDisconnectionManagement;

const int DELAY_10 = 10;
const int DELAY_50 = 50;
const int DELAY_200 = 200;
const int DELAY_500 = 500;
const int DELAY_1000 = 1000;
const int DELAY_1500 = 1500;
const int DELAY_3000 = 3000;
const int DELAY_2000 = 2000;
const int DELAY_4000 = 4000;
const int DELAY_5000 = 5000;

const String ON_CMD = "ON";
const String OFF_CMD = "OFF";
const String on_CMD = "on";
const String off_CMD = "off";
const String VALUE = "value";
const String EMPTY_STR = "";

// 'arduino', 45x31px
const unsigned char ARDUINOLOGO [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x01, 0xe0, 0x00, 0x01, 0xff, 0x80, 0x0f, 
	0xfc, 0x40, 0x07, 0xff, 0xe0, 0x3f, 0xff, 0x00, 0x0f, 0xff, 0xf0, 0x7f, 0xff, 0x80, 0x1f, 0x01, 
	0xf8, 0xfc, 0x07, 0xc0, 0x1e, 0x00, 0x7d, 0xf0, 0x03, 0xc0, 0x3c, 0x00, 0x3d, 0xe0, 0x01, 0xe0, 
	0x38, 0x00, 0x1f, 0xc0, 0xc0, 0xe0, 0x78, 0x00, 0x1f, 0xc0, 0xc0, 0xf0, 0x78, 0x7e, 0x0f, 0x83, 
	0xf0, 0xf0, 0x78, 0x7e, 0x0f, 0x83, 0xf0, 0xf0, 0x78, 0x00, 0x0f, 0x80, 0xc0, 0xf0, 0x38, 0x00, 
	0x1f, 0xc0, 0xc0, 0xe0, 0x3c, 0x00, 0x3d, 0xe0, 0x01, 0xe0, 0x3e, 0x00, 0x7d, 0xf0, 0x03, 0xe0, 
	0x1f, 0x00, 0xf8, 0xf8, 0x07, 0xc0, 0x0f, 0xe7, 0xf0, 0x7f, 0x3f, 0x80, 0x07, 0xff, 0xe0, 0x3f, 
	0xff, 0x00, 0x03, 0xff, 0xc0, 0x1f, 0xfe, 0x00, 0x00, 0xff, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x7b, 0xec, 0x9e, 0xc9, 0xe0, 
	0x1c, 0x4b, 0x2c, 0x8c, 0xeb, 0x20, 0x16, 0x5b, 0x3c, 0x8c, 0xea, 0x30, 0x16, 0x73, 0x3c, 0x8c, 
	0xfa, 0x30, 0x1e, 0x5b, 0x2c, 0x8c, 0xdb, 0x30, 0x22, 0x4b, 0x6c, 0x8c, 0xd9, 0x60, 0x23, 0x4f, 
	0x83, 0x1e, 0xc8, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#define ARDUINOLOGOW  45
#define ARDUINOLOGOH  31

// 'home-assistant_big', 44x44px
static const unsigned char HABIGLOGO [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00,
	0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x01, 0xff, 0xf8,
	0x00, 0x00, 0x00, 0x03, 0xf0, 0xfd, 0xe0, 0x00, 0x00, 0x07, 0xe0, 0x7f, 0xe0, 0x00, 0x00, 0x0f,
	0xcf, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xcf, 0x3f, 0xe0, 0x00, 0x00, 0x3f, 0xe6, 0x7f, 0xe0, 0x00,
	0x00, 0x7f, 0xe0, 0x7f, 0xe0, 0x00, 0x00, 0xff, 0xf0, 0xff, 0xf0, 0x00, 0x01, 0xff, 0xf0, 0xff,
	0xf8, 0x00, 0x03, 0xff, 0xf0, 0xff, 0xfc, 0x00, 0x07, 0xf0, 0xf0, 0xf0, 0xfe, 0x00, 0x0f, 0xe0,
	0x70, 0xe0, 0x7f, 0x00, 0x1f, 0xce, 0x70, 0xe7, 0x3f, 0x80, 0x1f, 0xcf, 0x70, 0xef, 0x3f, 0x80,
	0x01, 0xce, 0x70, 0xe7, 0x38, 0x00, 0x01, 0xe0, 0x30, 0xc0, 0x78, 0x00, 0x01, 0xf0, 0x10, 0x80,
	0xf8, 0x00, 0x01, 0xff, 0x00, 0x0f, 0xf8, 0x00, 0x01, 0xff, 0x80, 0x0f, 0xf8, 0x00, 0x01, 0xff,
	0xc0, 0x1f, 0xf8, 0x00, 0x01, 0xff, 0xe0, 0x3f, 0xf8, 0x00, 0x01, 0xff, 0xf0, 0x7f, 0xf8, 0x00,
	0x01, 0xff, 0xf0, 0xff, 0xf8, 0x00, 0x01, 0xff, 0xf0, 0xff, 0xf8, 0x00, 0x01, 0xff, 0xf0, 0xff,
	0xf8, 0x00, 0x01, 0xff, 0xf0, 0xff, 0xf8, 0x00, 0x01, 0xff, 0xf0, 0xff, 0xf8, 0x00, 0x01, 0xff,
	0xf0, 0xff, 0xf8, 0x00, 0x01, 0xff, 0xf0, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#define HABIGLOGOW  44
#define HABIGLOGOH  44

class Helpers {

  public:
    void smartPrint(String msg);
    void smartPrintln(String msg);
    void smartPrint(int msg);
    void smartPrintln(int msg);
    void smartDisplay();
    String getValue(String data, char separator, int index);
	String getValue(String string);
	char* string2char(const String command);
	void setDateTime(String timeConst);
	String isOnOff(StaticJsonDocument<BUFFER_SIZE> json);

};

#endif

