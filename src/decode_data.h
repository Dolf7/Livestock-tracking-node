#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <string>
// #include <Arduino.h>

class data_collect
{
public:
    // Time - 3 Bytes
    uint8_t hour, minute, second;
    // Speed - Acceleration - 1 Byte
    uint8_t speed;
    // GPS - 8 Bytes
    int lat, lng;

    data_collect(char* data, int size){
        this->hour = char_to_int(data[0]);
        this->minute = char_to_int(data[1]);
        this->second = char_to_int(data[2]);
        this->speed = char_to_int(data[3]);
    }

private:
    int char_to_int(char a){
        return ((int) a);
    }

};