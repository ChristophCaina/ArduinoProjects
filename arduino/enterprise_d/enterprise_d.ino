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

const byte frontTorpedoDt          = 0;       // PCF_IN P01
const byte frontTorpedoSw          = 1;       // PCF_IN P02
const byte frontTorpedoClk         = 2;       // PCF_IN P03
const byte rearTorpedoDt           = 3;       // PCF_IN P04
const byte rearTorpedoSw           = 4;       // PCF_IN P05
const byte rearTorpedoClk          = 5;       // PCF_IN P06

int frontTorpedoLastClkState;
int rearTorpedoLastClkState;

int frontTorpedoAmount             = 0;
int rearTorpedoAmount              = 0;

unsigned long frontTorpedoLastBtn  = 0;
unsigned long rearTorpedoLastBtn   = 0;

const unsigned long torpedoOnTime  = 120;
const unsigned long torpedoOffTime = 450;

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

  pinMode(rearTorpedoDt, INPUT);                       // Define Pin Mode for the DT connector (Rear Torpedo)
  pinMode(rearTorpedoSw, INPUT_PULLUP);                // Define Pin Mode for the SW connector (Rear Torpedo)
  pinMode(rearTorpedoClk, INPUT);                      // Define Pin Mode for the CLK cocnnector (Rear Torpedo)
  rearTorpedoLastClkState = digitalRead(rearTorpedoClk);

  if(digitalRead(debugPin)) {
    debug = true;
    Serial.begin(115200);
  }
  else {
    debug = false;
  }
}

void loop() {
  ReadEncoderChange(1);     // FrontTorpedo
  TorpedoLauncher(1);       // FrontTorpedo
  
  ReadEncoderChange(2);     // RearTorpedo
  TorpedoLauncher(2);       // RearTorpedo
}

//--------------------------------------------------------------------------

void TorpedoLauncher(byte torpedoRamp) {
  static byte blinkAmount = 0;
  byte torpedoSw;
  int torpedoAmount;
  byte type;
  
  switch(torpedoRamp) {
    case 1:
      torpedoSw = PCF_IN.digitalRead(frontTorpedoSw);
      torpedoAmount = frontTorpedoAmount;
      type = FRONTTORPEDO;
      break;
      
    case 2:
      torpedoSw = PCF_IN.digitalRead(rearTorpedoSw);
      torpedoAmount = rearTorpedoAmount;
      type = REARTORPEDO;
      break;
  }
    
  if(!torpedoSw) {
    if(debug) {
      Serial.println("torpedo launcher activated!");
    }
    blinkAmount = torpedoAmount;
  }
    
  if(debug) {
    Serial.print("Torpedo Launcher: ");
    Serial.println(type);
    Serial.print("amount of torpedos: ");
    Serial.println(blinkAmount);
  }

  flashingGroup[type].run(blinkAmount);
}

//--------------------------------------------------------------------------

void ReadEncoderChange(byte torpedoRamp) {
  int currentClkState;
  int lastClkState;
  int torpedoAmount                  = 0;
  int encMaxCount                    = 9;
  int encMinCount                    = 0;
  byte torpedoDt;
  
  switch(torpedoRamp) {
    case 1:
      currentClkState = PCF_IN.digitalRead(frontTorpedoClk);
      lastClkState = frontTorpedoLastClkState;
      torpedoDt = PCF_IN.digitalRead(frontTorpedoDt);
      break;

    case 2:
      currentClkState = PCF_IN.digitalRead(rearTorpedoClk);
      lastClkState = rearTorpedoLastClkState;
      torpedoDt = PCF_IN.digitalRead(rearTorpedoDt);
      break;
  }

  if(currentClkState != lastClkState && currentClkState == 0) {
    if(torpedoDt != currentClkState) {
      torpedoAmount ++ ;                         // Increase the Counter, but only until MaxCount has been reached
      if(torpedoAmount > encMaxCount) {
        torpedoAmount = encMaxCount;
      }
      if(debug) {
        Serial.println("increasing front torpedo amount");
      }
    }
    else {
      torpedoAmount -- ;                         // Decrease the Counter, but only until MinCount has been reached
      if(torpedoAmount < encMinCount) {
        torpedoAmount = encMinCount;
      }
      if(debug) {
        Serial.println("decreasing front torpedo amount");
      }
    }
  }
  lastClkState = currentClkState;
  
  switch(torpedoRamp) {
    case 1:  
      frontTorpedoAmount = torpedoAmount;
      break;

    case 2:
      rearTorpedoAmount = rearTorpedoAmount;
      break;
  }
}

//--------------------------------------------------------------------------
