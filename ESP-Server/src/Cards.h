#ifndef CARDS_H
#define CARDS_H

#include <Arduino.h>

// تعريف الهيكل (Structure)
struct Card {
    String uid;
    const char* name;
    int track;
};

// استخدام كلمة inline ضروري جداً في PlatformIO 
// لضمان عدم حدوث خطأ عند عمل Compile للمشروع
inline Card cards[] = {
    {"47 27 DB A2", "أ", 1},
    {"A8 5F 7C A2", "ب", 2},
    {"B8 30 24 A2", "ت", 3},
    {"58 5 A5 A2", "ث", 4},
    {"99 AA BB CC", "ج", 5}
};

// حساب عدد البطاقات تلقائياً
inline int totalCards = sizeof(cards) / sizeof(cards[0]);

#endif