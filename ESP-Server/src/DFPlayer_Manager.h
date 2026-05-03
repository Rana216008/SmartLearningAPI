#ifndef DFPLAYER_MANAGER_H
#define DFPLAYER_MANAGER_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define BUSY_PIN 4

inline HardwareSerial mySerial(2);
inline DFRobotDFPlayerMini player;

inline void DF_init() {
  mySerial.begin(9600, SERIAL_8N1, 16, 17);
  pinMode(BUSY_PIN, INPUT);

  if (!player.begin(mySerial)) {
    Serial.println("DFPlayer Error");
    // حذفنا while(true) لضمان عدم تعليق الكود بالكامل
  }
  player.volume(30);
}

inline void playTrack(int track) {
  player.play(track);
  while (digitalRead(BUSY_PIN) == LOW) {
    delay(10);
  }
}

#endif