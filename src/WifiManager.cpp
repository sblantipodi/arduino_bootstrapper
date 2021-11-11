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
  #if (DISPLAY_ENABLED) 
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(5,17);
    display.drawRoundRect(0, 0, display.width()-1, display.height()-1, display.height()/4, WHITE);
  #endif
  helper.smartPrintln(F("DPsoftware domotics"));
  helper.smartDisplay();
  delay(DELAY_3000);

  #if (DISPLAY_ENABLED) 
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
  #endif
  helper.smartPrintln(F("Connecting to: "));
  helper.smartPrint(qsid); helper.smartPrintln(F("..."));
  helper.smartDisplay();

  delay(DELAY_2000);

  WiFi.persistent(true);   // Solve possible wifi init errors (re-add at 6.2.1.16 #4044, #4083)
  WiFi.disconnect(true);    // Delete SDK wifi config
  delay(DELAY_200);
  WiFi.mode(WIFI_STA);      // Disable AP mode
#if defined(ESP8266)
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
#endif
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
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
      Serial.println(F("WiFi connection is lost."));
      WiFi.reconnect();
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

  reconnectToWiFi(manageDisconnections, manageHardwareButton);

  helper.smartPrintln(F("WIFI CONNECTED"));
  microcontrollerIP = WiFi.localIP().toString();
  MAC = WiFi.macAddress();
  helper.smartPrintln(microcontrollerIP);

  delay(DELAY_1500);

  // reset the lastWIFiConnection to off, will be initialized by next time update
  lastWIFiConnection = OFF_CMD;

}

void WifiManager::reconnectToWiFi(void (*manageDisconnections)(), void (*manageHardwareButton)()) {

  wifiReconnectAttemp = 0;

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
      #if (DISPLAY_ENABLED) 
        display.setCursor(0,0);
        display.clearDisplay();
        display.setTextSize(1);
      #endif
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
      Serial.println(F("End"));
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
        content = "<!DOCTYPE HTML>\n"
                  "<html><header>\n"
                  "    <div>\n"
                  "        <a href='https://github.com/sblantipodi/'>";
        content += WIFI_DEVICE_NAME;
        content +="    </a></div>\n"
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
        content +="    <form method='get' action='setting' id='form1'>\n"
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
        content +="        <label for='additionalParam'><span class='line'></span>"; content += ADDITIONAL_PARAM_TEXT; content += "</label><input type='text' id='additionalParam' name='additionalParam'>\n"
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
        DynamicJsonDocument doc(1024);

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

#if defined(ESP8266)
        // Write to LittleFS
        Serial.println(F("Saving setup.json"));
        File jsonFile = LittleFS.open("/setup.json", "w");
        if (!jsonFile) {
          Serial.println("Failed to open [setup.json] file for writing");
        } else {
          serializeJsonPretty(doc, Serial);
          serializeJson(doc, jsonFile);
          jsonFile.close();
          Serial.println("[setup.json] written correctly");
        }
        delay(DELAY_200);
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
          } else {
            Serial.println(F("Failed to mount FS for write"));
          }
#endif
        delay(DELAY_1000);
#if defined(ESP8266)
        ESP.reset();
#elif defined(ESP32)
        ESP.restart();
#endif

    });

  }
}
