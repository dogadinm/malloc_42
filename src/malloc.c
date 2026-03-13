#include "malloc.h"

void			*g_first_addr = NULL;
pthread_mutex_t	g_malloc_mutex;

__attribute__((constructor))
static void	init_mutex(void)
{
	pthread_mutexattr_t	attr;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&g_malloc_mutex, &attr);
	pthread_mutexattr_destroy(&attr);
	init_debug_env();
}

void	*malloc(size_t size)
{
	t_map	*first_map;
	t_chunk	*chunk;
	void	*result;

	pthread_mutex_lock(&g_malloc_mutex);
	first_map = init_first_map(size);
	if (first_map == NULL)
	{
		pthread_mutex_unlock(&g_malloc_mutex);
		return (NULL);
	}
	chunk = get_chunk(NULL, first_map, size);
	if (chunk == NULL)
	{
		pthread_mutex_unlock(&g_malloc_mutex);
		return (NULL);
	}
	set_chunk(chunk, size);
	result = chunk->ptr_data;
	debug_scribble_alloc(result, size);
	debug_malloc(result, size);
	pthread_mutex_unlock(&g_malloc_mutex);
	return (result);
}
