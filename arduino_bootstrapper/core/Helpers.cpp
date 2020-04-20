/*
  Helpers.cpp - Helper classes
  
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

#include "Helpers.h"


String lastMQTTConnection = "OFF";
String lastWIFiConnection = "OFF";
unsigned long previousMillis = 0;     
const long interval = 200;           
bool ledTriggered = false;
int blinkCounter = 0;
const int blinkTimes = 6; 

void Helpers::smartPrint(String msg) {

  if (PRINT_TO_DISPLAY) {
    display.print(msg);
  } else {
    Serial.print(msg);
  }

}

void Helpers::smartPrintln(String msg) {

  if (PRINT_TO_DISPLAY) {
    display.println(msg);
  } else {
    Serial.println(msg);
  }

}

void Helpers::smartPrint(int msg) {

  if (PRINT_TO_DISPLAY) {
    display.print(msg);
  } else {
    Serial.print(msg);
  }

}

void Helpers::smartPrintln(int msg) {

  if (PRINT_TO_DISPLAY) {
    display.println(msg);
  } else {
    Serial.println(msg);
  }

}

void Helpers::smartDisplay() {

  if (PRINT_TO_DISPLAY) {
    display.display();
  }
    
}