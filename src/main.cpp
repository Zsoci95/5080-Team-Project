#include "functions.h"

#define BNO055_SAMPLERATE_DELAY_MS (10)
#define SERVICE_UUID        "bd0f56c6-a403-4d3a-86ba-6fed11ce8473" //Randomly generated UUID
#define CHARACTERISTIC_UUID "1fe90638-437c-490c-ad92-bda3b9423bab" 
//BLEDescriptor *m5_descriptor = new BLEDescriptor(BLEUUID((uint16_t)0x2901));


Adafruit_BNO055 bno_0 = Adafruit_BNO055(55, 0x28);
Adafruit_BNO055 bno_1 = Adafruit_BNO055(55, 0x28);
Adafruit_BNO055 bno_2 = Adafruit_BNO055(55, 0x28);

//100Hz = 10ms 
const unsigned long period = 10; //ms
unsigned long previous_millis = 0; 

BLEServer *m5_server = NULL; // Initialize pointers to null
BLEService *m5_service = NULL;
BLECharacteristic *m5_characteristic = NULL;
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
  

  BLEDevice::init("M5Stack-Server");
  BLEAddress address = BLEDevice::getAddress();
  Serial.print("BLE Address: ");
  Serial.println(address.toString().c_str());
  m5_server = BLEDevice::createServer();
  m5_server->setCallbacks(new MyserverCallbacks());
  m5_service = m5_server->createService(SERVICE_UUID);
  m5_characteristic = m5_service->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  BLE2902 *m5_descriptor = new BLE2902(); 
  m5_descriptor->setNotifications(true); 
  m5_descriptor->setIndications(false);
  m5_characteristic->addDescriptor(m5_descriptor);
  m5_characteristic->setCallbacks(new MyCallbacks());
  m5_service->start();
  
  BLEAdvertising *m5_advertising = BLEDevice::getAdvertising();
  m5_advertising->addServiceUUID(SERVICE_UUID);
  //m5_advertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  m5_advertising->setMaxInterval(0x100); // TODO: What happens if setminpreffered is 0 and these are removed?
  m5_advertising->setMinInterval(0x75);
  m5_advertising->setScanResponse(false); 
  
  //m5_advertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
  
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

    buffer = createBuffer(2, quat_0, quat_1, quat_2, accel_0, accel_1, accel_2);
    
  }
  
  //Serial.println(buffer);
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

  while (millis() - previous_millis < period) {
    // do nothing
  }
  previous_millis = millis();
 
}