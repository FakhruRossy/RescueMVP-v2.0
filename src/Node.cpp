#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_dkk.h"

#define LORA_TX 17
#define LORA_RX 16

HardwareSerial loraSerial(1);
const unsigned long SEND_INTERVAL = 3000;

void sendToLoRa(const SensorData &data) {
  StaticJsonDocument<128> doc;
  doc["lat"] = data.latitude;
  doc["lon"] = data.longitude;
  doc["alt"] = data.altitude;
  doc["sos"] = data.sos;

  String jsonStr;
  serializeJson(doc, jsonStr);
  loraSerial.println(jsonStr);
  Serial.println("Sent: " + jsonStr);
}

void setup() {
  Serial.begin(115200);
  loraSerial.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX);
  Serial.println("ESP32 LoRa Transmitter Ready");
}

void loop() {
  SensorData data = readSensorData();
  sendToLoRa(data);
  delay(SEND_INTERVAL);
}
