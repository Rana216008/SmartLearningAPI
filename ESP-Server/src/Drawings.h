#ifndef DRAWINGS_H
#define DRAWINGS_H

#include <TFT_eSPI.h>

extern TFT_eSPI tft;

void drawStandardCardTemplate();
void SplashScreen();
void AlefCard();   // 1
void BaaCard();    // 2
void TaaCard();    // 3
void ACard();     // 4
void BCard();     // 5
void CCard();     // 6
void RedColorCard();     // 7
void GreenColorCard();   // 8
void BlueColorCard();    // 9
// void drawCircleCard();      // 10

#endif