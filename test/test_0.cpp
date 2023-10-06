#include <bitset>
#include <iostream>

using namespace std;



void printBitsFloat(unsigned int my_float_variable)
{
    // Convert the float variable to an unsigned int.
    unsigned int my_unsigned_int_variable = reinterpret_cast<unsigned int>(my_float_variable);

    // Print the bits of the unsigned int variable.
    for (int i = 31; i >= 0; i--)
    {
        cout << ((my_unsigned_int_variable >> i) & 1);
    }
    cout << endl;
}

/* Prints Contents of Memory Blocks */
int print_bytes(const void *object, size_t size){

    int x; 
    const unsigned char * const bytes = static_cast<const unsigned char *>(object);
    size_t i;

    printf("[");
    for(i = 0; i < size; i++)
    {
        //printf(bytes[i]);
        int binary[8];
        for(int n = 0; n < 8; n++){
            binary[7-n] = (bytes[size -1 - i] >> n) & 1;
            x |= ((bytes[size -1 - i] >> n) & 1) << 8 * (5-i)
        }
        /* print result */
        for(int n = 0; n < 8; n++){
            printf("%d", binary[n]);
        }
       
    }
    printf("]\n\n");

    return x;
}


main()
{
    int time = 86499;
    int speed = 53;
    float lat = 123.456f;
    bitset<32> debug;
    int data = 0;
    
    // print_bytes(&lat, sizeof(lat));

    debug = lat;
    std::cout << lat << " " << debug << endl;

    // std::cout << speed << " " << debug << endl;

    // bitset<88> data;

    data = time & 0x1ffff;
    std::cout << data << endl;
    data |= (speed<<17);
    std::cout << data << endl;

    return 0;
}