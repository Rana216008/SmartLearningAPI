#include "DisplayManager.h"
#include "DFPlayerManager.h"

// // ===== Include Image Headers =====
// #include "C.h"
// #include "A.h"
// #include "AR.h"
// #include "B.h"
// #include "BLUE.h"
// #include "BT.h"
// #include "GREEN.h"
// #include "RED.h"
// #include "T.h"
#include "R1.h"  // Eyes open
#include "R3.h"  // Eyes closed

TFT_eSPI tft = TFT_eSPI();
static bool robotEyesOpen = true;

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
    if (y >= tft.height()) return 0;
    tft.pushImage(x, y, w, h, bitmap);
    return 1;
}

void initDisplay() {
    Serial.println("[DISPLAY] Initializing...");
    tft.init();
    tft.setRotation(1);
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(tft_output);
    Serial.println("[DISPLAY] OK");
}

void displayImage(const unsigned char* jpegData, size_t jpegSize, const char* name) {
    Serial.printf("[DISPLAY] Displaying: %s (Size: %d)\n", name, jpegSize);
    if (!TJpgDec.drawJpg(0, 0, jpegData, jpegSize)) {
        Serial.println("[DISPLAY] ERROR! Check JPEG data.");
    }
}

void displayTrackImage(int track) {
    for (size_t i = 0; i < TOTAL_CARDS; i++) {
        if (cards[i].trackNumber == track) {
            displayImage(cards[i].imageData, cards[i].imageSize, cards[i].imageName);
            return;
        }
    }
    Serial.println("[DISPLAY] Unknown track!");
    showTestCardScreen();
}

void displayImageByTrack(int track) {
    for (size_t i = 0; i < TOTAL_CARDS; i++) {
        if (cards[i].trackNumber == track) {
            displayImage(cards[i].imageData, cards[i].imageSize, cards[i].imageName);
            playVoice(cards[i].voiceTrack);
            return;
        }
    }
    Serial.println("[DISPLAY] Unknown track!");
    showTestCardScreen();
}

void showTestCardScreen() {
    if (robotEyesOpen) {
        displayImage(R1, R1_size, "Robot Face - Eyes Open");
    } else {
        displayImage(R3, R3_size, "Robot Face - Eyes Closed");
    }
}

void updateRobotFace() {
    robotEyesOpen = !robotEyesOpen;
    showTestCardScreen();
}

void showCategoryError(String category) {
    if (category == "English") {
        displayImage(R1, R1_size, "Error English");
        playVoice(21);
    } else if (category == "Arabic") {
        displayImage(R1, R1_size, "Error Arabic");
        playVoice(20);
    } else if (category == "Colors") {
        displayImage(R3, R3_size, "Error Colors");
        playVoice(19);
    } else {
        displayImage(R3, R3_size, "Error Fallback");
        playVoice(21);
    }
}