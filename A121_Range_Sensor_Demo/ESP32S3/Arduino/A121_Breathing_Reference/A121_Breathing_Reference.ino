#include "A121_Breathing_Reference.h"

void setup() {
  A121_init();
}

void loop() {
  delay(10);
  if(digitalRead(A121_BUSY_PIN))
  {
    A121_Get_Breathing_application();
  }
}
