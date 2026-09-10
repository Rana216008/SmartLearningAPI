#ifndef RFID_MANAGER_H
#define RFID_MANAGER_H

#include <Arduino.h>
#include <MFRC522.h>

extern MFRC522 mfrc522;

void hardResetMFRC522();
bool isRFIDAlive();
bool reviveRFID();

#endif