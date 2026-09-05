#include <Arduino.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <MFRC522.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include "API_Manager.h"

// ===== Include ALL your JPEG arrays =====
#include "C.h"
#include "A.h"
#include "AR.h"
#include "B.h"
#include "BLUE.h"
#include "BT.h"
#include "GREEN.h"
#include "RED.h"
#include "T.h"

// ===== Exam Mode Images =====
#include "TM.h"
#include "FQ.h"
#include "SQ.h"
#include "TQ.h"
#include "WA.h"
#include "CA.h"

// ===== Include Error Images =====
#include "EE.h"
#include "EA.h"
#include "EC.h"

// ===== Robot Face Images =====
#include "R1.h"  // Eyes open
#include "R3.h"  // Eyes closed

// ===== DFPlayer Pin Definitions =====
#define DFPLAYER_RX_PIN 16
#define DFPLAYER_TX_PIN 17

// ===== Current Category Setting =====
String currentCategory = "All";

// ===== Pin Definitions =====
#define TFT_BL 21
#define RFID_SS   5
#define RFID_RST  22

// ============================================
// ROBOT FACE ANIMATION STATE
// ============================================
bool robotEyesOpen = true;  // Start with eyes open
unsigned long lastFaceChange = 0;
const unsigned long FACE_CHANGE_INTERVAL = 2000;  // Change every 2 seconds

// ===== Object Initialization =====
TFT_eSPI tft = TFT_eSPI();
MFRC522 mfrc522(RFID_SS, RFID_RST);
HardwareSerial dfSerial(2);
DFRobotDFPlayerMini myDFPlayer;

// ===== Target Card UIDs =====
byte aUID[4] = {0xB8, 0x30, 0x24, 0xA2};
byte bUID[4] = {0x48, 0x27, 0xDB, 0xA2};
byte tUID[4] = {0xA8, 0x5F, 0x7C, 0xA2};
byte redUID[4] = {0x58, 0x05, 0xA5, 0xA2};

// ============================================
// EXAM MODE STATE MACHINE
// ============================================
enum ExamState {
    EXAM_IDLE,
    EXAM_START,
    EXAM_FQ,
    EXAM_SQ,
    EXAM_TQ,
    EXAM_SHOW_IMAGE,
    EXAM_WAIT_RETURN
};

ExamState examState = EXAM_IDLE;
String examTargetUID = "";
String examImageToShow = "";
bool examModeActive = false;
unsigned long examImageStartTime = 0;
const unsigned long IMAGE_DISPLAY_DURATION = 3000;

// ============================================
// DFPLAYER VOICE FUNCTIONS
// ============================================

void playVoice(int trackNumber) {
    myDFPlayer.play(trackNumber);
    Serial.print("[DFPlayer] Playing voice track: ");
    Serial.println(trackNumber);
}

// ============================================
// HARD RESET FUNCTION
// ============================================
void hardResetMFRC522() {
    Serial.println("[RFID] 🔄 Performing HARD RESET...");
    pinMode(RFID_RST, OUTPUT);
    digitalWrite(RFID_RST, LOW);
    delay(150);
    digitalWrite(RFID_RST, HIGH);
    delay(150);
    mfrc522.PCD_Reset();
    delay(150);
    Serial.println("[RFID] ✅ Hard reset complete.");
}

bool isRFIDAlive() {
    byte version = mfrc522.PCD_ReadRegister(MFRC522::VersionReg);
    return (version == 0x92 || version == 0x91);
}

bool reviveRFID() {
    Serial.println("[RFID] ⚠️ Attempting revival...");
    for (int attempt = 0; attempt < 3; attempt++) {
        hardResetMFRC522();
        mfrc522.PCD_Init();
        delay(100);
        if (isRFIDAlive()) {
            Serial.println("[RFID] ✅ Revived!");
            return true;
        }
        delay(200);
    }
    Serial.println("[RFID] ❌ Failed to revive!");
    return false;
}

// ============================================
// CALLBACK FUNCTION for TJpg_Decoder
// ============================================
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
    if (y >= tft.height()) return 0;
    tft.pushImage(x, y, w, h, bitmap);
    return 1;
}

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

void showCategoryError(String category) {
    if (category == "English") {
        displayImage(EE, EE_size, "Error English");
        playVoice(19);
    } else if (category == "Arabic") {
        displayImage(EA, EA_size, "Error Arabic");
        playVoice(19);
    } else if (category == "Colors") {
        displayImage(EC, EC_size, "Error Colors");
        playVoice(19);
    } else {
        displayImage(EE, EE_size, "Error Fallback");
        playVoice(19);
    }
}

// ============================================
// ROBOT FACE ANIMATION (LIVE FACE)
// ============================================
void showTestCardScreen() {
    // This function is called when the system is idle
    // It displays the current robot face (R1 or R2) based on the state
    // The actual animation timing is handled in loop()
    if (robotEyesOpen) {
        displayImage(R1, R1_size, "Robot Face - Eyes Open");
    } else {
        displayImage(R3, R3_size, "Robot Face - Eyes Closed");
    }
}

void updateRobotFace() {
    // Toggle between eyes open and closed
    robotEyesOpen = !robotEyesOpen;
    showTestCardScreen();
}

void displayImageByTrack(int track) {
    Serial.print("[DISPLAY] Displaying image for track: ");
    Serial.println(track);
    
    switch(track) {
        case 1:
            displayImage(A, A_size, "A");
            playVoice(4);
            break;
        case 2:
            displayImage(BT, BT_size, "BT (ب)");
            playVoice(2);
            break;
        case 3:
            displayImage(T, T_size, "T");
            playVoice(3);
            break;
        case 5:
            displayImage(RED, RED_size, "RED");
            playVoice(7);
            break;
        case 6:
            displayImage(GREEN, GREEN_size, "GREEN");
            playVoice(6);
            break;
        case 7:
            displayImage(BLUE, BLUE_size, "BLUE");
            playVoice(5);
            break;
        default:
            Serial.println("[DISPLAY] Unknown track!");
            showTestCardScreen();
            break;
    }
}

bool compareUID(byte *uid1, byte *uid2, byte size) {
    for (byte i = 0; i < size; i++) {
        if (uid1[i] != uid2[i]) return false;
    }
    return true;
}

String buildUIDString(byte *uid, byte size) {
    String result = "";
    for (byte i = 0; i < size; i++) {
        if (uid[i] < 0x10) result += "0";
        result += String(uid[i], HEX);
        if (i < size - 1) result += " ";
    }
    result.toUpperCase();
    return result;
}

// ============================================
// EXAM MODE FUNCTIONS
// ============================================

void startExamMode() {
    Serial.println("[EXAM] 🎯 Starting Exam Mode!");
    examModeActive = true;
    examState = EXAM_START;
    displayImage(TM, TM_size, "TM - Test Mode");
    // NO VOICE FOR TM
    examImageStartTime = millis();
}

void showCurrentQuestion() {
    switch(examState) {
        case EXAM_FQ:
            displayImage(FQ, FQ_size, "FQ - First Question (ب)");
            playVoice(11);
            break;
        case EXAM_SQ:
            displayImage(SQ, SQ_size, "SQ - Second Question (ت)");
            playVoice(12);
            break;
        case EXAM_TQ:
            displayImage(TQ, TQ_size, "TQ - Third Question (A)");
            playVoice(13);
            break;
        default:
            break;
    }
}

void handleCorrectAnswer(String imageName) {
    Serial.println("[EXAM] ✅ Correct Answer!");
    
    if (imageName == "BT") {
        displayImage(BT, BT_size, "BT - ب");
        playVoice(2);
    } else if (imageName == "T") {
        displayImage(T, T_size, "T - ت");
        playVoice(3);
    } else if (imageName == "A") {
        displayImage(A, A_size, "A - A");
        playVoice(4);
    }
    
    delay(3000);
    
    displayImage(CA, CA_size, "CA - Correct Answer!");
    playVoice(16);
    
    delay(3000);
    
    switch(examState) {
        case EXAM_FQ:
            examState = EXAM_SQ;
            showCurrentQuestion();
            break;
        case EXAM_SQ:
            examState = EXAM_TQ;
            showCurrentQuestion();
            break;
        case EXAM_TQ:
            examState = EXAM_FQ;
            showCurrentQuestion();
            break;
        default:
            break;
    }
}

void handleWrongAnswer() {
    Serial.println("[EXAM] ❌ Wrong Answer!");
    
    displayImage(WA, WA_size, "WA - Wrong Answer!");
    playVoice(18);
    delay(2000);
    
    showCurrentQuestion();
}

void processExamCard(String uid) {
    Serial.println("[EXAM] Processing card: " + uid);
    
    bool matched = false;
    
    byte readUID[4];
    int idx = 0;
    String temp = uid;
    temp.replace(" ", "");
    for (int i = 0; i < temp.length(); i += 2) {
        String hex = temp.substring(i, i+2);
        readUID[idx++] = (byte)strtol(hex.c_str(), NULL, 16);
    }
    
    switch(examState) {
        case EXAM_FQ:
            if (idx == 4 && compareUID(readUID, bUID, 4)) {
                handleCorrectAnswer("BT");
                matched = true;
            }
            break;
        case EXAM_SQ:
            if (idx == 4 && compareUID(readUID, tUID, 4)) {
                handleCorrectAnswer("T");
                matched = true;
            }
            break;
        case EXAM_TQ:
            if (idx == 4 && compareUID(readUID, aUID, 4)) {
                handleCorrectAnswer("A");
                matched = true;
            }
            break;
        default:
            break;
    }
    
    if (!matched) {
        handleWrongAnswer();
    }
}

// ============================================
// SETUP
// ============================================
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("========================================");
    Serial.println("=== ROBOT SYSTEM STARTING ===");
    Serial.println("========================================");

    // --- Initialize Display ---
    Serial.println("[DISPLAY] Initializing...");
    tft.init();
    tft.setRotation(1);
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    Serial.println("[DISPLAY] OK");

    // --- Initialize JPEG Decoder ---
    Serial.println("[JPEG] Initializing...");
    TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(tft_output);
    Serial.println("[JPEG] OK");

    // --- Initialize DFPlayer ---
    Serial.println("[DFPlayer] Initializing serial...");
    dfSerial.begin(9600, SERIAL_8N1, DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);
    delay(1000);

    if (myDFPlayer.begin(dfSerial)) {
        Serial.println("[DFPlayer] ✅ DFPlayer initialized!");
        myDFPlayer.volume(25);
        Serial.println("[DFPlayer] Volume set to 25");
    } else {
        Serial.println("[DFPlayer] ❌ DFPlayer initialization failed!");
    }

    // --- Show initial robot face ---
    showTestCardScreen();
    Serial.println("[DISPLAY] Showing Robot Face Animation");

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
    // STEP 1: UPDATE ROBOT FACE ANIMATION (IDLE ONLY)
    // ============================================
    // Only update face if:
    // 1. No card is being processed
    // 2. Not in exam mode
    // 3. System is idle
    if (!examModeActive && examState == EXAM_IDLE) {
        // Check if it's time to change the face
        if (currentTime - lastFaceChange >= FACE_CHANGE_INTERVAL) {
            lastFaceChange = currentTime;
            updateRobotFace();  // Toggle between R1 and R2
        }
    } else {
        // Reset the timer when not idle so animation resumes smoothly
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
    // STEP 4: CARD DETECTED - READ UID
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
    // STEP 6: NORMAL MODE (Learning Mode)
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

    if (!matched) {
        if (mfrc522.uid.size == 4) {
            bool isA = compareUID(mfrc522.uid.uidByte, aUID, 4);
            bool isB = compareUID(mfrc522.uid.uidByte, bUID, 4);
            bool isT = compareUID(mfrc522.uid.uidByte, tUID, 4);
            bool isRed = compareUID(mfrc522.uid.uidByte, redUID, 4);
            
            String cardCategory = "";
            if (isA) cardCategory = "English";
            else if (isB || isT) cardCategory = "Arabic";
            else if (isRed) cardCategory = "Colors";
            
            if (cardCategory != "" && (currentCategory == "All" || cardCategory == currentCategory)) {
                if (isA) {
                    trackToPlay = 1;
                    matched = true;
                    Serial.println("[LOCAL] A card (English) - Category allowed!");
                } else if (isB) {
                    trackToPlay = 2;
                    matched = true;
                    Serial.println("[LOCAL] ب card (Arabic) - Category allowed!");
                } else if (isT) {
                    trackToPlay = 3;
                    matched = true;
                    Serial.println("[LOCAL] ت card (Arabic) - Category allowed!");
                } else if (isRed) {
                    trackToPlay = 5;
                    matched = true;
                    Serial.println("[LOCAL] Red card (Colors) - Category allowed!");
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

    if (matched) {
        displayImageByTrack(trackToPlay);
        delay(5000);
        showTestCardScreen();
        Serial.println("[DISPLAY] Returned to 'Test your card'");
    } else {
        Serial.println("[RFID] Unknown card!");
        displayImage(EE, EE_size, "Unknown Card - Showing Error English");
        playVoice(19);
        delay(3000);
        showTestCardScreen();
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}