#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_dkk.h"
#include "lora_util.h"
#include "BluetoothSerial.h"

#define NODE_ID 1 // ID node! 
#define LORA_M0 22
#define LORA_M1 23
#define LORA_RX 16 // RX di ESP32, terhubung ke TXD LoRa
#define LORA_TX 17 // TX di ESP32, terhubung ke RXD LoRa
#define LORA_AUX 21

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to enable it
#endif

volatile bool sos_status = false; // 'volatile' adalah praktik yang baik untuk variabel 
                                  // yang bisa diubah oleh proses eksternal (seperti interupsi/input)
BluetoothSerial BTSerial;
HardwareSerial loraSerial(2);

void setup() {
  Serial.begin(115200);
  setupSensors();
  loraSerial.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX);
  Serial.println("Node Slave (ID: " + String(NODE_ID) + ") Ready, waiting for poll...");
  BTSerial.begin("Zona_Node"); 
  pinMode(LORA_M0, OUTPUT);
  pinMode(LORA_M1, OUTPUT);
  pinMode(LORA_AUX, INPUT);

  digitalWrite(LORA_M0, LOW);
  digitalWrite(LORA_M1, LOW);
}

void loop() {
  if (BTSerial.available()) {
    char command = BTSerial.read(); // Baca karakter yang masuk

    if (command == '1') {
      sos_status = true;
      BTSerial.println("SOS AKTIF"); // Kirim balasan konfirmasi
    } else if (command == '0') {
      sos_status = false;
      BTSerial.println("SOS NON-AKTIF"); // Kirim balasan konfirmasi
    }
  }
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

        String dataTerkirim = loraSend(loraSerial, data, NODE_ID);
        BTSerial.println(dataTerkirim); // log ke Bluetooth
      }
    }
  }
}
