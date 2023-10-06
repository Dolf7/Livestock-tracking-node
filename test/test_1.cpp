#include <iostream>
#include <stdio.h>

using namespace std;

int main()
{
    float p = 0x3233FB41;
    float *q = &p;

    std::cout << "Float : " << p << endl;
    printf("%4f", *q);
    // int x = 10;
    // int *speed;

    // *speed = 320;

    // char *test = reinterpret_cast<char *>(speed);
    // char *nextLocation = reinterpret_cast<char *>(test + 1);

    // // printf("%d\n", &speed);
    // std::cout << *speed << " - " << speed << endl;
    // std::cout << *(speed + 1) << " - " << speed + 1 << endl;
    // std::cout << *test << " - " << &test << endl;
    // std::cout << (test + 1) << " - " << &test + 1 << endl;

    // std::cout << *nextLocation << " - " << &nextLocation << std::endl;

    return 0;
}