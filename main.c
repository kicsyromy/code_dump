#include <stdio.h>
#include <stdint.h>

extern int test(int);

int main()
{
    return test(5);
}