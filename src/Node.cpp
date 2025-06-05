#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_dkk.h"
#include "lora_util.h"

#define LORA_TX 17
#define LORA_RX 16

HardwareSerial loraSerial(1);
const unsigned long SEND_INTERVAL = 3000;

void setup() {
  Serial.begin(115200);
  loraBegin(loraSerial, 9600, LORA_RX, LORA_TX);
  Serial.println("ESP32 LoRa Sender Ready");
}

void loop() {
  SensorData data = readSensorData();
  loraSend(loraSerial, data);
  delay(SEND_INTERVAL);
}
