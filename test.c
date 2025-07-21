// #include <stdio.h>
// #include <string.h>
// #include "./inc/malloc.h"


// int main(void)
// {
//     char *s = malloc(10);
//     if (s == NULL)
//     {
//         printf("malloc failed\n");
//         return (1);
//     }

//     strcpy(s, "Hello");
//     printf("Before realloc: %s\n", s);

//     char *new_s = realloc(s, 20);
//     if (new_s == NULL)
//     {
//         printf("realloc failed\n");
//         return (1);
//     }

//     printf("After realloc: %s\n", new_s);

//     strcpy(new_s + 5, " World");
//     printf("After write: %s\n", new_s);

//     return 0;
// }

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./inc/malloc.h"
// подключи заголовок с твоими функциями
// #include "malloc.h"

// // заглушки на случай, если тестируешь system malloc
// #define malloc malloc
// #define free free
// #define realloc realloc

void test_malloc_free()
{
    printf("=== test_malloc_free ===\n");

    size_t size = 1026;
    char *ptr = (char *)malloc(size);

    if (ptr == NULL)
    {
        printf("FAIL: malloc returned NULL\n");
        return;
    }

    memset(ptr, 0x42, size);
    printf("Allocated %zu bytes and filled with 'B'.\n", size);

    for (size_t i = 0; i < size; i++)
    {
        if (ptr[i] != 0x42)
        {
            printf("FAIL: memory corruption at offset %zu\n", i);
            return;
        }
    }

    free(ptr);
    printf("PASS: malloc/free test\n\n");
}

void test_realloc_expand()
{
    printf("=== test_realloc_expand ===\n");

    size_t size1 = 50;
    size_t size2 = 150;

    char *ptr = (char *)malloc(size1);
    if (!ptr)
    {
        printf("FAIL: malloc returned NULL\n");
        return;
    }

    memset(ptr, 0x33, size1);

    char *ptr2 = (char *)realloc(ptr, size2);
    if (!ptr2)
    {
        printf("FAIL: realloc returned NULL\n");
        free(ptr);
        return;
    }

    // check old data is still there
    int ok = 1;
    for (size_t i = 0; i < size1; i++)
    {
        if (ptr2[i] != 0x33)
        {
            printf("FAIL: realloc did not preserve data at %zu\n", i);
            ok = 0;
            break;
        }
    }

    if (ok)
        printf("PASS: realloc expanded and preserved data\n");
    free(ptr2);
    printf("\n");
}

void test_realloc_shrink()
{
    printf("=== test_realloc_shrink ===\n");

    size_t size1 = 200;
    size_t size2 = 50;

    char *ptr = (char *)malloc(size1);
    if (!ptr)
    {
        printf("FAIL: malloc returned NULL\n");
        return;
    }

    memset(ptr, 0x77, size1);

    char *ptr2 = (char *)realloc(ptr, size2);
    if (!ptr2)
    {
        printf("FAIL: realloc returned NULL\n");
        free(ptr);
        return;
    }

    int ok = 1;
    for (size_t i = 0; i < size2; i++)
    {
        if (ptr2[i] != 0x77)
        {
            printf("FAIL: realloc shrink corrupted data at %zu\n", i);
            ok = 0;
            break;
        }
    }

    if (ok)
        printf("PASS: realloc shrink test\n");

    free(ptr2);
    printf("\n");
}

void test_malloc_zero()
{
    printf("=== test_malloc_zero ===\n");

    char *ptr = (char *)malloc(0);
    if (ptr == NULL)
    {
        printf("PASS: malloc(0) returned NULL (allowed)\n");
    }
    else
    {
        printf("PASS: malloc(0) returned non-NULL pointer (also allowed)\n");
        free(ptr);
    }
    printf("\n");
}

int main()
{
    test_malloc_free();
    test_realloc_expand();
    test_realloc_shrink();
    test_malloc_zero();

    printf("All tests completed.\n");
    return 0;
}