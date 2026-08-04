#include "Drawings.h"

#define COLOR_BLUE_BORDER  0x037F 
#define COLOR_RED_CIRCLE   0xE800 
#define COLOR_GREEN_CARD   0x07E0 
#define COLOR_BLUE_CARD    0x001F 

void drawStandardCardTemplate() {
  tft.fillScreen(TFT_WHITE);
  for (int i = 0; i < 5; i++) {
    tft.drawRect(i, i, 480 - (2 * i), 320 - (2 * i), COLOR_BLUE_BORDER);
  }
}

void AlefCard() {
  drawStandardCardTemplate();
  int x = 220, y = 100;
  tft.fillRect(x, y, 12, 100, TFT_BLACK); 
  tft.drawArc(x + 5, y - 20, 15, 10, 45, 225, TFT_BLACK, TFT_WHITE, false); 
  tft.drawLine(x - 5, y - 10, x + 15, y - 10, TFT_BLACK);

  tft.setTextFont(1);
  tft.setTextSize(4);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setCursor(200, 240);
  tft.print("Alef");
}

void BaaCard() {
  drawStandardCardTemplate();
  int x = 160, y = 150;
  tft.fillRect(x, y, 8, 40, TFT_BLACK); 
  tft.fillRect(x, y + 40, 160, 8, TFT_BLACK); 
  tft.fillRect(x + 152, y, 8, 40, TFT_BLACK); 
  tft.fillCircle(x + 80, y + 65, 7, TFT_BLACK); 

  tft.setTextFont(1);
  tft.setTextSize(4);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setCursor(200, 240);
  tft.print("Baa");
}

void TaaCard() {
  drawStandardCardTemplate();
  int x = 160, y = 150;
  tft.fillRect(x, y, 8, 40, TFT_BLACK); 
  tft.fillRect(x, y + 40, 160, 8, TFT_BLACK); 
  tft.fillRect(x + 152, y, 8, 40, TFT_BLACK); 
  tft.fillCircle(x + 60, y - 15, 7, TFT_BLACK);
  tft.fillCircle(x + 100, y - 15, 7, TFT_BLACK);

  tft.setTextFont(1);
  tft.setTextSize(4);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setCursor(200, 240);
  tft.print("Taa");
}

void ACard() {
  drawStandardCardTemplate();
  tft.setTextFont(1);
  tft.setTextSize(12);
  tft.setTextColor(COLOR_BLUE_BORDER, TFT_WHITE);
  tft.setCursor(210, 80);
  tft.print("A");

  tft.setTextSize(4);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setCursor(160, 240);
  tft.print("Letter A");
}

void BCard() {
  drawStandardCardTemplate();
  tft.setTextFont(1);
  tft.setTextSize(12);
  tft.setTextColor(COLOR_BLUE_BORDER, TFT_WHITE);
  tft.setCursor(210, 80);
  tft.print("B");

  tft.setTextSize(4);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setCursor(160, 240);
  tft.print("Letter B");
}

void CCard() {
  drawStandardCardTemplate();
  int cX = 240, cY = 130;
  for (int r = 45; r <= 60; r++) {
    tft.drawArc(cX, cY, r, r - 3, 320, 220, COLOR_BLUE_BORDER, TFT_WHITE, false);
  }

  tft.setTextFont(1);
  tft.setTextSize(4);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setCursor(160, 240);
  tft.print("Letter C");
}

void RedColorCard() {
  drawStandardCardTemplate();
  tft.fillCircle(240, 130, 65, COLOR_RED_CIRCLE);
  tft.setTextFont(1);
  tft.setTextSize(4);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setCursor(150, 240);
  tft.print("red - ahmar");
}

void GreenColorCard() {
  drawStandardCardTemplate();
  tft.fillCircle(240, 130, 65, COLOR_GREEN_CARD);
  tft.setTextFont(1);
  tft.setTextSize(4);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setCursor(130, 240);
  tft.print("green - akhdar");
}

void BlueColorCard() {
  drawStandardCardTemplate();
  tft.fillCircle(240, 130, 65, COLOR_BLUE_CARD);
  tft.setTextFont(1);
  tft.setTextSize(4);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setCursor(140, 240);
  tft.print("blue - azraq");
}

void SplashScreen() {
    tft.fillScreen(TFT_BLACK);

    uint16_t c = TFT_CYAN;

    const int s = 40;   // حجم كل مربع
    const int g = 6;    // المسافة بين المربعات

    //====================
    // العين اليسرى
    //====================

    // أعلى
    tft.fillRect(110, 60, s, s, c);
    tft.fillRect(110 + (s + g), 60, s, s, c);
    tft.fillRect(110 + 2 * (s + g), 60, s, s, c);

    // يسار
    tft.fillRect(70, 100, s, s, c);
    tft.fillRect(70, 100 + (s + g), s, s, c);

    // يمين
    tft.fillRect(110 + 3 * (s + g), 100, s, s, c);
    tft.fillRect(110 + 3 * (s + g), 100 + (s + g), s, s, c);


    //====================
    // العين اليمنى
    //====================

    int ox = 250;

    tft.fillRect(ox, 60, s, s, c);
    tft.fillRect(ox + (s + g), 60, s, s, c);
    tft.fillRect(ox + 2 * (s + g), 60, s, s, c);

    tft.fillRect(ox - 40, 100, s, s, c);
    tft.fillRect(ox - 40, 100 + (s + g), s, s, c);

    tft.fillRect(ox + 3 * (s + g), 100, s, s, c);
    tft.fillRect(ox + 3 * (s + g), 100 + (s + g), s, s, c);


    //====================
    // الابتسامة
    //====================

    int mx = 160;
    int my = 220;

    tft.fillRect(mx - 40, my - 20, s, s, c);

    for (int i = 0; i < 6; i++) {
        tft.fillRect(mx + i * (s + g), my, s, s, c);
    }

    tft.fillRect(mx + 6 * (s + g), my - 20, s, s, c);
}
// void drawCircleCard() {
//   drawStandardCardTemplate();
//   int cX = 55, cY = 55;
//   for (int r = 24; r <= 36; r++) {
//     tft.drawArc(cX, cY, r, r - 3, 320, 220, COLOR_BLUE_BORDER, TFT_WHITE, false);
//   }

//   int centerX = 240, centerY = 145, radius = 70;
//   for (int r = radius; r <= radius + 3; r++) {
//     tft.drawCircle(centerX, centerY, r, TFT_BLACK);
//   }
//   tft.fillCircle(centerX, centerY, radius - 1, COLOR_BLUE_CARD);

//   tft.setTextFont(1);
//   tft.setTextSize(4);
//   tft.setTextColor(TFT_BLACK, TFT_WHITE);
//   tft.setCursor(170, 245);
//   tft.print("Circle");
// }