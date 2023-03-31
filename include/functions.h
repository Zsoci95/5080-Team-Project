#include <M5Stack.h>
#include <Wire.h>
#include <BLEDevice.h>
#include <BLEServer.h> 
#include "TCA9548.h"
#include "Adafruit_BNO055.h"
#include <BLE2902.h>

#define TCA9548A_ADDRESS      0x70
#define SERVICE_UUID          "bd0f56c6-a403-4d3a-86ba-6fed11ce8473" //Randomly generated UUID
#define CHARACTERISTIC_UUID   "1fe90638-437c-490c-ad92-bda3b9423bab" 
#define BATTERY_READ_INTERVAL 300000 //5 minutes in ms 

void displayESPInfo();
void displaySensorDetails(Adafruit_BNO055 imu);
void M5DisplayText(String text, int x, int y, int size, int color, int time); 
void TCASelect(uint8_t i2c_address);

class MyserverCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer);
  void onDisconnect(BLEServer* pServer); 
};

class MyCallbacks: public BLECharacteristicCallbacks {
  void onNotify(BLECharacteristic *pCharacteristic);
};

String createBuffer(char digits, imu::Quaternion quat_0, imu::Quaternion quat_1, imu::Quaternion quat_2, 
imu::Vector<3> accel_0,  imu::Vector<3> accel_1,  imu::Vector<3> accel_2);

void initBNO(Adafruit_BNO055 bno, int bno_number); 

void batteryISR(); 