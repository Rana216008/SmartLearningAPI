#ifndef CARDS_H
#define CARDS_H

#include <Arduino.h>

struct Card {
    String uid;
    const char* name;     // اسم الكرت / الحرف
    int track;            // رقم المقطع الصوتي
    const char* imageName;// اسم الصورة/الرسمة
};

inline Card cards[] = {
    {"5805A5A2", "أ", 1, "Arabic Alef Card"},
    {"4827DBA2", "ب", 2, "Arabic Baa Card"},
    {"A85F7CA2", "ت", 3, "Arabic Taa Card"},
    {"B83024A2", "A", 4, "English A Card"},
    {"138498AA", "B", 5, "English B Card"},
    {"E2ADB889", "C", 6, "English C Card"},
    {"F3DBFDA6", "Red", 7, "Red Color Card"},
    {"131876BD", "Green", 8, "Green Color Card"},
    {"2326B11B", "Blue", 9 , "Blue Color Card"}
};

// حساب عدد البطاقات تلقائياً (تعريف واحد فقط)
inline int totalCards = sizeof(cards) / sizeof(cards[0]);

// دالة مساعدة للحصول على اسم الصورة من رقم التراك
inline String getImageNameByTrack(int track) {
    switch (track) {
        case 1: return "Arabic Alef Card";
        case 2: return "Arabic Baa Card";
        case 3: return "Arabic Taa Card";
        case 4: return "English A Card";
        case 5: return "English B Card";
        case 6: return "English C Card";
        case 7: return "Red Color Card";
        case 8: return "Green Color Card";
        case 9: return "Blue Color Card";
        case 10: return "Circle Shape Card";
        default: return "Default Card Template";
    }
}

// دالة مساعدة للحصول على اسم الحرف/الكرت من رقم التراك
inline String getCardNameByTrack(int track) {
    for (int i = 0; i < totalCards; i++) {
        if (cards[i].track == track) {
            return String(cards[i].name);
        }
    }
    return "Unknown Card";
}

#endif // CARDS_H