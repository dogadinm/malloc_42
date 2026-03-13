#include "../inc/malloc.h"
#include <string.h>
#include <unistd.h>

static void print(const char *s) { write(1, s, strlen(s)); }

int main(void)
{
    /* --- TINY --- */
    void *a = malloc(1);
    void *b = malloc(512);
    void *c = malloc(1024);
    free(a);
    free(b);
    free(c);

    /* --- SMALL --- */
    void *d = malloc(1025);
    void *e = malloc(1024 * 16);
    void *f = malloc(1024 * 32);
    free(d);
    free(e);
    free(f);

    /* --- LARGE --- */
    void *g = malloc(1024 * 1024);
    void *h = malloc(1024 * 1024 * 16);
    free(g);
    free(h);

    /* --- realloc --- */
    void *r = malloc(128);
    r = realloc(r, 512);
    r = realloc(r, 64);
    free(r);

    /* --- realloc(NULL) --- */
    void *n = realloc(NULL, 256);
    free(n);

    void *z = malloc(64);
    realloc(z, 0);

    print("=== show_alloc_mem ===\n");
    show_alloc_mem();
    return (0);
}
