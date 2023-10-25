#include <iostream>
#include <vector>
#include "../src/data.h"

using namespace std;

int main()
{
    int hour = 12, minute = 12, second = 12;
    std::vector<double> acceleration_x = {10, 15, 20, 10};  // Replace with your data
    std::vector<double> acceleration_y = {0.2, 0.5, 0.7, 0.37};  // Replace with your data
    std::vector<double> acceleration_z = {0.1, 0.4, 0.8, 0.23}; // Replace with your data
    int time_interval = 200;
    double lng = -7.123456, lat = 120.789012;

    data_store data(hour, minute, second, acceleration_x, acceleration_y, acceleration_z, time_interval, lat, lng);

    // Test Print
    cout << "Object Data Properties : " << endl;
    cout << "Time = " << (int)data.hour << " : " << (int)data.minute << " : " << (int)data.second << endl;
    cout << "Speed = " << (int)data.speed << endl;
    cout << "Location =  " << data.lat << " | " << data.lng << endl;

    cout <<"Size of object : " << sizeof(data) << endl;
    cout << sizeof(data.hour) << "-";
    cout << sizeof(data.minute) << "-";
    cout << sizeof(data.second) << "-";
    cout << sizeof(data.speed) << "-";
    cout << sizeof(data.lat) << "-";
    cout << sizeof(data.lng) << "-";

    return 0;
}