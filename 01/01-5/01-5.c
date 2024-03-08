#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define Size(type) printf("Size of "#type": %d\n", (int)sizeof(type))

int main() {
    Size(char);
    Size(short);
    Size(short int);
    Size(int);
    Size(long int);
    Size(unsigned int);
    Size(void *);
    Size(size_t);
    Size(float);
    Size(double);
    Size(int8_t);
    Size(int16_t);
    Size(int32_t);
    Size(int64_t);
    Size(time_t);
    Size(clock_t);
    Size(struct tm);
    Size(NULL);
    return 114514;
}
