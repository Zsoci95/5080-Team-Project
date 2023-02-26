
#include <M5Stack.h>
#include <Wire.h>
#include "Adafruit_BNO055.h"
#include <BLEDevice.h>
#include <BLEServer.h> 

#define BNO055_SAMPLERATE_DELAY_MS (10)
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer *m5_server;
BLEService *m5_service;
BLECharacteristic *m5_characteristic;
String buffer = "";

int frequency_counter = 0;
unsigned long start_time; 
float frequency = 0;

void displaySensorDetails(void)
{
  sensor_t sensor;
  bno.getSensor(&sensor);
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
}


void setup() {
  //Should be pulled up by default, but just in case, I've had some errors with this.
  pinMode(SDA, INPUT_PULLUP); 
  pinMode(SCL, INPUT_PULLUP);

  M5.begin();        // Init M5Core.
  M5.Power.begin();  // Init Power module.

  M5.Lcd.setTextColor(WHITE); 
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(TFT_WIDTH / 2, TFT_HEIGHT / 2);
  M5.Lcd.println("Starting...");
  
  
  Serial.begin(115200);
  delay(10);
  Serial.println("WebSerial 3D Firmware"); Serial.println("");

  if(!bno.begin())
  {
    Serial.print("Error: No BNO055 detected");
    while(1);
  }

  delay(1000);
  bno.setExtCrystalUse(true);

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
  /*
  sensors_event_t event;
  bno.getEvent(&event);
  
  Serial.print(F("Orientation: "));
  Serial.print(360 - (float)event.orientation.x);
  Serial.print(F(", "));
  Serial.print((float)event.orientation.y);
  Serial.print(F(", "));
  Serial.print((float)event.orientation.z);
  Serial.println(F(""));
  */

  /* The WebSerial 3D Model Viewer also expects data as roll, pitch, heading */
  imu::Quaternion quat = bno.getQuat();

  buffer = String((float)quat.w(), 4) + "," + String((float)quat.x(), 4) + "," + String((float)quat.y(), 4) + "," + String((float)quat.z(), 4) + "," + String((float)quat.w(), 4) + "," + String((float)quat.x(), 4) + "," + String((float)quat.y(), 4) + "," + String((float)quat.z(), 4) + "," + String((float)quat.w(), 4) + "," + String((float)quat.x(), 4) + "," + String((float)quat.y(), 4) + "," + String((float)quat.z(), 4);
  buffer += buffer; //double the data to test if it can be sent
  
  //code that can send quaternion data via ble to a client
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
    Serial.println("Frequency: " + String(frequency));
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