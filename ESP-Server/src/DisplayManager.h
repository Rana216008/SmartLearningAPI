#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include "Config.h"

extern TFT_eSPI tft;

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap);
void initDisplay();
void displayImage(const unsigned char* jpegData, size_t jpegSize, const char* name);
void displayTrackImage(int track);
void displayImageByTrack(int track);
void showTestCardScreen();
void updateRobotFace();
void showCategoryError(String category);

#endif