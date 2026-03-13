#include "../inc/malloc.h"
#include <string.h>
#include <unistd.h>

static void	print(const char *s) { write(1, s, strlen(s)); }

int	main(void)
{
	char	*hello;
	char	*nums;
	char	*binary;

	hello = malloc(13);
	memcpy(hello, "Hello, World!", 13);

	nums = malloc(32);
	int i = 0;
	while (i < 32) { ((unsigned char*)nums)[i] = (unsigned char)i; i++; }

	binary = malloc(20);
	memset(binary, 0xFF, 10);
	memset(binary + 10, 0x00, 10);

	print("=== show_alloc_mem (standard) ===\n");
	show_alloc_mem();

	print("\n=== show_alloc_mem_ex (with hex dump) ===\n");
	show_alloc_mem_ex();

	free(hello);
	free(nums);
	free(binary);

	print("\n=== after free: show_alloc_mem_ex (должно быть пусто) ===\n");
	show_alloc_mem_ex();

	return (0);
}
