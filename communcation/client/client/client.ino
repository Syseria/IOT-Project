
#include <LoRa.h>
#include <AESLib.h>

static uint16_t id = 1;
bool occupied = false;
bool state_read_by_sensor = true;

AESLib aesLib;

char cleartext[32];
char ciphertext[64];

//helpers
void send_print(uint16_t id, bool occuped){
  // send packet
  LoRa.beginPacket();
  LoRa.print(id);
  LoRa.print(":");
  LoRa.println(occuped);
  LoRa.endPacket();
  
  Serial.print(id);
  Serial.print(":");
  Serial.println(occuped);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Client");

  if (!LoRa.begin(868500000)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  else{
    Serial.println("Starting Lora");
  }
}

void loop() {

  if(state_read_by_sensor != occupied){
    occuped = state_read_by_sensor;
    send_print(id, occupied);
  }
  

  delay(5000);

}