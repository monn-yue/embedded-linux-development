
#include <stdio.h>

int add(const int *a, const int *b)
{
    int result = *a + *b;
    return result;
}

int main(void)
{
    int x = 10;
    int y = 20;

    int *px = &x;
    int *py = &y;

    int sum = add(px, py);

    printf("x = %d\n", x);
    printf("y = %d\n", y);
    printf("*px = %d\n", *px);
    printf("*py = %d\n", *py);
    printf("sum = %d\n", sum);

    return 0;
}
