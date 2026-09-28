#ifndef API_MANAGER_H
#define API_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Rana";
const char* password = "ranaakram216008";

const char* serverUrl = "http://192.168.97.102:5000/api/scan";
const char* statusUrl = "http://192.168.97.102:5000/api/learning/status"; 
const char* healthUrl = "http://192.168.97.102:5000/";
const String SERVER_BASE_URL = "http://192.168.97.102:5000";

struct ApiResponse {
    bool success;
    String action;      // "correct_and_next", "wrong", "play", "ask_question", "error" ...
    int imageTrack;     // رقم الصورة للمسح
    int audioTrack;     // رقم تراك السؤال الجديد أو اسم/صوت العنصر
    int feedbackTrack;  // رقم تراك التعزيز التشجيعي (إجابة صحيحة / أحسنت)
    int delayMs;
    String message;
};

inline void WiFi_init() {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ WiFi Connected!");
        Serial.print("📡 IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\n❌ WiFi Connection Failed!");
    }
}

inline bool checkServerHealth() {
    if (WiFi.status() != WL_CONNECTED) return false;

    HTTPClient http;
    http.begin(healthUrl);
    http.addHeader("Connection", "close");
    http.setTimeout(3000);

    int httpCode = http.GET();
    http.end();
    return (httpCode == 200);
}

inline ApiResponse sendUID(String uid) {
    ApiResponse result;
    result.success = false;
    result.action = "none";
    result.imageTrack = -1;
    result.audioTrack = -1;
    result.feedbackTrack = -1;
    result.delayMs = 3000;
    result.message = "";

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[API] ⚠️ WiFi Disconnected");
        return result;
    }

    HTTPClient http;
    if (!http.begin(serverUrl)) return result;

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Connection", "close");
    http.setTimeout(5000);

    JsonDocument docOut;
    docOut["UID"] = uid;
    String body;
    serializeJson(docOut, body);

    Serial.print("[API] 📤 Sending UID: ");
    Serial.println(body);

    int httpCode = http.POST(body);

    if (httpCode > 0) {
        String response = http.getString();
        Serial.print("[API] 📥 Server Action Response: ");
        Serial.println(response);

        JsonDocument docIn;
        DeserializationError error = deserializeJson(docIn, response);

        if (!error) {
            result.success = true;
            result.action = docIn["action"] | "none";
            
            result.audioTrack = docIn["track"] | -1;
            result.feedbackTrack = docIn["feedbackTrack"] | -1;
            result.imageTrack = docIn["track"] | -1; 
            
            result.delayMs = docIn["delayMs"] | 3000;
            result.message = docIn["message"] | "";
        } else {
            Serial.print("[API] ❌ JSON Error: ");
            Serial.println(error.c_str());
        }
    } else {
        Serial.printf("[API] ❌ HTTP Error: %d\n", httpCode);
        if (httpCode == -1 || httpCode == -11) {
            WiFi.disconnect();
            delay(200);
            WiFi.reconnect();
        }
    }

    http.end();
    return result;
}

#endif