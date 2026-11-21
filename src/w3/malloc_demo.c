#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int local_value = 42;
    int *heap_value = malloc(sizeof *heap_value);

    if (heap_value == NULL) {
        puts("malloc failed");
        return 1;
    }

    *heap_value = 99;
    printf("local value=%d, address=%p\n", local_value, (void *)&local_value);
    printf("allocated value=%d, address=%p\n", *heap_value, (void *)heap_value);

    free(heap_value);
    return 0;
}
