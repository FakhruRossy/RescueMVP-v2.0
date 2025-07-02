// #include <Arduino.h>
// #include <ArduinoJson.h>

// #define LORA_M0 22
// #define LORA_M1 23
// #define LORA_RX 16 // RX di ESP32, terhubung ke TXD LoRa
// #define LORA_TX 17 // TX di ESP32, terhubung ke RXD LoRa
// #define LORA_AUX 21

// HardwareSerial loraSerial(2);
// const int NODE_COUNT = 2; // Jumlah total node yang akan di-poll
// int nodesToPoll[NODE_COUNT] = {1, 2}; // Daftar ID node
// int currentNodeIndex = 0;
// const unsigned long POLLING_INTERVAL = 5000; // Interval antar polling node
// const unsigned long RESPONSE_TIMEOUT = 3000; // Waktu tunggu balasan dr node
// unsigned long lastPollTime = 0;

// // Fungsi untuk mengirim permintaan ke node
// void pollNode(int nodeId) {
//   StaticJsonDocument<100> doc;
//   doc["poll_req"] = nodeId; // Mengirim pesan permintaan ke node ID tertentu
//   String pollMsg;
//   serializeJson(doc, pollMsg);
//   loraSerial.println(pollMsg);
//   Serial.println("Polling Node " + String(nodeId) + "...");
// }

// void setup() {
//   Serial.begin(115200);
//   loraSerial.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX);
//   Serial.println("E220 Receiver with JSON Parsing");

//   pinMode(LORA_M0, OUTPUT);
//   pinMode(LORA_M1, OUTPUT);
//   pinMode(LORA_AUX, INPUT);

//   // Set LoRa ke mode transmisi normal (Mode 0)
//   digitalWrite(LORA_M0, LOW);
//   digitalWrite(LORA_M1, LOW);
// }

// void loop() {
//   // 1. Kirim permintaan polling sesuai jadwal
//   if (millis() - lastPollTime > POLLING_INTERVAL) {
//     lastPollTime = millis();
//     int targetNodeId = nodesToPoll[currentNodeIndex];
//     pollNode(targetNodeId);
    
//     // Pindah ke node selanjutnya untuk polling berikutnya
//     currentNodeIndex = (currentNodeIndex + 1) % NODE_COUNT;
//   }

//   // 2. Dengarkan balasan dari node dalam rentang waktu timeout
//   unsigned long startTime = millis();
//   while (millis() - startTime < RESPONSE_TIMEOUT) {
//     if (loraSerial.available()) {
//       String msg = loraSerial.readStringUntil('\n');
      
//       StaticJsonDocument<200> doc;
//       DeserializationError err = deserializeJson(doc, msg);

//       if (err) {
//         Serial.println("JSON parsing failed!");
//         continue; // Lanjut mendengarkan
//       }

//     // Pastikan ini adalah balasan data, bukan pesan lain
//     int nodeId = doc["node_id"];
//     float lat = doc["lat"];
//     float lon = doc["lon"];
//     float alt = doc["alt"];
//     bool sos = doc["sos"];

//     // Tampilkan data beserta ID node yang mengirim
//     Serial.printf("[From Node %d] Lat: %.5f | Lon: %.5f | Alt: %.2f m | SOS: %s\n", nodeId, lat, lon, alt, sos ? "YES" : "NO");
//     }
//     }
// }

