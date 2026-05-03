#ifndef RFID_MANAGER_H
#define RFID_MANAGER_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 5
#define RST_PIN 22

// استخدام extern أو تعريف الكائن هنا (بما أنها دوال بسيطة سنبقيها هكذا)
inline MFRC522 rfid(SS_PIN, RST_PIN);

inline void RFID_init() {
  SPI.begin();
  rfid.PCD_Init();
}

inline String RFID_read() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return "";

  String content = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    content += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) content += " ";
  }

  content.toUpperCase();
  rfid.PICC_HaltA();
  return content;
}

#endif