#include <Arduino.h>
#include <MKRWAN.h>

// ================================================================
// CONFIG UART PROTOCOL
// ================================================================
#define START_BYTE 0xAA
#define TYPE_BITMAP 0xB1
#define END_BYTE 0x55

#define NB_BYTES_BITMAP 19
#define NB_CAPTEURS 150

// ================================================================
// LORAWAN
// ================================================================
LoRaModem modem;

String appEui = "0000000000000000";
String appKey = "F67071CFC32B79B57309979AC90E7C8F";

unsigned long lastSend = 0;
const unsigned long SEND_INTERVAL = 30000;

// ================================================================
// BUFFERS
// ================================================================
uint8_t buffer[NB_BYTES_BITMAP];
uint8_t lastBuffer[NB_BYTES_BITMAP];

uint8_t compressed[NB_BYTES_BITMAP * 2 + 1];
uint8_t compressedSize = 0;

bool firstSend = true;

// ================================================================
// DEBUG OPTION (À ENLEVER PLUS TARD)
// ================================================================
#define DEBUG_UART 1   // <-- mettre 0 en production

#if DEBUG_UART
  #define DPRINT(x) Serial.print(x)
  #define DPRINTLN(x) Serial.println(x)
#else
  #define DPRINT(x)
  #define DPRINTLN(x)
#endif

// ================================================================
// UART STATE MACHINE ROBUSTE
// ================================================================
enum State {
  WAIT_START,
  WAIT_TYPE,
  READ_DATA,
  WAIT_END
};

State state = WAIT_START;
uint8_t indexBuffer = 0;
bool frameValid = false;

// ================================================================
// ETATS (debug local)
// ================================================================
bool etats[NB_CAPTEURS];

// ================================================================
// DECODE BITMAP
// ================================================================
void decodeBitmap() {
  for (int i = 0; i < NB_CAPTEURS; i++) {
    int byteIndex = i / 8;
    int bitIndex  = i % 8;
    etats[i] = (buffer[byteIndex] >> bitIndex) & 0x01;
  }
}

// ================================================================
// DEBUG SIMPLE
// ================================================================
void printEtatSimple() {
#if DEBUG_UART
  int occupe = 0;
  for (int i = 0; i < NB_CAPTEURS; i++) {
    if (etats[i]) occupe++;
  }
  Serial.print("[UART] Occupées: ");
  Serial.print(occupe);
  Serial.print(" / ");
  Serial.println(NB_CAPTEURS);
#endif
}

// ================================================================
// DELTA COMPRESSION LORA
// ================================================================
void buildDeltaPayload() {

  compressedSize = 0;
  compressed[compressedSize++] = 0xD0;

  for (int i = 0; i < NB_BYTES_BITMAP; i++) {

    uint8_t current = buffer[i];
    uint8_t prev    = lastBuffer[i];

    uint8_t diff = firstSend ? current : (current ^ prev);

    if (firstSend || diff != 0) {
      compressed[compressedSize++] = i;
      compressed[compressedSize++] = current;
    }
 
    lastBuffer[i] = current;
  }

  firstSend = false;
}

// ================================================================
// LORAWAN SEND
// ================================================================
void sendLoRaBitmap() {

  if (!firstSend && millis() - lastSend < SEND_INTERVAL) return;

  buildDeltaPayload();

  Serial.print("[LoRa] Payload size: ");
  Serial.println(compressedSize);

  modem.beginPacket();
  modem.write(compressed, compressedSize);

  // IMPORTANT: mode bloquant pour debug fiable
  int err = modem.endPacket(true);

  Serial.print("[LoRa] endPacket: ");
  Serial.println(err);

  if (err > 0) {
    lastSend = millis();
    Serial.println("[LoRa] OK");
  } else {
    Serial.println("[LoRa] ERREUR TX");
  }
}

// ================================================================
// RESET UART FRAME
// ================================================================
void resetFrame() {
  state = WAIT_START;
  indexBuffer = 0;
  frameValid = false;
}

// ================================================================
// UART PARSER ROBUSTE
// ================================================================
void traiterUART(uint8_t data) {

#if DEBUG_UART
  Serial.print(data, HEX);
  Serial.print(" ");
#endif

  switch (state) {

    case WAIT_START:
      if (data == START_BYTE) {
        state = WAIT_TYPE;
        indexBuffer = 0;
        frameValid = true;
      }
      break;

    case WAIT_TYPE:
      if (data == TYPE_BITMAP) {
        state = READ_DATA;
      } else {
        resetFrame();
      }
      break;

    case READ_DATA:

      // PROTECTION OVERFLOW (IMPORTANT)
      if (indexBuffer < NB_BYTES_BITMAP) {
        buffer[indexBuffer++] = data;
      } else {
        // frame corrompue → reset
        resetFrame();
        return;
      }

      if (indexBuffer >= NB_BYTES_BITMAP) {
        state = WAIT_END;
      }
      break;

    case WAIT_END:

      if (data == END_BYTE && frameValid) {

        decodeBitmap();
        printEtatSimple();

        // SEULEMENT SI FRAME OK
        sendLoRaBitmap();

      } else {
        Serial.println("[UART] Frame invalide");
      }

      resetFrame();
      break;
  }
}

// ================================================================
// SETUP
// ================================================================
void setup() {

  Serial.begin(115200);
  Serial1.begin(9600);

  Serial.println("=== MKR #2 ROBUST UART + LORA ===");

  if (!modem.begin(EU868)) {
    Serial.println("[LoRa] init failed");
    while (1);
  }

  Serial.println("[LoRa] joining...");

  if (!modem.joinOTAA(appEui, appKey)) {
    Serial.println("[LoRa] join failed");
    while (1);
  }

  Serial.println("[LoRa] connected");

  // IMPORTANT DEBUG LORA
  modem.setPort(1);
  modem.dataRate(5);
}

// ================================================================
// LOOP
// ================================================================
void loop() {

  while (Serial1.available()) {
    traiterUART(Serial1.read());
  }

  // IMPORTANT pour stabilité LoRaWAN
  modem.poll();
}