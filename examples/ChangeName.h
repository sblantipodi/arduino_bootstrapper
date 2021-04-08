/*
  ChangeName.h - Main header
  
  Copyright (C) 2020 - 2021  Davide Perini
  
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

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "BootstrapManager.h"


/****************** BOOTSTRAP MANAGER ******************/
BootstrapManager bootstrapManager;


/**************************** PIN DEFINITIONS **************************************************/
// Define your pin here


/************* MQTT TOPICS **************************/
const char* CHANGE_ME_TOPIC = "tele/changeme/CHANGEME";
const char* CHANGE_ME_JSON_TOPIC = "tele/changeme/CHANGEME_JSON";


/********************************** FUNCTION DECLARATION (NEEDED BY PLATFORMIO WHILE COMPILING CPP FILES) *****************************************/
void callback(char* topic, byte* payload, unsigned int length);
void manageDisconnections();
void manageQueueSubscription();
void manageHardwareButton();
