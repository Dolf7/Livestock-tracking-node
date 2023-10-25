#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <Arduino.h>

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

    data_store(int hour, int minute, int second, std::vector<double> ax, std::vector<double> ay, std::vector<double> az, int time_interval, double lat, double lng)
    {
        this->hour = hour;
        this->minute = minute;
        this->second = second;
        this->speed = acc_to_speed(calculate_speed(ax, time_interval), calculate_speed(ay, time_interval), calculate_speed(az, time_interval));
        this->lat = floatToIntWithIndex(lat);
        this->lng = floatToIntWithIndex(lng);
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
    int floatToIntWithIndex(double double_num)
    {
        if(double_num == 0)
            return -7.123456;
        
        // Convert double to int (64bit -> 32bit)
        // ex : (double) 123.456789 -> (int) 1234567893
        // 3 in the of int is the index of dot(.) in real double number

        // Convert the float to a string
        std::string float_str = std::to_string(double_num);
        int decimal_point_index = float_str.find('.');

        // Remove the decimal point and concatenate the resulting string
        std::string int_str = float_str.substr(0, decimal_point_index) + float_str.substr(decimal_point_index + 1) + std::to_string((float_str[0] != '-' ? decimal_point_index : decimal_point_index - 1));

        // Convert the string to an integer
        int int_result = std::stoi(int_str);
        return int_result;
    }

    uint8_t acc_to_speed(double ax, double ay, double az)
    {
        // Convert Acc from XYZ angle(3*4 bytes) to speed (1 Bytes)
        return sqrt((ax * ax) + (ay * ay) + (az * az));
    }
    // Function to calculate speed from acceleration data
    double calculate_speed(const std::vector<double> &acceleration, double time_interval)
    {
        // Calculate velocity by integrating acceleration
        std::vector<double> velocity;
        for (const auto &accel : acceleration)
        {
            velocity.push_back(accel * (time_interval / 1000)); // divide by 1000 convert (ms -> s)
        }

        // Calculate speed by integrating velocity
        double speed = 0.0;
        for (const auto &v : velocity)
        {
            speed += v;
        }
        return speed;
    }
};
