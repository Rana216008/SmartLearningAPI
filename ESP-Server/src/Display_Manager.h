#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "Drawings.h"

inline TFT_eSPI tft = TFT_eSPI();

const int backlightPin = 21;
const int ledChannel   = 0;
const int ledFreq      = 5000;
const int ledResolution= 8;

inline void Display_init() {
    tft.init();
    tft.setRotation(1); // وضع أفقي

    ledcSetup(ledChannel, ledFreq, ledResolution);
    ledcAttachPin(backlightPin, ledChannel);
    ledcWrite(ledChannel, 255); // تشغيل الإضاءة كاملة

    // عرض شاشة البداية
    SplashScreen();
}

// دالة اختيار الرسمة بناءً على رقم التراك
inline void displayTrackUI(int track) {
    switch (track) {
        case 1: AlefCard(); break;
        case 2: BaaCard(); break;
        case 3: TaaCard(); break;
        case 4: ACard(); break;
        case 5: BCard(); break;
        case 6: CCard(); break;
        case 7: RedColorCard(); break;
        case 8: GreenColorCard(); break;
        case 9: BlueColorCard(); break;
        // case 10: drawCircleCard(); break;
        default:
            tft.fillScreen(TFT_WHITE);
            drawStandardCardTemplate();
            tft.setTextSize(3);
            tft.setTextColor(TFT_BLACK, TFT_WHITE);
            tft.setCursor(140, 140);
            tft.print("Track: ");
            tft.print(track);
            break;
    }
}

#endif