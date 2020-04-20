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
- Start a project from scratch
- Import this utilities to your existing project


### Import this utilities to your existing project
To link this project to your existing one, you can use a git submodules.  
```
git submodule add git@github.com:sblantipodi/arduino_bootstrapper.git arduino_bootstrapper
```

### Start a project from scratch
Clone the bootstrapper
```
git clone git@github.com:sblantipodi/arduino_bootstrapper.git
```
Rename main files with the name of your projects.

    .
    ├── ...
    ├── src                    # Test files (alternatively `spec` or `tests`)
    │   ├── ChangeName.cpp     # Main file with loop() and setup() function
    │   └── unit               # Unit tests
    └── ...
    
## License
This program is licensed under MIT License



git submodule add git@github.com:sblantipodi/arduino_bootstrapper.git arduino_bootstrapper

lib_extra_dirs = arduino_bootstrapper

abilitarte i sumblink
export MSYS=winsymlinks:nativestrict
