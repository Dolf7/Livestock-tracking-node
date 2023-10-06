#include <Arduino.h>
#include <eeprom.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include "RTClib.h"
#include <LoRa.h>
#include <SPI.h>

#define EEPROM_SIZE 255
#define RXPin 16
#define TXPin 17
#define ss 5
#define rst 14
#define dio0 2

HardwareSerial gpsSerial(1); // Serial object for GPS module

Adafruit_MPU6050 mpu;
RTC_DS3231 rtc;
TinyGPSPlus gps;
uint8_t hour, minute, seconds;
uint8_t eeprom_addres_to_write = -1;

float latitude, longitude;

void write_to_eeprom();
void print_all_eeprom();
void clear_all_eeprom();
void gps_func();

void setup(void)
{
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
      delay(1000);
      Serial.print(".");
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
      delay(1000);
      Serial.print(".-");
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
    Serial.println(".");
    delay(500);
  }

  LoRa.setSpreadingFactor(6);
  LoRa.setCodingRate4(4);

  LoRa.setSyncWord(0xF3);

  Serial.println("");
  delay(3500);
}

void loop()
{
  sensors_event_t a, g, temp;
  DateTime now = rtc.now();

  hour = (uint8_t)now.hour();
  minute = (uint8_t)now.minute();
  seconds = (uint8_t)now.second();

  write_to_eeprom();

  Serial.print(hour, DEC);
  Serial.print(':');
  Serial.print(minute, DEC);
  Serial.print(':');
  Serial.print(seconds, DEC);
  Serial.println();

  mpu.getEvent(&a, &g, &temp);

  gps_func();
  Serial.print("Lat : ");
  Serial.print(latitude);
  Serial.print("Lng : ");
  Serial.print(longitude);

  Serial.println("");
  delay(5000);
}

void write_to_eeprom()
{
  EEPROM.write(++eeprom_addres_to_write, hour);
  EEPROM.write(++eeprom_addres_to_write, minute);
  EEPROM.write(++eeprom_addres_to_write, seconds);
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
}

void gps_func()
{
  int i = 0, j = 0;
  while (gpsSerial.available() > 0)
  {
    if (gps.encode(gpsSerial.read()))
    {
      if (gps.location.isValid())
      {
        Serial.print("Latitude: ");
        Serial.println(gps.location.lat(), DEC);
        Serial.print("Longitude: ");
        Serial.println(gps.location.lng(), DEC);
        longitude = gps.location.lng();
        latitude = gps.location.lat();
      }
    }
  }
  Serial.println("LOOP FIN");
  delay(1000);
}
