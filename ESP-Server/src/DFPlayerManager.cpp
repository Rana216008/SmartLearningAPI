#include "DFPlayerManager.h"
#include "Config.h"

HardwareSerial dfSerial(2);
DFRobotDFPlayerMini myDFPlayer;

void playVoice(int trackNumber) {
    myDFPlayer.play(trackNumber);
    Serial.printf("[DFPlayer] Playing voice track: %d\n", trackNumber);
}