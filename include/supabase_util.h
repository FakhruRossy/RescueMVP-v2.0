#ifndef SUPABASE_UTIL_H
#define SUPABASE_UTIL_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "sensor_dkk.h" // Butuh struct SensorData

// Fungsi untuk koneksi ke Wi-Fi
void setupWifi(const char* ssid, const char* password) {
  Serial.println();
  Serial.print("Menghubungkan ke Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 20) { // Coba selama 10 detik
    delay(500);
    Serial.print(".");
    attempt++;
  }

  if(WiFi.status() == WL_CONNECTED){
    Serial.println("\nWi-Fi terhubung!");
    Serial.print("Alamat IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWi-Fi gagal terhubung.");
  }
}

// Fungsi untuk mengirim data ke Supabase (mengembalikan true/false)
bool sendToSupabase(const SensorData& data, int nodeId, const char* supabaseUrl, const char* supabaseKey) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi tidak terhubung. Gagal mengirim ke Supabase.");
    return false;
  }

  HTTPClient http;
  String url = String(supabaseUrl) + "/rest/v1/lora_data"; // Sesuaikan "lora_data" dengan nama tabel Anda

  http.begin(url);
  http.addHeader("apikey", supabaseKey);
  http.addHeader("Authorization", "Bearer " + String(supabaseKey));
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<512> doc;
  doc["node_id"] = nodeId;
  doc["latitude"] = data.latitude;
  doc["longitude"] = data.longitude;
  doc["altitude"] = data.altitude;
  doc["sos"] = data.sos;
  doc["utc_time"] = data.utcTime;
  doc["sats"] = data.satellites;
  doc["hdop"] = data.hdop;
  doc["speed_kmph"] = data.speed_kmph;

  String jsonBody;
  serializeJson(doc, jsonBody);

  int httpCode = http.POST(jsonBody);
  
  // Selalu tutup koneksi http
  http.end(); 

  if (httpCode >= 200 && httpCode < 300) {
    Serial.printf("[Supabase] Sukses, Kode: %d\n", httpCode);
    return true;
  } else {
    Serial.printf("[Supabase] Gagal, Kode: %d\n", httpCode);
    return false;
  }
}

#endif