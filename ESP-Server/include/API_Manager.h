#ifndef API_MANAGER_H
#define API_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ===== WiFi Credentials =====
const char* ssid = "HTR";
const char* password = "12345678SAS";

// ===== Server URL =====
const char* serverUrl = "http://192.168.1.103:5000/api/scan";
const char* healthUrl = "http://192.168.1.103:5000/";

// ===== Response Structure =====
struct ApiResponse {
    String action;
    int track;
    String message;
    String imageName;
    String mode;
    String category;  // "All", "English", "Arabic", "Colors"
    bool success;
};

// ============================================
// WiFi Initialization
// ============================================
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

// ============================================
// Health Check - ONE ATTEMPT ONLY
// ============================================
inline bool checkServerHealth() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[HEALTH] ❌ WiFi not connected!");
        return false;
    }

    HTTPClient http;
    http.begin(healthUrl);
    http.setTimeout(3000);
    int httpCode = http.GET();
    http.end();

    if (httpCode == 200) {
        Serial.println("[HEALTH] ✅ Server is reachable!");
        return true;
    } else {
        Serial.print("[HEALTH] ❌ Server not reachable (HTTP ");
        Serial.print(httpCode);
        Serial.println(")");
        return false;
    }
}

// ============================================
// Send UID to Server - ONE ATTEMPT ONLY
// ============================================
inline ApiResponse sendUID(String uid) {
    ApiResponse result;
    result.track = -1;
    result.action = "error";
    result.message = "";
    result.imageName = "";
    result.mode = "";
    result.category = "All";
    result.success = false;

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[API] ❌ WiFi not connected!");
        return result;
    }

    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(5000);

    JsonDocument docOut;
    docOut["UID"] = uid;
    String body;
    serializeJson(docOut, body);

    Serial.print("[API] 📤 Sending: ");
    Serial.println(body);

    int httpResponseCode = http.POST(body);

    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.print("[API] 📥 Response: ");
        Serial.println(response);

        JsonDocument docIn;
        DeserializationError error = deserializeJson(docIn, response);

        if (!error) {
            result.track = docIn["track"] | -1;
            result.action = docIn["action"] | "error";
            result.message = docIn["message"] | "";
            result.imageName = docIn["imageName"] | "";
            result.mode = docIn["mode"] | "Learning";
            result.category = docIn["category"] | "All";  // NEW
            result.success = true;
            
            Serial.print("[API] ✅ Track: ");
            Serial.print(result.track);
            Serial.print(", Action: ");
            Serial.print(result.action);
            Serial.print(", Mode: ");
            Serial.print(result.mode);
            Serial.print(", Category: ");
            Serial.println(result.category);
        } else {
            Serial.print("[API] ❌ JSON parse error: ");
            Serial.println(error.c_str());
        }
    } else {
        Serial.print("[API] ❌ HTTP error: ");
        Serial.println(httpResponseCode);
        
        http.end();
        
        if (httpResponseCode == -1 || httpResponseCode == -11) {
            Serial.println("[API] 🔄 Resetting WiFi connection...");
            WiFi.disconnect();
            delay(100);
            WiFi.reconnect();
            delay(500);
        }
    }

    http.end();
    return result;
}

// ============================================
// Check Current Mode (using a dummy UID)
// ============================================
inline String checkCurrentMode() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[MODE] ❌ WiFi not connected!");
        return "Learning";
    }

    String dummyUID = "CHECK_MODE";
    
    ApiResponse result = sendUID(dummyUID);
    
    if (result.mode == "Exam" || result.mode == "Learning") {
        Serial.print("[MODE] Current mode: ");
        Serial.println(result.mode);
        return result.mode;
    }
    
    return "Learning";
}

#endif