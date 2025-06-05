// #include <Arduino.h>
// #include <ArduinoJson.h>

// #define LORA_TX 17
// #define LORA_RX 16

// HardwareSerial loraSerial(1);

// void setup() {
//   Serial.begin(115200);
//   loraSerial.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX);
//   Serial.println("E220 Receiver with JSON Parsing");
// }

// void loop() {
//   if (loraSerial.available()) {
//     String msg = loraSerial.readStringUntil('\n');
//     Serial.println("Received: " + msg);

//     StaticJsonDocument<128> doc;
//     DeserializationError err = deserializeJson(doc, msg);

//     if (err) {
//       Serial.println("JSON parsing failed!");
//       return;
//     }

//     float lat = doc["lat"];
//     float lon = doc["lon"];
//     float alt = doc["alt"];
//     bool sos = doc["sos"];

//     Serial.printf("Lat: %.5f | Lon: %.5f | Alt: %.2f m | SOS: %s\n",
//                   lat, lon, alt, sos ? "YES" : "NO");
//   }
// }
