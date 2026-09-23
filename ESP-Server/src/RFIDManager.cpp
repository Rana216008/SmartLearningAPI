#include "RFIDManager.h"
#include "Config.h"
#include <SPI.h>

MFRC522 mfrc522(RFID_SS, RFID_RST);

void hardResetMFRC522() {
    Serial.println("[RFID] 🔄 Performing HARD RESET...");
    pinMode(RFID_RST, OUTPUT);
    digitalWrite(RFID_RST, LOW);
    delay(150);
    digitalWrite(RFID_RST, HIGH);
    delay(150);
    mfrc522.PCD_Reset();
    delay(150);
    Serial.println("[RFID] ✅ Hard reset complete.");
}

bool isRFIDAlive() {
    byte version = mfrc522.PCD_ReadRegister(MFRC522::VersionReg);
    return (version == 0x92 || version == 0x91);
}

bool reviveRFID() {
    Serial.println("[RFID] ⚠️ Attempting revival...");
    for (int attempt = 0; attempt < 3; attempt++) {
        hardResetMFRC522();
        mfrc522.PCD_Init();
        delay(100);
        if (isRFIDAlive()) {
            Serial.println("[RFID] ✅ Revived!");
            return true;
        }
        delay(200);
    }
    Serial.println("[RFID] ❌ Failed to revive!");
    return false;
}