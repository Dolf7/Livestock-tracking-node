#include <Arduino.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include "RTClib.h"
#include <LoRa.h>
#include <SPI.h>
#include <EEPROM.h>
#include "data.h"

// Define for Pin
#define RXPin 16
#define TXPin 17
#define ss 5
#define rst 14
#define dio0 2
#define lora_trig_pin 25
#define rtc_trig_pin 27
#define gps_trig_pin 26

// Define for compiled static value
#define SPREADING_FACTOR 7 //SET SPREADING FACTOR
#define BANDWIDTH 125E3
#define CODERATE 4
#define SYNCWORD 0x34

#ifndef SPREADING_FACTOR
Serial.println("NOT INIT SF");
#elif SPREADING_FACTOR == 7
#define EEPROM_SIZE 220
#elif SPREADING_FACTOR == 8
#define EEPROM_SIZE 220
#elif SPREADING_FACTOR == 9
#define EEPROM_SIZE 110 
#elif SPREADING_FACTOR == 10
#define EEPROM_SIZE 44
#elif SPREADING_FACTOR == 11
#define EEPROM_SIZE 44
#elif SPREADING_FACTOR == 12
#define EEPROM_SIZE 44
#endif

#define uS_TO_mS_FACTOR 1000 /* Conversion factor for micro seconds to mili seconds */

// declare Initizalise Function
void init_LoRa(int SF, int CR, long BW, uint8_t SW);
void init_RTC();
void init_GPS();
void init_eeprom();
void stop_all_sensor();
void stop_rtc();
void stop_gps();
void stop_lora();

// Declare Function
void run();
void time_acquisition();
void gps_acquisition();
void wait_gps();
void write_to_eeprom(data_store data_sens);
void clear_all_eeprom();
void print_all_eeprom();
void read_from_eeprom_and_send();
void send_data(uint8_t stringData[EEPROM_SIZE], uint8_t deviceId);
void go_to_sleep(unsigned long run_time);
uint64_t count_acqusition_time();
void test_print();

// Global Object
HardwareSerial gpsSerial(1);
RTC_DS3231 rtc;
TinyGPSPlus gps;

// Global Variable
RTC_DATA_ATTR int bootCount = 0;
uint8_t eeprom_addres_to_write = 0;
uint8_t hour, minute, seconds;
double latitude, longitude;
const uint64_t time_to_each_acquisition = count_acqusition_time();

// DEVICE ID
const uint8_t deviceId = 73;

void setup(void)
{
  // Init Serial 1 - For Monitoring;
  Serial.begin(115200);

  pinMode(rtc_trig_pin, OUTPUT);
  pinMode(gps_trig_pin, OUTPUT);
  pinMode(lora_trig_pin, OUTPUT);

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  unsigned long start_time = millis();
  run();
  unsigned long run_time = millis() - start_time;

  Serial.print("Active Time : ");
  Serial.println(run_time);

  go_to_sleep(run_time);
}

void loop()
{ // NEVER CALLED
}

void run()
{
  Wire.begin();
  // Location & speed acquition

  unsigned long noww = millis();

  init_GPS();
  wait_gps();
  gps_acquisition();
  stop_gps();
  Serial.print("GPS STATE : ");
  Serial.println(millis() - noww);

  noww = millis();
  // Time Acquisition
  init_RTC();
  time_acquisition();
  stop_rtc();
  Serial.print("RTC STATE : ");
  Serial.println(millis() - noww);

  // test_print();
  noww = millis();
  data_store data_sensor(hour, minute, seconds, latitude, longitude);

  data_sensor.test_print_properties();

  init_eeprom();
  write_to_eeprom(data_sensor);
  Serial.print("SAVE STATE : ");
  Serial.println(millis() - noww);

  if (eeprom_addres_to_write >= EEPROM_SIZE)
  {
    read_from_eeprom_and_send();
    Serial.println("FINISH READ AND SEND");
  }

  Serial.println("-----------------------");
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

  Serial.print("GPS = ");
  Serial.print(longitude, 6);
  Serial.print(" | ");
  Serial.println(latitude, 6);
  Serial.println("-----");
}

// INIT FUNCTION

void init_LoRa(int SF, int CR, long BW, uint8_t SW)
{
  digitalWrite(lora_trig_pin, HIGH);
  delay(100);

  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(433E6))
  {
    Serial.println("Init LoRa ...");
  }

  LoRa.setSpreadingFactor(SF);
  LoRa.setCodingRate4(CR);
  LoRa.setSignalBandwidth(BW);
  LoRa.setSyncWord(SW);

  Serial.println("Start LoRa.");
}

void init_RTC()
{
  digitalWrite(rtc_trig_pin, HIGH);
  delay(100);
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (!rtc.begin())
    {
      Serial.println("init RTC ...");
    }
  }
  if (rtc.lostPower())
  {
    Serial.println("RTC lost power, setting the time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void init_GPS()
{
  digitalWrite(gps_trig_pin, HIGH);
  delay(100);
  gpsSerial.begin(9600, SERIAL_8N1, RXPin, TXPin);
  Serial.println("Init GPS...");
}

void init_eeprom()
{
  EEPROM.begin(EEPROM_SIZE + 1);
  if (bootCount == 1)
  {
    Serial.println("FIRST WAKE set EEPROM 0");
    EEPROM.write(EEPROM_SIZE, 0);
    clear_all_eeprom();
    EEPROM.commit();
  }
}

void stop_all_sensor()
{
  digitalWrite(gps_trig_pin, LOW);
  digitalWrite(rtc_trig_pin, LOW);
}

void stop_rtc()
{
  digitalWrite(rtc_trig_pin, LOW);
}

void stop_gps()
{
  digitalWrite(gps_trig_pin, LOW);
}

void stop_lora()
{
  digitalWrite(lora_trig_pin, LOW);
}

// Acquisistion Function
void time_acquisition()
{
  DateTime now = rtc.now();
  hour = (uint8_t)now.hour();
  minute = (uint8_t)now.minute();
  seconds = (uint8_t)now.second();
}

void gps_acquisition()
{
  while (gpsSerial.available() > 0)
  {
    if (gps.encode(gpsSerial.read()))
    {
      if (gps.location.isValid())
      {
        latitude = gps.location.lat();
        Serial.print("lat : ");
        Serial.println(latitude, 6);
        longitude = gps.location.lng();
        Serial.print("lng : ");
        Serial.println(longitude, 6);
      }
    }
  }
}

// WAIT STATE
void wait_gps()
{
  Serial.println("WAIT GPS STATE");
  unsigned long e = millis();
  float test_lat = 0;
  while (test_lat == 0)
  {
    while (gpsSerial.available() > 0)
    {
      if (gps.encode(gpsSerial.read()))
      {
        if (gps.location.isValid())
        {
          test_lat = gps.location.lat();
          if (test_lat != 0)
          {
            Serial.println("GPS");
            Serial.println(millis() - e);
            return;
          }
        }
      }
    }
  }
}

// SAVING FUCNTIONS
void write_to_eeprom(data_store data_sens)
{
  eeprom_addres_to_write = EEPROM.read(EEPROM_SIZE);
  Serial.print("EEPROM Address : ");
  Serial.println(eeprom_addres_to_write);
  EEPROM.commit();

  EEPROM.put(eeprom_addres_to_write, data_sens);
  eeprom_addres_to_write += sizeof(data_sens);

  EEPROM.write(EEPROM_SIZE, eeprom_addres_to_write);
  EEPROM.commit();
}

void print_all_eeprom()
{
  for (int i = 0; i < EEPROM_SIZE + sizeof(eeprom_addres_to_write); i++)
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
  EEPROM.write(EEPROM_SIZE, eeprom_addres_to_write);
  EEPROM.commit();
}

// Sending Function
void read_from_eeprom_and_send()
{
  unsigned long nowww = millis();
  // READ DATA FROM AND SAVE IN LOCAL VARIABLE
  uint8_t dataString[EEPROM_SIZE];
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    dataString[i] = EEPROM.read(i);
  }
  Serial.print("Data read from EEPROM: ");

  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    Serial.print(dataString[i], HEX);
    Serial.print(' ');
  }
  Serial.println(" ");
  Serial.print("READ STATE");
  Serial.println(millis() - nowww);


  send_data(dataString, deviceId);

  // CLEAR EEPROM
  clear_all_eeprom();
}

void send_data(uint8_t dataString[EEPROM_SIZE], uint8_t deviceId)
{

  // INIT LORA
  unsigned long a = millis();
  init_LoRa(SPREADING_FACTOR, CODERATE, BANDWIDTH, SYNCWORD);
  // SEND
  LoRa.beginPacket();
  LoRa.write(deviceId);
  LoRa.write(dataString, EEPROM_SIZE);
  LoRa.endPacket();
  Serial.println(millis() - a);

  stop_lora();
}

uint8_t getDataRate()
{
  if (SPREADING_FACTOR == 12 && BANDWIDTH == 125E3)
    return 0;
  if (SPREADING_FACTOR == 11 && BANDWIDTH == 125E3)
    return 1;
  if (SPREADING_FACTOR == 10 && BANDWIDTH == 125E3)
    return 2;
  if (SPREADING_FACTOR == 9 && BANDWIDTH == 125E3)
    return 3;
  if (SPREADING_FACTOR == 8 && BANDWIDTH == 125E3)
    return 4;
  if (SPREADING_FACTOR == 7 && BANDWIDTH == 125E3)
    return 5;
  if (SPREADING_FACTOR == 7 && BANDWIDTH == 250E3)
    return 6;
}

uint64_t count_acqusition_time()
{
  uint8_t data_rate = getDataRate();
  switch (data_rate)
  {
  case 0:
    return 1987700;
    break;
  case 1:
    return 993816;
    break;
  case 2:
    return 544104;
    break;
  case 3:
    return 259805;
  case 4:
    return 138082;
  case 5:
    return 39242;
  default:
    return 0;
    break;
  }
}

void go_to_sleep(unsigned long run_time)
{
  unsigned long sleep_time = (time_to_each_acquisition - run_time < 0 ? 0 : time_to_each_acquisition - run_time);

  Serial.print("Sleep Time : ");
  Serial.println(sleep_time);
  esp_sleep_enable_timer_wakeup(sleep_time * uS_TO_mS_FACTOR);
  Serial.println("Going to Sleep");
  Serial.flush();
  esp_deep_sleep_start();
}