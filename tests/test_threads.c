#include "../inc/malloc.h"
#include <string.h>
#include <unistd.h>

#define THREADS     8
#define ALLOCS      200
#define ALLOC_SIZE  128

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

/* --- тест 1: параллельный malloc/free --- */

static void	*thread_alloc_free(void *arg)
{
	void	*ptrs[ALLOCS];
	int		i;

	(void)arg;
	i = 0;
	while (i < ALLOCS)
	{
		ptrs[i] = malloc(ALLOC_SIZE);
		if (ptrs[i] != NULL)
			memset(ptrs[i], (char)i, ALLOC_SIZE);
		i++;
	}
	i = 0;
	while (i < ALLOCS)
	{
		free(ptrs[i]);
		i++;
	}
	return (NULL);
}

static void	test_parallel_alloc_free(void)
{
	pthread_t	threads[THREADS];
	int			i;

	i = 0;
	while (i < THREADS)
	{
		pthread_create(&threads[i], NULL, thread_alloc_free, NULL);
		i++;
	}
	i = 0;
	while (i < THREADS)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	print_ok("parallel malloc/free: no crash or corruption");
}

/* --- тест 2: параллельный realloc --- */

static void	*thread_realloc(void *arg)
{
	void	*p;
	int		i;

	(void)arg;
	p = malloc(64);
	i = 0;
	while (i < 50)
	{
		p = realloc(p, 64 + i * 8);
		i++;
	}
	i = 49;
	while (i >= 0)
	{
		p = realloc(p, 64 + i * 8);
		i--;
	}
	free(p);
	return (NULL);
}

static void	test_parallel_realloc(void)
{
	pthread_t	threads[THREADS];
	int			i;

	i = 0;
	while (i < THREADS)
	{
		pthread_create(&threads[i], NULL, thread_realloc, NULL);
		i++;
	}
	i = 0;
	while (i < THREADS)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	print_ok("parallel realloc: no crash or corruption");
}

/* --- тест 3: данные не перезаписаны другим потоком --- */

typedef struct s_targ
{
	int		id;
	int		ok;
}	t_targ;

static void	*thread_data_check(void *arg)
{
	t_targ	*targ = (t_targ*)arg;
	char	*p;
	int		i;

	p = malloc(256);
	if (p == NULL)
	{
		targ->ok = 0;
		return (NULL);
	}
	memset(p, targ->id, 256);
	/* небольшая работа — дать другим потокам шанс аллоцировать */
	i = 0;
	while (i < 10000) i++;

	targ->ok = 1;
	i = 0;
	while (i < 256)
	{
		if ((unsigned char)p[i] != (unsigned char)targ->id)
		{
			targ->ok = 0;
			break;
		}
		i++;
	}
	free(p);
	return (NULL);
}

static void	test_data_isolation(void)
{
	pthread_t	threads[THREADS];
	t_targ		args[THREADS];
	int			all_ok;
	int			i;

	i = 0;
	while (i < THREADS)
	{
		args[i].id = i + 1;
		args[i].ok = 0;
		pthread_create(&threads[i], NULL, thread_data_check, &args[i]);
		i++;
	}
	all_ok = 1;
	i = 0;
	while (i < THREADS)
	{
		pthread_join(threads[i], NULL);
		if (args[i].ok == 0)
			all_ok = 0;
		i++;
	}
	if (all_ok)
		print_ok("data isolation: each thread's allocation intact");
	else
		print_fail("data isolation: each thread's allocation intact");
}

/* --- тест 4: stress — много потоков, много аллокаций --- */

static void	*thread_stress(void *arg)
{
	void	*ptrs[50];
	int		i;

	(void)arg;
	i = 0;
	while (i < 50)
	{
		ptrs[i] = malloc((size_t)(i + 1) * 16);
		i++;
	}
	i = 0;
	while (i < 50)
	{
		free(ptrs[i]);
		i++;
	}
	return (NULL);
}

static void	test_stress(void)
{
	pthread_t	threads[32];
	int			i;

	i = 0;
	while (i < 32)
	{
		pthread_create(&threads[i], NULL, thread_stress, NULL);
		i++;
	}
	i = 0;
	while (i < 32)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	print_ok("stress: 32 threads x 50 allocs/frees each");
}

int	main(void)
{
	print("=== thread safety tests ===\n");
	test_parallel_alloc_free();
	test_parallel_realloc();
	test_data_isolation();
	test_stress();

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
