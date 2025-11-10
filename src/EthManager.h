/*
  EthManager.h - Managing Wifi and OTA
  
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

#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32)

#include <ETH.h>

#endif

#ifndef _DPSOFTWARE_ETH_MANAGER_H
#define _DPSOFTWARE_ETH_MANAGER_H

#if defined(ARDUINO_ARCH_ESP32)
typedef struct EthConfig {
    uint8_t address;
    int power;
    int mdc;
    int mdio;
#if CONFIG_IDF_TARGET_ESP32
    eth_phy_type_t type;
    eth_clock_mode_t clk_mode;
#endif
} ethernet_config;

extern const ethernet_config ethernetDevices[];

class EthManager {

public:
    static void connectToEthernet(int8_t deviceNumber);
    static void deallocateEthernetPins(int8_t deviceNumber);
};

#endif
#endif