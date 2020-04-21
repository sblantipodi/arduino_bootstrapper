/*
  BoostrapManager.cpp - Main header for bootstrapping arduino projects
  
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

#ifndef _DPSOFTWARE_BOOTSTRAP_MANAGER_H
#define _DPSOFTWARE_BOOTSTRAP_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "../../include/Configuration.h"
#include "Helpers.h"
#include "WifiManager.h"
#include "QueueManager.h"

// Maximum JSON Object Size
const int BUFFER_SIZE = JSON_OBJECT_SIZE(MAX_JSON_OBJECT_SIZE);
 
class BootstrapManager {
  
  public:
    void bootstrapSetup(void (*manageDisconnectionFunction)(), void (*manageHardwareButton)(), void (*callback)(char*, byte*, unsigned int)); // bootstrap setup()
    void bootstrapLoop(void (*manageDisconnectionFunction)(), void (*manageQueueSubscription)(), void (*manageHardwareButton)()); // bootstrap loop()
    void nonBlokingBlink(); // blink default LED when sending data to the queue
    void getMicrocontrollerInfo(); // print or display microcontroller's info
    void drawInfoPage(String softwareVersion, String author); // draw a page with all the microcontroller's info
    void drawScreenSaver(String txt); // useful for OLED displays

};

#endif