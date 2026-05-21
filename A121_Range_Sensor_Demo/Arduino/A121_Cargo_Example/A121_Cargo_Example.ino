#include "A121_Cargo_Example.h"

void setup() {
  A121_init();
}

void loop() {
  delay(10);
  if(digitalRead(A121_BUSY_PIN))
  {
    A121_Get_Cargo_application();
  }
}
