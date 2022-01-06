/*
  Secrets.h - Store your secrets here
  
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

#ifndef _DPSOFTWARE_SECRETS_H
#define _DPSOFTWARE_SECRETS_H

#ifndef WIFI_SSID
#define WIFI_SSID "XXX"
#endif
const char* const SSID = WIFI_SSID;

#ifndef WIFI_PWD
#define WIFI_PWD "XXX"
#endif
const char* const PASSWORD = WIFI_PWD;

#ifndef MQTT_USER
#define MQTT_USER "XXX"
#endif
const char* const MQTT_USERNAME = MQTT_USER;

#ifndef MQTT_PWD
#define MQTT_PWD "XXX"
#endif
const char* const MQTT_PASSWORD = MQTT_PWD;

#ifndef OTA_PWD
#define OTA_PWD "XXX"
#endif
const char* const OTAPASSWORD = OTA_PWD; //the password you will need to enter for OTA upload

#endif