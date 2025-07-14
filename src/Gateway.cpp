#include <Arduino.h>
#include <ArduinoJson.h>
// #include "BluetoothSerial.h"

// ===== PERUBAHAN: Sertakan file-file pendukung =====
#include "sensor_dkk.h"
#include "supabase_util.h"

// ===== PERUBAHAN: Tambahkan kredensial di sini =====
#define WIFI_SSID "MARIOZ"
#define WIFI_PASS "harmoni3A"
#define SUPABASE_URL "https://udjikcciviviyslhgdab.supabase.co" 
#define SUPABASE_KEY "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InVkamlrY2Npdml2aXlzbGhnZGFiIiwicm9sZSI6InNlcnZpY2Vfcm9sZSIsImlhdCI6MTc1MjUwMTcwOCwiZXhwIjoyMDY4MDc3NzA4fQ.gSw5MwOh0O8fobgjOYiCpI05UGh4GmER3j9wwHoUQN0"

// --- Konfigurasi LoRa & Polling ---
#define LORA_M0 22
#define LORA_M1 23
#define LORA_RX 16
#define LORA_TX 17
#define LORA_AUX 21
HardwareSerial loraSerial(2);

const int NODE_COUNT = 2;
int nodesToPoll[NODE_COUNT] = {1, 2};
int currentNodeIndex = 0;
const unsigned long POLLING_INTERVAL = 5000;
const unsigned long RESPONSE_TIMEOUT = 3000;
unsigned long lastPollTime = 0;

// --- Objek Bluetooth ---
// BluetoothSerial BTSerial;

// Fungsi untuk mengirim permintaan polling ke node
void pollNode(int nodeId) {
  StaticJsonDocument<100> doc;
  doc["poll_req"] = nodeId;
  String pollMsg;
  serializeJson(doc, pollMsg);
  loraSerial.println(pollMsg);

  String pollingMsg = "Polling Node " + String(nodeId) + "...";
  Serial.println(pollingMsg);
  // BTSerial.println(pollingMsg);
}

void setup() {
  Serial.begin(115200);

  // ===== PERUBAHAN: Inisialisasi Wi-Fi =====
  setupWifi(WIFI_SSID, WIFI_PASS);

  loraSerial.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX);
  // BTSerial.begin("Zona_Gateway");

  Serial.println("\nGateway Ready - Meneruskan data ke Supabase & Bluetooth");
  // BTSerial.println("Gateway Ready - Menunggu data dari node...");
  
  pinMode(LORA_M0, OUTPUT);
  pinMode(LORA_M1, OUTPUT);
  pinMode(LORA_AUX, INPUT);
  digitalWrite(LORA_M0, LOW);
  digitalWrite(LORA_M1, LOW);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nKoneksi Wi-Fi terputus! Mencoba menyambung kembali...");
    setupWifi(WIFI_SSID, WIFI_PASS); // Panggil lagi fungsi setup Wi-Fi
  }
  // 1. Kirim permintaan polling sesuai jadwal
  if (millis() - lastPollTime > POLLING_INTERVAL) {
    lastPollTime = millis();
    int targetNodeId = nodesToPoll[currentNodeIndex];
    pollNode(targetNodeId);
    currentNodeIndex = (currentNodeIndex + 1) % NODE_COUNT;
  }

  // 2. Dengarkan balasan dari node
  unsigned long startTime = millis();
  while (millis() - startTime < RESPONSE_TIMEOUT) {
    if (loraSerial.available()) {
      String msg = loraSerial.readStringUntil('\n');
      
      StaticJsonDocument<384> doc; // Ukuran disesuaikan untuk data lengkap
      DeserializationError err = deserializeJson(doc, msg);

      if (err) {
        Serial.println("JSON parsing failed!");
        // BTSerial.println("❌ Gagal parsing JSON!");
        return; 
      }

      // Kemas data ke dalam struct
      SensorData receivedData;
      int nodeId = doc["node_id"];
      receivedData.latitude = doc["lat"];
      receivedData.longitude = doc["lon"];
      receivedData.altitude = doc["alt"];
      receivedData.sos = doc["sos"];
      receivedData.utcTime = doc["utc"].as<String>();
      receivedData.date = doc["date"].as<String>();
      receivedData.speed_kmph = doc["spd_kmph"];
      receivedData.satellites = doc["sats"];
      receivedData.hdop = doc["hdop"];

      // Tampilkan data yang diterima ke BT
      char buffer[256];
      sprintf(buffer, 
            "[FROM NODE %d]\n"
            "  Date/Time : %s %s UTC\n"
            "  Location  : %.5f, %.5f\n"
            "  Sats/HDOP : %d / %.2f\n",
            nodeId, receivedData.date.c_str(), receivedData.utcTime.c_str(), 
            receivedData.latitude, receivedData.longitude, 
            receivedData.satellites, receivedData.hdop);
      // BTSerial.print(buffer);
      Serial.print(buffer); // Tampilkan juga di Serial Monitor

      // Notifikasi proses Supabase
      // BTSerial.println("📡 Mencoba mengirim ke Supabase...");
      sendToSupabase(receivedData, nodeId, SUPABASE_URL, SUPABASE_KEY);
      // Panggil fungsi pengiriman dan cek hasilnya
      // if (sendToSupabase(receivedData, nodeId, SUPABASE_URL, SUPABASE_KEY)) {
      //   Serial.println("✅ Sukses terkirim ke Supabase!");
      // } else {
      //   Serial.println("❌ Gagal mengirim ke Supabase!");
      // }
      // Serial.println("---------------------------------");
      
      break; // Keluar dari loop tunggu karena sudah dapat data
    }
  }
}