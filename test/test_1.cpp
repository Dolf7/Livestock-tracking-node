#include <iostream>
#include <stdio.h>

using namespace std;

int main()
{
    float p = 0x3233FB41;
    float *q = &p;

    std::cout << "Float : " << p << endl;
    printf("%4f", *q);

    
    return 0;
}