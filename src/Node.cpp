#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_dkk.h"
#include "lora_util.h"

#define NODE_ID 2 // ID node! 
#define LORA_M0 22
#define LORA_M1 23
#define LORA_RX 16 // RX di ESP32, terhubung ke TXD LoRa
#define LORA_TX 17 // TX di ESP32, terhubung ke RXD LoRa
#define LORA_AUX 21

HardwareSerial loraSerial(2);

void setup() {
  Serial.begin(115200);
  loraSerial.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX);
  Serial.println("Node Slave (ID: " + String(NODE_ID) + ") Ready, waiting for poll...");

  pinMode(LORA_M0, OUTPUT);
  pinMode(LORA_M1, OUTPUT);
  pinMode(LORA_AUX, INPUT);

  digitalWrite(LORA_M0, LOW);
  digitalWrite(LORA_M1, LOW);
}

void loop() {
  // Terus menerus mendengarkan pesan dari Gateway
  if (loraSerial.available()) {
    String msg = loraSerial.readStringUntil('\n');

    StaticJsonDocument<100> doc;
    DeserializationError err = deserializeJson(doc, msg);

    if (err) {
      // Abaikan jika JSON tidak valid
      return;
    }

    // Cek apakah pesan ini adalah permintaan polling untuk node ini
    if (doc.containsKey("poll_req")) {
      int targetNodeId = doc["poll_req"];

      if (targetNodeId == NODE_ID) {
        Serial.println("Poll request received. Sending data back...");
        
        // Jika ya, kumpulkan data sensor dan kirim sebagai balasan
        SensorData data = readSensorData();
        loraSend(loraSerial, data, NODE_ID); // Gunakan fungsi loraSend yang sudah ada
      }
    }
  }
}
