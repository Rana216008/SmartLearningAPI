#include <Arduino.h>
#include "RFID_Manager.h"
#include "DFPlayer_Manager.h"
#include "Cards.h"
#include "API_Manager.h"
#include "Display_Manager.h"
#include "SPIFFS.h"

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Error");
  }

  Display_init(); // تهيئة شاشة TFT
  RFID_init();    // تهيئة قارئ RFID
  DF_init();      // تهيئة مشغل الصوت DFPlayer
  WiFi_init();    // تهيئة الاتصال بالشبكة

  Serial.println("\n----------------------------------");
  Serial.println("       --- System Ready ---       ");
  Serial.println("----------------------------------\n");
}

void loop() {
  String uid = RFID_read();
  if (uid == "") return;

  Serial.println("\n==================================");
  Serial.print("💳 New Card Detected: ");
  Serial.println(uid);

  ApiResponse result = sendUID(uid);

  // إذا تم استلام رقم تراك صحيح من الخادم
  if (result.track > 0) {
    String cardName = getCardNameByTrack(result.track);
    String imageName = getImageNameByTrack(result.track);

    Serial.println("----------------------------------");
    Serial.print("🔹 Card UID:    "); Serial.println(uid);
    Serial.print("🔤 Card Name:   "); Serial.println(cardName);
    Serial.print("🔊 Audio Track: "); Serial.println(result.track);
    Serial.print("🖼️ Image Name:  "); Serial.println(imageName);
    Serial.print("🌐 Server Action: "); Serial.println(result.action);
    Serial.println("----------------------------------");

    // 1. عرض البطاقة المصورة على الشاشة
    displayTrackUI(result.track);

    // 2. تشغيل المقطع الصوتي المطابق
    playTrack(result.track);
    
    delay(1500); 
  } 
  else {
    // الوضع الاحتياطي المحلي في حال انقطاع السيرفر أو عدم التعرف عليه
    Serial.println("⚠️ Server unavailable/No Track returned. Switching to Local Backup...");
    
    bool cardFoundLocally = false;
    for (int i = 0; i < totalCards; i++) {
      if (uid == cards[i].uid) {
        Serial.println("----------------------------------");
        Serial.print("🔹 Card UID:    "); Serial.println(cards[i].uid);
        Serial.print("🔤 Card Name:   "); Serial.println(cards[i].name);
        Serial.print("🔊 Audio Track: "); Serial.println(cards[i].track);
        Serial.print("🖼️ Image Name:  "); Serial.println(cards[i].imageName);
        Serial.println("----------------------------------");

        // 1. عرض الرسومات من النظام المحلي
        displayTrackUI(cards[i].track);

        // 2. تشغيل الصوت محلياً
        playTrack(cards[i].track);

        cardFoundLocally = true;
        break;
      }
    }
    
    if (!cardFoundLocally) {
      Serial.println("❌ Card not found in local backup list.");
    }
    delay(1500);
  }
}