#if defined(ARDUINO_SEEED_XIAO_NRF52840_SENSE) || defined(ARDUINO_SEEED_XIAO_NRF52840)
#error "XIAO nRF52840 please use the non-mbed-enable version."
#endif

#include <mmwave_for_xiao.h>
#include <LoRa.h>

// Use the MKR WAN 1310 hardware UART
#define COMSerial Serial1
#define ShowSerial Serial

Seeed_HSP24 xiao_config(COMSerial, ShowSerial);

// ================================================================
//  CAPTEUR CONFIGURATION
// ================================================================
#define MON_ID  0x00000001
#define FREQUENCE 868E6
#define SPREADING 7
#define PUISSANCE 14

// types de messages
#define REGISTER  0x02
#define DATA      0x04
#define ACK_DATA  0x05
#define ACK_REG   0x03

// Variables Réseau
bool enAttenteACK = false;
uint8_t seq = 0;
uint8_t nbRetry = 0;
uint32_t dernierEnvoi = 0;
uint32_t timeoutACK = 1500;
bool connecte = false;
uint8_t nodeID = 0xFF;

// ───────────────────────────────────────────────
// GESTION DU RADAR (FIXED)
// ───────────────────────────────────────────────
// We store the last known state. It only changes when we get a VALID new frame.
bool etatCourantOccupe = false; 

bool placeOccupee() {
  // Read the radar buffer without blocking
  Seeed_HSP24::RadarStatus radarStatus = xiao_config.getStatus();

  // Only update our logic if we caught a valid frame
  if (radarStatus.distance != -1) {
    // ShowSerial.println("Distance: " + String(radarStatus.distance));
    
    // Threshold check (50cm)
    if(radarStatus.distance >= 50){
      etatCourantOccupe = false;
    } else {
      etatCourantOccupe = true;
    }
  }

  return etatCourantOccupe; 
}


// ───────────────────────────────────────────────
// REGISTER (rejoin réseau)
// ───────────────────────────────────────────────
void envoyerRegister() {
  LoRa.beginPacket();
  LoRa.write(REGISTER);
  LoRa.write((MON_ID >> 24) & 0xFF);
  LoRa.write((MON_ID >> 16) & 0xFF);
  LoRa.write((MON_ID >>  8) & 0xFF);
  LoRa.write(MON_ID & 0xFF);
  LoRa.endPacket();

  ShowSerial.println("[CAPTEUR] REGISTER envoyé");
  dernierEnvoi = millis();
}

// ───────────────────────────────────────────────
// DATA
// ───────────────────────────────────────────────
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

  ShowSerial.print("[CAPTEUR] DATA envoyé — ");
  ShowSerial.println(occupe ? "OCCUPÉE" : "LIBRE");
}

// ───────────────────────────────────────────────
// RX
// ───────────────────────────────────────────────
void traiterPaquet(int taille) {
  if (taille < 1) return;

  uint8_t type = LoRa.read();

  // ACK REGISTER
  if (type == ACK_REG && taille >= 2) {
    nodeID = LoRa.read();
    connecte = true;
    nbRetry = 0;
    ShowSerial.print("[CAPTEUR] CONNECTÉ node=");
    ShowSerial.println(nodeID);
  }
  // ACK DATA
  else if (type == ACK_DATA && taille >= 2) {
    uint8_t id = LoRa.read();
    if (id == nodeID) {
      enAttenteACK = false;
      nbRetry = 0;
      ShowSerial.println("[CAPTEUR] ACK DATA reçu");
    }
  }
}

// ───────────────────────────────────────────────
// SETUP
// ───────────────────────────────────────────────
void setup() {
  delay(5000); // Buffer to prevent brownouts
  
  ShowSerial.begin(115200);
  COMSerial.begin(256000);
  
  // Wait for serial monitor for debugging, but timeout after 3 seconds 
  // so the board can run on battery without a PC attached!
  // This is due to the sensor pulling too much mA at startup
  uint32_t t = millis();
  while(!ShowSerial && millis() - t < 3000);  

  if (!LoRa.begin(FREQUENCE)) {
    ShowSerial.println("LoRa KO");
    while (true);
  }

  LoRa.setSpreadingFactor(SPREADING);
  LoRa.setTxPower(PUISSANCE);

  envoyerRegister();
  ShowSerial.println("Programme Starting!");
}

// ───────────────────────────────────────────────
// LOOP
// ───────────────────────────────────────────────
void loop() {
  // 1. Process any incoming LoRa packets instantly
  int taille = LoRa.parsePacket();
  if (taille > 0) traiterPaquet(taille);

  // 2. Retry REGISTER if not connected
  if (!connecte && millis() - dernierEnvoi > 3000) {
    envoyerRegister();
  }

  // 3. Send DATA only if state changed
  static bool dernierEtat = false;
  bool etat = placeOccupee(); // This now safely returns the latched state

  if (connecte && !enAttenteACK && etat != dernierEtat) {
    envoyerData();
    dernierEtat = etat;
  }

  // 4. Retry DATA if ACK timed out
  if (enAttenteACK && millis() - dernierEnvoi > timeoutACK) {
    if (nbRetry >= 5) {
      ShowSerial.println("[CAPTEUR] perte connexion → reset");
      connecte = false;
      enAttenteACK = false;
      nbRetry = 0;
      envoyerRegister();
    } else {
      nbRetry++;
      ShowSerial.print("[CAPTEUR] RETRY DATA #");
      ShowSerial.println(nbRetry);
      envoyerData();
    }
  }
}
