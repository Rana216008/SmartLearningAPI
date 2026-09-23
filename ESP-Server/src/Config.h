#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ===== Pin Definitions =====
#define DFPLAYER_RX_PIN 16
#define DFPLAYER_TX_PIN 17
#define TFT_BL          21
#define RFID_SS         5
#define RFID_RST        22

// ===== Card Data Structure =====
struct CardModel {
    byte uid[4];
    const unsigned char* imageData;
    size_t imageSize;
    const char* imageName;
    int voiceTrack;
    int trackNumber;
    String category;
};

// ===== External Declarations =====
extern const CardModel cards[];
extern const size_t TOTAL_CARDS;

// ===== Helper Functions =====
inline bool compareUID(byte *uid1, byte *uid2, byte size = 4) {
    for (byte i = 0; i < size; i++) {
        if (uid1[i] != uid2[i]) return false;
    }
    return true;
}

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