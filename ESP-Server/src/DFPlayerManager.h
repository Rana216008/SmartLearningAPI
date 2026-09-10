#ifndef DFPLAYER_MANAGER_H
#define DFPLAYER_MANAGER_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>

extern HardwareSerial dfSerial;
extern DFRobotDFPlayerMini myDFPlayer;

void playVoice(int trackNumber);

#endif