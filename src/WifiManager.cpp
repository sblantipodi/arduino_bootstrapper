/*
  WifiManager.cpp - Managing WiFi and OTA

  Copyright © 2020 - 2026  Davide Perini

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
#elif defined(ARDUINO_ARCH_ESP32)
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
byte improvActive; //0: no improv packet received, 1: improv active, 2: provisioning
byte improvError;
char serverDescription[33] = "Luciferin";
char cmDNS[33] = "x";
char clientSSID[33];
char clientPass[65];
unsigned long previousMillisEsp32Reconnect = 0;
unsigned long intervalEsp32Reconnect = 15000;

/********************************** SETUP WIFI *****************************************/
void WifiManager::setupWiFi(void (*manageDisconnections)(), void (*manageHardwareButton)()) {
  wifiReconnectAttemp = 0;
  // DPsoftware domotics
#if (DISPLAY_ENABLED)
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(5,17);
  display.drawRoundRect(0, 0, display.width()-1, display.height()-1, display.height()/4, WHITE);
#endif
  Helpers::smartPrintln(F("DPsoftware domotics"));
  helper.smartDisplay(DELAY_3000);
#if (DISPLAY_ENABLED)
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
#endif
  Helpers::smartPrintln(F("Connecting to: "));
  Helpers::smartPrint(qsid);
  Helpers::smartPrintln(F("..."));
  helper.smartDisplay(DELAY_2000);
  WiFi.persistent(true);   // Solve possible wifi init errors (re-add at 6.2.1.16 #4044, #4083)
  WiFi.disconnect(true);    // Delete SDK wifi config
  delay(DELAY_200);
  WiFi.mode(WIFI_STA);      // Disable AP mode
#if defined(ESP8266)
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
#endif
//  WiFi.setAutoConnect(true); // TODO
  WiFi.setAutoReconnect(true);
  Serial.println(microcontrollerIP);
  if (!microcontrollerIP.equals("DHCP")) {
    WiFi.config(IPAddress(Helpers::getValue(microcontrollerIP, '.', 0).toInt(),
                          Helpers::getValue(microcontrollerIP, '.', 1).toInt(),
                          Helpers::getValue(microcontrollerIP, '.', 2).toInt(),
                          Helpers::getValue(microcontrollerIP, '.', 3).toInt()),
                IPAddress(Helpers::getValue(IP_GATEWAY, '.', 0).toInt(),
                          Helpers::getValue(IP_GATEWAY, '.', 1).toInt(),
                          Helpers::getValue(IP_GATEWAY, '.', 2).toInt(),
                          Helpers::getValue(IP_GATEWAY, '.', 3).toInt()),
                IPAddress(Helpers::getValue(IP_SUBNET, '.', 0).toInt(),
                          Helpers::getValue(IP_SUBNET, '.', 1).toInt(),
                          Helpers::getValue(IP_SUBNET, '.', 2).toInt(),
                          Helpers::getValue(IP_SUBNET, '.', 3).toInt()),
                IPAddress(Helpers::getValue(IP_DNS, '.', 0).toInt(),
                          Helpers::getValue(IP_DNS, '.', 1).toInt(),
                          Helpers::getValue(IP_DNS, '.', 2).toInt(),
                          Helpers::getValue(IP_DNS, '.', 3).toInt()));
    Serial.println(F("Using static IP address"));
    dhcpInUse = false;
  } else {
    Serial.println(F("Using DHCP"));
    dhcpInUse = true;
  }
#if defined(ESP8266)
  WiFi.hostname(Helpers::string2char(deviceName));
  // Set wifi power in dbm range 0/0.25, set to 0 to reduce PIR false positive due to wifi power, 0 low, 20.5 max.
  WiFi.setOutputPower(WIFI_POWER);
  if (microcontrollerIP.equals("DHCP")) {
    WiFi.config(0U, 0U, 0U);
  }
  WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &event) {
      Serial.println(F("WiFi connection is lost."));
      WiFi.reconnect();
  });
#elif defined(ARDUINO_ARCH_ESP32)
  WiFi.setHostname(helper.string2char(deviceName));
#if !CONFIG_IDF_TARGET_ESP32S2
  btStop();
#endif
#endif
  // Start wifi connection
  WiFi.begin(qsid.c_str(), qpass.c_str());
#if defined(ARDUINO_ARCH_ESP32)
  setTxPower();
  WiFi.setSleep(false);
#endif
  reconnectToWiFi(manageDisconnections, manageHardwareButton);
  MAC = WiFi.macAddress();
  delay(DELAY_1500);
  // reset the lastWIFiConnection to off, will be initialized by next time update
  lastWIFiConnection = OFF_CMD;
}

/**
 * Set the TX power based on dBm values.
 */
#if defined(ARDUINO_ARCH_ESP32)
void WifiManager::setTxPower() const {
  wifi_power_t val = WIFI_POWER_MINUS_1dBm;
  if (WIFI_POWER < 0) {
    val = WIFI_POWER_MINUS_1dBm;
  } else if (WIFI_POWER < 3.5f) {
    val = WIFI_POWER_2dBm;
  } else if (WIFI_POWER < 6) {
    val = WIFI_POWER_5dBm;
  } else if (WIFI_POWER < 8) {
    val = WIFI_POWER_7dBm;
  } else if (WIFI_POWER < 10) {
    val = WIFI_POWER_8_5dBm;
  } else if (WIFI_POWER < 12) {
    val = WIFI_POWER_11dBm;
  } else if (WIFI_POWER < 14) {
    val = WIFI_POWER_13dBm;
  } else if (WIFI_POWER < 16) {
    val = WIFI_POWER_15dBm;
  } else if (WIFI_POWER < 17.75f) {
    val = WIFI_POWER_17dBm;
  } else if (WIFI_POWER < 18.75f) {
    val = WIFI_POWER_18_5dBm;
  } else if (WIFI_POWER < 19.25f) {
    val = WIFI_POWER_19dBm;
  } else {
    val = WIFI_POWER_19_5dBm;
  }
  WiFi.setTxPower(val);
}
#endif

void WifiManager::reconnectToWiFi(void (*manageDisconnections)(), void (*manageHardwareButton)()) {
  wifiReconnectAttemp = 0;
  // loop here until connection
  while ((WiFi.status() != WL_CONNECTED && !ethConnected) && Serial.peek() == -1) {
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
#if (DISPLAY_ENABLED)
      display.setCursor(0,0);
      display.clearDisplay();
      display.setTextSize(1);
#endif
#if defined(ESP8266)
      ESP.wdtFeed();
#else
      esp_task_wdt_reset();
#endif
      Helpers::smartPrint(F("Wifi attemps= "));
      Helpers::smartPrintln(wifiReconnectAttemp);
#if defined(ARDUINO_ARCH_ESP32)
      // Arduino 2.x for ESP32 seems to not support callback, polling to reconnect.
      unsigned long currentMillisEsp32Reconnect = millis();
      if (currentMillisEsp32Reconnect - previousMillisEsp32Reconnect >= intervalEsp32Reconnect) {
        WiFi.disconnect();
        WiFi.begin(qsid.c_str(), qpass.c_str());
        setTxPower();
        WiFi.setSleep(false);
        previousMillisEsp32Reconnect = currentMillisEsp32Reconnect;
      }
#endif
      if (wifiReconnectAttemp >= MAX_RECONNECT) {
        Helpers::smartPrintln(F("Max retry reached, powering off peripherals."));
        manageDisconnections();
      }
      helper.smartDisplay();
    } else if (wifiReconnectAttemp > 10000) {
      wifiReconnectAttemp = 0;
    }
  }
  if (currentWiFiIp != WiFi.localIP()) {
    Helpers::smartPrint(F("\nWIFI CONNECTED\nIP Address: "));
    if (!ethConnected) {
      microcontrollerIP = WiFi.localIP().toString();
    }
    currentWiFiIp = WiFi.localIP();
    Helpers::smartPrintln(WiFi.localIP().toString());
    Helpers::smartPrint(F("nb of attempts: "));
    Helpers::smartPrintln(wifiReconnectAttemp);
  }
}

/********************************** SETUP OTA *****************************************/
void WifiManager::setupOTAUpload() {
  //OTA SETUP
  ArduinoOTA.setPort(OTA_PORT);
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(Helpers::string2char(deviceName));
  // No authentication by default
  ArduinoOTA.setPassword((const char *) Helpers::string2char(OTApass));
  ArduinoOTA.onStart([]() {
      Serial.println(F("Starting"));
  });
  ArduinoOTA.onEnd([]() {
      Serial.println(F("End"));
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
#if defined(ARDUINO_ARCH_ESP32)
    esp_task_wdt_reset();
# endif
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
  Serial.println(F("Turning HotSpot On"));
  setupAP();
  launchWeb();
  while (WiFi.status() != WL_CONNECTED && Serial.peek() == -1) {
    delay(10);
    server.handleClient();
  }
}

void WifiManager::launchWebServerCustom(void (*listener)()) {
  WiFi.disconnect();
  Serial.println(F("Turning HotSpot On"));
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(DELAY_200);
  WiFi.softAP(WIFI_DEVICE_NAME, "");
  listener();
  server.begin();
  while (WiFi.status() != WL_CONNECTED && Serial.peek() == -1) {
    delay(10);
    server.handleClient();
  }
}

// Manage improv wifi
void WifiManager::manageImprovWifi() {
  handleImprovPacket();
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
#elif defined(ARDUINO_ARCH_ESP32)
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
#elif defined(ARDUINO_ARCH_ESP32)
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
        content = "<!DOCTYPE HTML>\n"
                  "<html><header>\n"
                  "    <div>\n"
                  "        <a href='https://github.com/sblantipodi/'>";
        content += WIFI_DEVICE_NAME;
        content += "    </a></div>\n"
                   "    <style>\n"
                   "        body{background: #b7b7b7; margin:0; padding:0; font-family:'Open Sans',sans-serif}"
                   "        input[type='checkbox'] {\n"
                   "            width: auto !important;\n"
                   "            margin: 1rem;\n"
                   "        }\n"
                   "        input[type='checkbox'] {\n"
                   "            height: 3.5rem;\n"
                   "            width: 3.5rem;\n"
                   "            position: relative;\n"
                   "            -webkit-appearance: none;\n"
                   "            cursor: pointer;\n"
                   "            margin-right: 10px;\n"
                   "        }\n"
                   "        input[type='checkbox']:before {\n"
                   "            content: '';\n"
                   "            display: inline-block;\n"
                   "            position: absolute;\n"
                   "            box-sizing: border-box;\n"
                   "            height: 3.5rem;\n"
                   "            width: 3.5rem;\n"
                   "            border-radius: 0.1rem;\n"
                   "            border: 0.3rem solid orange;\n"
                   "            background-color: #ffdca7;\n"
                   "        }\n"
                   "        input[type='checkbox']:hover::before {\n"
                   "            border: 0.1rem solid darkorange;\n"
                   "        }\n"
                   "        input[type='checkbox']:checked:before {\n"
                   "            content: '';\n"
                   "            display: inline-block;\n"
                   "            position: absolute;\n"
                   "            box-sizing: border-box;\n"
                   "            height: 3.5rem;\n"
                   "            width: 3.5rem;\n"
                   "            border-radius: 0.1rem;\n"
                   "            border: 1rem solid #ffdca7;\n"
                   "            background-color: #ffdca7;\n"
                   "        }\n"
                   "        input[type='checkbox']:checked:after {\n"
                   "            content: '';\n"
                   "            display: inline-block;\n"
                   "            position: absolute;\n"
                   "            top: 0.6rem;"
                   "            left: -0.1rem;"
                   "            box-sizing: border-box;"
                   "            height: 2.5rem;"
                   "            width: 4.5rem;"
                   "            border-left: 0.6rem solid orange;\n"
                   "            border-bottom: 0.6rem solid orange;\n"
                   "            -webkit-transform: translateY(-1.5px) rotate(-45deg);\n"
                   "            transform: translateY(-1.5px) rotate(-45deg);\n"
                   "        }\n"
                   "        ::-webkit-input-placeholder{color: orange;}\n"
                   "        .buttonSubmit {border-radius: 0.5rem !important;} label {font-size: 2.5rem; color: orange;display: inline-block;margin-top: 20px;}\n"
                   "        header,footer{background: #808080; width:100%; text-align:center}  header{padding-top:100px}  footer{padding-bottom:30px}\n"
                   "        header>div,footer>div{background: #ff6500; color:#FFF; width:90%; margin:0  auto; padding:22px; box-sizing:border-box}\n"
                   "        header>div>a{font-size:3rem; color:#FFF; text-decoration:none}\n"
                   "        .form{background:#fff; width:90%; margin:0 auto; padding:30px; box-sizing:border-box}\n"
                   "        .form input{font-size:3rem; display:block; width:100%; border:none; border-bottom:solid 1px #ccc; color: orange; padding:10px 10px 10px 0; box-sizing:border-box; outline:none; font-family:'Open Sans',sans-serif;}\n"
                   "        .line{display:block; background: #ffa500; height:3px; margin:0 auto; margin-top:-2px; width:0px; transition:width  .3s cubic-bezier(0.4,0,0.2,1)}\n"
                   "        .form input:focus + span.line{width:100%}\n"
                   "        #send{width:85%; margin:0 auto; margin-top:30px; background: #ffa500; border-bottom:none; color:#fff; cursor:pointer}\n"
                   "        .social i{height:40px; width:40px; border-radius:40px; text-align:center; line-height:40px; color:#fff}\n"
                   "       .pwd{display:block; color:#F93; text-decoration:none; width:280px; margin:20px auto; padding:10px 0}\n"
                   "        @media(max-width:100%){header>div,footer>div,.form{width:90%} header>div>a{font-size:3rem; padding:0.5rem 0; display:block}}\n"
                   "        #wifi {font-family: 'Trebuchet MS', Arial, Helvetica, sans-serif;border-collapse: collapse;width: 100%;}#wifi td, #wifi th {border: 1px solid #ddd;padding: 0.5rem;}#wifi tr:nth-child(even){background-color: #f2f2f2;}#wifi tr:hover {background-color: #ddd;}#wifi th {padding-top: 0.6rem;padding-bottom: 0.6rem;text-align: left; background-color: orange;color: white;}\n"
                   "    </style>\n"
                   "</header>\n"
                   "<div class='form'>\n";
        content += htmlString;
        content += "    <form method='get' action='setting' id='form1'>\n"
                   "        <label for='deviceName'>Device Name *</label><input type='text' id='deviceName' name='deviceName' maxlength='25' required><span class='line'></span>\n"
                   "        <label for='microcontrollerIP'>IP address</label><input type='text' id='microcontrollerIP' name='microcontrollerIP'><span class='line'></span>\n"
                   "        <label for='ssid'>SSID *</label><input type='text' id='ssid' name='ssid' required>"
                   "        <label for='pass'>WiFi password *</label><input type='password' id='pass' name='pass' required><span class='line'></span>\n"
                   "        <label for='OTApass'>OTA password *</label><input type='password' id='OTApass' name='OTApass' required><span class='line'></span>\n"
                   "        <label for='mqttCheckbox'>Enable MQTT</label><input type='checkbox' id='mqttCheckbox' name='mqttCheckbox' checked>"
                   "            <div id='mqttclass'><label id='labelMqttIp' for='mqttIP'>MQTT server IP *</label><input id='inputMqttIp' type='text' id='mqttIP' name='mqttIP' required><span class='line'></span>\n"
                   "        <label for='mqttPort'>MQTT server port *</label><input type='text' id='mqttPort' name='mqttPort' required><span class='line'></span>\n"
                   "        <label for='mqttuser'>MQTT server username</label><input type='text' id='mqttuser' name='mqttuser'><span class='line'></span>\n"
                   "        <label for='mqttpass'>MQTT server password</label><input type='password' id='mqttpass' name='mqttpass'><span class='line'></span></div>\n";
        content += "        <label for='additionalParam'><span class='line'></span>";
        content += ADDITIONAL_PARAM_TEXT;
        content += "</label><input type='text' id='additionalParam' name='additionalParam'>\n"
                   "        <input type='submit' class='buttonSubmit' value='STORE CONFIG' id='send'>\n"
                   "    </form>\n"
                   "</div>\n"
                   "<footer>\n"
                   "    <div class='social'>* Please insert the required fields, please double check them before submit or you will need to reflash.</div>\n"
                   "    <a class='pwd' href='https://github.com/sblantipodi/'>GitHub</a>\n"
                   "</footer>"
                   "<script>\n"
                   "    const cb = document.getElementById('mqttCheckbox');\n"
                   "    const x = document.getElementById('mqttclass');\n"
                   "    console.log(cb.checked);\n"
                   "    cb.onclick = () => {\n"
                   "        if (cb.checked) {\n"
                   "            x.style.display = 'block';\n"
                   "            document.getElementById('inputMqttIp').setAttribute('required','')\n"
                   "            document.getElementById('mqttPort').setAttribute('required','')\n"
                   "        } else {\n"
                   "            x.style.display = 'none';\n"
                   "            document.getElementById('inputMqttIp').removeAttribute('required')\n"
                   "            document.getElementById('mqttPort').removeAttribute('required')\n"
                   "            document.getElementById(\"mqttPort\").value = \"\";"
                   "            document.getElementById(\"mqttuser\").value = \"\";"
                   "            document.getElementById(\"mqttpass\").value = \"\";"
                   "            document.getElementById(\"inputMqttIp\").value = \"\";"
                   "        }\n"
                   "    };"
                   "</script></html>";
        server.send(200, "text/html", content);
    });

    server.on("/setting", []() {
        String deviceName = server.arg("deviceName");
        String microcontrollerIP = server.arg("microcontrollerIP");
        String qsid = server.arg("ssid");
        String qpass = server.arg("pass");
        String mqttCheckbox = server.arg("mqttCheckbox");
        String mqttIP = server.arg("mqttIP");
        String mqttPort = server.arg("mqttPort");
        String OTApass = server.arg("OTApass");
        String mqttuser = server.arg("mqttuser");
        String mqttpass = server.arg("mqttpass");
        String additionalParam = server.arg("additionalParam");
        JsonDocument doc;

        if (deviceName.length() > 0 && qsid.length() > 0 && qpass.length() > 0 && OTApass.length() > 0
            && ((mqttCheckbox.length() == 0) || (mqttIP.length() > 0 && mqttPort.length() > 0))) {

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

          doc["deviceName"] = deviceName;
          doc["microcontrollerIP"] = microcontrollerIP;
          doc["qsid"] = qsid;
          doc["qpass"] = qpass;
          doc["OTApass"] = OTApass;
          if (mqttCheckbox.equals("on")) {
            doc["mqttIP"] = mqttIP;
            doc["mqttPort"] = mqttPort;
            doc["mqttuser"] = mqttuser;
            doc["mqttpass"] = mqttpass;
          } else {
            doc["mqttIP"] = "";
            doc["mqttPort"] = "";
            doc["mqttuser"] = "";
            doc["mqttpass"] = "";
          }
          doc["additionalParam"] = additionalParam;
          content = "Success: rebooting the microcontroller using your credentials.";
          statusCode = 200;
        } else {
          content = "Error: missing required fields.";
          statusCode = 404;
          Serial.println("Sending 404");
        }
        delay(DELAY_500);
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(statusCode, "text/plain", content);
        delay(DELAY_500);

        // Write to LittleFS
        Serial.println(F("Saving setup.json"));
        File jsonFile = LittleFS.open("/setup.json", FILE_WRITE);
        if (!jsonFile) {
          Serial.println("Failed to open [setup.json] file for writing");
        } else {
          serializeJsonPretty(doc, Serial);
          serializeJson(doc, jsonFile);
          jsonFile.close();
          Serial.println("[setup.json] written correctly");
        }
        delay(DELAY_200);
#if defined(ARDUINO_ARCH_ESP32)
        ESP.restart();
#elif defined(ESP8266)
        EspClass::restart();
#endif
    });
  }
}

bool WifiManager::isConnected() {
  return (WiFi.localIP()[0] != 0 && WiFi.status() == WL_CONNECTED);
}

void WifiManager::sendImprovStateResponse(uint8_t state, bool error) {
  if (!error && improvError > 0 && improvError < 3) sendImprovStateResponse(0x00, true);
  if (error) improvError = state;
  char out[11] = {'I', 'M', 'P', 'R', 'O', 'V'};
  out[6] = IMPROV_VERSION;
  out[7] = error ? ImprovPacketType::Error_State : ImprovPacketType::Current_State;
  out[8] = 1;
  out[9] = state;
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < 10; i++) checksum += out[i];
  out[10] = checksum;
  Serial.write((uint8_t *) out, 11);
  Serial.write('\n');
  Serial.flush();
}

void WifiManager::sendImprovRPCResponse(byte commandId) {
  sendImprovRPCResponse(commandId, false);
}

void WifiManager::sendImprovRPCResponse(byte commandId, bool forceConnection) {
  if (improvError > 0 && improvError < 3) sendImprovStateResponse(0x00, true);
  uint8_t packetLen = 12;
  char out[64] = {'I', 'M', 'P', 'R', 'O', 'V'};
  out[6] = IMPROV_VERSION;
  out[7] = ImprovPacketType::RPC_Response;
  out[8] = 2; //Length (set below)
  out[9] = commandId;
  out[10] = 0; //Data len (set below)
  out[11] = '\0'; //URL len (set below)
  if (isConnected() || forceConnection) {
    IPAddress localIP = WiFi.localIP();
    uint8_t len = sprintf(out + 12, "http://%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);
    if (len > 24) return; //sprintf fail
    out[11] = len;
    out[10] = 1 + len;
    out[8] = 3 + len; //RPC command type + data len + url len + url
    packetLen = 13 + len;
  }
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < packetLen - 1; i++) checksum += out[i];
  out[packetLen - 1] = checksum;
  Serial.write((uint8_t *) out, packetLen);
  Serial.write('\n');
  Serial.flush();
  improvActive = 1; //no longer provisioning
}

void WifiManager::sendImprovInfoResponse() {
  if (improvError > 0 && improvError < 3) sendImprovStateResponse(0x00, true);
  uint8_t packetLen = 12;
  char out[128] = {'I', 'M', 'P', 'R', 'O', 'V'};
  out[6] = IMPROV_VERSION;
  out[7] = ImprovPacketType::RPC_Response;
  //out[8] = 2; //Length (set below)
  out[9] = ImprovRPCType::Request_Info;
  //out[10] = 0; //Data len (set below)
  out[11] = 4; //Firmware len ("FIRM")
  out[12] = 'F';
  out[13] = 'I';
  out[14] = 'R';
  out[15] = 'M';
  uint8_t lengthSum = 17;
  uint8_t vlen = sprintf_P(out + lengthSum, firmwareVersion.c_str());
  out[16] = vlen;
  lengthSum += vlen;
  uint8_t hlen = 7;
#ifdef ESP8266
  strcpy(out + lengthSum + 1, "esp8266");
#else
  hlen = 5;
  strcpy(out + lengthSum + 1, "esp32");
#endif
  out[lengthSum] = hlen;
  lengthSum += hlen + 1;
  //Use serverDescription if it has been changed from the default "FIRM", else mDNS name
  bool useMdnsName = (strcmp(serverDescription, "FIRM") == 0 && strlen(cmDNS) > 0);
  strcpy(out + lengthSum + 1, useMdnsName ? cmDNS : serverDescription);
  uint8_t nlen = strlen(useMdnsName ? cmDNS : serverDescription);
  out[lengthSum] = nlen;
  lengthSum += nlen + 1;
  packetLen = lengthSum + 1;
  out[8] = lengthSum - 9;
  out[10] = lengthSum - 11;
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < packetLen - 1; i++) checksum += out[i];
  out[packetLen - 1] = checksum;
  Serial.write((uint8_t *) out, packetLen);
  Serial.write('\n');
  Serial.flush();
  DIMPROV_PRINT("Info checksum");
  DIMPROV_PRINTLN(checksum);
}

void WifiManager::parseWiFiCommand(char *rpcData) {
  uint8_t len = rpcData[0];
  if (!len || len > 126) return;
  uint8_t ssidLen = rpcData[1];
  if (ssidLen > len - 1 || ssidLen > 32) return;
  memset(clientSSID, 0, 32);
  memcpy(clientSSID, rpcData + 2, ssidLen);
  memset(clientPass, 0, 64);
  if (len > ssidLen + 1) {
    uint8_t passLen = rpcData[2 + ssidLen];
    memset(clientPass, 0, 64);
    memcpy(clientPass, rpcData + 3 + ssidLen, passLen);
  }
  sendImprovStateResponse(0x03, false); //provisioning
  improvActive = 2;
  JsonDocument doc;
  bool connected = isConnected();
  String devName = String(random(0, 90000));
  doc["deviceName"] = String(DEVICE_NAME) + "_" + devName;
  doc["microcontrollerIP"] = "DHCP";
  doc["qsid"] = clientSSID;
  doc["qpass"] = clientPass;
  doc["OTApass"] = "";
  doc["mqttIP"] = "";
  doc["mqttPort"] = "";
  doc["mqttuser"] = "";
  doc["mqttpass"] = "";
  additionalParam = "2";
  File jsonFile = LittleFS.open("/setup.json", FILE_WRITE);
  if (!jsonFile) {
    Serial.println("Failed to open [setup.json] file for writing");
  } else {
    serializeJsonPretty(doc, Serial);
    serializeJson(doc, jsonFile);
    jsonFile.close();
    if (connected) {
      IPAddress localIP = WiFi.localIP();
      Serial.printf("IMPROV http://%d.%d.%d.%d\n", localIP[0], localIP[1], localIP[2], localIP[3]);
    }
  }
#if defined(ARDUINO_ARCH_ESP32)
  WiFi.disconnect();
  WiFi.begin(clientSSID, clientPass);
  while (!isConnected()) {}
#endif
  sendImprovRPCResponse(ImprovRPCType::Request_State);
  sendImprovStateResponse(0x04, false);
  Serial.flush();
#if defined(ARDUINO_ARCH_ESP32)
  ESP.restart();
#elif defined(ESP8266)
  EspClass::restart();
#endif
}

//blocking function to parse an Improv Serial packet
void WifiManager::handleImprovPacket() {
  uint8_t header[6] = {'I', 'M', 'P', 'R', 'O', 'V'};
  bool timeout = false;
  uint16_t packetByte = 0;
  uint8_t packetLen = 9;
  uint8_t checksum = 0;
  uint8_t waitTime = 255;
  uint8_t rpcCommandType = 0;
  char rpcData[128];
  rpcData[0] = 0;
  while (!timeout) {
    if (Serial.available() < 1) {
      delay(1);
      waitTime--;
      if (!waitTime) timeout = true;
      continue;
    }
    byte next = Serial.read();
    DIMPROV_PRINT("Received improv byte: "); DIMPROV_PRINTF("%x\r\n", next);
    switch (packetByte) {
      case ImprovPacketByte::Version: {
        if (next != IMPROV_VERSION) {
          DIMPROV_PRINTLN(F("Invalid version"));
          return;
        }
        break;
      }
      case ImprovPacketByte::PacketType: {
        if (next != ImprovPacketType::RPC_Command) {
          DIMPROV_PRINTF("Non RPC-command improv packet type %i\n", next);
          return;
        }
        if (!improvActive) {
          improvActive = 1;
          improvePacketReceived = true;
        }
        break;
      }
      case ImprovPacketByte::Length:
        packetLen = 9 + next;
        break;
      case ImprovPacketByte::RPC_CommandType:
        rpcCommandType = next;
        break;
      default: {
        if (packetByte >= packetLen) { //end of packet, check checksum match
          if (checksum != next) {
            DIMPROV_PRINTF("Got RPC checksum %i, expected %i", next, checksum);
            sendImprovStateResponse(0x01, true);
            return;
          }
          switch (rpcCommandType) {
            case ImprovRPCType::Command_Wifi:
              parseWiFiCommand(rpcData);
              break;
            case ImprovRPCType::Request_State: {
              uint8_t improvState = 0x02; //authorized
              if (isWifiConfigured()) improvState = 0x03; //provisioning
              if (WiFi.localIP()[0] != 0 && WiFi.status() == WL_CONNECTED) improvState = 0x04; //provisioned
              sendImprovStateResponse(improvState, false);
              if (improvState == 0x04) sendImprovRPCResponse(ImprovRPCType::Request_State);
              break;
            }
            case ImprovRPCType::Request_Info:
              sendImprovInfoResponse();
              break;
            default: {
              DIMPROV_PRINTF("Unknown RPC command %i\n", next);
              sendImprovStateResponse(0x02, true);
            }
          }
          return;
        }
        if (packetByte < 6) { //check header
          if (next != header[packetByte]) {
            DIMPROV_PRINTLN(F("Invalid improv header"));
            return;
          }
        } else if (packetByte > 9) { //RPC data
          rpcData[packetByte - 10] = next;
          if (packetByte > 137) return; //prevent buffer overflow
        }
      }
    }
    checksum += next;
    packetByte++;
  }
}


