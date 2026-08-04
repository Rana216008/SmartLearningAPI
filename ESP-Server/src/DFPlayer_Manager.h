#ifndef DFPLAYER_MANAGER_H
#define DFPLAYER_MANAGER_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define BUSY_PIN 4

inline HardwareSerial mySerial(2);
inline DFRobotDFPlayerMini player;

inline void DF_init() {
  // تهيئة السيريال رقم 2 على المنافذ (RX=16, TX=17)
  mySerial.begin(9600, SERIAL_8N1, 16, 17);
  pinMode(BUSY_PIN, INPUT);

  Serial.println("Initializing DFPlayer...");
  
  int attempts = 0;
  // محاولة الاتصال حتى 5 مرات مع مهلة ثانية بين كل محاولة لضمان استقرار القطعة
  while (!player.begin(mySerial) && attempts < 5) {
    delay(1000);
    attempts++;
    Serial.print("Retrying DFPlayer connection... Attempt ");
    Serial.println(attempts);
  }

  if (attempts >= 5) {
    Serial.println("DFPlayer Error: Hardware connection failed.");
  } else {
    Serial.println("DFPlayer Connected Successfully!");
    player.volume(30); // ضبط مستوى الصوت (من 0 إلى 30)
  }
}

// دالة تشغيل تراك واحد (تُستخدم للتعلم، للاختبار، وللإجابات الصحيحة والخاطئة)
inline void playTrack(int track) {
  Serial.print("DFPlayer - Playing track: ");
  Serial.println(track);
  
  player.play(track);
  
  // مهلة كافية لمعالجة الأمر الصوتي ومنع المقاطعة الفورية
  delay(500); 
}

#endif