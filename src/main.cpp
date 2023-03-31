#include "functions.h"

// define the three IMUs
Adafruit_BNO055 bno_0 = Adafruit_BNO055(55, 0x28);
Adafruit_BNO055 bno_1 = Adafruit_BNO055(55, 0x28);
Adafruit_BNO055 bno_2 = Adafruit_BNO055(55, 0x28);

//Variables for calculating frequency and keeping the loop at a constant rate
const unsigned long period = 10; //ms, therefore 100hz
unsigned long previous_millis = 0; 
int frequency_counter = 0;
unsigned long start_time; 
float frequency = 0;

//Variables for BLE
BLEServer *m5_server = NULL; // Initialize pointers to null
BLEService *m5_service = NULL;
BLECharacteristic *m5_characteristic = NULL;
String buffer = "";


//Variables for battery
unsigned long last_battery_check = 0;
float battery_voltage = 0;


//Set to true if you don't have the TCA9548A chip
bool no_tca = false; //Used for debugging, if you don't have the TCA9548A chip, set this to true.



void setup() {

  //Should be pulled up by default, but just in case, I've had some errors with this.
  pinMode(SDA, INPUT_PULLUP); 
  pinMode(SCL, INPUT_PULLUP);

  M5.begin();        // Init M5Core.
  M5.Power.begin();  // Init Power module.

  M5DisplayText("Starting!", TFT_WIDTH / 2, TFT_HEIGHT / 2, 1, WHITE, 1000);
 
 
  Serial.begin(115200);
  delay(10);
  M5DisplayText("Serial Started", TFT_WIDTH / 2, TFT_HEIGHT / 2, 1, WHITE, 1000);
  

  
  Wire.setClock(400000); //set the i2c speed to 400khz

  if (no_tca) {
    initBNO(bno_0, 0);
  } else {
    TCASelect(0);
    initBNO(bno_0, 0);

    TCASelect(1);
    initBNO(bno_1, 1);
   
    TCASelect(2);
    initBNO(bno_2, 2);
  }
  
  //Init BLE Device
  BLEDevice::init("M5Stack-Server");
  BLEAddress address = BLEDevice::getAddress();
  Serial.print("BLE Address: ");
  Serial.println(address.toString().c_str());

  //Init BLE Server
  m5_server = BLEDevice::createServer();
  m5_server->setCallbacks(new MyserverCallbacks());

  //Init BLE Service, Characteristic and Descriptor
  m5_service = m5_server->createService(SERVICE_UUID);
  m5_characteristic = m5_service->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  BLE2902 *m5_descriptor = new BLE2902(); 
  m5_descriptor->setNotifications(true); 
  m5_descriptor->setIndications(false);
  m5_characteristic->addDescriptor(m5_descriptor);
  m5_characteristic->setCallbacks(new MyCallbacks());
  m5_service->start();
  
  //Init BLE Advertising
  BLEAdvertising *m5_advertising = BLEDevice::getAdvertising();
  m5_advertising->addServiceUUID(SERVICE_UUID);
  m5_advertising->setMaxInterval(0x100); 
  m5_advertising->setMinInterval(0x75);
  m5_advertising->setScanResponse(false); 
  BLEDevice::startAdvertising();


  Serial.println("Init done!");
  M5DisplayText("Init done!", TFT_WIDTH / 2, TFT_HEIGHT / 2, 1, WHITE, 1000);
  
}


void loop() {

 
  if (no_tca) {
    imu::Quaternion quat_0 = bno_0.getQuat();
    imu::Vector<3> accel_0 = bno_0.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

    //Send the data 3 times to simulate 3 IMUs
    buffer = createBuffer(2, quat_0, quat_0, quat_0, accel_0, accel_0, accel_0);
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

    buffer = createBuffer(4, quat_0, quat_1, quat_2, accel_0, accel_1, accel_2);
    
  }
  
  //Send the data via BLE
  m5_characteristic->setValue(buffer.c_str());
  m5_characteristic->notify();
  
  if (frequency_counter == 0) {
    start_time = millis(); // start the timer
  }
  frequency_counter++;
  unsigned long current_time = millis();
  unsigned long elapsed_time = current_time - start_time;

  // Avoid zero division
  if (elapsed_time != 0) {
    frequency = frequency_counter / (elapsed_time / 1000.0);
    //Serial.println(String(frequency));
  }


  //Keep the loop at a constant rate
  while (millis() - previous_millis < period) {
    // do nothing
  }
  previous_millis = millis();
 
}