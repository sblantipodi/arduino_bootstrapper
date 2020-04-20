#ifndef _DPSOFTWARE_HELPERS_UTILS_H
#define _DPSOFTWARE_HELPERS_UTILS_H

#include "../../include/Configuration.h"

extern String lastMQTTConnection;
extern String lastWIFiConnection;

const int DELAY_10 = 10;
const int DELAY_50 = 50;
const int DELAY_200 = 200;
const int DELAY_500 = 500;
const int DELAY_1000 = 1000;
const int DELAY_1500 = 1500;
const int DELAY_3000 = 3000;
const int DELAY_2000 = 2000;
const int DELAY_4000 = 4000;
const int DELAY_5000 = 5000;

const char* const ON_CMD = "ON";
const char* const OFF_CMD = "OFF";

class Helpers {

  public:
    void smartPrint(String msg);
    void smartPrintln(String msg);
    void smartPrint(int msg);
    void smartPrintln(int msg);
    void smartDisplay();

};

#endif

