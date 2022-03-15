/*
 * 
 *   Controller for the Eaglemoss Enterprise D Model Kit
 *   by Christoph Caina
 *   
 */

//#include <PCF8591.h>                        // Analog In-/Output extension
#include <PCF8574.h>                          // Digital In-/Output extension
#include <IRremote.hpp>                       // IR remote

PCF8574 PCF_OUT(0x20);                        // OUTPUTS
PCF8574 PCF_IN(0x25);                         // INPUTS

bool debug;                                   // for DebugOption we want to shorten a specific PIN. default value is true
const byte debugPin                = 2;

const byte frontTorpedoDt          = 0;       // PCF_IN P01
const byte frontTorpedoSw          = 1;       // PCF_IN P02
const byte frontTorpedoClk         = 2;       // PCF_IN P03
const byte rearTorpedoDt           = 3;       // PCF_IN P04
const byte rearTorpedoSw           = 4;       // PCF_IN P05
const byte rearTorpedoClk          = 5;       // PCF_IN P06

int frontTorpedoAmount             = 0;
int rearTorpedoAmount              = 0;
unsigned long frontTorpedoLastBtn  = 0;
unsigned long rearTorpedoLastBtn   = 0;
int frontTorpedoLastClkState;
int rearTorpedoLastClkState;
int frontTorpedoCurrentClkState;
int rearTorpedoCurrentClkState;

const unsigned long torpedoOnTime  = 120;
const unsigned long torpedoOffTime = 450;

int encMaxCount                    = 9;
int encMinCount                    = 0;

const byte frontTorpedoLed         = 0;       // PCF_OUT P01
const byte rearTorpedoLed          = 1;       // PCF_OUT P02

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
  
  //--------------------------------------------------------------------------
  
  void run(byte & blinkAmount) {
    uint32_t now = millis();

    switch (step) {
      case _step::Start:
        if (blinkAmount) {
          PCF_OUT.digitalWrite(ledPin, HIGH);
          step = _step::OnPhase;
          timeMarker = now;
        }
        break;
        
      case _step::OnPhase:
        if (now - timeMarker >= onPhase) {
          PCF_OUT.digitalWrite(ledPin, LOW);           
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

enum FlashingGroup : byte {FRONTTORPEDO, REARTORPEDO, /*BEACON*/};
                               
Flasher flashingGroup[] = 
{                                          
  {frontTorpedoLed, torpedoOnTime, torpedoOffTime},
  {rearTorpedoLed, torpedoOnTime, torpedoOffTime},
  //{beaconLightsLed, beaconLightOnTime, beaconLightOffTime},
};

//--------------------------------------------------------------------------

void setup() {
  pinMode(debugPin, INPUT);
  
  pinMode(frontTorpedoDt, INPUT);                      // Define Pin Mode for the DT connector (Front Torpedo)
  pinMode(frontTorpedoSw, INPUT_PULLUP);               // Define Pin Mode for the SW connector (Front Torpedo)
  pinMode(frontTorpedoClk, INPUT);                     // Define Pin Mode for the CLK cocnnector (Front Torpedo)
  frontTorpedoLastClkState = digitalRead(frontTorpedoClk);
  frontTorpedoCurrentClkState = digitalRead(frontTorpedoClk);

  pinMode(rearTorpedoDt, INPUT);                       // Define Pin Mode for the DT connector (Rear Torpedo)
  pinMode(rearTorpedoSw, INPUT_PULLUP);                // Define Pin Mode for the SW connector (Rear Torpedo)
  pinMode(rearTorpedoClk, INPUT);                      // Define Pin Mode for the CLK cocnnector (Rear Torpedo)
  rearTorpedoLastClkState = digitalRead(rearTorpedoClk);
  rearTorpedoCurrentClkState = digitalRead(rearTorpedoClk);


  if(digitalRead(debugPin)) {
    debug = true;
    Serial.begin(115200);
  }
  else {
    debug = false;
  }
}

void loop() {
    TorpedoLauncher();
}

//--------------------------------------------------------------------------

void TorpedoLauncher() {
  static byte blinkFrontTAmount = 0;
  static byte blinkRearTAmount = 0;
  
  if(!PCF_IN.digitalRead(frontTorpedoSw)) {
    if(debug) {
      Serial.println("front torpedolauncher activated!");
    }
    blinkFrontTAmount = frontTorpedoAmount;
  }
  
  if(!PCF_IN.digitalRead(rearTorpedoSw)) {
    if(debug) {
      Serial.println("rear torpedolauncher activated!");
    }
    blinkRearTAmount = rearTorpedoAmount;
  }
  
  if(debug) {
    Serial.print("front torpedos: ");
    Serial.println(blinkFrontTAmount);
    
    Serial.print("rear torpedos: ");
    Serial.println(blinkRearTAmount);
  }

  flashingGroup[FRONTTORPEDO].run(blinkFrontTAmount);
  flashingGroup[REARTORPEDO].run(blinkRearTAmount);
}

//--------------------------------------------------------------------------

void ReadEncoderChange() {
  frontTorpedoCurrentClkState = PCF_IN.digitalRead(frontTorpedoClk);
  rearTorpedoCurrentClkState = PCF_IN.digitalRead(rearTorpedoClk);
  
  if(frontTorpedoCurrentClkState != frontTorpedoLastClkState && frontTorpedoCurrentClkState == 0) {
    if(PCF_IN.digitalRead(frontTorpedoDt) != frontTorpedoCurrentClkState) {
      frontTorpedoAmount ++ ;                         // Increase the Counter, but only until MaxCount has been reached
      if(frontTorpedoAmount > encMaxCount) {
        frontTorpedoAmount = encMaxCount;
      }
      if(debug) {
        Serial.println("increasing front torpedo amount");
      }
    }
    else {
      frontTorpedoAmount -- ;                         // Decrease the Counter, but only until MinCount has been reached
      if(frontTorpedoAmount < encMinCount) {
        frontTorpedoAmount = encMinCount;
      }
      if(debug) {
        Serial.println("decreasing front torpedo amount");
      }
    }
  }
  frontTorpedoLastClkState = frontTorpedoCurrentClkState;

  if(rearTorpedoCurrentClkState != rearTorpedoLastClkState && rearTorpedoCurrentClkState == 0) {
    if(PCF_IN.digitalRead(rearTorpedoDt) != rearTorpedoCurrentClkState) {
      rearTorpedoAmount ++ ;
      if(rearTorpedoAmount > encMaxCount) {
        rearTorpedoAmount = encMaxCount;
      }
      if(debug) {
        Serial.println("increasing rear torpedo amount");
      }
    }
    else {
      rearTorpedoAmount -- ;
      if(rearTorpedoAmount < encMinCount) {
        rearTorpedoAmount = encMinCount;
      }
      if(debug) {
        Serial.println("decreasing rear torpedo amount");
      }
    }
  }
}
