# Arduino Bootstrapper

Utility classes for bootstrapping arduino projects with Wifi management, OTA upload management, memory management,   
MQTT and queue management.  
_Written for Arduino IDE and PlatformIO._

[![GitHub version](https://img.shields.io/github/v/release/sblantipodi/arduino_bootstrapper.svg)](https://img.shields.io/github/v/release/sblantipodi/arduino_bootstrapper.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://GitHub.com/sblantipodi/arduino_bootstrapper/graphs/commit-activity)

If you like **Arduino Bootstrapper**, give it a star, or fork it and contribute!

[![GitHub stars](https://img.shields.io/github/stars/sblantipodi/arduino_bootstrapper.svg?style=social&label=Star)](https://github.com/sblantipodi/arduino_bootstrapper/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/sblantipodi/arduino_bootstrapper.svg?style=social&label=Fork)](https://github.com/sblantipodi/arduino_bootstrapper/network)

## Credits
- Davide Perini

## How To
There is two way to bootstrap your software using this utilities.
1) Start a project from scratch
2) Import this utilities to your existing project

### Bootstrap a project from scratch
Clone the bootstrapper
```
git clone git@github.com:sblantipodi/arduino_bootstrapper.git
```
Simply edit those file as per description, few minutes required.

    .
    ├── ...
    ├── src                      # Project src folder
    │   ├── ChangeName.cpp       # Main file with loop() and setup() function, rename it with the name of your project
    │   ├── Configuration.cpp    # Config impl, initialize global vars here like the one used for displays
    │   └── ...   
    ├── include                  # Project header folder
    │   ├── ChangeName.h         # Main header file, rename it with the name of your project
    │   ├── Secrets.h            # Configure your Wifi info (SSID, PWD), MQTT server info (USERNAME, PWD, PORT), OTA PWD
    │   ├── Configuration.h      # Configure all the required info (ex: Wifi device name, DNS gateway, ecc.)
    │   └── ...       
    ├── core                     # Folder for core files, edit those files and contribute!
    │   ├── BootstrapManager.h   # Core header file with utility classes for bootstrapping
    │   ├── QueueManager.h       # Core header file with utility classes for Queue and MQTT management
    │   ├── WifiManager.h        # Core header file with utility classes for Wifi and OTA upload management
    │   ├── Helpers.h            # Core header file with helper classes 
    │   └── ...       
    ├── platformio.ini           # Edit platform with the one you are using (default: ESP8266, board: d1_mini)  
    └── ...
    
### Import those utilities to your existing project
To link this project to your existing one use git submodules.  
```
git submodule add git@github.com:sblantipodi/arduino_bootstrapper.git arduino_bootstrapper
```
Add extra dirs to your `platformio.ini`
```
lib_extra_dirs = arduino_bootstrapper
```
Copy and configure `~/arduino_bootstrapper/include/Secrets.h` to your `include` folder  
Copy and configure `~/arduino_bootstrapper/include/Configuration.h` to your `include` folder  
Copy and configure `~/arduino_bootstrapper/include/Configuration.cpp` to your `src` folder  
  
Please follow the `Bootstrap a project from scratch` instructions without the initial git clone part.

#### Enable symlinks in GIT for Windows
export MSYS=winsymlinks:nativestrict

## License
This program is licensed under MIT License
