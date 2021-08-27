/*
  WifiManager.cpp - Managing WiFi and OTA
  
  Copyright (C) 2020 - 2021  Davide Perini
  
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

#include "WifiManager.h"

//Establishing Local server at port 80 whenever required
#if defined(ESP8266)
  ESP8266WebServer server(80);
#elif defined(ESP32)
  WebServer server(80);
#endif  
// WiFiClient
WiFiClient espClient;
// WebServer content
String content;
// WebServer status code
int statusCode;
// WebServer HTML frontend
String htmlString;


/********************************** SETUP WIFI *****************************************/
void WifiManager::setupWiFi(void (*manageDisconnections)(), void (*manageHardwareButton)()) {
  
  wifiReconnectAttemp = 0;

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
  helper.smartPrint(qsid); helper.smartPrintln(F("..."));
  helper.smartDisplay();
 
  delay(DELAY_2000);

  WiFi.persistent(false);   // Solve possible wifi init errors (re-add at 6.2.1.16 #4044, #4083)
  WiFi.disconnect(true);    // Delete SDK wifi config
  delay(DELAY_200);

  WiFi.mode(WIFI_STA);      // Disable AP mode
#if defined(ESP8266)
  //WiFi.setSleepMode(WIFI_NONE_SLEEP);
#endif
  WiFi.setAutoConnect(true);
  if (!microcontrollerIP.equals("DHCP")) {
    WiFi.config(IPAddress(helper.getValue(microcontrollerIP, '.', 0).toInt(),
                          helper.getValue(microcontrollerIP, '.', 1).toInt(),
                          helper.getValue(microcontrollerIP, '.', 2).toInt(),
                          helper.getValue(microcontrollerIP, '.', 3).toInt()),
                IPAddress(helper.getValue(IP_GATEWAY, '.', 0).toInt(),
                          helper.getValue(IP_GATEWAY, '.', 1).toInt(),
                          helper.getValue(IP_GATEWAY, '.', 2).toInt(),
                          helper.getValue(IP_GATEWAY, '.', 3).toInt()),
                IPAddress(helper.getValue(IP_DNS, '.', 0).toInt(),
                          helper.getValue(IP_DNS, '.', 1).toInt(),
                          helper.getValue(IP_DNS, '.', 2).toInt(),
                          helper.getValue(IP_DNS, '.', 3).toInt()));
  } else {
    Serial.println("Using DHCP");
  }
  #if defined(ESP8266)
    WiFi.hostname(helper.string2char(deviceName));
    // Set wifi power in dbm range 0/0.25, set to 0 to reduce PIR false positive due to wifi power, 0 low, 20.5 max.
    WiFi.setOutputPower(WIFI_POWER);
    if (microcontrollerIP.equals("DHCP")) {
      WiFi.config(0U, 0U,0U);
    }
    WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &event) {
      Serial.println(F("WiFi is lost while connecting to MQTT, disconnecting. handlaer"));
      WiFi.disconnect();
    });
  #elif defined(ESP32)
    WiFi.setHostname(helper.string2char(deviceName));
    btStop();
  #endif
  // Start wifi connection
  WiFi.begin(qsid.c_str(), qpass.c_str());
  #if defined(ESP32)
    WiFi.setSleep(false);
  #endif
  // loop here until connection
  while (WiFi.status() != WL_CONNECTED) {

    manageHardwareButton();   
    delay(DELAY_500);
    Serial.print(F("."));
    wifiReconnectAttemp++;
    if (wifiReconnectAttemp > 10) {
      // if fastDisconnectionManagement we need to execute the callback immediately, 
      // example: power off a watering system can't wait MAX_RECONNECT attemps
      if (fastDisconnectionManagement) {
        manageDisconnections();
      }
      if (PRINT_TO_DISPLAY) {
        display.setCursor(0,0);
        display.clearDisplay();
      }
      helper.smartPrint(F("Wifi attemps= "));
      helper.smartPrintln(wifiReconnectAttemp);
      if (wifiReconnectAttemp >= MAX_RECONNECT) {
        helper.smartPrintln(F("Max retry reached, powering off peripherals."));
        manageDisconnections();
      }
      helper.smartDisplay();
    } else if (wifiReconnectAttemp > 10000) {
      wifiReconnectAttemp = 0;
    }

  }

  helper.smartPrintln(F("WIFI CONNECTED"));
  microcontrollerIP = WiFi.localIP().toString();
  MAC = WiFi.macAddress();
  helper.smartPrintln(microcontrollerIP);

  delay(DELAY_1500);

  // reset the lastWIFiConnection to off, will be initialized by next time update
  lastWIFiConnection = OFF_CMD;

}

/********************************** SETUP OTA *****************************************/
void WifiManager::setupOTAUpload() {

  //OTA SETUP
  ArduinoOTA.setPort(OTA_PORT);
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(helper.string2char(deviceName));

  // No authentication by default
  ArduinoOTA.setPassword((const char *)helper.string2char(OTApass));

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

/*
   Return the quality (Received Signal Strength Indicator) of the WiFi network.
   Returns a number between 0 and 100 if WiFi is connected.
   Returns -1 if WiFi is disconnected.
*/
int WifiManager::getQuality() {
  if (WiFi.status() != WL_CONNECTED) {
    return -1;
  }    
  int dBm = WiFi.RSSI();
  if (dBm <= -100) {
    return 0;
  }    
  if (dBm >= -50) {
    return 100;
  }    
  return 2 * (dBm + 100);
}

// check if wifi is correctly configured
bool WifiManager::isWifiConfigured() {

  if (strcmp(SSID, "XXX") != 0) {
    return true;
  }
  return false;

}

// if no ssid available, launch web server to get config params via browser
void WifiManager::launchWebServerForOTAConfig() {
  
  WiFi.disconnect();
  Serial.println("Turning HotSpot On");
  
  setupAP();
  launchWeb();

  while ((WiFi.status() != WL_CONNECTED)) {
    Serial.print(".");
    delay(100);
    server.handleClient();    
  }
  
}

void WifiManager::launchWeb() {

  Serial.println("");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
  }    
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  server.begin();
  Serial.println("Server started");

}

void WifiManager::setupAP(void) {

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(DELAY_200);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      #if defined(ESP8266)
        Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      #elif defined(ESP32)
        Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      #endif  
      delay(10);
    }
  }
  Serial.println("");
  htmlString = "<table id='wifi'><tr><th>SSID</th><th>RSSI</th><th>Enctipted</th></tr>";
  for (int i = 0; i < n; ++i) {
    htmlString += "<tr>";
    htmlString += "<td>"; 
    htmlString += WiFi.SSID(i);
    htmlString += "</td>";
    htmlString += "<td>";
    htmlString += WiFi.RSSI(i);
    htmlString += "</td>";
    htmlString += "<td>";
    #if defined(ESP8266)
      htmlString += ((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "PUBLIC" : "ENCRYPTED");
    #elif defined(ESP32)
      htmlString += ((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "PUBLIC" : "ENCRYPTED");
    #endif 
    htmlString += "</td>";
    htmlString += "</tr>";
  }
  htmlString += "</table>";
  delay(100);
  WiFi.softAP(WIFI_DEVICE_NAME, "");
  launchWeb();

}

void WifiManager::createWebServer() {
  {
    server.on("/", []() {
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html><head><style>body {padding:0% 5% 0% 5%;font-size:4vw;width: 90%;font-weight:bold;text-align:center; color:#202020}#centerContainer { margin: 0px auto; }input {font-size:4vw;width: 100%;padding: 12px 20px;margin: 8px 0;box-sizing: border-box;border: 6px solid orange;-webkit-transition: 0.5s;transition: 0.5s;outline: none;}input:focus {border: 12px solid #BF5F00;font-weight:bold;}#wifi {font-family: 'Trebuchet MS', Arial, Helvetica, sans-serif;border-collapse: collapse;width: 100%;}#wifi td, #wifi th {border: 1px solid #ddd;padding: 8px;}#wifi tr:nth-child(even){background-color: #f2f2f2;}#wifi tr:hover {background-color: #ddd;}#wifi th {padding-top: 12px;padding-bottom: 12px;text-align: left; background-color: orange;color: white;}.button {background-color: orange;border: none;color: white;padding: 20px;text-align: center;text-decoration: none;display: inline-block;font-size: 16px;margin: 4px 2px;cursor: pointer;}.button3 {font-size:4vw;border-radius: 8px;width:100%;font-weight:bold;}label{font-size:4vw;}</style></head><body><div id='centerContainer'>";
      content += "<h1>";
      content += WIFI_DEVICE_NAME;
      content += "</h1>";
      content += htmlString;
      content += "<br><br><form method='get' action='setting' id='form1'>";
      content += "<label for='deviceName'>Device Name *</label><input type='text' id='deviceName' name='deviceName' maxlength='25' required>";
      content += "<label for='microcontrollerIP'>IP ADDRESS</label><input type='text' id='microcontrollerIP' name='microcontrollerIP'>";
      content += "<label for='ssid'>SSID *</label><input type='text' id='ssid' name='ssid' required>";
      content += "<label for='pass'>WIFI PASSWORD *</label><input type='password' id='pass' name='pass' required>";
      content += "<label for='OTApass'>OTA PASSWORD *</label><input type='password' id='OTApass' name='OTApass' required>";
      content += "<label for='mqttIP'>MQTT SERVER IP *</label><input type='text' id='mqttIP' name='mqttIP' required>";
      content += "<label for='mqttPort'>MQTT SERVER PORT *</label><input type='text' id='mqttPort' name='mqttPort' required>";
      content += "<label for='mqttuser'>MQTT SERVER USERNAME</label><input type='text' id='mqttuser' name='mqttuser'>";
      content += "<label for='mqttpass'>MQTT SERVER PASSWORD</label><input type='password' id='mqttpass' name='mqttpass'>";
      content += "<label for='additionalParam'>"; content += ADDITIONAL_PARAM_TEXT; content += "</label><input type='text' id='additionalParam' name='additionalParam'>";
      content += "</form><br><br><button type='submit' form='form1' value='Submit' class='button button3'>STORE CONFIG</button><br><br><p>* Please insert the required fields, please double check them before submit or you will need to reflash.</p><br></div></body>";
      content += "</html>";
      server.send(200, "text/html", content);
    });

    server.on("/setting", []() {
      String deviceName = server.arg("deviceName");
      String microcontrollerIP = server.arg("microcontrollerIP");
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      String mqttIP = server.arg("mqttIP");
      String mqttPort = server.arg("mqttPort");
      String OTApass = server.arg("OTApass");
      String mqttuser = server.arg("mqttuser");
      String mqttpass = server.arg("mqttpass");
      String additionalParam = server.arg("additionalParam");

      if (deviceName.length() > 0 && qsid.length() > 0 && qpass.length() > 0 && OTApass.length() > 0 && mqttIP.length() > 0 && mqttPort.length() > 0) {

        Serial.println("deviceName");
        Serial.println(deviceName);
        Serial.println("microcontrollerIP");
        if (microcontrollerIP.length() == 0) {
          microcontrollerIP = "DHCP";
        }
        Serial.println(microcontrollerIP);
        Serial.println("qsid");
        Serial.println(qsid);
        Serial.println("qpass");
        Serial.println(qpass);
        Serial.println("OTApass");
        Serial.println(OTApass);
        Serial.println("mqttIP");
        Serial.println(mqttIP);
        Serial.println("mqttPort");
        Serial.println(mqttPort);
        Serial.println("mqttuser");
        Serial.println(mqttuser);
        Serial.println("mqttpass");
        Serial.println(mqttpass);
        Serial.println("additionalParam");
        Serial.println(additionalParam);

        DynamicJsonDocument doc(1024);
        doc["deviceName"] = deviceName;
        doc["microcontrollerIP"] = microcontrollerIP;
        doc["qsid"] = qsid;
        doc["qpass"] = qpass;     
        doc["OTApass"] = OTApass;
        doc["mqttIP"] = mqttIP;
        doc["mqttPort"] = mqttPort;
        doc["mqttuser"] = mqttuser;
        doc["mqttpass"] = mqttpass;
        doc["additionalParam"] = additionalParam;

        #if defined(ESP8266)
          // Write to LittleFS
          Serial.println(F("\nSaving setup.json\n"));
          File jsonFile = LittleFS.open("/setup.json", "w");
          if (!jsonFile) {
            Serial.println("Failed to open [setup.json] file for writing");
            content = "Error: can't write to storage.";
            statusCode = 404;
          } else {
            serializeJsonPretty(doc, Serial);
            serializeJson(doc, jsonFile);
            jsonFile.close();
            Serial.println("[setup.json] written correctly");
          }
          delay(DELAY_200);
          content = "Success: rebooting the microcontroller using your credentials.";
          statusCode = 200;
        #elif defined(ESP32)
        SPIFFS.format();
        if (SPIFFS.begin()) {
            File configFile = SPIFFS.open("/setup.json", "w");
            if (!configFile) {
              Serial.println("Failed to open [setup.json] file for writing");
            }
            serializeJsonPretty(doc, Serial);
            serializeJson(doc, configFile);
            configFile.close();
            Serial.println("[setup.json] written correctly");
            content = "Success: rebooting the microcontroller using your credentials.";
            statusCode = 200;
          } else {
            Serial.println(F("Failed to mount FS for write"));
            content = "Error: can't write to storage.";
            statusCode = 404;
          }
        #endif
      } else {
        content = "Error: missing required fields.";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      delay(DELAY_500);
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "text/plain", content);
      delay(DELAY_500);
      #if defined(ESP8266)
      ESP.reset();
      #elif defined(ESP32)
      ESP.restart();
      #endif

    });

  } 
}
