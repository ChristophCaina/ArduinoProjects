/*
 * 
 *   Controller for the Eaglemoss Enterprise D Model Kit
 *   by Christoph Caina
 *   
 */

#include <PCF8591.h>                  // Analog In-/Output extension
#include <PCF8574.h>                  // Digital In-/Output extension
#include <IRremote.hpp>               // IR remote

bool debug = true;                   // for DebugOption we want to shorten a specific PIN. default value is true
const byte debugPin = D2;

void setup() {
  pinMode(debugPin, INPUT);

  if(!digitalRead(debugPin)) {
    debug = false;
  }
  else {
    Serial.begin(115200);
  }
}

void loop() {
    // put your main code here, to run repeatedly:
}
