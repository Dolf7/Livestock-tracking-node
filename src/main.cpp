#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include "RTClib.h"
#include <LoRa.h>
#include <SPI.h>
#include <EEPROM.h>
#include <vector>
#include "data.h"

// Define for Pin
#define RXPin 16
#define TXPin 17
#define ss 5
#define rst 14
#define dio0 2

// Define for compiled static value
#define EEPROM_SIZE 216
#define SF 12 // Define spreading Factor

// Global Object
HardwareSerial gpsSerial(1);
Adafruit_MPU6050 mpu;
RTC_DS3231 rtc;
TinyGPSPlus gps;

// Global Variable
uint8_t eeprom_addres_to_write = 0;
uint8_t hour, minute, seconds;
double latitude, longitude;
float ax, ay, az;

// Declare Function
void gps_func();
void test_print();
void write_to_eeprom(data_store data_sens);
void clear_all_eeprom();
void print_all_eeprom();
void read_and_send();

void setup(void)
{
  // Init Serial 1 - For Monitoring;
  Serial.begin(115200);

  // Init EEPROM
  EEPROM.begin(EEPROM_SIZE);
  clear_all_eeprom();

  Wire.begin();
  // Init RTC
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (!rtc.begin())
    {
      Serial.println("init RTC ...");
    }
  }
  Serial.println("RTC Found!");

  if (rtc.lostPower())
  {
    Serial.println("RTC lost power, setting the time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Init MPU - Accelerometer;
  if (!mpu.begin(0x69))
  {
    Serial.println("Failed to find MPU6050 chip");
    while (!mpu.begin(0x69))
    {
      Serial.println("Init MPU ...");
    }
  }
  Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);

  // INIT GPS
  gpsSerial.begin(9600, SERIAL_8N1, RXPin, TXPin);

  // INIT LORA
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(433E6))
  {
    Serial.println("Init LoRa ...");
  }

  LoRa.setSpreadingFactor(8);
  LoRa.setCodingRate4(4);
  LoRa.setSyncWord(0xF3);

  Serial.println("Run Program !");
  delay(3500);
}

void loop()
{

  DateTime now = rtc.now();
  hour = (uint8_t)now.hour();
  minute = (uint8_t)now.minute();
  seconds = (uint8_t)now.second();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  ax = a.acceleration.x;
  ay = a.acceleration.y;
  az = a.acceleration.z;

  gps_func();

  test_print();

  data_store data_sensor(hour, minute, seconds, ax, ay, az, latitude, longitude);
  write_to_eeprom(data_sensor);
  // print_all_eeprom();

  if (eeprom_addres_to_write >= EEPROM_SIZE)
  {
    read_and_send();
    Serial.println("FINISH READ AND SEND");
  }

  Serial.print("EEPROM ADDRESS : ");
  Serial.println(eeprom_addres_to_write);
  Serial.println("-----------------------");

  delay(5000);
}

void test_print()
{
  Serial.print("RTC =  ");
  Serial.print(hour, DEC);
  Serial.print(':');
  Serial.print(minute, DEC);
  Serial.print(':');
  Serial.print(seconds, DEC);
  Serial.println(" ");

  Serial.print("MPU =  ");
  Serial.print(ax);
  Serial.print(':');
  Serial.print(ay);
  Serial.print(':');
  Serial.print(az);
  Serial.println(" ");

  Serial.print("GPS = ");
  Serial.print(longitude, 6);
  Serial.print(" | ");
  Serial.println(latitude, 6);

  Serial.println("-----");
}

void gps_func()
{
  while (gpsSerial.available() > 0)
  {
    if (gps.encode(gpsSerial.read()))
    {
      if (gps.location.isUpdated())
      {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
      }
    }
  }
}

void accelero_acquisition(){

}

void write_to_eeprom(data_store data_sens)
{
  EEPROM.put(eeprom_addres_to_write, data_sens);
  // Serial.println("Size of Data : ");
  // Serial.println(sizeof(data_sens));
  eeprom_addres_to_write += sizeof(data_sens);
}

void print_all_eeprom()
{
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    if (i % 3 == 0)
    {
      Serial.println(" ");
      Serial.print(i);
      Serial.print("  ");
    }
    Serial.print("0x");
    Serial.print(EEPROM.read(i), HEX);
    Serial.print("\t");
  }
}

void clear_all_eeprom()
{
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    EEPROM.write(i, 0xFF);
    EEPROM.commit();
  }
  eeprom_addres_to_write = 0;
}

void read_and_send()
{
  //READ DATA FROM AND SAVE IN LOCAL VARIABLE
  char dataString[EEPROM_SIZE + 1];
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    dataString[i] = EEPROM.read(i);
  }
  dataString[EEPROM_SIZE] = '\0';
  Serial.print("Data read from EEPROM: ");

  for(int i=0; i<EEPROM_SIZE; i++){
    Serial.print(dataString[i], HEX);
    Serial.print(' ');
  }
  Serial.println(" ");

  // SEND
  LoRa.beginPacket();
  LoRa.print(dataString);
  LoRa.endPacket();

  //CLEAR EEPROM
  clear_all_eeprom();
}