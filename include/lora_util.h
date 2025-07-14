#ifndef LORA_UTIL_H
#define LORA_UTIL_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_dkk.h"

inline void loraBegin(HardwareSerial& serial, unsigned long baud, int rx, int tx) {
  serial.begin(baud, SERIAL_8N1, rx, tx);
}

inline String loraSend(HardwareSerial& serial, const SensorData& data, int nodeId) {
  // Ukuran dibesarkan untuk menampung semua data baru
  StaticJsonDocument<384> doc; 

  doc["node_id"] = nodeId;
  doc["lat"] = data.latitude;
  doc["lon"] = data.longitude;
  doc["alt"] = data.altitude;
  doc["sos"] = data.sos;
  doc["utc"] = data.utcTime;
  doc["date"] = data.date;
  doc["spd_kmph"] = data.speed_kmph;
  doc["course"] = data.course_deg;
  doc["sats"] = data.satellites;
  doc["hdop"] = data.hdop;

  String jsonStr;
  serializeJson(doc, jsonStr);
  serial.println(jsonStr);
  Serial.println("Sent from Node " + String(nodeId) + ": " + jsonStr);

  return jsonStr; // untuk log BT
}

#endif