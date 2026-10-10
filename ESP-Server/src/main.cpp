#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "Cards.h"
#include "API_Manager.h"
#include "DisplayManager.h"
#include "RFIDManager.h"
#include "DFPlayerManager.h"

// المتغيرات العامة
unsigned long lastFaceChange = 0;
const unsigned long FACE_INTERVAL = 2000;
bool isExecutingAction = false;

String lastMode = "";
String lastCategory = "";
int lastQuizTrack = -1;
unsigned long lastStatusCheck = 0;
const unsigned long STATUS_CHECK_INTERVAL = 1200;

void playVoiceWithDelay(int track, unsigned long durationMs) {
    if (track <= 0) return;
    playVoice(track);
    delay(durationMs);
}

// دالة تشغيل صوت السؤال (بدون عرض صورة - مخصصة لوضع الاختبار)
void playQuestionSequence(int quizTrack) {
    if (quizTrack <= 0) return;

    Serial.printf("[Exam System] Playing Question Track: %d\n", quizTrack);
    
    // تم إلغاء displayImageByTrack من هنا حتى لا تظهر صورة مع السؤال
    playVoiceWithDelay(quizTrack, 5000); 
}

void onModeOrCategoryChanged(const String& mode, const String& category, int quizTrack) {
    Serial.println("[Event Triggered] Mode: " + mode + " | Category: " + category + " | Track: " + String(quizTrack));

    isExecutingAction = true;

    if (mode.equalsIgnoreCase("Exam")) {
        Serial.println("[Exam] Playing Intro Track 22...");
        playVoice(22); 
        delay(3500); 

        playQuestionSequence(quizTrack);
    } 
    else {
        if (category.equalsIgnoreCase("Arabic")) playVoice(19);
        else if (category.equalsIgnoreCase("English")) playVoice(20);
        else if (category.equalsIgnoreCase("Colors")) playVoice(21);
        else playVoice(26); 
        delay(2500);
    }

    showDefaultFace();
    isExecutingAction = false;
}

void executeServerAction(const ApiResponse& response) {
    if (!response.success) return;

    isExecutingAction = true;

    if (response.action.equalsIgnoreCase("correct_and_next")) {
        // تم إزالة عرض صورة الكارت الممسوح هنا لإيقاف الصور في وضع الاختبار

        // 1. تشغيل صوت التعزيز (أحسنت / إجابة صحيحة)
        int correctSoundTrack = (response.feedbackTrack > 0) ? response.feedbackTrack : 23;
        Serial.printf("[Exam] Correct! Playing Track: %d\n", correctSoundTrack);
        playVoiceWithDelay(correctSoundTrack, 2500); 

        // 2. تشغيل صوت السؤال الجديد
        if (response.audioTrack > 0) {
            lastQuizTrack = response.audioTrack; 
            Serial.printf("[Exam] Playing Next Question Track: %d\n", response.audioTrack);
            playQuestionSequence(response.audioTrack); 
        } 
        else {
            showDefaultFace();
        }
    } 
    else if (response.action.equalsIgnoreCase("wrong")) {
        showDefaultFace();

        Serial.println("[Exam] Playing Wrong Sound Track 25...");
        playVoiceWithDelay(25, 2500);

        if (lastQuizTrack > 0) {
            playQuestionSequence(lastQuizTrack);
        }
    }
    else if (response.action.equalsIgnoreCase("ask_question")) {
        showDefaultFace();
        if (response.audioTrack > 0) {
            lastQuizTrack = response.audioTrack;
            playQuestionSequence(response.audioTrack);
        }
    }
    else {
        // الوضع العادي (التعلم)
        
        if (response.imageTrack > 0) {
            displayImageByTrack(response.imageTrack);
        }
        if (response.audioTrack > 0) {
            playVoice(response.audioTrack);
        } 
        delay(5000); 

        showDefaultFace();
        isExecutingAction = false;
    }
   
    showDefaultFace();
    isExecutingAction = false;
}

// void checkServerStatus() {
//     if (WiFi.status() != WL_CONNECTED || isExecutingAction) return;

//     HTTPClient http;
//     http.begin(statusUrl);
//     http.setTimeout(1000); 

//     int httpCode = http.GET();

//     if (httpCode == HTTP_CODE_OK) {
//         String payload = http.getString();
        
//         JsonDocument doc;
//         DeserializationError error = deserializeJson(doc, payload);

//         if (!error) {
//             String currentMode = doc["mode"] | "";
//             String currentCategory = doc["category"] | "";
//             int quizTrack = doc["quizTrack"] | -1;

//             if (lastMode == "" && lastCategory == "") {
//                 lastMode = currentMode;
//                 lastCategory = currentCategory;
//                 lastQuizTrack = quizTrack;

//                 if (currentMode.equalsIgnoreCase("Exam") && quizTrack > 0) {
//                     onModeOrCategoryChanged(currentMode, currentCategory, quizTrack);
//                 }
//             }
//             else if (currentMode != lastMode || currentCategory != lastCategory || (currentMode.equalsIgnoreCase("Exam") && quizTrack != lastQuizTrack && quizTrack > 0)) {
//                 lastMode = currentMode;
//                 lastCategory = currentCategory;
//                 lastQuizTrack = quizTrack;
                
//                 onModeOrCategoryChanged(currentMode, currentCategory, quizTrack);
//             }
//         }
//     }
//     http.end();
// }
void checkServerStatus() {
    // التأكد من وجود لابتوب/جهاز متصل بشبكة الـ ESP32
    if (WiFi.softAPgetStationNum() == 0 || isExecutingAction) return;

    HTTPClient http;
    http.begin(statusUrl);
    http.setTimeout(1000); 

    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
            String currentMode = doc["mode"] | "";
            String currentCategory = doc["category"] | "";
            int quizTrack = doc["quizTrack"] | -1;

            if (lastMode == "" && lastCategory == "") {
                lastMode = currentMode;
                lastCategory = currentCategory;
                lastQuizTrack = quizTrack;

                if (currentMode.equalsIgnoreCase("Exam") && quizTrack > 0) {
                    onModeOrCategoryChanged(currentMode, currentCategory, quizTrack);
                }
            }
            else if (currentMode != lastMode || currentCategory != lastCategory || (currentMode.equalsIgnoreCase("Exam") && quizTrack != lastQuizTrack && quizTrack > 0)) {
                lastMode = currentMode;
                lastCategory = currentCategory;
                lastQuizTrack = quizTrack;
                
                onModeOrCategoryChanged(currentMode, currentCategory, quizTrack);
            }
        }
    }
    http.end();
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    initDisplay();

    dfSerial.begin(9600, SERIAL_8N1, DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);
    delay(1000);

    if (myDFPlayer.begin(dfSerial)) {
        myDFPlayer.volume(50);
    }

    showDefaultFace();
    playVoice(27); 

    SPI.begin();
    hardResetMFRC522();
    mfrc522.PCD_Init();

    WiFi_init();
}

void loop() {
    unsigned long currentTime = millis();

    if (!isExecutingAction) {
        if (currentTime - lastFaceChange >= FACE_INTERVAL) {
            lastFaceChange = currentTime;
            updateRobotFaceAnimation();
        }

        if (currentTime - lastStatusCheck >= STATUS_CHECK_INTERVAL) {
            lastStatusCheck = currentTime;
            checkServerStatus();
        }
    }

    if (isExecutingAction) return;

    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        delay(10);
        return;
    }

    String uidStr = buildUIDString(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println("[RFID] Read UID: " + uidStr);

    ApiResponse actionResponse = sendUID(uidStr);

    if (actionResponse.success) {
        executeServerAction(actionResponse);
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}