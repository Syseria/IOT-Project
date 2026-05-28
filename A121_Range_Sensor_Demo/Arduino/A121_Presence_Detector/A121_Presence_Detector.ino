#include "A121_Presence_Detector.h"

unsigned long prev, curr;
bool isConnected = false;
void setup() {
  A121_init();
  //Serial.begin(9600);
  //prev = millis
}

void loop() {
  Serial.print("test2");
  delay(1000);
  /*if(digitalRead(A121_BUSY_PIN))
  {
    A121_Get_Presence_Detector();
  }*/
}
