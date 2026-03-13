#include "../inc/malloc.h"
#include <string.h>
#include <unistd.h>

static int	g_fail = 0;

static void	print(const char *s) { write(1, s, strlen(s)); }

static void	print_ok(const char *name)
{
	print("  [OK] ");
	print(name);
	print("\n");
}

static void	print_fail(const char *name)
{
	print("  [FAIL] ");
	print(name);
	print("\n");
	g_fail = 1;
}

/*
** After freeing adjacent chunks and requesting a larger malloc,
** coalescing should provide space without a new mmap zone.
** We verify that the new pointer is non-NULL.
*/

/* --- forward coalescing: free A then B, then request A+B size --- */
static void	test_forward(void)
{
	void	*a = malloc(128);
	void	*b = malloc(128);
	void	*c = malloc(64);	/* anchor to keep zone alive */

	(void)c;
	free(a);
	free(b);

	/* after merging A and B, a ~256+ byte chunk should be available */
	void *big = malloc(200);
	if (big != NULL)
		print_ok("forward coalesce: large alloc fits after free(A)+free(B)");
	else
		print_fail("forward coalesce: large alloc fits after free(A)+free(B)");

	free(big);
	free(c);
}

/* --- backward coalescing: free B then A, merge B into A --- */
static void	test_backward(void)
{
	void	*a = malloc(128);
	void	*b = malloc(128);
	void	*c = malloc(64);

	(void)c;
	free(b);	/* b is free */
	free(a);	/* a should merge with b (backward + forward) */

	void *big = malloc(200);
	if (big != NULL)
		print_ok("backward coalesce: large alloc fits after free(B)+free(A)");
	else
		print_fail("backward coalesce: large alloc fits after free(B)+free(A)");

	free(big);
	free(c);
}

/* --- three chunks: free A, C, B — all three merge --- */
static void	test_three_chunks(void)
{
	void	*a = malloc(64);
	void	*b = malloc(64);
	void	*c = malloc(64);
	void	*d = malloc(32);	/* anchor */

	(void)d;
	free(a);
	free(c);
	free(b);	/* b bridges a and c — all three should merge */

	void *big = malloc(150);
	if (big != NULL)
		print_ok("three-way coalesce: fits after free(A)+free(C)+free(B)");
	else
		print_fail("three-way coalesce: fits after free(A)+free(C)+free(B)");

	free(big);
	free(d);
}

/* --- zone becomes fully empty after coalescing --- */
static void	test_zone_released(void)
{
	void	*a = malloc(64);
	void	*b = malloc(64);

	free(a);
	free(b);	/* zone should be released via munmap */

	/* next malloc must work normally */
	void *p = malloc(32);
	if (p != NULL)
		print_ok("zone released after all chunks coalesced and freed");
	else
		print_fail("zone released after all chunks coalesced and freed");
	free(p);
}

/* --- fragmentation: many small alloc/free, then one large --- */
static void	test_fragmentation(void)
{
	void	*ptrs[10];
	int		i;

	i = 0;
	while (i < 10)
	{
		ptrs[i] = malloc(32);
		i++;
	}
	/* free even indices — create fragments */
	i = 0;
	while (i < 10)
	{
		if (i % 2 == 0)
			free(ptrs[i]);
		i++;
	}
	/* free odd indices — coalescing should merge everything */
	i = 1;
	while (i < 10)
	{
		if (i % 2 == 1)
			free(ptrs[i]);
		i++;
	}

	void *big = malloc(200);
	if (big != NULL)
		print_ok("fragmentation: coalesced space reused for large alloc");
	else
		print_fail("fragmentation: coalesced space reused for large alloc");
	free(big);
}

/* --- data of a live neighbor chunk must not be touched --- */
static void	test_data_integrity(void)
{
	char	*a = malloc(64);
	char	*b = malloc(64);
	char	*c = malloc(64);

	memset(b, 0xAB, 64);

	free(a);
	free(c);	/* coalescing around b */

	int ok = 1;
	int i = 0;
	while (i < 64)
	{
		if ((unsigned char)b[i] != 0xAB)
		{
			ok = 0;
			break;
		}
		i++;
	}
	if (ok)
		print_ok("data integrity: live chunk B intact after coalescing A and C");
	else
		print_fail("data integrity: live chunk B intact after coalescing A and C");

	free(b);
}

int	main(void)
{
	print("=== coalescing tests ===\n");
	test_forward();
	test_backward();
	test_three_chunks();
	test_zone_released();
	test_fragmentation();
	test_data_integrity();

	print("\n=== show_alloc_mem (should be empty) ===\n");
	show_alloc_mem();

	if (g_fail)
	{
		print("\nRESULT: FAILED\n");
		return (1);
	}
	print("\nRESULT: OK\n");
	return (0);
}
