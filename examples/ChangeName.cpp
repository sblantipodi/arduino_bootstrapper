/*
  ChangeName.cpp - Main Arduino File - Utility classes for bootstrapping arduino projects with Wifi management, 
  OTA upload management, memory management, MQTT and queue management.

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

#include "ChangeName.h"

void setup() {

  Serial.begin(SERIAL_RATE);

  bootstrapManager.bootstrapSetup(manageDisconnections, manageHardwareButton, callback);
  
  // ENTER YOUR CODE HERE

}

/********************************** MANAGE WIFI AND MQTT DISCONNECTION *****************************************/
void manageDisconnections() {
 
}

/********************************** MQTT SUBSCRIPTIONS *****************************************/
void manageQueueSubscription() {
  
  // example to topic subscription
  bootstrapManager.subscribe(CHANGE_ME_TOPIC);
  bootstrapManager.subscribe(CHANGE_ME_JSON_TOPIC);

}

/********************************** MANAGE HARDWARE BUTTON *****************************************/
void manageHardwareButton() {
 
}

/********************************** START CALLBACK *****************************************/
void callback(char* topic, byte* payload, unsigned int length) {

  // Transform all messages in a JSON format  
  StaticJsonDocument<BUFFER_SIZE> json = bootstrapManager.parseQueueMsg(topic, payload, length);

  if(strcmp(topic, CHANGE_ME_TOPIC) == 0) {
    String simpleMsg = json[VALUE];
    // Serial.println(simpleMsg);    
  } else if(strcmp(topic, CHANGE_ME_JSON_TOPIC) == 0) {
    String simpleMsg = json["ROOT_EXAMPLE"];
    // Serial.println(simpleMsg);
  }

}

/********************************** START MAIN LOOP *****************************************/
void loop() {

  if (isConfigFileOk) {
  
    // Bootsrap loop() with Wifi, MQTT and OTA functions
    bootstrapManager.bootstrapLoop(manageDisconnections, manageQueueSubscription, manageHardwareButton);

    // ENTER YOUR CODE HERE

    Serial.print("Hello World");
    delay(1000);

    // Send MSG to the MQTT queue with no retention
    bootstrapManager.publish(CHANGE_ME_TOPIC, "SEND SIMPLE MSG TO THE QUEUE", false);  
    
    // Send JSON MSG to the MQTT queue with no retention
    JsonObject root = bootstrapManager.getJsonObject();    
    root["ROOT_EXAMPLE"] = "SEND JSON MSG TO THE QUEUE";
    bootstrapManager.publish(CHANGE_ME_JSON_TOPIC, root, false);  

  }
  
}