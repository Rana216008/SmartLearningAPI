#include "DFPlayerManager.h"
#include "Config.h"

// ===== Object Initialization =====
HardwareSerial dfSerial(2);
DFRobotDFPlayerMini myDFPlayer;

// ============================================
// Play Voice Function
// ============================================
void playVoice(int trackNumber) {
    myDFPlayer.play(trackNumber);
    Serial.print("[DFPlayer] Playing voice track: ");
    Serial.println(trackNumber);
}