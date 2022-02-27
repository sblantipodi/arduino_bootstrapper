/*
  PingESP.cpp - Utility class for pining the default gateway from ESP8266,
  some routers isn't able to detect connected devices if there is no traffic over the internet or direct to the gateway.
  This is helpful as a stay alive class, useful for not triggering security features on routers.

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

#if defined(ESP8266)

#include "PingESP.h"

extern "C" void esp_schedule();
extern "C" void esp_yield();
extern "C" void __esp_suspend();

extern "C" void suspend_or_yield() {
  Serial.printf("This is the default handler\n");
  esp_yield();
}

void __esp_suspend(void) __attribute__((weak, alias("suspend_or_yield")));

PingESP::PingESP() {}

byte PingESP::pingError = 0;
byte PingESP::pingSuccess = 0;

/**
 * Ping gateway to keep the connection alive
 * @param dest gateway to ping
 * @return
 */
bool PingESP::ping(IPAddress dest) {
  pingError = 0;
  pingSuccess = 0;
  memset(&pingOptions, 0, sizeof(struct ping_option));
  pingOptions.count = 1; // repeat only 1 time
  pingOptions.coarse_time = 1;
  pingOptions.ip = dest;
  pingOptions.recv_function = reinterpret_cast<ping_recv_function>(&PingESP::receivePingCallback);
  pingOptions.sent_function = NULL;
  if (ping_start(&pingOptions)) {
    // Sleep until the ping is finished
    __esp_suspend();
  }
  return (pingSuccess > 0);
}

/**
 * Callback from the ESP core ping_start
 * @param opt options
 * @param resp response
 */
void PingESP::receivePingCallback(void *opt, void *resp) {
  // ping_resp from ESP core
  ping_resp *ping_resp = reinterpret_cast<struct ping_resp *>(resp);
  // Check for errors
  if (ping_resp->ping_err == -1) {
    pingError++;
    Serial.println(F("PING KO"));
  } else {
    pingSuccess++;
  }
  if (pingSuccess + pingError == 1) {
    // ping sent, return
    esp_schedule();
  }
}

///**
// * Weak function, if there is some implementation of esp_suspend() elsewhere, use that implementation,
// * use this implementation instead.
// */

#endif