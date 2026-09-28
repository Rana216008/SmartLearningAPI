#ifndef CARDS_H
#define CARDS_H

#include <Arduino.h>

// صور العرض المتوفرة على الذاكرة المحلية
#include "A.h"
#include "B.h"
#include "C.h"
#include "AR.h"
#include "BT.h"
#include "T.h"
#include "RED.h"
#include "GREEN.h"
#include "BLUE.h"
#include "R1.h"
#include "R3.h"

// تعاريف المنافذ (Pins)
#define DFPLAYER_RX_PIN 16
#define DFPLAYER_TX_PIN 17
#define TFT_BL          21
#define RFID_SS         5
#define RFID_RST        22

// هيكل الكروت والصور
struct ImageResource {
    const char* uid;
    int TrackNumber;
    int QuizTrackNumber;
    const char* displayName;
    const unsigned char* imageData;
    size_t imageSize;
};

// جدول الكروت المخزنة محلياً
static const ImageResource images[] = {
    // { UID, TrackNumber, QuizTrackNumber, displayName, imageData, imageSize }
    {"B8 30 24 A2", 4, 13, "A",     A,     A_size},     // حرف A
    {"48 27 DB A2", 2, 11, "ب",     BT,    BT_size},    // حرف الباء
    {"A8 5F 7C A2", 3, 12, "ت",     T,     T_size},     // حرف التاء
    {"58 05 A5 A2", 1, 10, "أ",     AR,    AR_size},    // حرف الألف
    {"13 84 98 AA", 5, 14, "B",     B,     B_size},     // حرف B
    {"E2 AD B9 89", 6, 15, "C",     C,     C_size},     // حرف C
    {"F3 DB FD A6", 7, 16, "RED",   RED,   RED_size},   // أحمر
    {"13 18 76 BD", 8, 17, "GREEN", GREEN, GREEN_size}, // أخضر
    {"23 26 B1 1B", 9, 18, "BLUE",  BLUE,  BLUE_size}   // أزرق
};

static const size_t TOTAL_IMAGES = sizeof(images) / sizeof(images[0]);

// دالة تحويل UID إلى النص المنسق
inline String buildUIDString(byte *uid, byte size) {
    String result = "";
    for (byte i = 0; i < size; i++) {
        if (uid[i] < 0x10) result += "0";
        result += String(uid[i], HEX);
        if (i < size - 1) result += " ";
    }
    result.toUpperCase();
    return result;
}

#endif