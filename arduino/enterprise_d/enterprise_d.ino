/*
 * 
 *   Controller for the Eaglemoss Enterprise D Model Kit
 *   by Christoph Caina
 *   
 */

#include <PCF8591.h>                          // Analog In-/Output extension
#include <PCF8574.h>                          // Digital In-/Output extension
#include <IRremote.hpp>                       // IR remote

PCF8574 PCF_OUT(0x20);                        // OUTPUTS
PCF8574 PCF_IN(0x25);                         // INPUTS

bool debug;                                   // for DebugOption we want to shorten a specific PIN. default value is true
const byte debugPin                = 2;

bool useRearTorpedoLauncher        = true;    // will the rearTorpedoLauncher will be used?

const byte frontTorpedoDt          = 4;       // change to PCF8574 Pin
const byte frontTorpedoSw          = 2;       // change to PCF8574 Pin
const byte frontTorpedoClk         = 7;       // change to PCF8574 Pin
const byte rearTorpedoDt           = ;        // change to PCF8574 Pin
const byte rearTorpedoSw           = ;        // change to PCF8574 Pin
const byte rearTorpedoClk          = ;        // change to PCF8574 Pin

int frontTorpedoAmount             = 0;
int rearTorpedoAmount              = 0;
unsigned long frontTorpedoLastBtn  = 0;
unsigned long rearTorpedoLastBtn   = 0;
int frontTorpedoLastClkState;
int rearTorpedoLastClkState;
int frontTorpedoCurrentClkState;
int rearTorpedoCurrentClkState;

int encMaxCount                    = 9;
int encMinCount                    = 0;


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
  
  pinMode(frontTorpedoDt, INPUT);                      // Define Pin Mode for the DT connector (Front Torpedo)
  pinMode(frontTorpedoSw, INPUT_PULLUP);               // Define Pin Mode for the SW connector (Front Torpedo)
  pinMode(frontTorpedoClk, INPUT);                     // Define Pin Mode for the CLK cocnnector (Front Torpedo)
  
  pinMode(rearTorpedoDt, INPUT);                       // Define Pin Mode for the DT connector (Rear Torpedo)
  pinMode(rearTorpedoSw, INPUT_PULLUP);                // Define Pin Mode for the SW connector (Rear Torpedo)
  pinMode(rearTorpedoClk, INPUT);                      // Define Pin Mode for the CLK cocnnector (Rear Torpedo)
  
  frontTorpedoLastClkState = digitalRead(frontTorpedoClk);
  frontTorpedoCurrentClkState = digitalRead(frontTorpedoClk);
  rearTorpedoLastClkState = digitalRead(rearTorpedoClk);
  rearTorpedoCurrentClkState = digitalRead(rearTorpedoClk);

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
