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
#define EEPROM_SIZE 216

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 5        /* Time ESP32 will go to sleep (in seconds) */

// Global Object
HardwareSerial gpsSerial(1);
RTC_DS3231 rtc;
TinyGPSPlus gps;

// Global Variable
RTC_DATA_ATTR int bootCount = 0;
uint8_t eeprom_addres_to_write = 0;
uint8_t hour, minute, seconds;
double gps_speed;
double latitude, longitude;

// declare Initizalise Function
void init_LoRa(int SF, int CR, long BW, uint8_t SW);
void init_RTC();
void init_GPS();
void init_eeprom();

void stop_all_sensor();

// Declare Function
void run();
void time_acquisition();
void gps_acquisition();
void test_print();
void write_to_eeprom(data_store data_sens);
void clear_all_eeprom();
void print_all_eeprom();
void read_and_send();
void print_wakeup_reason();

void setup(void)
{
  // Init Serial 1 - For Monitoring;
  Serial.begin(115200);

  pinMode(rtc_trig_pin, OUTPUT);
  pinMode(gps_trig_pin, OUTPUT);
  pinMode(lora_trig_pin, OUTPUT);

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  // Print the wakeup reason for ESP32
  print_wakeup_reason();
  Serial.println("Run Program !");

  run();

  // delay(2000);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Going to Sleep");
  Serial.flush();
  esp_deep_sleep_start();
}

void loop()
{ // NEVER CALLED
}

void run()
{
  // Init EEPROM
  init_eeprom();

  Wire.begin();
  // Init RTC
  init_RTC();
  // INIT GPS
  init_GPS();

  time_acquisition();
  gps_acquisition();

  // test_print();
  stop_all_sensor();

  data_store data_sensor(hour, minute, seconds, gps_speed, latitude, longitude);

  data_sensor.test_print_properties();

  write_to_eeprom(data_sensor);
  print_all_eeprom();

  if (eeprom_addres_to_write >= EEPROM_SIZE)
  {
    read_and_send();
    Serial.println("FINISH READ AND SEND");
  }

  Serial.print("EEPROM ADDRESS : ");
  Serial.println(eeprom_addres_to_write);
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

  Serial.print("GPS Speed = ");
  Serial.println(gps_speed);

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
  Serial.println("RTC Found!");

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
    EEPROM.commit();
  }
}

void stop_all_sensor()
{
  digitalWrite(gps_trig_pin, LOW);
  digitalWrite(rtc_trig_pin, LOW);
}

void time_acquisition(){
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
      if (gps.location.isUpdated())
      {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
        gps_speed = gps.speed.mps();
      }
    }
  }
}

void write_to_eeprom(data_store data_sens)
{
  eeprom_addres_to_write = EEPROM.read(EEPROM_SIZE);
  Serial.print("");
  Serial.print("EEPROM Address : ");
  Serial.print(eeprom_addres_to_write);
  EEPROM.commit();

  EEPROM.put(eeprom_addres_to_write, data_sens);
  Serial.println("Size of Data : ");
  Serial.println(sizeof(data_sens));
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

void read_and_send()
{
  // INIT LORA
  init_LoRa(8, 4, 125E3, 0xF3);

  // READ DATA FROM AND SAVE IN LOCAL VARIABLE
  uint8_t dataString[EEPROM_SIZE];
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    dataString[i] = EEPROM.read(i);
  }
  // dataString[EEPROM_SIZE] = '\0';
  Serial.print("Data read from EEPROM: ");

  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    Serial.print(dataString[i], HEX);
    Serial.print(' ');
  }
  Serial.println(" ");

  // String string_send(dataString);

  // Serial.print("STR : ");
  // Serial.print(string_send);
  // Serial.print(" With Size : ");
  // Serial.println(string_send.length());

  // SEND
  LoRa.beginPacket();
  LoRa.write(dataString, EEPROM_SIZE+1);
  LoRa.endPacket();

  // CLEAR EEPROM
  clear_all_eeprom();
}

void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup caused by ULP program");
    break;
  default:
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
    break;
  }
}