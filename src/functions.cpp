#include "functions.h"

void displayESPInfo(){

  

};
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

void M5DisplayText(String text, int x, int y, int size, int color, int time)
{
  M5.Lcd.setTextSize(size);
  M5.Lcd.setTextColor(color);
  M5.Lcd.setCursor(x, y);
  M5.Lcd.print(text);
  delay(time);
  if (time > 0) {
    M5.Lcd.clear(); 
  }
  
}; 

void TCASelect(uint8_t i2c_address)
{
  Wire.beginTransmission(TCA9548A_ADDRESS);
  Wire.write(1 << i2c_address);
  Wire.endTransmission();
};

void MyserverCallbacks::onConnect(BLEServer* pServer) {
  //pServer->startAdvertising(); // TODO: check if advertising stops after connection
};

void MyserverCallbacks::onDisconnect(BLEServer* pServer) {
  pServer->startAdvertising(); // restart advertising
};

void MyCallbacks::onNotify(BLECharacteristic *pCharacteristic) {

};

String createBuffer(char digits, imu::Quaternion quat_0, imu::Quaternion quat_1, imu::Quaternion quat_2, 
imu::Vector<3> accel_0,  imu::Vector<3> accel_1,  imu::Vector<3> accel_2) {

  String buffer = "[" + String((float)quat_0.w(), digits) + "," + String((float)quat_0.x(), digits) + "," + String((float)quat_0.y(), digits) + "," + String((float)quat_0.z(), digits) + "," 
                      + String((float)quat_1.w(), digits) + "," + String((float)quat_1.x(), digits) + "," + String((float)quat_1.y(), digits) + "," + String((float)quat_1.z(), digits) + "," 
                      + String((float)quat_2.w(), digits) + "," + String((float)quat_2.x(), digits) + "," + String((float)quat_2.y(), digits) + "," + String((float)quat_2.z(), digits) + ","
                      + String((float)accel_0.x(), digits) + "," + String((float)accel_0.y(), digits) + "," + String((float)accel_0.z(), digits) + "," 
                      + String((float)accel_1.x(), digits) + "," + String((float)accel_1.y(), digits) + "," + String((float)accel_1.z(), digits) + "," 
                      + String((float)accel_2.x(), digits) + "," + String((float)accel_2.y(), digits) + "," + String((float)accel_2.z(), digits) +  "]";
  
  return buffer;

}; 

void initBNO(Adafruit_BNO055 bno, int bno_number) {

  String message = "Not detected BNO number: " + String(bno_number);
  
  if(!bno.begin())
  {
    Serial.print(message);
    M5DisplayText(message, TFT_WIDTH / 3, TFT_HEIGHT / 2, 1, WHITE, 0);
    while(1);
  }
  delay(1000);
  bno.setExtCrystalUse(true);
  
};

void batteryISR() {
  
};
