#if defined(ARDUINO_SEEED_XIAO_NRF52840_SENSE) || defined(ARDUINO_SEEED_XIAO_NRF52840)
#error "XIAO nRF52840 please use the non-mbed-enable version."
#endif

#include <mmwave_for_xiao.h>

// Use the MKR WAN 1310 hardware UART
#define COMSerial Serial1

// Creates a global Serial object for printing debugging information
#define ShowSerial Serial

// Initialising the radar configuration
Seeed_HSP24 xiao_config(COMSerial, ShowSerial);
//Seeed_HSP24 xiao_config(COMSerial);

Seeed_HSP24::RadarStatus radarStatus;

bool placeOccupee(){
  int retryCount = 0;
  const int MAX_RETRIES = 10;  // Maximum number of retries to prevent infinite loops
  //ShowSerial.println("début");
  //Get radar status
  do {
    radarStatus = xiao_config.getStatus();
    retryCount++;
//    ShowSerial.println(radarStatus.distance);
  } while (radarStatus.distance == -1 && retryCount < MAX_RETRIES);

  //ShowSerial.println("status done");

  //Parses radar status and prints results from debug serial port
  if (radarStatus.distance != -1) {
    //ShowSerial.print("Status: " + String(targetStatusToString(radarStatus.targetStatus)) + "  ----   ");
    ShowSerial.println("Distance: " + String(radarStatus.distance) + "  Mode: " + String(radarStatus.radarMode));
    //ShowSerial.print("Move: ");

    if(radarStatus.distance >= 50){
      return false;
    }else{
      return true;
    }
  }

  return false;
}

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

void setup() {
  /* Be sure to change the baud rate for radar reporting to 9600 before using this routine, 
  refer to the wiki: https://wiki.seeedstudio.com/mmwave_for_xiao_arduino/#xiao-example */
  Serial.begin(115200);
  Serial1.begin(115200);
  while(!ShowSerial);  

  if (!LoRa.begin(FREQUENCE)) {
    Serial.println("LoRa KO");
    while (true);
  }

  LoRa.setSpreadingFactor(SPREADING);
  LoRa.setTxPower(PUISSANCE);

  envoyerRegister();

  ShowSerial.println("Programme Starting!");
}

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

  delay(1000);
}

// Parsing the acquired radar status
const char* targetStatusToString(Seeed_HSP24::TargetStatus status) {
  switch (status) {
    case Seeed_HSP24::TargetStatus::NoTarget:
      return "NoTarget";
    case Seeed_HSP24::TargetStatus::MovingTarget:
      return "MovingTarget";
    case Seeed_HSP24::TargetStatus::StaticTarget:
      return "StaticTarget";
    case Seeed_HSP24::TargetStatus::BothTargets:
      return "BothTargets";
    default:
      return "Unknown";
  }
}
