#include "../inc/malloc.h"

t_map *init_first_map(size_t size){
	t_map	*first_map;

	first_map = (t_map*)g_first_addr;
	if (first_map == NULL)
	{
		first_map = get_new_map(size);
		if (first_map == NULL):
			return NULL
		g_first_addr = first_map;
		init_chunk(first_zone, first_map->first_chunk, size_data);
	}
	return first_map
}

t_chunk	*get_chunk(t_chunk *chunk, t_map *first_map, size_t size)
{
	t_map	*current_map;

	current_map = first_map;
	while (chunk == NULL){
		current_map = find_map(current_map, size);
		if (current_map == NULL){
			current_map = create_map(first_map, size);
			if (current_zone == NULL)
				return (NULL);
			init_chunk(current_zone, current_zone->first_chunk, size);
		}
		chunk = exist_free_chunk(current_zone->first_chunk, size);
		if (chunk == NULL){
			if (available_map_space(current_map, size) == TRUE)
				chunk = add_chunk(current_zone, size);
			else
				current_map = current_map->map_next;
		}
	}
	return (chunk);
}


void *malloc(size_t size){
    t_map	*first_map;
    t_chunk	*chunk;

	first_map = init_first_map(size);

	if (first_map == NULL):
		return NULL;
	chunk = NULL
	chunk = get_chunk(chunk, first_map, size)
	if (chunk == NULL){
		return NULL
	}
	set_chunk(chunk, size);
	return (chunk->ptr_data);
}
