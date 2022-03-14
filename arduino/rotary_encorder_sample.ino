const byte encDt           = 4;
const byte encSw           = 2;
const byte encClk          = 7;

int encCounter             = 0;
unsigned long lastBtnPress = 0;
int encMaxCount            = 9;
int encMinCount            = 0;
int encLastClkState;
int encCurrentClkState;
String encCurrentDir;


void setup() {
  //
  pinMode(encDt, INPUT);                      // Define Pin Mode for the DT connector
  pinMode(encSw, INPUT_PULLUP);               // Define Pin Mode for the SW connector
  pinMode(encClk, INPUT);                     // Define Pin Mode for the CLK ocnnector
  
  Serial.begin(9600);                         // start Serial Monitor

  encLastClkState = digitalRead(encClk);      // LastClkState will be set to the current ClkReading
  encCurrentClkState = digitalRead(encClk);   // CurrentClkState will be set to the current ClkReading

  Serial.print("Encoder Counter: ");          // print the current Reading (should be 0)
  Serial.println(encCounter);
}

void loop() {
  // Begin our Loop ...
  ReadEncoderChange();
  ReadEncoderBtnPress();
}

void ReadEncoderChange() {
  encCurrentClkState = digitalRead(encClk);   // Read Clk State

  if(encCurrentClkState != encLastClkState && encCurrentClkState == 0) {
    if(digitalRead(encDt) != encCurrentClkState) {
      encCounter ++ ;                         // Increase the Counter, but only until MaxCount has been reached
      if(encCounter > encMaxCount) {
        encCounter = encMaxCount;
      }
      encCurrentDir = "CW";
    }
    else {
      encCounter -- ;                         // Decrease the Counter, but only until MinCount has been reached
      if (encCounter < encMinCount) {
        encCounter = encMinCount;
      }
      encCurrentDir = "CCW";
    }
    Serial.print("Encoder Counter: ");
    Serial.println(encCounter);
    Serial.print("Current Direction: ");
    Serial.println(encCurrentDir);
  }
  encLastClkState = encCurrentClkState;
}

void ReadEncoderBtnPress() {
  int encBtnState = digitalRead(encSw);
  if(!encBtnState) {                          // If we want do do something when the Button on the Encoder has been pushed, we can do it here... 
                                              // Since it is defined as Input_Pullup, we need to invert the logic
    if(millis() - lastBtnPress > 50) {
      //resetCounter();
      Serial.print("Encoder Button pressed @ ");
      Serial.println(lastBtnPress);
    }
    lastBtnPress = millis();
  }
}
