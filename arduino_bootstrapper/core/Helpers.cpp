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
String lastBoot = " ";
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

/*
  This function returns a single string separated by a predefined character at a given index. For example:
    String split = "hi this is a split test";
    String word3 = getValue(split, ' ', 2);
    Serial.println(word3);
  Should print 'is'. You also can try with index 0 returning 'hi' or safely trying index 5 returning 'test'.
*/
String Helpers::getValue(String data, char separator, int index) {

  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++) {
    if(data.charAt(i)==separator || i==maxIndex) {
      found++;
      strIndex[0] = strIndex[1]+1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";

}