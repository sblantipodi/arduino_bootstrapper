/*
  EthManager.cpp - Managing WiFi and OTA

  Copyright © 2020 - 2024  Davide Perini

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
const ethernet_config ethernetDevices[] = {
        // No Ethernet
        {
        },
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
        }
};

void EthManager::connectToEthernet(int8_t deviceNumber) {
  ETH.begin(
          ethernetDevices[deviceNumber].address,
          ethernetDevices[deviceNumber].power,
          ethernetDevices[deviceNumber].mdc,
          ethernetDevices[deviceNumber].mdio,
          ethernetDevices[deviceNumber].type,
          ethernetDevices[deviceNumber].clk_mode
  );
}

#endif
