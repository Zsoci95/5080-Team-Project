#include "functions.h"


void displaySensorDetails(Adafruit_BNO055 imu)
{
  sensor_t sensor;
  imu.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" xxx");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" xxx");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" xxx");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}; 

void M5DisplayText(String text, int x, int y, int size, int color)
{
  M5.Lcd.setTextSize(size);
  M5.Lcd.setTextColor(color);
  M5.Lcd.setCursor(x, y);
  M5.Lcd.print(text);
}; 

void TCASelect(uint8_t i2c_address)
{
  Wire.beginTransmission(TCA9548A_ADDRESS);
  Wire.write(1 << i2c_address);
  Wire.endTransmission();
};

void MyserverCallbacks::onConnect(BLEServer* pServer) {
  pServer->startAdvertising(); // restart advertising
};

void MyserverCallbacks::onDisconnect(BLEServer* pServer) {
  pServer->startAdvertising(); // restart advertising
};

