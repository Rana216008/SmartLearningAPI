#ifndef EXAM_MODE_H
#define EXAM_MODE_H

#include <Arduino.h>

// ===== Exam State Machine =====
enum ExamState {
    EXAM_IDLE,
    EXAM_START,
    EXAM_FQ,
    EXAM_SQ,
    EXAM_TQ,
    EXAM_SHOW_IMAGE,
    EXAM_WAIT_RETURN
};

extern ExamState examState;
extern bool examModeActive;

void startExamMode();
void showCurrentQuestion();
void handleCorrectAnswer(String imageName);
void handleWrongAnswer();
void processExamCard(String uid);

#endif