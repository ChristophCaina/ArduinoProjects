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

//--------------------------------------------------------------------------

class Flasher {
  protected:
    const byte ledPin;
    const uint32_t onPhase;
    const uint32_t offPhase;
    uint32_t timeMarker = 0;
    
    enum _step : byte {Start, OnPhase, OffPhase};
    Flasher::_step step = _step::Start;

  public:
    Flasher(const byte ledPin, const unsigned long onPhase, const unsigned long offPhase): ledPin(ledPin), onPhase(onPhase), offPhase(offPhase) {}
  
  void run(byte & blinkAmount) {
    uint32_t now = millis();

    switch (step) {
      case _step::Start:
        if (blinkAmount) {
          PCF_OUT.write(ledPin, HIGH);
          step = _step::OnPhase;
          timeMarker = now;
        }
        break;
      case _step::OnPhase:
        if (now - timeMarker >= onPhase) {
          PCF_OUT.write(ledPin, LOW);           
          step = _step::OffPhase;
          timeMarker = now;
        }
        break;
      case _step::OffPhase:
        if (now - timeMarker >= offPhase) {
          if (blinkAmount > 0) {
            blinkAmount-- ;
        }
        step = _step::Start;
      }
    }
  }
};

//--------------------------------------------------------------------------

enum FlashingGroup : byte {FRONTTORPEDO, REARTORPEDO, BEACON};
                               
Flasher flashingGroup[] = 
{                                          
  {frontTorpedoLauncherLed, torpedoLaunchOnTime, torpedoLaunchOffTime},
  {rearTorpedoLauncherLed, torpedoLaunchOnTime, torpedoLaunchOffTime},
  {beaconLightsLed, beaconLightOnTime, beaconLightOffTime},
};

//--------------------------------------------------------------------------

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
    TorpedoLauncher(1);
}

//--------------------------------------------------------------------------

void TorpedoLauncher(byte caller) {
  static byte blinkFrontAmount = 0;
  if(debug) {
    Serial.print("Amount Front Torpedo-Launches: ");
    Serial.println(blinkFrontAmount);
  }

  switch(caller) {
    case 1:  
      if(debug) {
        Serial.print("Function-Caller: ");
        Serial.print(caller);
        Serial.println(" (loop)");
      }
      
      // change this code according to new PCF Library... !
      /*
      if (PCF_IN.read(frontTorpedoLaunchBtn))
      {
        if(debug) {
          Serial.println("front torpedolauncher activated!");
        }
        blinkFrontAmount = amountFrontTorpedoLaunches;
      }
      */
      
      flashingGroup[FRONTTORPEDO].run(blinkFrontAmount);
      break;
      
      case 2:
        if(debug) {
          Serial.print("Function-Caller: ");
          Serial.print(caller);
          Serial.println(" (WebUi)");
        }
      
        // Add Code for WebInterface
        // Example:
        /*
        if (header.indexOf("GET /5/on") >= 0) {
          Serial.println("GPIO 5 on");
          output5State = "on";
          blinkFrontAmount = amountFrontTorpedoLaunches;
          //digitalWrite(output5, HIGH);
        } 
        */
        blinkFrontAmount = amountFrontTorpedoLaunches;
        break;
  }
}
