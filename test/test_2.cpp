//Test for convert float to int with specific value

#include <iostream>
#include <string>

int floatToIntWithIndex(double float_num) {
    // Convert double to int (64bit -> 32bit)
    // ex : (double) 123.456789 -> (int) 1234567893
    // 3 in the of int is the index of dot(.) in real double number
    // Convert the float to a string
    std::string float_str = std::to_string(float_num);

    std::cout<< float_str << std::endl;

    // Find the index of the decimal point
    size_t decimal_point_index = float_str.find('.');
    

    // Remove the decimal point and concatenate the resulting string
    std::string int_str = float_str.substr(0, decimal_point_index) + float_str.substr(decimal_point_index + 1) + std::to_string(decimal_point_index);

    // Convert the string to an integer
    int int_result = std::stoi(int_str);
    return int_result;
}

int main() {
    double float_number = -150.123456;
    auto result = floatToIntWithIndex(float_number);

    std::cout << "Integer representation: " << result << std::endl;

    return 0;
}
