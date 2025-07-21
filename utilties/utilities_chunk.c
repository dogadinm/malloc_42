#include "malloc.h"

t_bool	available_chunk_space(t_chunk *chunk, size_t data_size){
	if (chunk != NULL && chunk->data_size >= data_size)
		return (TRUE);
	return (FALSE);
}

t_chunk	*available_chunk(t_chunk *chunk){
	t_chunk *current_chunk;

	current_chunk = chunk;
	while (current_chunk != NULL && current_chunk->available == FALSE)
		current_chunk = current_chunk->next_chunk;
	return (current_chunk);
}

t_chunk	*exist_free_chunk(t_chunk *chunk, size_t data_size){
	t_chunk *current_chunk;

	current_chunk = available_chunk(chunk);
	while (current_chunk != NULL &&
		available_chunk_space(current_chunk, data_size) == FALSE)
	{
		current_chunk = available_chunk(current_chunk->next_chunk);
	}
	return (current_chunk);
}

t_chunk	*get_last_chunk(t_map *map){
	t_chunk *current_chunk;

	current_chunk = map->first_chunk;
	while (current_chunk->next_chunk != NULL){
		current_chunk = current_chunk->next_chunk;
	}
	return (current_chunk);
}

t_bool	check_chunk_exist(t_chunk *addr_chunk)
{
	t_map	*current_map;
	t_chunk	*current_chunk;
	t_bool	is_exist;

	is_exist = FALSE;
	current_map = (t_map *)g_first_addr;
	while (current_map != NULL)
	{
		current_chunk = current_map->first_chunk;
		while (current_chunk != NULL)
		{
			if (current_chunk == addr_chunk)
			{
				is_exist = TRUE;
			}
			current_chunk = (t_chunk*)current_chunk->next_chunk;
		}
		current_map = (t_map*)current_map->map_next;
	}
	return (is_exist);
}