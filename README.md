# Arduino Bootstrapper

Utility classes for bootstrapping arduino projects with Wifi management, OTA upload management, memory management,   
MQTT and queue management.  
Espressif ESP8266 is the default board used but it can be easily generalized for other boards.
_Written for Arduino IDE and PlatformIO._

[![GitHub Actions CI](https://github.com/sblantipodi/arduino_bootstrapper/workflows/GitHub%20Actions%20CI/badge.svg)](https://github.com/sblantipodi/arduino_bootstrapper/actions)
[![GitHub version](https://img.shields.io/github/v/release/sblantipodi/arduino_bootstrapper.svg)](https://github.com/sblantipodi/arduino_bootstrapper/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://GitHub.com/sblantipodi/arduino_bootstrapper/graphs/commit-activity)
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
1) Start a project from scratch
2) Import this utilities to your existing project

#### There are other projects that uses this utility, you can explore their sources here:  
[Smart Thermostat](https://github.com/sblantipodi/smart_thermostat), [Solar Station](https://github.com/sblantipodi/solar_station), [PC Ambilight](https://github.com/sblantipodi/pc_ambilight), [Smart Watch Winder](https://github.com/sblantipodi/smart_watch_winder)  


## Bootstrap a project from scratch
Clone the bootstrapper
```
git clone git@github.com:sblantipodi/arduino_bootstrapper.git
```
Simply edit those file as per description, _few minutes required_

    .
    ├── ...
    ├── src                             # Project src folder
    │   ├── ChangeName.cpp              # Main file with loop() and setup() function, rename it with the name of your project
    │   ├── Configuration.cpp.template  # Config impl, initialize global vars here like the one used for displays
    │   └── ...   
    ├── include                         # Project header folder
    │   ├── ChangeName.h                # Main header file, rename it with the name of your project
    │   ├── Secrets.h.template          # Configure WiFi/MQTT passwords 
    │   ├── Configuration.h.template    # Configure all the required info (ex: Wifi device name, DNS gateway, ecc.)
    │   └── ...       
    ├── core                            # Folder for core files, edit those files and contribute!
    │   ├── BootstrapManager.h          # Core header file with utility classes for bootstrapping
    │   ├── QueueManager.h              # Core header file with utility classes for Queue and MQTT management
    │   ├── WifiManager.h               # Core header file with utility classes for Wifi and OTA upload management
    │   ├── Helpers.h                   # Core header file with helper classes 
    │   └── ...       
    ├── platformio.ini                  # Edit platform with the one you are using (default: ESP8266, board: d1_mini)  
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
    
## Import those utilities to your existing project
To link this project to your existing one use git submodules.  
```
git submodule add git@github.com:sblantipodi/arduino_bootstrapper.git arduino_bootstrapper
```
Add extra dirs to your `platformio.ini`
```ini
lib_extra_dirs = arduino_bootstrapper
```
Copy and configure `~/arduino_bootstrapper/include/Secrets.h.template` into your `include/Secrets.h` folder  
Copy and configure `~/arduino_bootstrapper/include/Configuration.h.template` into your `include/Configuration.h` folder  
Copy and configure `~/arduino_bootstrapper/src/Configuration.cpp.template` into your `src/Configuration.cpp` folder 
  
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
Arduino Bootstrapper will search for a `Secrets.h` file inside the `include` folder,  
if you don't configure it, access point is started. You can connect to the AP with your mobile and go to http://192.168.4.1 to access the gui  
that will let you enter all the passwords without the needs of hardcoding them.
 
![Access Point Frontend](https://raw.githubusercontent.com/sblantipodi/arduino_bootstrapper/master/data/img/arduinobootstrapper.png)

## License
This program is licensed under MIT License
