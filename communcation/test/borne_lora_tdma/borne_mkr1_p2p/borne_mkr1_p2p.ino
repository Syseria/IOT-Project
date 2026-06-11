#include <LoRa.h>

#define FREQUENCE 868E6
#define SPREADING 7
#define PUISSANCE 14

#define REGISTER  0x02
#define DATA      0x04
#define ACK_DATA  0x05
#define ACK_REG   0x03

bool firstSend = true;

// ───────────────────────────────────────────────
// 🔥 PASSAGE À 150 CAPTEURS
// ───────────────────────────────────────────────
#define MAX_CAPTEURS 150

uint32_t lastSeq[MAX_CAPTEURS];

// ───────────────────────────────────────────────
// TABLE CAPTEURS
// ───────────────────────────────────────────────
uint32_t capteurIDs[MAX_CAPTEURS];
bool etats[MAX_CAPTEURS];

// ───────────────────────────────────────────────
// BITMAP 150 bits = 19 bytes
// ───────────────────────────────────────────────
uint8_t bitmap[19];

// ───────────────────────────────────────────────
// INIT
// ───────────────────────────────────────────────
void initTables() {

  for (int i = 0; i < MAX_CAPTEURS; i++) {
    capteurIDs[i] = 0xFFFFFFFF;
    etats[i] = false;
    lastSeq[i] = 255;
  }
}

// ───────────────────────────────────────────────
// FIND CAPTEUR
// ───────────────────────────────────────────────
int trouverIndex(uint32_t id) {
  for (int i = 0; i < MAX_CAPTEURS; i++) {
    if (capteurIDs[i] == id) return i;
  }
  return -1;
}

// ───────────────────────────────────────────────
// ADD CAPTEUR
// ───────────────────────────────────────────────
int ajouterCapteur(uint32_t id) {

  for (int i = 0; i < MAX_CAPTEURS; i++) {
    if (capteurIDs[i] == 0xFFFFFFFF) {
      capteurIDs[i] = id;
      etats[i] = false;
      lastSeq[i] = 255;
      return i;
    }
  }
  return -1;
}

// ───────────────────────────────────────────────
// ACK REG
// ───────────────────────────────────────────────
void envoyerAckReg(uint8_t nodeID) {
  delay(50);

  LoRa.beginPacket();
  LoRa.write(ACK_REG);
  LoRa.write(nodeID);
  LoRa.endPacket();

  Serial.print("[BORNE] ACK REG → ");
  Serial.println(nodeID);
}

// ───────────────────────────────────────────────
// ACK DATA
// ───────────────────────────────────────────────
void envoyerAckData(uint8_t nodeID) {
  delay(50);

  LoRa.beginPacket();
  LoRa.write(ACK_DATA);
  LoRa.write(nodeID);
  LoRa.endPacket();

  Serial.print("[BORNE] ACK DATA → ");
  Serial.println(nodeID);
}

// ───────────────────────────────────────────────
// BITMAP UPDATE
// ───────────────────────────────────────────────
void updateBitmap() {

  for (int i = 0; i < 19; i++) bitmap[i] = 0;

  for (int i = 0; i < MAX_CAPTEURS; i++) {

    if (capteurIDs[i] != 0xFFFFFFFF) {

      if (etats[i]) {

        int byteIndex = i / 8;
        int bitIndex = i % 8;

        bitmap[byteIndex] |= (1 << bitIndex);
      }
    }
  }
}

// ───────────────────────────────────────────────
// UART SEND BITMAP
// ───────────────────────────────────────────────
void envoyerUARTBitmap() {

  updateBitmap();

  Serial1.write(0xAA);
  Serial1.write(0xB1);

  for (int i = 0; i < 19; i++) {
    Serial1.write(bitmap[i]);
  }

  Serial1.write(0x55);

  Serial.println("[BORNE] BITMAP UART envoyé");
}

// ───────────────────────────────────────────────
// TRAITEMENT LORA
// ───────────────────────────────────────────────
void traiterPaquet(int taille) {

  if (taille < 1) return;

  uint8_t type = LoRa.read();

  // ───────── REGISTER ─────────
  if (type == REGISTER && taille >= 5) {

    uint32_t id = ((uint32_t)LoRa.read() << 24) |
                  ((uint32_t)LoRa.read() << 16) |
                  ((uint32_t)LoRa.read() << 8)  |
                  (uint32_t)LoRa.read();

    int index = trouverIndex(id);

    if (index == -1) {
      index = ajouterCapteur(id);
    }

    if (index == -1) {
      Serial.println("[BORNE] Parking plein !");
      return;
    }

    envoyerAckReg(index);

    Serial.print("[BORNE] REGISTER id=");
    Serial.print(id);
    Serial.print(" index=");
    Serial.println(index);
  }

  // ───────── DATA ─────────
  else if (type == DATA && taille >= 4) {

    uint8_t nodeID = LoRa.read();
    bool occupe = LoRa.read();
    uint8_t seq = LoRa.read();

    if (nodeID >= MAX_CAPTEURS) return;

    if (seq == lastSeq[nodeID]) {
      Serial.print("[BORNE] Doublon ignoré, renvoi ACK pour node=");
      Serial.println(nodeID);
      envoyerAckData(nodeID);
      return; 
    }

    lastSeq[nodeID] = seq;

    etats[nodeID] = occupe;

    Serial.print("[BORNE] DATA node=");
    Serial.print(nodeID);
    Serial.println(occupe ? " OCCUPÉE" : " LIBRE");

    envoyerAckData(nodeID);
  }
}

// ───────────────────────────────────────────────
// SETUP
// ───────────────────────────────────────────────
void setup() {

  Serial.begin(115200);
  Serial1.begin(9600);

  if (!LoRa.begin(FREQUENCE)) {
    Serial.println("LoRa KO");
    while (true);
  }

  LoRa.setSpreadingFactor(SPREADING);
  LoRa.setTxPower(PUISSANCE);

  initTables();

  Serial.println("BORNE READY 150 CAPTEURS");
}

// ───────────────────────────────────────────────
// LOOP
// ───────────────────────────────────────────────
void loop() {

  int taille = LoRa.parsePacket();
  if (taille > 0) traiterPaquet(taille);

  static uint32_t last = 0;

  if (firstSend || millis() - last > 2000) {
    firstSend = false;
    envoyerUARTBitmap();
    last = millis();
  }
}