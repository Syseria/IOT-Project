#include <Arduino.h>
#include <MKRWAN.h>

// ================================================================
// MKR #2 — UART BITMAP + LORAWAN DELTA (PROPRE ARCHITECTURE)
// ================================================================

// ───────────────────────────────────────────────
// UART CONFIG
// ───────────────────────────────────────────────
#define START_BYTE 0xAA
#define TYPE_BITMAP 0xB1
#define END_BYTE 0x55

#define NB_BYTES_BITMAP 19
#define NB_CAPTEURS 150

// ───────────────────────────────────────────────
// LORAWAN
// ───────────────────────────────────────────────
LoRaModem modem;

String appEui = "0000000000000000";
String appKey = "YOUR_APP_KEY";

unsigned long lastSend = 0;
const unsigned long SEND_INTERVAL = 30000;

// ───────────────────────────────────────────────
// BUFFERS
// ───────────────────────────────────────────────
uint8_t buffer[NB_BYTES_BITMAP];      // bitmap courant UART
uint8_t lastBuffer[NB_BYTES_BITMAP];  // bitmap précédent envoyé

uint8_t compressed[NB_BYTES_BITMAP * 2 + 1];
uint8_t compressedSize = 0;

bool firstSend = true;

// ───────────────────────────────────────────────
// UART STATE
// ───────────────────────────────────────────────
uint8_t indexBuffer = 0;

// ───────────────────────────────────────────────
// ETATS (debug local)
// ───────────────────────────────────────────────
bool etats[NB_CAPTEURS];

// ================================================================
// DECODE LOCAL (debug uniquement)
// ================================================================
void decodeBitmap() {

  for (int i = 0; i < NB_CAPTEURS; i++) {

    int byteIndex = i / 8;
    int bitIndex  = i % 8;

    etats[i] = (buffer[byteIndex] >> bitIndex) & 0x01;
  }
}

// ================================================================
// DEBUG
// ================================================================
void printEtatSimple() {

  int occupe = 0;

  for (int i = 0; i < NB_CAPTEURS; i++) {
    if (etats[i]) occupe++;
  }

  Serial.print("[MKR2] Occupées: ");
  Serial.print(occupe);
  Serial.print(" / ");
  Serial.println(NB_CAPTEURS);
}

// ================================================================
// COMPRESSION DELTA (UNIQUEMENT AVANT ENVOI)
// ================================================================
void buildDeltaPayload() {

  compressedSize = 0;

  // header
  compressed[compressedSize++] = 0xD0;

  for (int i = 0; i < NB_BYTES_BITMAP; i++) {

    uint8_t current = buffer[i];
    uint8_t prev    = lastBuffer[i];

    uint8_t diff;

    if (firstSend) {
      diff = current;
    } else {
      diff = current ^ prev;
    }

    if (firstSend || diff != 0) {
      compressed[compressedSize++] = i;      // index byte
      compressed[compressedSize++] = diff;   // delta
    }

    lastBuffer[i] = current;
  }

  firstSend = false;
}

// ================================================================
// ENVOI LORAWAN
// ================================================================
void sendLoRaBitmap() {

  if (millis() - lastSend < SEND_INTERVAL) return;

  buildDeltaPayload(); // 👈 compression ICI uniquement

  Serial.print("[LoRa] Payload: ");
  Serial.print(compressedSize);
  Serial.println(" bytes");

  modem.beginPacket();
  modem.write(compressed, compressedSize);

  int err = modem.endPacket(false);

  if (err > 0) {
    Serial.println("[LoRa] OK");
    lastSend = millis();
  } else {
    Serial.println("[LoRa] ERREUR");
  }
}

// ================================================================
// UART PARSER (SANS LOGIQUE LORA)
// ================================================================
void traiterUART(uint8_t data) {

  static int state = 0;

  switch (state) {

    case 0:
      if (data == START_BYTE) {
        state = 1;
        indexBuffer = 0;
      }
      break;

    case 1:
      if (data == TYPE_BITMAP) {
        state = 2;
      } else {
        state = 0;
      }
      break;

    case 2:
      buffer[indexBuffer++] = data;

      if (indexBuffer >= NB_BYTES_BITMAP) {
        state = 3;
      }
      break;

    case 3:
      if (data == END_BYTE) {

        decodeBitmap();
        printEtatSimple();

        sendLoRaBitmap(); // 👈 uniquement déclenchement
      }

      state = 0;
      break;
  }
}

// ================================================================
// SETUP
// ================================================================
void setup() {

  Serial.begin(115200);
  Serial1.begin(9600);

  Serial.println("=== MKR #2 READY (UART + LORAWAN CLEAN ARCH) ===");

  if (!modem.begin(EU868)) {
    Serial.println("[LoRa] modem init failed");
    while (1);
  }

  Serial.println("[LoRa] joining OTAA...");

  if (!modem.joinOTAA(appEui, appKey)) {
    Serial.println("[LoRa] join failed");
    while (1);
  }

  Serial.println("[LoRa] connected");
}

// ================================================================
// LOOP
// ================================================================
void loop() {

  while (Serial1.available()) {
    traiterUART(Serial1.read());
  }
}