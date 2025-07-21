#include "malloc.h"


t_bool	zone_is_empty(t_map *map){
	t_chunk *chunk;

	chunk = map->first_chunk;
	while (chunk != NULL){
		if (chunk->available == FALSE)
			return (FALSE);
		chunk = chunk->next_chunk;
	}
	return (TRUE);
}

void release_zone(t_map *map){
    munmap(map->map_origin, map->total_size);
}

void	release_empty_zone(t_map *map_origin){
	t_map *map_prev;
	t_map *map_next;

	map_prev = NULL;
	map_next = NULL;
	if (map_origin != NULL){
		map_next = map_origin->map_next;
		map_prev = map_origin->map_prev;
		if (zone_is_empty(map_origin) == TRUE){
			if (map_prev != NULL){
				release_zone(map_origin);
				map_prev->map_next = map_next;
				if (map_next != NULL)
					map_next->map_prev = map_prev;
			}
		}
	}
}