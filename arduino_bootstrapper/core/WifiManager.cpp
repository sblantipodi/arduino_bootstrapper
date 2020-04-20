#include "WifiManager.h"


/********************************** SETUP WIFI *****************************************/
void WifiManager::setupWiFi(void (*manageDisconnectionFunction)(), void (*manageHardwareButton)()) {
  
  // Helpers classes
  Helpers helper;

  int reconnectAttemp = 0;

  // DPsoftware domotics 
  if (PRINT_TO_DISPLAY) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(5,17);
    display.drawRoundRect(0, 0, display.width()-1, display.height()-1, display.height()/4, WHITE);    
  }
  helper.smartPrintln(F("DPsoftware domotics"));
  helper.smartDisplay();
  delay(DELAY_3000);

  if (PRINT_TO_DISPLAY) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
  }
  helper.smartPrintln(F("Connecting to: "));
  helper.smartPrint(ssid); helper.smartPrintln(F("..."));
  helper.smartDisplay();
 
  delay(DELAY_2000);

  WiFi.persistent(false);   // Solve possible wifi init errors (re-add at 6.2.1.16 #4044, #4083)
  WiFi.disconnect(true);    // Delete SDK wifi config
  delay(DELAY_200);

  WiFi.mode(WIFI_STA);      // Disable AP mode
  //WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.setAutoConnect(true);  
  WiFi.config(WifiConfig::arduinoip, WifiConfig::mydns, WifiConfig::mygateway);
  WiFi.hostname(WIFI_DEVICE_NAME);

  // Set wifi power in dbm range 0/0.25, set to 0 to reduce PIR false positive due to wifi power, 0 low, 20.5 max.
  WiFi.setOutputPower(WIFI_POWER);
  
  // Start wifi connection
  WiFi.begin(ssid, password);

  // loop here until connection
  while (WiFi.status() != WL_CONNECTED) {

    manageHardwareButton();  
    if (reconnectAttemp >= MAX_RECONNECT) {
      manageDisconnectionFunction();
    }       

    delay(DELAY_500);
    Serial.print(F("."));
    reconnectAttemp++;
    if (reconnectAttemp > 10) {
      if (PRINT_TO_DISPLAY) {
        display.setCursor(0,0);
        display.clearDisplay();
      }
      helper.smartPrint(F("Wifi attemps= "));
      helper.smartPrint(reconnectAttemp);
      if (reconnectAttemp >= MAX_RECONNECT) {
        helper.smartPrintln(F("Max retry reached, powering off peripherals."));
      }
      helper.smartDisplay();
    } else if (reconnectAttemp > 10000) {
      reconnectAttemp = 0;
    }

  }

  helper.smartPrintln(F("WIFI CONNECTED"));
  helper.smartPrintln(WiFi.localIP());

  delay(DELAY_1500);

  // reset the lastWIFiConnection to off, will be initialized by next time update
  lastWIFiConnection = OFF_CMD;

}

/********************************** SETUP OTA *****************************************/
void WifiManager::setupOTAUpload() {

  //OTA SETUP
  ArduinoOTA.setPort(OTA_PORT);
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(WIFI_DEVICE_NAME);

  // No authentication by default
  ArduinoOTA.setPassword((const char *)OTApassword);

  ArduinoOTA.onStart([]() {
    Serial.println(F("Starting"));
  });
  ArduinoOTA.onEnd([]() {
    Serial.println(F("\nEnd"));
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
    else if (error == OTA_END_ERROR) Serial.println(F("End Failed"));
  });
  ArduinoOTA.begin();

}