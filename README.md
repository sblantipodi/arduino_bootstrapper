# Arduino Bootstrapper

Utility classes for bootstrapping arduino projects with Wifi management, OTA upload management, memory management, MQTT and queue management.  
Espressif **`ESP8266/ESP32`** are the default platforms used but it can be easily configured for other platforms and boards.  
_Written for Arduino IDE and PlatformIO._

[![GitHub Actions CI](https://github.com/sblantipodi/arduino_bootstrapper/workflows/GitHub%20Actions%20CI/badge.svg)](https://github.com/sblantipodi/arduino_bootstrapper/actions)
[![arduino-library-badge](https://www.ardu-badge.com/badge/Bootstrapper.svg?)](https://www.ardu-badge.com/Bootstrapper)
[![GitHub version](https://img.shields.io/github/v/release/sblantipodi/arduino_bootstrapper.svg)](https://github.com/sblantipodi/arduino_bootstrapper/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-purple.svg)](https://GitHub.com/sblantipodi/arduino_bootstrapper/graphs/commit-activity)
[![DPsoftware](https://img.shields.io/static/v1?label=DP&message=Software&color=orange)](https://www.dpsoftware.org)

If you like **Arduino Bootstrapper**, give it a star, or fork it and contribute!

[![GitHub stars](https://img.shields.io/github/stars/sblantipodi/arduino_bootstrapper.svg?style=social&label=Star)](https://github.com/sblantipodi/arduino_bootstrapper/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/sblantipodi/arduino_bootstrapper.svg?style=social&label=Fork)](https://github.com/sblantipodi/arduino_bootstrapper/network)

## Credits
- Davide Perini 

## How to get help if you are stuck?
Open an issue here on Github, your questions could be useful to other users.

## How To
There is two way to bootstrap your software using this utilities.  
1) Bootstrap a project from scratch  
2) Import those utilities to your existing project 
    
#### There are other projects that uses this utility, you can explore their sources here:  
[Smart Thermostat](https://github.com/sblantipodi/smart_thermostat), [Solar Station](https://github.com/sblantipodi/solar_station), [PC Ambilight](https://github.com/sblantipodi/pc_ambilight), [Smart Watch Winder](https://github.com/sblantipodi/smart_watch_winder)  


## 1) Bootstrap a project from scratch
Clone the bootstrapper
```
git clone git@github.com:sblantipodi/arduino_bootstrapper.git
```
Simply edit those file as per description, _few minutes required_

    .
    ├── ...
    ├── examples                 # Project src folder
    │   ├── ChangeName.cpp       # Main file with loop() and setup() function, rename and copy it into your src folder  
    │   ├── ChangeName.h         # Main header file, rename and copy it into your include folder  
    │   └── ...      
    ├── src                      # Folder for core files, edit those files and contribute!
    │   ├── BootstrapManager.h   # Core header file with utility classes for bootstrapping
    │   ├── QueueManager.h       # Core header file with utility classes for Queue and MQTT management
    │   ├── WifiManager.h        # Core header file with utility classes for Wifi and OTA upload management
    │   ├── Helpers.h            # Core header file with helper classes 
    │   └── ...       
    ├── platformio.ini           # Configure all the required info (ex: Wifi device name, DNS gateway, ecc.)
    ├── secrets.ini.template     # Configure password and rename the file in secrets.ini 
    └── ...
    
***NOTE:***  
You should implement those functions that are passed by _*pointer_ to the main bootstrap functions:   
```c++
class BootstrapManager {
  ...
  public:
    void bootstrapSetup(
        void (*manageDisconnectionFunction)(), 
        void (*manageHardwareButton)(), 
        void (*callback)(char*, byte*, unsigned int)
    );
    void bootstrapLoop(
        void (*manageDisconnectionFunction)(), 
        void (*manageQueueSubscription)(), 
        void (*manageHardwareButton)()
    );  
    ...
};
```  

- `manageDisconnections()`           # _OPTIONAL_ put the logic you need in case your microcontroller is disconnected from the network
- `manageHardwareButton()`           # _OPTIONAL_ put special instruction for hardware button management during network disconnections
- `manageQueueSubscription()`        # subscribe to the desired mqtt topics
- `callback()`                       # callback function called when a message arrives from the queue

## 2) Import those utilities to your existing project 
You can import `Arduino Bootstrapper` into your existing projects in two way:
1) Import via public registries (easyest way to import)
```
// For PlatformIO
Add `lib_deps` to your `platformio.ini`
lib_deps = ArduinoBootstrapper
```
```
// For ArduinoIDE
Simply import the Bootstrapper library from the library manager
```
2) Import via git submodules (faster updates to latest releases)  
```
git submodule add https://github.com/sblantipodi/arduino_bootstrapper.git arduino_bootstrapper
```  

For both importing method you should then add extra dirs to your `platformio.ini`
```ini
lib_extra_dirs = arduino_bootstrapper
```
Copy and configure `~/arduino_bootstrapper/secrets.ini.template` into `secrets.ini.template` 
  
Please include BootrapManager.h into your main header file:
```c++
#include "BootstrapManager.h"
```
and initialize the BootstrapManager class:
```
BootstrapManager bootstrapManager;
```

In your `setup()` function add the Wifi, MQTT and OTA bootstrapper
```
bootstrapManager.bootstrapSetup(manageDisconnections, manageHardwareButton, callback);
```

In your `loop()` function add the bootstrap manager function
```
bootstrapManager.bootstrapLoop(manageDisconnections, manageQueueSubscription, manageHardwareButton);
```

Please follow the `Bootstrap a project from scratch` instructions without the initial git clone part.

#### Enable symlinks in GIT for Windows
This project uses symlinks, Windows does not enable symlinks by default, to enable it, run this cmd from an admin console:
```bash
export MSYS=winsymlinks:nativestrict
```

## Continuous Integrations
This project supports CI via GitHub Actions.
In the `.github/workflows` folder there are two workflows
 - one for automatic release that is triggered when a git tag is pushed
 - one for building the project and creating the artifact (binary firmware)  
 
If you use this syntax:
```
git tag -a v1.0.0 -m "your commit msg";
git push origin --tags;
```
text in the commit message will be the description of your release.

## Access Point frontend 
Arduino Bootstrapper will search for a `secrets.ini`, if you don't configure it, access point is started.  
You can connect to the AP with your mobile and go to http://192.168.4.1 to access the gui  
that will let you enter all the passwords without the needs of hardcoding them.
 
![Access Point Frontend](https://raw.githubusercontent.com/sblantipodi/arduino_bootstrapper/master/data/img/arduinobootstrapper.png)

## License
This program is licensed under MIT License

## Thanks To 
|  Thanks              |  For                           |
|----------------------|--------------------------------|
|<a href="https://www.jetbrains.com/"><img width="200" src="https://raw.githubusercontent.com/sblantipodi/arduino_bootstrapper/master/data/img/jetbrains.png"></a>| For the <a href="https://www.jetbrains.com/clion">CLion IDE</a> licenses.|

