#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>

#include "API_Manager.h"
#include "Config.h"
#include "DisplayManager.h"
#include "RFIDManager.h"
#include "DFPlayerManager.h"
#include "ExamMode.h"

// ===== Headers for Card Images =====
#include "A.h"
#include "B.h"
#include "C.h"
#include "AR.h"
#include "BT.h"
#include "T.h"
#include "RED.h"
#include "GREEN.h"
#include "BLUE.h"
#include "CA.h"
#include "WA.h"
#include "TM.h"
#include "Q.h"
#include "R1.h"
#include "R3.h"
// ===== Central Cards Database Definition =====
const CardModel cards[] = {
    // UID                     Image, Size      Name    Voice Track  Category
    {{0xB8, 0x30, 0x24, 0xA2}, A,     A_size,    "A",    13,   1,     "English"},
    {{0x13, 0x84, 0x98, 0xAA}, B,     B_size,    "B",    12,   5,     "English"},
    {{0xE2, 0xAD, 0xB8, 0x89}, C,     C_size,    "C",    10,   6,     "English"},
    {{0x58, 0x05, 0xA5, 0xA2}, AR,    AR_size,   "AR",   16,   4,     "Arabic"},
    {{0x48, 0x27, 0xDB, 0xA2}, BT,    BT_size,   "BT",   15,   2,     "Arabic"},
    {{0xA8, 0x5F, 0x7C, 0xA2}, T,     T_size,    "T",    14,   3,     "Arabic"},
    {{0xF3, 0xDB, 0xFD, 0xA6}, RED,   RED_size,  "RED",  9,    7,     "Colors"},
    {{0x13, 0x18, 0x76, 0xBD}, GREEN, GREEN_size,"GREEN",8,    8,     "Colors"},
    {{0x23, 0x26, 0xB1, 0x1B}, BLUE,  BLUE_size, "BLUE", 7,    9,     "Colors"}
};

const size_t TOTAL_CARDS = sizeof(cards) / sizeof(cards[0]);

// ===== Global System State =====
String currentCategory = "All";
unsigned long lastFaceChange = 0;
const unsigned long FACE_CHANGE_INTERVAL = 2000;

// Helper function to find local card by UID
int findLocalCardIndex(byte* uid) {
    for (size_t i = 0; i < TOTAL_CARDS; i++) {
        if (compareUID(uid, (byte*)cards[i].uid, 4)) {
            return i;
        }
    }
    return -1;
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("========================================");
    Serial.println("=== ROBOT SYSTEM STARTING ===");
    Serial.println("========================================");

    initDisplay();

    dfSerial.begin(9600, SERIAL_8N1, DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);
    delay(1000);

    if (myDFPlayer.begin(dfSerial)) {
        Serial.println("[DFPlayer] ✅ DFPlayer initialized!");
        myDFPlayer.volume(40);
    } else {
        Serial.println("[DFPlayer] ❌ DFPlayer initialization failed!");
    }

    showTestCardScreen();
    playVoice(11); // Startup sound
    delay(3000);

    SPI.begin();
    hardResetMFRC522();
    mfrc522.PCD_Init();
    delay(100);

    if (isRFIDAlive()) {
        Serial.println("[RFID] ✅ RFID reader ready!");
    } else {
        reviveRFID();
    }

    WiFi_init();

    if (checkServerHealth()) {
        Serial.println("[HEALTH] ✅ Server connected.");
    } else {
        Serial.println("[HEALTH] ⚠️ Server unreachable. Will use Local Fallback.");
    }
}

void loop() {
    static unsigned long lastRFIDCheck = 0;
    unsigned long currentTime = millis();

    // 1. Idle Robot Face Animation
    if (!examModeActive && examState == EXAM_IDLE) {
        if (currentTime - lastFaceChange >= FACE_CHANGE_INTERVAL) {
            lastFaceChange = currentTime;
            updateRobotFace();
        }
    } else {
        lastFaceChange = currentTime;
    }

    // 2. Periodic RFID Health Monitor
    if (currentTime - lastRFIDCheck > 5000) {
        lastRFIDCheck = currentTime;
        if (!isRFIDAlive()) {
            reviveRFID();
            if (isRFIDAlive()) showTestCardScreen();
        }
    }

    // 3. Scan RFID Card
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        delay(10);
        return;
    }

    String uidStr = buildUIDString(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println("[RFID] Card scanned: " + uidStr);

    // 4. Handle Exam Mode States
    if (examState == EXAM_START) {
        examState = EXAM_FQ;
        showCurrentQuestion();
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        return;
    }

    if (examModeActive && (examState == EXAM_FQ || examState == EXAM_SQ || examState == EXAM_TQ)) {
        ApiResponse result = sendUID(uidStr);
        if (result.mode == "Learning") {
            examModeActive = false;
            examState = EXAM_IDLE;
            showTestCardScreen();
        } else {
            processExamCard(uidStr);
        }
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        return;
    }

    // 5. Handle Normal / Learning Mode (Server Priority)
    bool matched = false;
    int trackToPlay = -1;

    if (WiFi.status() != WL_CONNECTED) WiFi.reconnect();

    ApiResponse result = sendUID(uidStr);

    if (result.success) {
        if (result.mode == "Exam") {
            startExamMode();
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
            return;
        }

        if (result.category != "") currentCategory = result.category;

        if (result.action == "wrong_category") {
            showCategoryError(currentCategory);
            delay(4000);
            showTestCardScreen();
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
            return;
        }

        if (result.track > 0) {
            trackToPlay = result.track;
            matched = true;
        }
    }

    // 6. Local Fallback (If Server Fails)
    if (!matched && mfrc522.uid.size == 4) {
        int cardIdx = findLocalCardIndex(mfrc522.uid.uidByte);
        
        if (cardIdx != -1) {
            String cardCat = cards[cardIdx].category;
            if (currentCategory == "All" || currentCategory == cardCat) {
                trackToPlay = cards[cardIdx].trackNumber;
                matched = true;
            } else {
                showCategoryError(currentCategory);
                delay(4000);
                showTestCardScreen();
                mfrc522.PICC_HaltA();
                mfrc522.PCD_StopCrypto1();
                return;
            }
        }
    }

    // 7. Render Result
    if (matched) {
        displayImageByTrack(trackToPlay);
        delay(5000);
        showTestCardScreen();
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}