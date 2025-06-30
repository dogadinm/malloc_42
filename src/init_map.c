#include "malloc.h"

void	init_map(void *first_map, size_t map_size, char type)
{
	t_map	*map;

	map = (t_map*)first_map_addr;
	map->first_chunk = first_map + sizeof(t_map);
	map->map_origin = first_map;
	map->map_next = NULL;
	map->map_prev = NULL;
	map->total_size = map_size;
	map->type = type;
}


t_map	*find_map(t_map *first_map, size_t size){
	t_map *current_map;

	current_map = first_map;
	while (current_map != NULL){
		if (check_type_map(current_map, size) == TRUE)
			return (current_map);
		current_map = current_map->map_next;
	}
	return (current_map);
}

t_bool	check_type_map(t_map *map, size_t size){
	if (map && map->type){
		if (map->type == 'T' && size <= TINY_CHUNK)
			return(TRUE);
		else if (map->type == 'S'
						&& size_data > TINY_CHUNK
						&& size_data <= SMALL_CHUNK)
			return(TRUE);
		else if (map->type == 'L' && size_data > SMALL_CHUNK)
			return(TRUE);
		else
			return(FALSE);
	}
	return (FALSE);
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
	current_map->map_next = get_new_map(size_data);
	if (current_map->next_map != NULL){
		next_map = current_map->map_next;
		next_map->map_prev = current_map;
	}
	return (current_map->map_next);
}

t_bool	available_map_space(t_map *map, size_t size_data_chunk)
{
	t_chunk	*last_chunk;
	void	*max_addr;
	void	*next_addr;

	last_chunk = last_chunk(map);
	max_addr = map->map_origin + map->total_size;
	next_addr = (void*)last_chunk + sizeof(t_chunk)
							+ last_chunk->data_size + sizeof(t_chunk)
							+ size_data_block;
	if (next_addr > max_addr){
		return (FALSE);
	}
	return (TRUE);
}