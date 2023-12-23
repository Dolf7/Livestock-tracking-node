#include <iostream>
unsigned int doubleToIntWithIndex(double double_num)
{

    std::cout << "________ENCODE_______"<< std::endl;
    if (double_num == 0)
        return 0;

    // Convert double to int (64bit -> 32bit)
    // ex : (double) 123.456789 -> (int) 1234567893
    // 3 in the of int is the index of dot(.) in real double number
    
    // Convert the float to a string
    std::string double_str = std::to_string(double_num);
    std::cout << "double_str = " << double_str << std::endl;
    int decimal_point_index = double_str.find('.');
    std::cout << "decima_point_index = " << decimal_point_index << std::endl;

    // Remove the decimal point and concatenate the resulting string
    std::string int_str = double_str.substr(0, decimal_point_index) + double_str.substr(decimal_point_index + 1) + std::to_string((double_str[0] != '-' ? decimal_point_index : decimal_point_index - 1));
    std::cout << "int_str = " << int_str << std::endl;

    // Convert the string to an integer
    int int_result = std::stoi(int_str);
    std::cout << "int_restult = " << int_result << std::endl;

    return int_result;
}

double intWithIndexToDouble(int int_num)
{
    std::cout << "_____________"<<std::endl;
    std::string int_str = std::to_string(int_num);
    // std::cout << "int_str : " << int_str  <<std::endl;
    

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

    std::cout << "_____________"<<std::endl;
    double real_data = std::stod(decimal_string_fix_value);
    return real_data;
}

int main() {
    double lat = -7.123456;
    unsigned int test_decoded_lat = 421541775;
    
    int lat_x = doubleToIntWithIndex(lat);
    std::cout << "Lat : " << lat_x << std::endl;
    double lat_back = intWithIndexToDouble(lat_x);
    std::cout << "Lat Dec: " << lat_back << std::endl;
    double lat_test = intWithIndexToDouble(test_decoded_lat);
    std::cout << "Lat Dec: " << lat_test  << std::endl;
}