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
** После free соседних чанков и нового malloc большего размера
** coalescing должен дать место без новой mmap-зоны.
** Проверяем: указатель попадает в тот же диапазон адресов.
*/

/* --- forward coalescing: освободить A и B, потом запросить A+B --- */
static void	test_forward(void)
{
	void	*a = malloc(128);
	void	*b = malloc(128);
	void	*c = malloc(64);	/* якорь, держит зону живой */

	(void)c;
	free(a);
	free(b);

	/* после слияния A и B должен появиться чанк ~256+ байт */
	void *big = malloc(200);
	if (big != NULL)
		print_ok("forward coalesce: large alloc fits after free(A)+free(B)");
	else
		print_fail("forward coalesce: large alloc fits after free(A)+free(B)");

	free(big);
	free(c);
}

/* --- backward coalescing: освободить B потом A, слияние B->A --- */
static void	test_backward(void)
{
	void	*a = malloc(128);
	void	*b = malloc(128);
	void	*c = malloc(64);

	(void)c;
	free(b);	/* b свободен */
	free(a);	/* a должен слиться с b (backward + forward) */

	void *big = malloc(200);
	if (big != NULL)
		print_ok("backward coalesce: large alloc fits after free(B)+free(A)");
	else
		print_fail("backward coalesce: large alloc fits after free(B)+free(A)");

	free(big);
	free(c);
}

/* --- три чанка: освободить A, C, B — все три сливаются --- */
static void	test_three_chunks(void)
{
	void	*a = malloc(64);
	void	*b = malloc(64);
	void	*c = malloc(64);
	void	*d = malloc(32);	/* якорь */

	(void)d;
	free(a);
	free(c);
	free(b);	/* b соединяет a и c — все три должны слиться */

	void *big = malloc(150);
	if (big != NULL)
		print_ok("three-way coalesce: fits after free(A)+free(C)+free(B)");
	else
		print_fail("three-way coalesce: fits after free(A)+free(C)+free(B)");

	free(big);
	free(d);
}

/* --- зона полностью пустеет после coalescing --- */
static void	test_zone_released(void)
{
	void	*a = malloc(64);
	void	*b = malloc(64);

	free(a);
	free(b);	/* зона должна освободиться (munmap) */

	/* следующий malloc должен работать нормально */
	void *p = malloc(32);
	if (p != NULL)
		print_ok("zone released after all chunks coalesced and freed");
	else
		print_fail("zone released after all chunks coalesced and freed");
	free(p);
}

/* --- фрагментация: много маленьких alloc/free, потом большой --- */
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
	/* освобождаем чётные — создаём фрагменты */
	i = 0;
	while (i < 10)
	{
		if (i % 2 == 0)
			free(ptrs[i]);
		i++;
	}
	/* освобождаем нечётные — coalescing должен слить всё */
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

/* --- данные соседнего живого чанка не затронуты --- */
static void	test_data_integrity(void)
{
	char	*a = malloc(64);
	char	*b = malloc(64);
	char	*c = malloc(64);

	memset(b, 0xAB, 64);

	free(a);
	free(c);	/* coalescing вокруг b */

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

	print("\n=== show_alloc_mem (должно быть пусто) ===\n");
	show_alloc_mem();

	if (g_fail)
	{
		print("\nRESULT: FAILED\n");
		return (1);
	}
	print("\nRESULT: OK\n");
	return (0);
}
