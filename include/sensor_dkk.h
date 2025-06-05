#ifndef SENSOR_DKK_H
#define SENSOR_DKK_H

struct SensorData {
  float latitude;
  float longitude;
  float altitude;
  bool sos;
};

// Fungsi-fungsi langsung diimplementasikan di sini

inline void readGPS(float &lat, float &lon) {
  lat = -7.12345;
  lon = 110.54321;
}

inline float getAltitude() {
  return 812.3;
}

inline bool getSOSStatus() {
  return false;
}

inline SensorData readSensorData() {
  SensorData data;
  readGPS(data.latitude, data.longitude);
  data.altitude = getAltitude();
  data.sos = getSOSStatus();
  return data;
}

#endif
