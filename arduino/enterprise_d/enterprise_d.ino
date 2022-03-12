/*
 * 
 *   Controller for the Eaglemoss Enterprise D Model Kit
 *   by Christoph Caina
 *   
 */

#include <PCF8591.h>                  // Analog In-/Output extension
#include <PCF8574.h>                  // Digital In-/Output extension
#include <IRremote.hpp>               // IR remote

bool debug;                           // for DebugOption we want to shorten a specific PIN. default value is true
const byte debugPin = 2;

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
}

void loop() {
    // put your main code here, to run repeatedly:
}
