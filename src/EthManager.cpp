/*
  EthManager.cpp - Managing WiFi and OTA

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
#include "EthManager.h"

#if defined(ARDUINO_ARCH_ESP32)
/**
 * Supported ethernet devices
 */
const ethernet_config ethernetDevices[] = {
  // No Ethernet
  {
  },
#if CONFIG_IDF_TARGET_ESP32
  // QuinLed-ESP32-Ethernet
  {
    0,
    5,
    23,
    18,
    ETH_PHY_LAN8720,
    ETH_CLOCK_GPIO17_OUT
  },
  // QuinLed-Dig-Octa Brainboard-32-8L and LilyGO-T-ETH-POE
  {
    0,
    -1,
    23,
    18,
    ETH_PHY_LAN8720,
    ETH_CLOCK_GPIO17_OUT
  },
  // WT32-EHT01
  // These pins works well: IO2, IO4, IO12, IO14, IO15
  // These not: IO35, IO36, IO39
  {
    1,
    16,
    23,
    18,
    ETH_PHY_LAN8720,
    ETH_CLOCK_GPIO0_IN
  },
  // ESP32-ETHERNET-KIT-VE
  {
    0,
    5,
    23,
    18,
    ETH_PHY_IP101,
    ETH_CLOCK_GPIO0_IN
  },
  // ESP32-POE
  {
    0,
    12,
    23,
    18,
    ETH_PHY_LAN8720,
    ETH_CLOCK_GPIO17_OUT
  },
  // WESP32
  {
    0,
    -1,
    16,
    17,
    ETH_PHY_LAN8720,
    ETH_CLOCK_GPIO0_IN
  },
  // LilyGO-T-POE-Pro
  {
    0,
    5,
    23,
    18,
    ETH_PHY_LAN8720,
    ETH_CLOCK_GPIO0_OUT
  },
  // ESP32-POE-WROVER
  {
    0,
    12,
    23,
    18,
    ETH_PHY_LAN8720,
    ETH_CLOCK_GPIO0_OUT
  }
#endif
};

const ethernet_config_w5500 ethernetDevices_w5500[] = {
  // T-ETH ELite ESP32-S3
  {
    47,
    21,
    48,
    45,
    14,
    -1,
    -1
  },
  // T-ETH Lite ESP32-S3
  {
    11,
    12,
    10,
    9,
    13,
    14,
    -1
  },
  // S3 new
  {
    4,
    18,
    35,
    44,
    12,
    -1,
    -1
  },
  //C3
  {
    2,
    7,
    8,
    20,
    1,
    -1,
    -1
  },
  //PICO
  {
    4,
    19,
    18,
    26,
    14,
    -1,
    27
  },
  //C6
  {
    3,
    20,
    19,
    11,
    2,
    -1,
    0
  },
  // S3pco
  {
    1,
    37,
    36,
    5,
    2,
    -1,
    -1
  }
};


/**
 * Init SPI ethernet
 * @param deviceNumber to use
 */
void EthManager::initSpiEthernet(int8_t &deviceNumber) {
  deviceNumber = 3; // w5500_start_index - deviceNumber;
  ETH.begin(
    ETH_PHY_W5500,
    ethernetDevices_w5500[deviceNumber].addr_nc,
    ethernetDevices_w5500[deviceNumber].cs_pin,
    ethernetDevices_w5500[deviceNumber].int_pin,
    ethernetDevices_w5500[deviceNumber].rst_pin,
#if CONFIG_IDF_TARGET_ESP32C3
    SPI2_HOST,
#else
    SPI3_HOST,
#endif
    ethernetDevices_w5500[deviceNumber].sclk_sck_pin,
    ethernetDevices_w5500[deviceNumber].miso_pin,
    ethernetDevices_w5500[deviceNumber].mosi_pin
  );
}

/**
 * Init RMII ethernet
 * @param deviceNumber to use
 */
void EthManager::initRmiiEthernet(int8_t deviceNumber) {
#if CONFIG_IDF_TARGET_ESP32
  ETH.begin(
    ethernetDevices[deviceNumber].type,
    ethernetDevices[deviceNumber].address,
    ethernetDevices[deviceNumber].mdc,
    ethernetDevices[deviceNumber].mdio,
    ethernetDevices[deviceNumber].power,
    ethernetDevices[deviceNumber].clk_mode
  );
#endif
}

/**
 * Connect to ethernet
 * @param deviceNumber to use
 */
void EthManager::connectToEthernet(int8_t deviceNumber) {
#if CONFIG_IDF_TARGET_ESP32
  if (deviceNumber < spiStartIdx) {
    initRmiiEthernet(deviceNumber);
  } else {
    initSpiEthernet(deviceNumber);
  }
#else
  initSpiEthernet(deviceNumber);
#endif
}

/**
 * Deallocate ethernet pins
 * @param deviceNumber to deallocate
 */
void EthManager::deallocateEthernetPins(int8_t deviceNumber) {
  if (deviceNumber < spiStartIdx) {
    gpio_reset_pin((gpio_num_t) ethernetDevices[deviceNumber].address);
    gpio_reset_pin((gpio_num_t) ethernetDevices[deviceNumber].power);
    gpio_reset_pin((gpio_num_t) ethernetDevices[deviceNumber].mdc);
    gpio_reset_pin((gpio_num_t) ethernetDevices[deviceNumber].mdio);
#if CONFIG_IDF_TARGET_ESP32
    gpio_reset_pin((gpio_num_t) ethernetDevices[deviceNumber].clk_mode);
#endif
  }
  delay(1);
}

#endif
