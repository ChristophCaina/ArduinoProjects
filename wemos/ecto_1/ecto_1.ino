/*
 * 
 *   Controller for the Eaglemoss Ecto-1 Model Kit
 *   by Christoph Caina
 *   
 */

#include <WiFiManager.h>              // WiFi Manager for ESP8266
#include <PCF8591.h>                  // Analog In-/Output extension
#include <PCF8574.h>                  // Digital In-/Output extension
#include <IRremote.hpp>               // IR remote

const byte debugPin = D2;
bool debug;                           // for DebugOption we want to shorten a specific PIN. default value is true
unsigned long cpTimeout = 120;        // seconds until the CP will time-out
char* wifiAutoSSID = "GhostBusters";  // our 'default' SSID when the CP starts

WiFiManager wifiManager;

void setup() {
  pinMode(debugPin, INPUT);

  if(digitalRead(debugPin)) {
    debug = true;
    Serial.begin(115200);
  }
  else {
    debug = false;
    wifiManager.setDebugOutput(false);
  }
  
  WiFi.mode(WIFI_STA);                // explicitly set mode, esp defaults to STA+AP 
  
  if(debug) {
    wifiManager.resetSettings();               // for debug and testing, we will reset the WiFiManager if debug is set to true
  }
  
  wifiManager.setConfigPortalBlocking(false);
  wifiManager.setConfigPortalTimeout(cpTimeout);

  if(wm.autoConnect()) {
    if(debug) {
      Serial.print("successfully connected to: ");
      Serial.println("SSID: " + (String)wifiManager.getWiFiSSID());
    }
  }
  else {
    if(wm.startConfigPortal(wifiAutoSSID)) {
      if(debug) {
        Serial.print("configuration portal started with");
        Serial.println("SSID: " + (String)wifiManager.getWiFiSSID());
      }
    }
    else {
      if(debug) {
        Serial.print("failed to launch configuration portal");
        Serial.println("restarting device!");
      }
      ESP.restart();
    }
  }
}

void loop() {
    wifiManager.process();
}
