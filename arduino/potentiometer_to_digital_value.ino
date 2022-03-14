/*
digital limit from 0 to 9 (for enterprise sketch)
can be set from 0 to 255
*/

const byte potentiometer = A0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  ReadPotentiometerValue();
}

void ReadPotentiometerValue() {
  int sensorValue = 0;
  int sensorOutput = 0;
  
  sensorValue = analogRead(A0);
  sensorOutput = map(sensorValue, 472, 1023, 0, 9);

  Serial.print("Sensor: ");
  Serial.println(sensorOutput);
  delay(250);
}  
