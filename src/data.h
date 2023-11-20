#include <stdio.h>
#include <iostream>
// #include <Arduino.h>

#define ACCELEROMETER_SENSITIVITY 16384 // Sensitivity for MPU-6050 accelerometer (for +/- 2g range)

class data_store
{
public:
    // Time - 3 Bytes
    uint8_t hour, minute, second;
    // Speed - Acceleration - 1 Byte
    uint8_t speed;
    // GPS - 8 Bytes
    int lat, lng;

    data_store(int hour, int minute, int second, int speed, double lat, double lng)
    {
        this->hour = hour;
        this->minute = minute;
        this->second = second;
        this->speed = speed;
        this->lat = doubleToIntWithIndex(lat);
        this->lng = doubleToIntWithIndex(lng);
    }

    double getLatValue()
    {
        return intWithIndexToDouble(this->lat);
    }

    double getLngValue()
    {
        return intWithIndexToDouble(this->lng);
    }

    void test_print_properties()
    {
        Serial.println("----------");
        Serial.println("Object Data Properties : ");

        Serial.print("Time = ");
        Serial.print(this->hour);
        Serial.print(" : ");
        Serial.print(this->minute);
        Serial.print(" : ");
        Serial.println(this->second);

        Serial.print("Speed = ");
        Serial.println(this->speed);

        Serial.print("GPS = ");
        Serial.print(this->lat);
        Serial.print(" | ");
        Serial.println(this->lng);
        Serial.println("---------");
    }

private:
    int doubleToIntWithIndex(double double_num)
    {
        if (double_num == 0)
            return 0;

        // Convert double to int (64bit -> 32bit)
        // ex : (double) 123.456789 -> (int) 1234567893
        // 3 in the of int is the index of dot(.) in real double number

        // Convert the float to a string
        std::string double_str = std::to_string(double_num);
        int decimal_point_index = double_str.find('.');

        // Remove the decimal point and concatenate the resulting string
        std::string int_str = double_str.substr(0, decimal_point_index) + double_str.substr(decimal_point_index + 1) + std::to_string((double_str[0] != '-' ? decimal_point_index : decimal_point_index - 1));

        // Convert the string to an integer
        int int_result = std::stoi(int_str);
        return int_result;
    }

    double intWithIndexToDouble(int int_num)
    {
        std::string int_str = std::to_string(int_num);

        int decimal_point_index = static_cast<int>(int_str[int_str.length() - 1]) - '0';

        std::string decimal_string = int_str.substr(0, int_str.length() - 1);

        std::string decimal_string_fix_value;
        int j = 0;
        for (int i = 0; i < decimal_string.length() + 1; i++)
        {
            if (i == (decimal_string[0] == '-' ? decimal_point_index + 1 : decimal_point_index))
            {
                decimal_string_fix_value += '.';
                continue;
            }
            decimal_string_fix_value += decimal_string[j];
            j++;
        }

        double real_data = std::stod(decimal_string_fix_value);
        return real_data;
    }
};
