#ifndef LORA_UTIL_H
#define LORA_UTIL_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_dkk.h"

inline void loraBegin(HardwareSerial& serial, unsigned long baud, int rx, int tx) {
  serial.begin(baud, SERIAL_8N1, rx, tx);
}

inline void loraSend(HardwareSerial& serial, const SensorData& data) {
  StaticJsonDocument<128> doc;
  doc["lat"] = data.latitude;
  doc["lon"] = data.longitude;
  doc["alt"] = data.altitude;
  doc["sos"] = data.sos;
  String jsonStr;
  serializeJson(doc, jsonStr);
  serial.println(jsonStr);
  Serial.println("Sent via LoRa: " + jsonStr);
}

#endif
