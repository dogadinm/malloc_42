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

/* check that the first n bytes all equal val */
static int	data_eq(const char *ptr, char val, size_t n)
{
	size_t i = 0;
	while (i < n)
	{
		if (ptr[i] != val)
			return (0);
		i++;
	}
	return (1);
}

/* --- tests --- */

/* realloc(NULL, size) == malloc(size) */
static void	test_null_ptr(void)
{
	void *p = realloc(NULL, 128);
	if (p != NULL)
		print_ok("realloc(NULL, 128) returns non-NULL");
	else
		print_fail("realloc(NULL, 128) returns non-NULL");
	free(p);
}

/* realloc(ptr, 0) == free */
static void	test_zero_size(void)
{
	void *p = malloc(64);
	realloc(p, 0);
	/* after this g_first_addr should be NULL or zone empty,
	   next malloc must work normally */
	void *q = malloc(8);
	if (q != NULL)
		print_ok("realloc(ptr, 0) frees memory, next malloc works");
	else
		print_fail("realloc(ptr, 0) frees memory, next malloc works");
	free(q);
}

/* grow within the same zone */
static void	test_grow_same_zone(void)
{
	char *p = malloc(64);
	size_t i = 0;
	while (i < 64) { p[i] = 'A'; i++; }

	char *p2 = realloc(p, 128);
	if (p2 == NULL)
	{
		print_fail("realloc grow: returned NULL");
		return ;
	}
	if (data_eq(p2, 'A', 64))
		print_ok("realloc grow: data preserved");
	else
		print_fail("realloc grow: data preserved");
	free(p2);
}

/* shrink — data must be preserved, remainder goes into a free chunk */
static void	test_shrink(void)
{
	char *p = malloc(512);
	size_t i = 0;
	while (i < 512) { p[i] = 'B'; i++; }

	char *p2 = realloc(p, 64);
	if (p2 == NULL)
	{
		print_fail("realloc shrink: returned NULL");
		return ;
	}
	if (data_eq(p2, 'B', 64))
		print_ok("realloc shrink: data preserved");
	else
		print_fail("realloc shrink: data preserved");

	/* remainder must be available for new allocations */
	void *q = malloc(64);
	if (q != NULL)
		print_ok("realloc shrink: freed remainder is reusable");
	else
		print_fail("realloc shrink: freed remainder is reusable");
	free(p2);
	free(q);
}

/* realloc chain: grow incrementally */
static void	test_chain_grow(void)
{
	char *p = malloc(128);
	size_t i = 0;
	while (i < 128) { p[i] = 'C'; i++; }

	p = realloc(p, 256);
	p = realloc(p, 512);
	p = realloc(p, 1024);

	if (p == NULL)
	{
		print_fail("realloc chain grow: not NULL");
		return ;
	}
	if (data_eq(p, 'C', 128))
		print_ok("realloc chain grow: initial data preserved");
	else
		print_fail("realloc chain grow: initial data preserved");
	free(p);
}

/* realloc chain: shrink gradually */
static void	test_chain_shrink(void)
{
	char *p = malloc(1024);
	size_t i = 0;
	while (i < 1024) { p[i] = 'D'; i++; }

	p = realloc(p, 512);
	p = realloc(p, 64);

	if (p == NULL)
	{
		print_fail("realloc chain shrink: not NULL");
		return ;
	}
	if (data_eq(p, 'D', 64))
		print_ok("realloc chain shrink: data preserved");
	else
		print_fail("realloc chain shrink: data preserved");
	free(p);
}

/* zone type change TINY -> LARGE */
static void	test_type_change(void)
{
	char *p = malloc(64);
	size_t i = 0;
	while (i < 64) { p[i] = 'E'; i++; }

	char *p2 = realloc(p, 1024 * 64);
	if (p2 == NULL)
	{
		print_fail("realloc type change TINY->LARGE: not NULL");
		return ;
	}
	if (data_eq(p2, 'E', 64))
		print_ok("realloc type change TINY->LARGE: data preserved");
	else
		print_fail("realloc type change TINY->LARGE: data preserved");
	free(p2);
}

/* same size — simply returns the same pointer */
static void	test_same_size(void)
{
	char *p = malloc(128);
	size_t i = 0;
	while (i < 128) { p[i] = 'F'; i++; }

	char *p2 = realloc(p, 128);
	if (p2 == NULL)
	{
		print_fail("realloc same size: not NULL");
		return ;
	}
	if (data_eq(p2, 'F', 128))
		print_ok("realloc same size: data preserved");
	else
		print_fail("realloc same size: data preserved");
	free(p2);
}

int	main(void)
{
	print("=== realloc tests ===\n");
	test_null_ptr();
	test_zero_size();
	test_grow_same_zone();
	test_shrink();
	test_chain_grow();
	test_chain_shrink();
	test_type_change();
	test_same_size();

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
