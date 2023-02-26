#include <M5Stack.h>
#include <Wire.h>
#include <BLEDevice.h>
#include <BLEServer.h> 
#include "TCA9548.h"
#include "Adafruit_BNO055.h"
#define TCA9548A_ADDRESS 0x70

void displaySensorDetails(Adafruit_BNO055 imu);
void M5DisplayText(String text, int x, int y, int size, int color); 
void TCASelect(uint8_t i2c_address);
