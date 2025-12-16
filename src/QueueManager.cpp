/*
  QueueManager.cpp - Managing MQTT queue
  
  Copyright © 2020 - 2025  Davide Perini
  
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

#include "QueueManager.h"


PubSubClient mqttClient(espClient);

/********************************** SETUP MQTT QUEUE **********************************/
void QueueManager::setupMQTTQueue(void (*callback)(char *, byte *, unsigned int)) {
  mqttClient.setServer(IPAddress(Helpers::getValue(mqttIP, '.', 0).toInt(),
                                 Helpers::getValue(mqttIP, '.', 1).toInt(),
                                 Helpers::getValue(mqttIP, '.', 2).toInt(),
                                 Helpers::getValue(mqttIP, '.', 3).toInt()), mqttPort.toInt());
  mqttClient.setCallback(callback);
  mqttClient.setBufferSize(MQTT_MAX_PACKET_SIZE);
  mqttClient.setKeepAlive(MQTT_KEEP_ALIVE);
}

/********************************** SET LAST WILL PARAMETERS **********************************/
void
QueueManager::setMQTTWill(const char *topic, const char *payload, const int qos, boolean retain, boolean cleanSession) {
  mqttWillTopic = topic;
  mqttWillPayload = payload;
  mqttWillQOS = qos;
  mqttWillRetain = retain;
  mqttCleanSession = cleanSession;
}

/********************************** MQTT RECONNECT **********************************/
void QueueManager::mqttReconnect(void (*manageDisconnections)(), void (*manageQueueSubscription)(),
                                 void (*manageHardwareButton)()) {
  // how many attemps to MQTT connection
  mqttReconnectAttemp = 0;
  // Loop until we're reconnected
  while (!mqttClient.connected() && (WiFi.status() == WL_CONNECTED || ethd >= 0) && Serial.peek() == -1) {
#if (DISPLAY_ENABLED)
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
#endif
    if (mqttReconnectAttemp <= 20) {
      Helpers::smartPrintln(F("Connecting to"));
      Helpers::smartPrintln(F("MQTT Broker..."));
    }
    helper.smartDisplay();
    // Manage hardware button if any
    manageHardwareButton();
    // Attempt to connect to MQTT server with QoS = 1 (pubsubclient supports QoS 1 for subscribe only, published msg have QoS 0 this is why I implemented a custom solution)
    boolean mqttSuccess;
    Serial.println("MQTT Last Will Params: ");
    Serial.print("willTopic: ");
    Serial.println(mqttWillTopic);
    Serial.print("willPayload: ");
    Serial.println(mqttWillPayload);
    Serial.print("qos: ");
    Serial.println(mqttWillQOS);
    Serial.print("retain: ");
    Serial.println(mqttWillRetain);
    Serial.print("clean session: ");
    Serial.println(mqttCleanSession);
    if (mqttuser.isEmpty() || mqttpass.isEmpty()) {
      mqttSuccess = mqttClient.connect(Helpers::string2char(deviceName), Helpers::string2char(mqttWillTopic),
                                       mqttWillQOS, mqttWillRetain, Helpers::string2char(mqttWillPayload));
    } else {
      mqttSuccess = mqttClient.connect(Helpers::string2char(deviceName), Helpers::string2char(mqttuser),
                                       Helpers::string2char(mqttpass), Helpers::string2char(mqttWillTopic), mqttWillQOS,
                                       mqttWillRetain, Helpers::string2char(mqttWillPayload), mqttCleanSession);
    }
    if (mqttSuccess) {
      Helpers::smartPrintln(F(""));
      Helpers::smartPrintln(F("MQTT CONNECTED"));
      Helpers::smartPrintln(F(""));
      Helpers::smartPrintln(F("Reading data from"));
      Helpers::smartPrintln(F("the network..."));
      helper.smartDisplay();
      // Subscribe to MQTT topics
      manageQueueSubscription();
      delay(DELAY_2000);
      mqttReconnectAttemp = 0;
      // reset the lastMQTTConnection to off, will be initialized by next time update
      lastMQTTConnection = OFF_CMD;
    } else {
      Helpers::smartPrintln(F("MQTT attempts="));
#if defined(ESP8266)
      ESP.wdtFeed();
#else
      esp_task_wdt_reset();
#endif
      Helpers::smartPrintln(mqttReconnectAttemp);
      helper.smartDisplay();
      // after MAX_RECONNECT attemps all peripherals are shut down
      if (mqttReconnectAttemp >= MAX_RECONNECT || fastDisconnectionManagement) {
        Helpers::smartPrintln(F("Max retry reached, powering off peripherals."));
        helper.smartDisplay();
        // Manage disconnections, powering off peripherals
        manageDisconnections();
      } else if (mqttReconnectAttemp > 10000) {
        mqttReconnectAttemp = 0;
      }
      mqttReconnectAttemp++;
      // Wait 500 millis before retrying
      delay(DELAY_500);
    }
    if (!blockingMqtt) {
      break;
    }
  }
}

void QueueManager::queueLoop(void (*manageDisconnections)(), void (*manageQueueSubscription)(),
                             void (*manageHardwareButton)()) {
  if (!mqttClient.connected()) {
    mqttConnected = false;
    mqttReconnect(manageDisconnections, manageQueueSubscription, manageHardwareButton);
  } else {
    mqttConnected = true;
  }
  mqttClient.loop();
}

/********************************** SEND A MESSAGE ON THE QUEUE **********************************/
void QueueManager::publish(const char *topic, const char *payload, boolean retained) {
  mqttClient.publish(topic, payload, retained);
}

/********************************** SUBSCRIBE TO A QUEUE TOPIC **********************************/
void QueueManager::unsubscribe(const char *topic) {
  mqttClient.unsubscribe(topic);
}

/********************************** SUBSCRIBE TO A QUEUE TOPIC **********************************/
void QueueManager::subscribe(const char *topic) {
  mqttClient.subscribe(topic);
}

/********************************** SUBSCRIBE TO A QUEUE TOPIC **********************************/
void QueueManager::subscribe(const char *topic, uint8_t qos) {
  mqttClient.subscribe(topic, qos);
}

