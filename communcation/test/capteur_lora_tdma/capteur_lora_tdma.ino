// ================================================================
//  CAPTEUR — LoRa P2P (ACK + REGISTER + RETRY ROBUSTE)
// ================================================================

#include <LoRa.h>

#define CAPTEUR_PIN  1
#define MON_ID  0x00000001

#define FREQUENCE 868E6
#define SPREADING 7
#define PUISSANCE 14

// types de messages
#define REGISTER  0x02
#define DATA      0x04
#define ACK_DATA  0x05
#define ACK_REG   0x03

// ───────────────────────────────────────────────
bool enAttenteACK = false;
uint8_t seq = 0;
uint8_t nbRetry = 0;

uint32_t dernierEnvoi = 0;
uint32_t timeoutACK = 1500;

// état réseau
bool connecte = false;
uint8_t nodeID = 0xFF;

// ───────────────────────────────────────────────
bool placeOccupee() {
  //return digitalRead(CAPTEUR_PIN) == HIGH;
  return true;
}

// ───────────────────────────────────────────────
// REGISTER (rejoin réseau)
void envoyerRegister() {

  LoRa.beginPacket();
  LoRa.write(REGISTER);
  LoRa.write((MON_ID >> 24) & 0xFF);
  LoRa.write((MON_ID >> 16) & 0xFF);
  LoRa.write((MON_ID >>  8) & 0xFF);
  LoRa.write(MON_ID & 0xFF);
  LoRa.endPacket();

  Serial.println("[CAPTEUR] REGISTER envoyé");
  dernierEnvoi = millis();
}

// ───────────────────────────────────────────────
// DATA
void envoyerData() {

  bool occupe = placeOccupee();

  LoRa.beginPacket();
  LoRa.write(DATA);
  LoRa.write(nodeID);
  LoRa.write(occupe ? 0x01 : 0x00);
  LoRa.write(seq++);
  LoRa.endPacket();

  enAttenteACK = true;
  dernierEnvoi = millis();

  Serial.print("[CAPTEUR] DATA envoyé — ");
  Serial.println(occupe ? "OCCUPÉE" : "LIBRE");
}

// ───────────────────────────────────────────────
// RX
void traiterPaquet(int taille) {

  if (taille < 1) return;

  uint8_t type = LoRa.read();

  // ACK REGISTER
  if (type == ACK_REG && taille >= 2) {

    nodeID = LoRa.read();
    connecte = true;
    nbRetry = 0;

    Serial.print("[CAPTEUR] CONNECTÉ node=");
    Serial.println(nodeID);
  }

  // ACK DATA
  else if (type == ACK_DATA && taille >= 2) {

    uint8_t id = LoRa.read();

    if (id == nodeID) {
      enAttenteACK = false;
      nbRetry = 0;
      Serial.println("[CAPTEUR] ACK DATA reçu");
    }
  }
}

// ───────────────────────────────────────────────
void setup() {

  Serial.begin(115200);
  pinMode(CAPTEUR_PIN, INPUT);

  if (!LoRa.begin(FREQUENCE)) {
    Serial.println("LoRa KO");
    while (true);
  }

  LoRa.setSpreadingFactor(SPREADING);
  LoRa.setTxPower(PUISSANCE);

  Serial.println("CAPTEUR READY");

  // 🔥 IMPORTANT : reconnect direct
  envoyerRegister();
}

// ───────────────────────────────────────────────
void loop() {

  int taille = LoRa.parsePacket();
  if (taille > 0) traiterPaquet(taille);

  // ── pas connecté → retry REGISTER
  if (!connecte && millis() - dernierEnvoi > 3000) {
    envoyerRegister();
  }

  // ── connecté → envoi DATA sur changement d’état
  static bool dernierEtat = false;
  bool etat = placeOccupee();

  if (connecte && !enAttenteACK && etat != dernierEtat) {
    envoyerData();
    dernierEtat = etat;
  }

  // ── retry DATA si pas ACK
  if (enAttenteACK && millis() - dernierEnvoi > timeoutACK) {

    if (nbRetry >= 5) {
      Serial.println("[CAPTEUR] perte connexion → reset");
      connecte = false;
      enAttenteACK = false;
      nbRetry = 0;
      envoyerRegister();
    } else {
      nbRetry++;
      Serial.print("[CAPTEUR] RETRY DATA #");
      Serial.println(nbRetry);
      envoyerData();
    }
  }
}