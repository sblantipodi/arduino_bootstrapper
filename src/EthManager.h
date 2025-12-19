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

typedef struct EthConfigW5500 {
  int miso_pin;
  int mosi_pin;
  int sclk_sck_pin;
  int cs_pin;
  int int_pin;
  int rst_pin;
  int addr_nc;
} ethernet_confi_spi;

extern const ethernet_confi_spi ethernetDevicesSpi[];

const uint8_t spiStartIdx = 100;

class EthManager {
public:
  static void connectToSpi(int8_t &deviceNumber);

  static void initSpiEthernet(int8_t deviceNumber, int8_t mosi, int8_t miso, int8_t sclk, int8_t cs, int8_t interrupt, int8_t rst);

  static void initRmiiEthernet(int8_t deviceNumber);

  static void connectToEthernet(int8_t deviceNumber, int8_t mosi, int8_t miso, int8_t sclk, int8_t cs, int8_t interrupt, int8_t rst);

  static void deallocateEthernetPins(int8_t deviceNumber);
};

#endif
#endif
