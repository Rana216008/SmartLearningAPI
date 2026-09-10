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

// ===== Current Category Setting =====
String currentCategory = "All";

// ============================================
// Robot Face Animation Timing
// ============================================
unsigned long lastFaceChange = 0;
const unsigned long FACE_CHANGE_INTERVAL = 2000;  // Change every 2 seconds

// ============================================
// Target Card UIDs (Definitions)
// ============================================
// English Cards
byte aUID[4] = {0xB8, 0x30, 0x24, 0xA2};   // A - Track 1
byte bUID[4] = {0x13, 0x84, 0x98, 0xAA};   // B - Track 5 (NEW)
byte cUID[4] = {0xE2, 0xAD, 0xB8, 0x89};   // C - Track 6 (NEW)

// Arabic Cards
byte arUID[4] = {0x58, 0x05, 0xA5, 0xA2};  // ا (Alef) - Track 4
byte btUID[4] = {0x48, 0x27, 0xDB, 0xA2};  // ب - Track 2
byte tUID[4] = {0xA8, 0x5F, 0x7C, 0xA2};   // ت - Track 3

// Colors Cards
byte redUID[4] = {0xF3, 0xDB, 0xFD, 0xA6};   // Red - Track 7
byte greenUID[4] = {0x13, 0x18, 0x76, 0xBD}; // Green - Track 8
byte blueUID[4] = {0x23, 0x26, 0xB1, 0x1B};  // Blue - Track 9

// ============================================
// SETUP
// ============================================
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("========================================");
    Serial.println("=== ROBOT SYSTEM STARTING ===");
    Serial.println("========================================");

    // --- Initialize Display + JPEG Decoder ---
    initDisplay();

    // --- Initialize DFPlayer ---
    Serial.println("[DFPlayer] Initializing serial...");
    dfSerial.begin(9600, SERIAL_8N1, DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);
    delay(1000);

    if (myDFPlayer.begin(dfSerial)) {
        Serial.println("[DFPlayer] ✅ DFPlayer initialized!");
        delay(100);
        myDFPlayer.volume(40);
        Serial.println("[DFPlayer] Volume set to 25");
    } else {
        Serial.println("[DFPlayer] ❌ DFPlayer initialization failed!");
    }

    // --- Show initial robot face AND play startup voice ---
    showTestCardScreen();
    Serial.println("[DISPLAY] Showing Robot Face Animation");

    // ============================================
    // STARTUP VOICE (PLAYS WITH ROBOT FACE)
    // ============================================
    playVoice(11);
    Serial.println("[DFPlayer] 🎵 Startup voice played (track 11)");
    delay(3000);  // Wait for the startup voice to finish

    // --- Initialize RFID ---
    Serial.println("[RFID] Initializing SPI bus...");
    SPI.begin();
    Serial.println("[RFID] SPI bus initialized.");

    hardResetMFRC522();

    Serial.println("[RFID] Initializing MFRC522...");
    mfrc522.PCD_Init();
    delay(100);

    if (isRFIDAlive()) {
        Serial.println("[RFID] ✅ RFID reader is responding!");
    } else {
        Serial.println("[RFID] ❌ RFID not responding! Attempting revival...");
        reviveRFID();
    }

    WiFi_init();

    Serial.println("[HEALTH] Checking server connection...");
    if (checkServerHealth()) {
        Serial.println("[HEALTH] ✅ Server connection confirmed.");
    } else {
        Serial.println("[HEALTH] ⚠️ Server unreachable. Will retry on card scan.");
    }

    Serial.println("[RFID] Reader ready. Waiting for cards...");
    Serial.println("========================================");
    Serial.println("Card Mappings (Local Fallback):");
    Serial.println("  B8 30 24 A2 -> Track 1 (A - English)");
    Serial.println("  48 27 DB A2 -> Track 2 (ب - Arabic) [BT Image]");
    Serial.println("  A8 5F 7C A2 -> Track 3 (ت - Arabic)");
    Serial.println("  58 05 A5 A2 -> Track 5 (Red - Colors)");
    Serial.println("========================================");
}

// ============================================
// LOOP
// ============================================
void loop() {
    static unsigned long lastRFIDCheck = 0;
    unsigned long currentTime = millis();

    // ============================================
    // STEP 1: ROBOT FACE ANIMATION (IDLE ONLY)
    // ============================================
    if (!examModeActive && examState == EXAM_IDLE) {
        if (currentTime - lastFaceChange >= FACE_CHANGE_INTERVAL) {
            lastFaceChange = currentTime;
            updateRobotFace();
        }
    } else {
        lastFaceChange = currentTime;
    }

    // ============================================
    // STEP 2: RFID HEALTH CHECK
    // ============================================
    if (currentTime - lastRFIDCheck > 5000) {
        lastRFIDCheck = currentTime;
        if (!isRFIDAlive()) {
            Serial.println("[RFID] 💀 RFID is DEAD! Reviving...");
            reviveRFID();
            if (isRFIDAlive()) {
                showTestCardScreen();
            }
        }
    }

    // ============================================
    // STEP 3: CHECK FOR NEW CARD
    // ============================================
    if (!mfrc522.PICC_IsNewCardPresent()) {
        delay(10);
        return;
    }

    // ============================================
    // STEP 4: READ UID
    // ============================================
    if (!mfrc522.PICC_ReadCardSerial()) {
        Serial.println("[RFID] Failed to read card UID!");
        delay(10);
        return;
    }

    Serial.println("[RFID] Card UID read successfully!");

    String uid = buildUIDString(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.print("[RFID] Card UID: ");
    Serial.println(uid);

    // ============================================
    // STEP 5: EXAM MODE STATE HANDLING
    // ============================================
    
    if (examState == EXAM_START) {
        Serial.println("[EXAM] 🎯 Card detected! Starting exam questions...");
        examState = EXAM_FQ;
        showCurrentQuestion();
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        return;
    }

    if (examModeActive && (examState == EXAM_FQ || examState == EXAM_SQ || examState == EXAM_TQ)) {
        ApiResponse result = sendUID(uid);
        
        if (result.mode == "Learning") {
            Serial.println("[EXAM] 📢 Mode changed back to Learning! Exiting Exam Mode...");
            examModeActive = false;
            examState = EXAM_IDLE;
            showTestCardScreen();
            
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
            return;
        }
        
        if (result.mode == "Exam" || !result.success) {
            processExamCard(uid);
        }
        
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        return;
    }

    // ============================================
    // STEP 6: NORMAL MODE (Learning)
    // ============================================

    bool matched = false;
    int trackToPlay = -1;

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[WiFi] Reconnecting...");
        WiFi.reconnect();
        delay(1000);
    }

    ApiResponse result = sendUID(uid);
    
    if (result.success) { 
        Serial.println("[API] ✅ Server responded!");
        
        if (result.mode == "Exam") {
            Serial.println("[EXAM] 🎯 Exam Mode triggered!");
            startExamMode();
            
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
            return;
        }
        
        if (result.category != "") {
            currentCategory = result.category;
            Serial.print("[CATEGORY] Updated from server: ");
            Serial.println(currentCategory);
        }
        
        if (result.action == "wrong_category") {
            Serial.println("[API] Wrong category!");
            
            String category = "English";
            String msg = result.message;
            
            if (msg.indexOf("English") >= 0) {
                category = "English";
            } else if (msg.indexOf("Arabic") >= 0) {
                category = "Arabic";
            } else if (msg.indexOf("Colors") >= 0) {
                category = "Colors";
            }
            
            currentCategory = category;
            Serial.print("[CATEGORY] Remembered: ");
            Serial.println(currentCategory);
            
            showCategoryError(category);
            delay(4000);
            showTestCardScreen();
            
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
            return;
        }
        
        if (result.track > 0 && result.track >= 1 && result.track <= 7) {
            trackToPlay = result.track;
            matched = true;
            Serial.println("[API] Server returned track: " + String(trackToPlay));
        }
    } else {
        Serial.println("[API] ⚠️ Server failed. Using local fallback...");
    }

    // ============================================
    // STEP 7: LOCAL FALLBACK
    // ============================================
    if (!matched) {
        if (mfrc522.uid.size == 4) {
            bool isA = compareUID(mfrc522.uid.uidByte, aUID, 4);
            bool isB = compareUID(mfrc522.uid.uidByte, bUID, 4);
            bool isC = compareUID(mfrc522.uid.uidByte, cUID, 4);
            bool isAR = compareUID(mfrc522.uid.uidByte, arUID, 4);
            bool isBT = compareUID(mfrc522.uid.uidByte, btUID, 4);
            bool isT = compareUID(mfrc522.uid.uidByte, tUID, 4);
            bool isRed = compareUID(mfrc522.uid.uidByte, redUID, 4);
            bool isGreen = compareUID(mfrc522.uid.uidByte, greenUID, 4);
            bool isBlue = compareUID(mfrc522.uid.uidByte, blueUID, 4);
            
            String cardCategory = "";
            if (isA || isB || isC) cardCategory = "English";
            else if (isAR || isBT || isT) cardCategory = "Arabic";
            else if (isRed || isGreen || isBlue) cardCategory = "Colors";
            
            if (cardCategory != "" && (currentCategory == "All" || cardCategory == currentCategory)) {
                if (isA) {
                    trackToPlay = 1;
                    matched = true;
                    Serial.println("[LOCAL] A card (English) - Category allowed!");
                } else if (isB) {
                    trackToPlay = 5;
                    matched = true;
                    Serial.println("[LOCAL] B card (English) - Category allowed!");
                } else if (isC) {
                    trackToPlay = 6;
                    matched = true;
                    Serial.println("[LOCAL] C card (English) - Category allowed!");
                } else if (isAR) {
                    trackToPlay = 4;
                    matched = true;
                    Serial.println("[LOCAL] ا card (Arabic) - Category allowed!");
                } else if (isBT) {
                    trackToPlay = 2;
                    matched = true;
                    Serial.println("[LOCAL] ب card (Arabic) - Category allowed!");
                } else if (isT) {
                    trackToPlay = 3;
                    matched = true;
                    Serial.println("[LOCAL] ت card (Arabic) - Category allowed!");
                } else if (isRed) {
                    trackToPlay = 7;
                    matched = true;
                    Serial.println("[LOCAL] Red card (Colors) - Category allowed!");
                } else if (isGreen) {
                    trackToPlay = 8;
                    matched = true;
                    Serial.println("[LOCAL] Green card (Colors) - Category allowed!");
                } else if (isBlue) {
                    trackToPlay = 9;
                    matched = true;
                    Serial.println("[LOCAL] Blue card (Colors) - Category allowed!");
                }
            } else if (cardCategory != "") {
                Serial.println("[LOCAL] ❌ Card blocked! Category mismatch.");
                Serial.print("[LOCAL] Card category: ");
                Serial.print(cardCategory);
                Serial.print(" | Allowed: ");
                Serial.println(currentCategory);
                
                showCategoryError(currentCategory);
                delay(4000);
                showTestCardScreen();
                
                mfrc522.PICC_HaltA();
                mfrc522.PCD_StopCrypto1();
                return;
            }
        }
    }

    // ============================================
    // STEP 8: DISPLAY
    // ============================================
    if (matched) {
        displayImageByTrack(trackToPlay);
        delay(5000);
        showTestCardScreen();
        Serial.println("[DISPLAY] Returned to 'Test your card'");
    } 

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}