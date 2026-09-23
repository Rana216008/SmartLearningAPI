#ifndef API_MANAGER_H
#define API_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ===== WiFi Credentials =====
const char* ssid = "Rana";
const char* password = "ranaakram216008";

// ===== Server URL =====
const char* serverUrl = "http://192.168.150.102:5000/api/scan";
const char* healthUrl = "http://192.168.150.102:5000/";

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
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[HEALTH] ⚠️ WiFi not connected!");
        return false;
    }

    HTTPClient http;
    http.begin(healthUrl);
    http.addHeader("Connection", "close");
    http.setTimeout(3000);

    int httpCode = http.GET();
    http.end();

    if (httpCode == 200) {
        Serial.println("[HEALTH] ✅ Server is reachable!");
        return true;
    } else {
        Serial.print("[HEALTH] ⚠️ Server not reachable (HTTP ");
        Serial.print(httpCode);
        Serial.println(")");
        return false;
    }
}

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
        Serial.println("[API] ⚠️ WiFi not connected!");
        return result;
    }

    HTTPClient http;
    if (!http.begin(serverUrl)) {
        Serial.println("[API] ❌ Failed to begin HTTP connection");
        return result;
    }

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Connection", "close"); // إجبار السيرفر على إغلاق الجلسة فور الرد
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
            result.category = docIn["category"] | "All";
            result.success = true;

            Serial.printf("[API] ✅ Track: %d, Action: %s, Mode: %s, Category: %s\n", 
                          result.track, result.action.c_str(), result.mode.c_str(), result.category.c_str());
        } else {
            Serial.print("[API] ❌ JSON parse error: ");
            Serial.println(error.c_str());
        }
    } else {
        Serial.print("[API] ❌ HTTP error code: ");
        Serial.println(httpResponseCode);

        if (httpResponseCode == -1 || httpResponseCode == -11) {
            Serial.println("[API] 🔄 Connection dropped. Reconnecting WiFi...");
            WiFi.disconnect();
            delay(200);
            WiFi.reconnect();
        }
    }

    http.end();
    return result;
}

inline String checkCurrentMode() {
    if (WiFi.status() != WL_CONNECTED) {
        return "Learning";
    }

    ApiResponse result = sendUID("CHECK_MODE");
    if (result.success && (result.mode == "Exam" || result.mode == "Learning")) {
        return result.mode;
    }

    return "Learning";
}

#endif