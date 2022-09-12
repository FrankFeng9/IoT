#include <Arduino.h>

unsigned long myTime;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  Serial.println("\n\nHello,World!");

  Serial.println("\nYizhe Feng -- 000798153");

  Serial.print("\nChip ID: " + String(ESP.getChipId()));
 
  Serial.print("\n\nFlash Chip ID: ");
  Serial.print(ESP.getFlashChipId());
}

void loop() {
  // put your main code here, to run repeatedly:
  myTime = millis();

  Serial.println("\nmilliseconds since D1 Mini start: " + String(myTime));

  delay(2000);


}