#include <stdio.h>
#include <string.h>
#include "./inc/malloc.h"

int main(void)
{
    char *s = my_malloc(20);
    if (s == NULL)
    {
        printf("malloc failed\n");
        return (1);
    }

    strcpy(s, "Hello, malloc!");
    printf("s = %s\n", s);

    return (0);
}