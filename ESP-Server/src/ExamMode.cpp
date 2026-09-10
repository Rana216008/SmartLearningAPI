#include "ExamMode.h"
#include "Config.h"
#include "DisplayManager.h"
#include "DFPlayerManager.h"

// ===== Exam-Specific Images =====
#include "TM.h"
#include "Q.h"
#include "WA.h"
#include "CA.h"

// ===== State Variables =====
ExamState examState = EXAM_IDLE;
bool examModeActive = false;

// ============================================
// Start Exam Mode - Show TM image
// ============================================
void startExamMode() {
    Serial.println("[EXAM] 🎯 Starting Exam Mode!");
    examModeActive = true;
    examState = EXAM_START;
    displayImage(TM, TM_size, "TM - Test Mode");
    playVoice(18);
}

// ============================================
// Show Current Question
// ============================================
void showCurrentQuestion() {
    switch(examState) {
        case EXAM_FQ:
            displayImage(Q, Q_size, "FQ - First Question (ب)");
            playVoice(6);
            break;
        case EXAM_SQ:
            displayImage(Q, Q_size, "SQ - Second Question (ت)");
            playVoice(5);
            break;
        case EXAM_TQ:
            displayImage(Q, Q_size, "TQ - Third Question (A)");
            playVoice(4);
            break;
        default:
            break;
    }
}

// ============================================
// Handle Correct Answer
// ============================================
void handleCorrectAnswer(String imageName) {
    Serial.println("[EXAM] ✅ Correct Answer!");
    
    if (imageName == "BT") {
        displayImageByTrack(2);  // BT image + voice 15
        delay(3000);
    } else if (imageName == "T") {
        displayImageByTrack(3);  // T image + voice 14
        delay(3000);
    } else if (imageName == "A") {
        displayImageByTrack(1);  // A image + voice 13
        delay(3000);
    }
    
    delay(3000);
    displayImage(CA, CA_size, "CA - Correct Answer!");
    playVoice(3);
    
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

// ============================================
// Handle Wrong Answer
// ============================================
void handleWrongAnswer() {
    Serial.println("[EXAM] ❌ Wrong Answer!");
    
    displayImage(WA, WA_size, "WA - Wrong Answer!");
    playVoice(17);
    delay(2000);
    
    showCurrentQuestion();
}

// ============================================
// Process Card During Exam Mode
// ============================================
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
            if (idx == 4 && compareUID(readUID, btUID, 4)) {
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