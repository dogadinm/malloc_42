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

t_chunk	*last_chunk(t_map *map){
	t_chunk *current_chunk;

	current_chunk = map->first_chunk;
	while (current_chunk->next_chunk != NULL){
		current_chunk = current_chunk->next_chunk;
	}
	return (current_chunk);
}
