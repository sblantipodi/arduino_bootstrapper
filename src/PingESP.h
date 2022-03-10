/*
  PingESP.h - Utility class for pining the default gateway from ESP8266,
  some routers aren't able to detect connected devices if there is no traffic over the internet or direct to the gateway.
  This is helpful as a stay alive class, useful for not triggering security features on routers.
  Usage:
  - add the following declaration in your application header file:
      #include "PingESP.h"
      PingESP pingESP;
  - use `pingESP.ping(WiFi.gatewayIP());` in your application code

  For the ESP32:
  - you need to install the ESP32Ping library from https://github.com/marian-craciunescu/ESP32Ping 
  - add the following declaration in your application header file:
      #include "PingESP.h"
      PingClass pingESP;`
  - use `pingESP.ping(WiFi.gatewayIP());` in your application code

  Copyright (C) 2020 - 2022  Davide Perini

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

#if defined(ESP8266)||(ESP32)

#ifndef PingESP_H
#define PingESP_H

#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>

extern "C" {
#include <ping.h>
}
#elif defined(ESP32)
    #include <WiFi.h>
    #include <ESP32Ping.h>
    #include <ping.h>
#endif 

#if defined(ESP8266)
class PingESP {
public:
    PingESP();
    bool ping(IPAddress dest);

protected:
    static void receivePingCallback(void *opt, void *pdata);
    ping_option pingOptions;
    static byte pingCount, pingError, pingSuccess;
};

#endif // ESP8226

#endif // PingESP_H

#endif // ESP8226 or ESP32
