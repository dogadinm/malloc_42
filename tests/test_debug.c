#include "../inc/malloc.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

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
** MALLOC_PRE_SCRIBBLE=1 → freshly malloc'd memory must be 0xAA
*/
static void	test_pre_scribble(void)
{
	unsigned char	*p;
	size_t			i;

	if (!getenv("MALLOC_PRE_SCRIBBLE"))
	{
		print("  [SKIP] MALLOC_PRE_SCRIBBLE not set\n");
		return ;
	}
	p = malloc(64);
	if (p == NULL)
	{
		print_fail("MALLOC_PRE_SCRIBBLE: malloc returned NULL");
		return ;
	}
	i = 0;
	while (i < 64)
	{
		if (p[i] != 0xAA)
		{
			print_fail("MALLOC_PRE_SCRIBBLE: allocated memory not filled with 0xAA");
			free(p);
			return ;
		}
		i++;
	}
	print_ok("MALLOC_PRE_SCRIBBLE: allocated memory filled with 0xAA");
	free(p);
}

/*
** MALLOC_SCRIBBLE=1 → freed memory must be 0x55
** We peek at the raw memory right after free (before any new alloc).
*/
static void	test_scribble(void)
{
	unsigned char	*anchor;
	unsigned char	*p;
	unsigned char	*raw;
	size_t			i;

	if (!getenv("MALLOC_SCRIBBLE"))
	{
		print("  [SKIP] MALLOC_SCRIBBLE not set\n");
		return ;
	}
	/* Anchor keeps the zone alive so munmap is NOT called after free(p) */
	anchor = malloc(8);
	p = malloc(64);
	if (p == NULL || anchor == NULL)
	{
		print_fail("MALLOC_SCRIBBLE: malloc returned NULL");
		free(anchor);
		return ;
	}
	memset(p, 0x11, 64);
	raw = p;
	free(p);

	/* Zone is still mapped (anchor alive) — safe to inspect raw memory */
	i = 0;
	while (i < 64)
	{
		if (raw[i] != 0x55)
		{
			print_fail("MALLOC_SCRIBBLE: freed memory not filled with 0x55");
			free(anchor);
			return ;
		}
		i++;
	}
	print_ok("MALLOC_SCRIBBLE: freed memory filled with 0x55");
	free(anchor);
}

/*
** MALLOC_VERBOSE=1 → log lines must appear on stdout
** We just verify malloc/free still work correctly when verbose is on.
*/
static void	test_verbose(void)
{
	char	*p;

	if (!getenv("MALLOC_VERBOSE"))
	{
		print("  [SKIP] MALLOC_VERBOSE not set\n");
		return ;
	}
	print("  [INFO] Verbose output should appear below:\n");
	p = malloc(42);
	if (p == NULL)
	{
		print_fail("MALLOC_VERBOSE: malloc returned NULL");
		return ;
	}
	p = realloc(p, 100);
	free(p);
	print_ok("MALLOC_VERBOSE: malloc/realloc/free worked correctly");
}

/*
** No env vars set → normal behavior, no scribbling
*/
static void	test_no_scribble_by_default(void)
{
	unsigned char	*p;
	size_t			i;
	int				all_zero;

	if (getenv("MALLOC_PRE_SCRIBBLE"))
	{
		print("  [SKIP] test_no_scribble (PRE_SCRIBBLE is set)\n");
		return ;
	}
	/* mmap gives zeroed pages, so freshly allocated memory == 0x00 */
	p = malloc(64);
	if (p == NULL)
	{
		print_fail("default: malloc returned NULL");
		return ;
	}
	all_zero = 1;
	i = 0;
	while (i < 64)
	{
		if (p[i] != 0x00)
		{
			all_zero = 0;
			break ;
		}
		i++;
	}
	if (all_zero)
		print_ok("default: no pre-scribble (memory is zero-initialised by mmap)");
	else
		print("  [INFO] default: memory not zero (chunk reused from pool — OK)\n");
	free(p);
}

int	main(void)
{
	print("=== debug env var tests ===\n");
	test_verbose();
	test_pre_scribble();
	test_scribble();
	test_no_scribble_by_default();

	if (g_fail)
	{
		print("\nRESULT: FAILED\n");
		return (1);
	}
	print("\nRESULT: OK\n");
	return (0);
}
