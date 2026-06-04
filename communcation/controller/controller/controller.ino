#include <MKRWAN.h>
#include <LoRa.h>
//#include <AESLib.h>

// Variable pour TTN
#define appEui "0000000000000000"
#define appKey "74115F48CD0EFE175206D148C03F5646"

LoRaModem modem;

String message = "";

void setup() {
  Serial.begin(115200);

  Serial.println("LoRaWAN TTN Sender");
/*
  if (!modem.begin(EU868)) {
    Serial.println("Modem failed");
    while (1);
  }

  Serial.print("DevEUI: ");
  Serial.println(modem.deviceEUI());

  if (!modem.joinOTAA(appEui, appKey)) {
    Serial.println("Join failed");
    while (1);
  }

  Serial.println("Connected to TTN!");
*/
  if (!LoRa.begin(868500000)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    // read packet
    while (LoRa.available()) {
      message += (char)LoRa.read();
    }

    Serial.println("Received: " + message);
  }
}
