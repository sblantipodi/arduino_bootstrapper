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