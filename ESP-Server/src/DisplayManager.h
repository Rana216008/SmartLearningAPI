#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include "Config.h"

extern TFT_eSPI tft;

void initDisplay();
void displayImageByTrack(int track);
void showDefaultFace();
void updateRobotFaceAnimation();

#endif