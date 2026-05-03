#include <Arduino.h>
#include "RFID_Manager.h"
#include "DFPlayer_Manager.h"
#include "Cards.h"
#include "API_Manager.h"
#include "SPIFFS.h"

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Error");
  }

  RFID_init();
  DF_init();
  WiFi_init(); 

  Serial.println("--- System Ready ---");
}

void loop() {
  String uid = RFID_read();
  if (uid == "") return;

  Serial.print("\nNew Card Detected: ");
  Serial.println(uid);

  ApiResponse result = sendUID(uid);

  if (result.track > 0) {
    Serial.print("Server Action: ");
    Serial.println(result.action);
    playTrack(result.track); 
    delay(2000); 
  } 
  else {
    Serial.println("Server unavailable or UID not in DB. Checking Local Data...");
    
    bool foundLocal = false;
    for (int i = 0; i < totalCards; i++) {
      if (uid == cards[i].uid) {
        Serial.print("Playing Local Track: ");
        Serial.println(cards[i].name);
        playTrack(cards[i].track);
        foundLocal = true;
        break;
      }
    }

    if (!foundLocal) {
      Serial.println("Unknown Card!");
    }
    delay(2000);
  }
}