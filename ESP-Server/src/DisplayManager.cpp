#include "DisplayManager.h"
#include "Cards.h"
#include <TJpg_Decoder.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
static bool robotEyesOpen = true;

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
    if (y >= tft.height()) return 0;
    tft.pushImage(x, y, w, h, bitmap);
    return 1;
}

void initDisplay() {
    tft.init();
    tft.setRotation(1);
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(tft_output);
}

void displayImageByTrack(int track) {
    if (track <= 0) return;

    for (size_t i = 0; i < TOTAL_IMAGES; i++) {
        // البحث برقم التراك العادي أو رقم تراك السؤال
        if (images[i].TrackNumber == track || images[i].QuizTrackNumber == track) {
            Serial.printf("[Display] 🖼️ Showing Image: %s for Track: %d\n", images[i].displayName, track);
            TJpgDec.drawJpg(0, 0, images[i].imageData, images[i].imageSize);
            // delay(4000);
            return;
        }
    }
    
    Serial.printf("[Display] ⚠️ Image for Track %d not found in Cards.h!\n", track);
    showDefaultFace();
}

void showDefaultFace() {
    TJpgDec.drawJpg(0, 0, R3, R3_size);
    robotEyesOpen = false;
    
    delay(150); 
    
    TJpgDec.drawJpg(0, 0, R1, R1_size);
    robotEyesOpen = true;
    // if (robotEyesOpen) {
    //     TJpgDec.drawJpg(0, 0, R1, R1_size);
    // } else {
    //     TJpgDec.drawJpg(0, 0, R3, R3_size);
    // }
}

void updateRobotFaceAnimation() {
    robotEyesOpen = !robotEyesOpen;
    showDefaultFace();
}