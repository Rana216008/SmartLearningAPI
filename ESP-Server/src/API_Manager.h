#ifndef API_MANAGER_H
#define API_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Rana"; 
const char* password = "ranaakram216008";
const char* serverUrl = "http://192.168.14.102:5000/api/scan";

struct ApiResponse {
  String action;
  int track;
};

inline void WiFi_init() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
}

inline ApiResponse sendUID(String uid) {
  ApiResponse result;
  result.track = -1; 
  result.action = "none";

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected during send!");
    return result;
  }

  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  JsonDocument docOut; 
  docOut["uid"] = uid; // تم تغييرها إلى حروف صغيرة لتطابق الـ .NET الافتراضي
  String body;
  serializeJson(docOut, body);

  int httpResponseCode = http.POST(body);
  Serial.print("HTTP Response Code from Server: ");
  Serial.println(httpResponseCode); // سيطبع لك 200 إذا نجح بالكامل

  if (httpResponseCode == 200) { // تم تغيير الشرط للتأكد من النجاح الفعلي لقراءة البيانات
    String response = http.getString();
    Serial.print("Response body: ");
    Serial.println(response);

    JsonDocument docIn; 
    DeserializationError error = deserializeJson(docIn, response);

    if (!error) {
      // مكتبة ArduinoJson النسخة 7 تتعامل بحروف صغيرة افتراضياً مع أوديّة الـ .NET المستلمة
      result.track = docIn["track"].as<int>();  
      result.action = docIn["action"].as<String>(); 
    } else {
      Serial.print("JSON Deserialization Error: ");
Serial.println(error.c_str());    }
  } else {
    String errorResponse = http.getString();
    Serial.print("Server Error Details: ");
    Serial.println(errorResponse);
  }
  http.end();
  return result;
}

#endif




// #ifndef API_MANAGER_H
// #define API_MANAGER_H

// #include <Arduino.h>
// #include <WiFi.h>
// #include <HTTPClient.h>
// #include <ArduinoJson.h>

// const char* ssid = "Rana"; 
// const char* password = "ranaakram216008";
// const char* serverUrl = "http://192.168.21.102:5000/api/scan";

// struct ApiResponse {
//   String action;
//   int track;
// };

// inline void WiFi_init() {
//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nWiFi Connected!");
// }

// inline ApiResponse sendUID(String uid) {
//   ApiResponse result;
//   result.track = -1; 
//   result.action = "none";

//   if (WiFi.status() != WL_CONNECTED) return result;

//   HTTPClient http;
//   http.begin(serverUrl);
//   http.addHeader("Content-Type", "application/json");

//   JsonDocument docOut; // استخدام JsonDocument للنسخة السابعة
//   docOut["UID"] = uid;
//   String body;
//   serializeJson(docOut, body);

//   int httpResponseCode = http.POST(body);

//   if (httpResponseCode > 0) {
//     String response = http.getString();
//     JsonDocument docIn; 
//     DeserializationError error = deserializeJson(docIn, response);

//     if (!error) {
//       result.track = docIn["track"].as<int>();  
//       result.action = docIn["action"].as<String>(); 
//     }
//   }
//   http.end();
//   return result;
// }

// #endif