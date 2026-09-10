#include "DisplayManager.h"
#include "Config.h"
#include "DFPlayerManager.h"

// ===== All Track / Error / Face Images =====
#include "C.h"
#include "A.h"
#include "AR.h"
#include "B.h"
#include "BLUE.h"
#include "BT.h"
#include "GREEN.h"
#include "RED.h"
#include "T.h"
#include "EE.h"
#include "EA.h"
#include "EC.h"
#include "R1.h"  // Eyes open
#include "R3.h"  // Eyes closed

// ===== Object Initialization =====
TFT_eSPI tft = TFT_eSPI();

// ===== Robot Face State =====
static bool robotEyesOpen = true;

// ============================================
// CALLBACK FUNCTION for TJpg_Decoder
// ============================================
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
    if (y >= tft.height()) return 0;
    tft.pushImage(x, y, w, h, bitmap);
    return 1;
}

// ============================================
// Initialize Display + JPEG Decoder
// ============================================
void initDisplay() {
    Serial.println("[DISPLAY] Initializing...");
    tft.init();
    tft.setRotation(1);
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    Serial.println("[DISPLAY] OK");

    Serial.println("[JPEG] Initializing...");
    TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(tft_output);
    Serial.println("[JPEG] OK");
}

// ============================================
// Display Any JPEG
// ============================================
void displayImage(const unsigned char* jpegData, size_t jpegSize, const char* name) {
    Serial.print("[DISPLAY] Displaying: ");
    Serial.println(name);
    Serial.print("[DISPLAY] JPEG size: ");
    Serial.println(jpegSize);
    
    bool result = TJpgDec.drawJpg(0, 0, jpegData, jpegSize);
    if (result) {
        Serial.println("[DISPLAY] OK");
    } else {
        Serial.println("[DISPLAY] ERROR! Check JPEG data.");
    }
}

// ============================================
// Display Track Image (no voice)
// ============================================
void displayTrackImage(int track) {
    switch(track) {
        case 1:  displayImage(A, A_size, "A"); break;
        case 2:  displayImage(BT, BT_size, "BT (ب)"); break;
        case 3:  displayImage(T, T_size, "T"); break;
        case 4:  displayImage(AR, AR_size, "AR (ا)"); break;
        case 5:  displayImage(B, B_size, "B"); break;
        case 6:  displayImage(C, C_size, "C"); break;
        case 7:  displayImage(RED, RED_size, "RED"); break;
        case 8:  displayImage(GREEN, GREEN_size, "GREEN"); break;
        case 9:  displayImage(BLUE, BLUE_size, "BLUE"); break;
        default:
            Serial.println("[DISPLAY] Unknown track!");
            showTestCardScreen();
            break;
    }
}

// ============================================
// Display Track Image + Play Voice
// ============================================
void displayImageByTrack(int track) {
    Serial.print("[DISPLAY] Displaying image for track: ");
    Serial.println(track);
    
    switch(track) {
        case 1:  displayImage(A, A_size, "A");          playVoice(13); break;
        case 2:  displayImage(BT, BT_size, "BT (ب)");   playVoice(15); break;
        case 3:  displayImage(T, T_size, "T");          playVoice(14); break;
        case 4:  displayImage(AR, AR_size, "AR (ا)");   playVoice(16); break;
        case 5:  displayImage(B, B_size, "B");          playVoice(12); break;
        case 6:  displayImage(C, C_size, "C");          playVoice(10); break;
        case 7:  displayImage(RED, RED_size, "RED");    playVoice(9);  break;
        case 8:  displayImage(GREEN, GREEN_size, "GREEN"); playVoice(8); break;
        case 9:  displayImage(BLUE, BLUE_size, "BLUE"); playVoice(7);  break;
        default:
            Serial.println("[DISPLAY] Unknown track!");
            showTestCardScreen();
            break;
    }
}

// ============================================
// Robot Face (Idle Animation)
// ============================================
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

// ============================================
// Category Error Display
// ============================================
void showCategoryError(String category) {
    if (category == "English") {
        displayImage(EE, EE_size, "Error English");
        playVoice(21);
    } else if (category == "Arabic") {
        displayImage(EA, EA_size, "Error Arabic");
        playVoice(20);
    } else if (category == "Colors") {
        displayImage(EC, EC_size, "Error Colors");
        playVoice(19);
    } else {
        displayImage(EE, EE_size, "Error Fallback");
        playVoice(21);
    }
}