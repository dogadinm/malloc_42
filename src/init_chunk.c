#include "malloc.h"

t_chunk	*init_chunk(t_map *map, void *chunk_ptr, size_t data_size)
{
	t_chunk	*chunk;

	chunk = (t_chunk*)chunk_ptr;
	chunk->parent_map = map;
	chunk->next_chunk = NULL;
	chunk->available = TRUE;
	chunk->data_size = data_size;
	chunk->ptr_data = chunk_ptr + sizeof(t_chunk);
	return (chunk);
}

void	set_chunk(t_chunk *chunk, size_t data_size)
{
	block->data_size = data_size;
	block->available = FALSE;
}