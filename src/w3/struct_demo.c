#include <stddef.h>
#include <stdio.h>

struct GpioReading {
    char direction;
    int level;
};

int main(void)
{
    struct GpioReading reading = {
        .direction = 'I',
        .level = 1
    };

    printf("direction=%c, level=%d\n", reading.direction, reading.level);
    printf("struct size=%zu bytes\n", sizeof reading);
    printf("level offset=%zu bytes\n", offsetof(struct GpioReading, level));

    return 0;
}
