#include "ExamMode.h"
#include "Config.h"
#include "DisplayManager.h"
#include "DFPlayerManager.h"


ExamState examState = EXAM_IDLE;
bool examModeActive = false;

void startExamMode() {
    Serial.println("[EXAM] 🎯 Starting Exam Mode!");
    examModeActive = true;
    examState = EXAM_START;
    displayTrackImage(18); // يعرض واجهة الاختبار بواسطة رقم الـ Track المخصص لها
    playVoice(18);
}

void showCurrentQuestion() {
    switch(examState) {
        case EXAM_FQ:
            displayTrackImage(20); // First Question
            playVoice(6);
            break;
        case EXAM_SQ:
            displayTrackImage(20); // Second Question
            playVoice(5);
            break;
        case EXAM_TQ:
            displayTrackImage(20); // Third Question
            playVoice(4);
            break;
        default:
            break;
    }
}

void handleCorrectAnswer(String imageName) {
    Serial.println("[EXAM] ✅ Correct Answer!");
    
    if (imageName == "BT") displayImageByTrack(2);
    else if (imageName == "T") displayImageByTrack(3);
    else if (imageName == "A") displayImageByTrack(1);
    
    delay(3000);
    displayTrackImage(30); // Correct Answer Image via Track Number
    playVoice(3);
    delay(3000);
    
    switch(examState) {
        case EXAM_FQ: examState = EXAM_SQ; break;
        case EXAM_SQ: examState = EXAM_TQ; break;
        case EXAM_TQ: examState = EXAM_FQ; break;
        default: break;
    }
    showCurrentQuestion();
}

void handleWrongAnswer() {
    Serial.println("[EXAM] ❌ Wrong Answer!");
    displayTrackImage(31); // Wrong Answer Image via Track Number
    playVoice(17);
    delay(2000);
    showCurrentQuestion();
}

void processExamCard(String uid) {
    Serial.println("[EXAM] Processing card: " + uid);
    
    byte readUID[4];
    int idx = 0;
    String temp = uid;
    temp.replace(" ", "");
    for (size_t i = 0; i < temp.length(); i += 2) {
        readUID[idx++] = (byte)strtol(temp.substring(i, i+2).c_str(), NULL, 16);
    }
    
    bool matched = false;
    if (idx == 4) {
        int expectedTrack = -1;
        if (examState == EXAM_FQ) expectedTrack = 2;       // BT (ب)
        else if (examState == EXAM_SQ) expectedTrack = 3;  // T (ت)
        else if (examState == EXAM_TQ) expectedTrack = 1;  // A

        for (size_t i = 0; i < TOTAL_CARDS; i++) {
            if (cards[i].trackNumber == expectedTrack && compareUID(readUID, (byte*)cards[i].uid, 4)) {
                handleCorrectAnswer(cards[i].imageName);
                matched = true;
                break;
            }
        }
    }
    
    if (!matched) {
        handleWrongAnswer();
    }
}