#include "malloc.h"

void	init_map(void *first_map, size_t size, char type)
{
	t_map	*map;

	map = (t_map*)first_map;
	map->first_chunk = first_map + sizeof(t_map);
	map->map_origin = first_map;
	map->map_next = NULL;
	map->map_prev = NULL;
	map->total_size = size;
	map->type = type;
}

t_map *create_type_map(size_t size, char type){
    size_t	map_size;
    void	*ptr_map;
    if (type == LARGE)
        map_size = size * sizeof(t_chunk);
    else
        map_size = calculate_map_size(size);

    ptr_map = allocate_memory(map_size);
	
    if (ptr_map != NULL)
		init_map(ptr_map, map_size, type);
	return (ptr_map);
}

t_map *create_map(size_t size){
    t_map *ptr_map;

    if (size <= TINY_CHUNK)
        ptr_map = create_type_map(TINY_CHUNK, TINY);
    else if (size <= SMALL_CHUNK)
        ptr_map = create_type_map(SMALL_CHUNK, SMALL);
    else
        ptr_map = create_type_map(size, LARGE);

    return (ptr_map);
}

t_map	*new_map(t_map *map, size_t size_data){
	t_map *current_map;
	t_map *next_map;

	if (map == NULL)
		return (NULL);
	current_map = map;
	while (current_map->map_next != NULL){
		current_map = current_map->map_next;
	}
	current_map->map_next = create_map(size_data);
	if (current_map->map_next != NULL){
		next_map = current_map->map_next;
		next_map->map_prev = current_map;
	}
	return (current_map->map_next);
}

t_map *init_first_map(size_t size){
	t_map	*first_map;

	first_map = (t_map*)g_first_addr;
	if (first_map == NULL){
		first_map = create_map(size);
		if (first_map == NULL)
			return (NULL);
		g_first_addr = first_map;
		init_chunk(first_map, first_map->first_chunk, size);
	}
	return (first_map);
}
