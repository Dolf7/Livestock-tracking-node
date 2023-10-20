#include <stdio.h>
#include <math.h>
#include <iostream>
class data_store
{
public:
    // Time - 3 Bytes
    uint8_t hour, minute, second;
    // Speed - Acceleration - 1 Byte
    uint8_t speed;
    // GPS - 8 Bytes
    int lat, lng;

    data_store(int hour, int minute, int second, float ax, float ay, float az, double lat, double lng)
    {
        this->hour = hour;
        this->minute = minute;
        this->second = second;
        this->speed = acc_to_speed(ax, ay, az);
        this->lat = floatToIntWithIndex(lat);
        this->lng = floatToIntWithIndex(lng);
    }

private:
    uint8_t acc_to_speed(float ax, float ay, float az)
    {
        // Convert Acc from XYZ angle(3*4 bytes) to speed (1 Bytes)
        return sqrt((ax * ax) + (ay * ay) + (az * az));
    }

    int floatToIntWithIndex(double double_num)
    {
        // Convert double to int (64bit -> 32bit)
        // ex : (double) 123.456789 -> (int) 1234567893
        // 3 in the of int is the index of dot(.) in real double number

        // Convert the float to a string
        std::string float_str = std::to_string(double_num);
        size_t decimal_point_index = float_str.find('.');

        // Remove the decimal point and concatenate the resulting string
        std::string int_str = float_str.substr(0, decimal_point_index) + float_str.substr(decimal_point_index + 1) + std::to_string(decimal_point_index);

        // Convert the string to an integer
        int int_result = std::stoi(int_str);
        return int_result;
    }
};

// REFRENCES FOR accelerometer data acquisition

#include <iostream>
#include <vector>
#include <cmath>

const double ACCELEROMETER_SENSITIVITY = 16384.0; // Sensitivity for MPU-6050 accelerometer (for +/- 2g range)

// Function to calculate speed from acceleration data
double calculate_speed(const std::vector<double> &acceleration, double time_interval)
{
    // Calculate velocity by integrating acceleration
    std::vector<double> velocity;
    for (const auto &accel : acceleration)
    {
        velocity.push_back(accel * time_interval);
    }

    // Calculate speed by integrating velocity
    double speed = 0.0;
    for (const auto &v : velocity)
    {
        speed += v;
    }
    speed /= velocity.size();
    return speed;
}

int acc_to_speed_fun()
{
    // Simulated accelerometer data (replace this with your actual data)
    // Assume acceleration values in X, Y, Z directions over time
    std::vector<double> acceleration_x = {0.2, 0.3, 0.4, 0.5}; // Replace with your data
    std::vector<double> acceleration_y = {0.1, 0.2, 0.3, 0.4}; // Replace with your data
    std::vector<double> acceleration_z = {0.3, 0.2, 0.1, 0.5}; // Replace with your data

    // Time interval between measurements in seconds
    double time_interval = 0.1; // Adjust as needed

    // Convert acceleration values to actual acceleration in m/s^2
    for (auto &ax : acceleration_x)
    {
        ax *= 9.81 / ACCELEROMETER_SENSITIVITY;
    }
    for (auto &ay : acceleration_y)
    {
        ay *= 9.81 / ACCELEROMETER_SENSITIVITY;
    }
    for (auto &az : acceleration_z)
    {
        az *= 9.81 / ACCELEROMETER_SENSITIVITY;
    }

    // Calculate speed in each direction
    double speed_x = calculate_speed(acceleration_x, time_interval);
    double speed_y = calculate_speed(acceleration_y, time_interval);
    double speed_z = calculate_speed(acceleration_z, time_interval);

    // Calculate the overall speed (magnitude of the speed vector)
    double overall_speed = sqrt(speed_x * speed_x + speed_y * speed_y + speed_z * speed_z);

    // Print the calculated speeds
    std::cout << "Speed in X direction: " << speed_x << " m/s" << std::endl;
    std::cout << "Speed in Y direction: " << speed_y << " m/s" << std::endl;
    std::cout << "Speed in Z direction: " << speed_z << " m/s" << std::endl;
    std::cout << "Overall speed: " << overall_speed << " m/s" << std::endl;

    return 0;
}
