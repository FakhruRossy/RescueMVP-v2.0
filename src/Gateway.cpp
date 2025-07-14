#include <Arduino.h>
#include <ArduinoJson.h>
#include "BluetoothSerial.h"

#define LORA_M0 22
#define LORA_M1 23
#define LORA_RX 16 // RX di ESP32, terhubung ke TXD LoRa
#define LORA_TX 17 // TX di ESP32, terhubung ke RXD LoRa
#define LORA_AUX 21

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to enable it
#endif

BluetoothSerial BTSerial;
HardwareSerial loraSerial(2);
const int NODE_COUNT = 2; // Jumlah total node yang akan di-poll
int nodesToPoll[NODE_COUNT] = {1, 2}; // Daftar ID node
int currentNodeIndex = 0;
const unsigned long POLLING_INTERVAL = 5000; // Interval antar polling node
const unsigned long RESPONSE_TIMEOUT = 3000; // Waktu tunggu balasan dr node, jadi jeda 2s sebelum pool berikutnya
unsigned long lastPollTime = 0;

// Fungsi untuk mengirim permintaan ke node
void pollNode(int nodeId) {
  StaticJsonDocument<100> doc;
  doc["poll_req"] = nodeId; // Mengirim pesan permintaan ke node ID tertentu
  String pollMsg;
  serializeJson(doc, pollMsg);
  loraSerial.println(pollMsg);
  Serial.println("Polling Node " + String(nodeId) + "...");
  BTSerial.println("Polling Node " + String(nodeId) + "...");
}

void setup() {
  Serial.begin(115200);
  loraSerial.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX);
  Serial.println("E220 Receiver with JSON Parsing");
  
  BTSerial.begin("Zona_Gateway"); // Nama ini akan muncul di HP Anda
  Serial.println("Gateway Ready - Output juga dikirim ke Bluetooth!");
  BTSerial.println("Gateway Ready - Output juga dikirim ke Bluetooth!");
  pinMode(LORA_M0, OUTPUT);
  pinMode(LORA_M1, OUTPUT);
  pinMode(LORA_AUX, INPUT);

  // Set LoRa ke mode transmisi normal (Mode 0)
  digitalWrite(LORA_M0, LOW);
  digitalWrite(LORA_M1, LOW);
}

void loop() {
  // 1. Kirim permintaan polling sesuai jadwal
  if (millis() - lastPollTime > POLLING_INTERVAL) {
    lastPollTime = millis();
    int targetNodeId = nodesToPoll[currentNodeIndex];
    pollNode(targetNodeId);
    
    // Pindah ke node selanjutnya untuk polling berikutnya
    currentNodeIndex = (currentNodeIndex + 1) % NODE_COUNT;
  }

  // 2. Dengarkan balasan dari node dalam rentang waktu timeout
  unsigned long startTime = millis();
  while (millis() - startTime < RESPONSE_TIMEOUT) {
    if (loraSerial.available()) {
      String msg = loraSerial.readStringUntil('\n');
      
      StaticJsonDocument<200> doc;
      DeserializationError err = deserializeJson(doc, msg);

      if (err) {
        Serial.println("JSON parsing failed!");
        continue; // Lanjut mendengarkan
      }

    // Ambil semua data dari JSON
    int nodeId = doc["node_id"];
    float lat = doc["lat"];
    float lon = doc["lon"];
    float alt = doc["alt"];
    const char* utc = doc["utc"];
    const char* date = doc["date"];
    float spd_kmph = doc["spd_kmph"];
    double course = doc["course"];
    int sats = doc["sats"];
    float hdop = doc["hdop"];

    // Tampilkan semua data dengan format yang lebih informatif
    Serial.println("Diterima dari Node");
    // Serial.printf("[FROM NODE %d]\n", nodeId);
    // Serial.printf("  Date/Time : %s %s UTC\n", date, utc);
    // Serial.printf("  Location  : %.5f, %.5f\n", lat, lon);
    // Serial.printf("  Altitude  : %.2f m\n", alt);
    // Serial.printf("  Speed     : %.2f km/h\n", spd_kmph);
    // Serial.printf("  Course    : %.2f deg\n", course);
    // Serial.printf("  Sats/HDOP : %d / %.2f\n", sats, hdop);
    // Serial.println("========================================");

    // Tampilkan semua data dengan BT
    // BTSerial.println("========================================");
    BTSerial.printf("[FROM NODE %d]\n", nodeId);
    BTSerial.printf("  Date/Time : %s %s UTC\n", date, utc);
    BTSerial.printf("  Location  : %.5f, %.5f\n", lat, lon);
    BTSerial.printf("  Altitude  : %.2f m\n", alt);
    BTSerial.printf("  Speed     : %.2f km/h\n", spd_kmph);
    BTSerial.printf("  Course    : %.2f deg\n", course);
    BTSerial.printf("  Sats/HDOP : %d / %.2f\n", sats, hdop);
    // BTSerial.println("========================================");
      }
    }

    
}

