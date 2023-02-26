#include "functions.h"

#define BNO055_SAMPLERATE_DELAY_MS (10)
#define SERVICE_UUID        "bd0f56c6-a403-4d3a-86ba-6fed11ce8473" //Randomly generated UUID
#define CHARACTERISTIC_UUID "1fe90638-437c-490c-ad92-bda3b9423bab" 

Adafruit_BNO055 bno_0 = Adafruit_BNO055(55, 0x28);
Adafruit_BNO055 bno_1 = Adafruit_BNO055(55, 0x28);
Adafruit_BNO055 bno_2 = Adafruit_BNO055(55, 0x28);
BLEServer *m5_server;
BLEService *m5_service;
BLECharacteristic *m5_characteristic;
String buffer = "";
bool no_tca = false; //Used for debugging, if you don't have the TCA9548A chip, set this to true.

int frequency_counter = 0;
unsigned long start_time; 
float frequency = 0;


void setup() {
  //Should be pulled up by default, but just in case, I've had some errors with this.
  pinMode(SDA, INPUT_PULLUP); 
  pinMode(SCL, INPUT_PULLUP);

  M5.begin();        // Init M5Core.
  M5.Power.begin();  // Init Power module.

  M5DisplayText("Starting!", TFT_WIDTH / 2, TFT_HEIGHT / 2, 1, WHITE);
  delay(1000);
  M5.Lcd.clear();
 
  Serial.begin(115200);
  delay(10);
  M5DisplayText("Serial Started", TFT_WIDTH / 2, TFT_HEIGHT / 2, 1, WHITE);
  delay(1000);
  M5.Lcd.clear();

  if (no_tca) {
    if (!bno_0.begin())
    {
      Serial.print("Error: BNO0 not detected");
      M5DisplayText("Error: BNO0 not detected", TFT_WIDTH / 2, TFT_HEIGHT / 2, 1, WHITE);
      while(1);
      delay(1000);
      bno_0.setExtCrystalUse(true);
    }
  }
  else {
    TCASelect(0);
    if(!bno_0.begin()) {
      Serial.print("Error: BNO0 not detected");
      M5DisplayText("Error: BNO0 not detected", TFT_WIDTH / 2, TFT_HEIGHT / 2, 1, WHITE);
      while(1);
    }
    delay(1000);
    bno_0.setExtCrystalUse(true);

    TCASelect(1);
    if(!bno_1.begin()) {
      Serial.print("Error: BNO1 not detected");
      M5DisplayText("Error: BNO1 not detected", TFT_WIDTH / 2, TFT_HEIGHT / 2, 1, WHITE);
      while(1);
    }
    delay(1000);
    bno_1.setExtCrystalUse(true);

    TCASelect(2);
    if(!bno_2.begin()) {
      Serial.print("Error: BNO2 not detected");
      M5DisplayText("Error: BNO2 not detected", TFT_WIDTH / 2, TFT_HEIGHT / 2, 1, WHITE);
      while(1);
    }
    delay(1000);
    bno_2.setExtCrystalUse(true);
  }
  

  BLEDevice::init("M5Stack-Server");
  m5_server = BLEDevice::createServer();
  m5_service = m5_server->createService(SERVICE_UUID);
  m5_characteristic = m5_service->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
  m5_characteristic->setValue("Hello, World!");
  m5_service->start();
  BLEAdvertising *m5_advertising = BLEDevice::getAdvertising();
  m5_advertising->addServiceUUID(SERVICE_UUID);
  m5_advertising->setScanResponse(true);
  m5_advertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  m5_advertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

void loop() {
  
  if (no_tca) {
    imu::Quaternion quat_0 = bno_0.getQuat();
    imu::Vector<3> accel_0 = bno_0.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    buffer = String((float)quat_0.w(), 4) + "," + String((float)quat_0.x(), 4) + "," + String((float)quat_0.y(), 4) + "," + String((float)quat_0.z(), 4) + "," + String((float)accel_0.x(), 4) + "," + String((float)accel_0.y(), 4) + "," + String((float)accel_0.z(), 4);

  }
  else {
    TCASelect(0);
    // read quaternion data
    imu::Quaternion quat_0 = bno_0.getQuat();
    // read accelerometer data
    imu::Vector<3> accel_0 = bno_0.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    TCASelect(1);
    imu::Quaternion quat_1 = bno_1.getQuat();
    imu::Vector<3> accel_1 = bno_1.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    TCASelect(2);
    imu::Quaternion quat_2 = bno_2.getQuat();
    imu::Vector<3> accel_2 = bno_2.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    
    buffer = String((float)quat_0.w(), 4) + "," + String((float)quat_0.x(), 4) + "," + String((float)quat_0.y(), 4) + "," + String((float)quat_0.z(), 4) + "," + String((float)quat_1.w(), 4) + "," + String((float)quat_1.x(), 4) + "," + String((float)quat_1.y(), 4) + "," + String((float)quat_1.z(), 4) + "," + String((float)quat_2.w(), 4) + "," + String((float)quat_2.x(), 4) + "," + String((float)quat_2.y(), 4) + "," + String((float)quat_2.z(), 4);
    buffer += "," + String((float)accel_0.x(), 4) + "," + String((float)accel_0.y(), 4) + "," + String((float)accel_0.z(), 4) + "," + String((float)accel_1.x(), 4) + "," + String((float)accel_1.y(), 4) + "," + String((float)accel_1.z(), 4) + "," + String((float)accel_2.x(), 4) + "," + String((float)accel_2.y(), 4) + "," + String((float)accel_2.z(), 4);
  }
  
  Serial.println(buffer);

  //send quaternion and accelerometer data via BLE
  m5_characteristic->setValue(buffer.c_str());

  
  //Serial.print(F("Quaternion: "));
  //Serial.println(buffer);
  

  //std::string value = m5_characteristic->getValue();
  //Serial.print("The new characteristic value is: ");
  //Serial.println(value.c_str());


  if (frequency_counter == 0) {
    start_time = millis(); // start the timer
  }
  frequency_counter++;
  unsigned long current_time = millis();
  unsigned long elapsed_time = current_time - start_time;

  // Avoid zero division
  if (elapsed_time != 0) {
    frequency = frequency_counter / (elapsed_time / 1000.0);
    //Serial.println("Frequency: " + String(frequency));
    /*
    M5.Lcd.clearDisplay();
    M5.Lcd.setTextColor(WHITE); 
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(TFT_WIDTH / 2, TFT_HEIGHT / 2);
    M5.Lcd.printf("Frequency: %f", frequency);
    */
    
  }
  delay(BNO055_SAMPLERATE_DELAY_MS);
}