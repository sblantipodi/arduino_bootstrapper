/*
 * Arduino Bootstrapper
 * DPsoftware (Davide Perini)
 * 
 * Utility classes for bootstrapping arduino projects with Wifi management, OTA upload management, memory management, MQTT and queue management.
 * 
 * MIT license
 */
#include "ChangeName.h"

void setup() {

  Serial.begin(SERIAL_RATE);
  Serial.print("setup");
  // LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.setTextColor(WHITE);
  // Bootsrap setup() with Wifi and MQTT functions
  bootstrapManager.bootstrapSetup(manageDisconnections, manageHardwareButton, callback);

  // ENTER YOUR CODE HERE

}

/********************************** MANAGE WIFI AND MQTT DISCONNECTION *****************************************/
void manageDisconnections() {
 
}

/********************************** MQTT SUBSCRIPTIONS *****************************************/
void manageQueueSubscription() {
  
}

/********************************** MANAGE HARDWARE BUTTON *****************************************/
void manageHardwareButton() {
 
}

/********************************** START CALLBACK *****************************************/
void callback(char* topic, byte* payload, unsigned int length) {
  
}

void loop() {

  // Bootsrap loop() with Wifi, MQTT and OTA functions
  bootstrapManager.bootstrapLoop(manageDisconnections, manageQueueSubscription, manageHardwareButton);

  // ENTER YOUR CODE HERE

  Serial.print("Hello World");
  delay(1000);
  
}