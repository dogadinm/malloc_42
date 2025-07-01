#include "malloc.h"

void *malloc(size_t size){
    t_map	*first_map;
    t_chunk	*chunk;

	first_map = init_first_map(size);
	if (first_map == NULL):
		return (NULL);

	chunk = NULL
	chunk = get_chunk(chunk, first_map, size)
	if (chunk == NULL)
		return (NULL)

	set_chunk(chunk, size);
	return (chunk->ptr_data);
}
