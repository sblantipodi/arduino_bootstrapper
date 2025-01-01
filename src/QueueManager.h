/*
  QueueManager.h - Managing MQTT queue
  
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

#ifndef _DPSOFTWARE_QUEUE_MANAGER_H
#define _DPSOFTWARE_QUEUE_MANAGER_H

#include <PubSubClient.h>
#include "WifiManager.h"


class QueueManager {

private:
    Helpers helper;

public:
    static void setupMQTTQueue(void (*callback)(char *, byte *, unsigned int)); // setup the queue
    static void setMQTTWill(const char *topic, const char *payload, int qos, boolean retain, boolean cleanSession); // set the last will parameters for mqtt
    void mqttReconnect(void (*manageDisconnections)(), void (*manageQueueSubscription)(), void (*manageHardwareButton)()); // manage reconnection on the queue
    void queueLoop(void (*manageDisconnections)(), void (*manageQueueSubscription)(), void (*manageHardwareButton)()); // manage queue loop
    static void publish(const char *topic, const char *payload, boolean retained); // send a message on the queue
    static void unsubscribe(const char *topic); // unsubscribe to a queue topic
    static void subscribe(const char *topic); // subscribe to a queue topic
    static void subscribe(const char *topic, uint8_t qos); // subscribe to a queue topic with qos 0 or 1

};

#endif