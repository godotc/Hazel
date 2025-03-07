
#include <stdint.h>
#include <stdio.h>


int main(int argc, char **argv)
{
    // int8 , int16, int32, int64, long long , signed long long , unsigned long long , float, double, char, bool
    printf("size of int8: %llu\n", sizeof(int8_t));
    printf("size of int16: %llu\n", sizeof(int16_t));
    printf("size of int32: %llu\n", sizeof(int32_t));
    printf("size of int64: %llu\n", sizeof(int64_t));
    printf("size of long long: %llu\n", sizeof(long long));
    printf("size of signed long long: %llu\n", sizeof(signed long long));
    printf("size of unsigned long long: %llu\n", sizeof(unsigned long long));
    printf("size of float: %llu\n", sizeof(float));
    printf("size of double: %llu\n", sizeof(double));
    printf("size of char: %llu\n", sizeof(char));
    printf("size of bool: %llu\n", sizeof(bool));

    // size_t
    printf("size of size_t: %llu\n", sizeof(size_t));

    // long double
    printf("size of long double: %llu\n", sizeof(long double));

    // long, signed long, unsigned long
    printf("size of long: %llu\n", sizeof(long));
    printf("size of signed long: %llu\n", sizeof(signed long));
    printf("size of unsigned long: %llu\n", sizeof(unsigned long));

    return 0;
}