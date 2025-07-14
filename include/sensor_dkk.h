#ifndef SENSOR_DKK_H
#define SENSOR_DKK_H

#include <Arduino.h>
#include <TinyGPS++.h>

#define GPS_RX_PIN 33
#define GPS_TX_PIN 32

extern volatile bool sos_status; 
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

struct SensorData {
  float latitude;
  float longitude;
  float altitude;
  bool sos;
  String utcTime;      // Waktu UTC (JJ:MM:SS)
  String date;         // Tanggal (DD/MM/YYYY)
  float speed_kmph;    // Kecepatan dalam km/jam
  double course_deg;   // Arah dalam derajat
  uint32_t satellites; // Jumlah satelit yang terhubung
  float hdop;          // Horizontal Dilution of Precision
};

inline void setupSensors() {
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println("GPS Module Initialized on UART1");
}

inline void readGPSData() {
  unsigned long startTime = millis();
  while (millis() - startTime < 1000) {
    if (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }
  }
}

inline SensorData readSensorData() {
  readGPSData();

  SensorData data;

  // Data Lokasi
  if (gps.location.isValid()) {
    data.latitude = gps.location.lat();
    data.longitude = gps.location.lng();
  } else {
    data.latitude = 0.0;
    data.longitude = 0.0;
  }

  // Data Ketinggian
  if (gps.altitude.isValid()) {
    data.altitude = gps.altitude.meters();
  } else {
    data.altitude = 0.0;
  }

  // Data Waktu (UTC)
  if (gps.time.isValid()) {
    char timeStr[9];
    sprintf(timeStr, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
    data.utcTime = timeStr;
  } else {
    data.utcTime = "00:00:00";
  }

  // Data Tanggal
  if (gps.date.isValid()) {
    char dateStr[11];
    sprintf(dateStr, "%02d/%02d/%04d", gps.date.day(), gps.date.month(), gps.date.year());
    data.date = dateStr;
  } else {
    data.date = "00/00/0000";
  }

  // Data Kecepatan
  if (gps.speed.isValid()) {
    data.speed_kmph = gps.speed.kmph();
  } else {
    data.speed_kmph = 0.0;
  }
  
  // Data Arah
  if (gps.course.isValid()) {
    data.course_deg = gps.course.deg();
  } else {
    data.course_deg = 0.0;
  }
  
  // Data Satelit
  if (gps.satellites.isValid()) {
    data.satellites = gps.satellites.value();
  } else {
    data.satellites = 0;
  }

  // Data HDOP (akurasi horizontal)
  if (gps.hdop.isValid()) {
    data.hdop = gps.hdop.hdop();
  } else {
    data.hdop = 99.99;
  }

  data.sos = sos_status;
  return data;
}

#endif