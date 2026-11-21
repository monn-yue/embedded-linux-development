#include <stdio.h>

int global_data = 42;
int global_zero;

int main(void)
{
    printf("global_data=%d, global_zero=%d\n", global_data, global_zero);
    return 0;
}
