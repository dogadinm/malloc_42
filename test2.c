#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "./inc/malloc.h"

int main()
{
    printf("==== Test #1 ====\n");
    char *a = my_malloc(32);
    char *b = my_malloc(128);
    char *c = my_malloc(48847);

    strcpy(a, "Hello tiny!");
    strcpy(b, "This is small allocation.");
    strcpy(c, "Large allocation test.");

    show_alloc_mem();

    // printf("\n==== Test #2 ====\n");
    // my_free(b);

    // show_alloc_mem();

    // printf("\n==== Test #3 ====\n");
    // char *d = my_malloc(64);
    // show_alloc_mem();

    // printf("\n==== Test #4 ====\n");
    // char *e = my_realloc(a, 100);
    // show_alloc_mem();

    // my_free(a);
    // my_free(c);
    // my_free(d);
    // my_free(e);

    // printf("\n==== Final state ====\n");
    // show_alloc_mem();

    return 0;
}
