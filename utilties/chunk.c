#include "malloc.h"

t_chunk *init_chunk(t_map *map, void *chunk_ptr, size_t data_size){
    t_chunk *chunk = (t_chunk*)chunk_ptr;

    chunk->parent_map = map;
    chunk->next_chunk = NULL;
    chunk->available  = TRUE;
    chunk->data_size  = data_size;
    chunk->ptr_data = ALIGN_UP_PTR((char*)chunk_ptr + sizeof(t_chunk), MAL_ALIGN);
    return chunk;
}

void	set_chunk(t_chunk *chunk, size_t data_size){
	chunk->data_size = data_size;
	chunk->available = FALSE;
}

void	release_chunk(t_chunk *chunk){
	chunk->available = TRUE;
}

void	coalesce_chunk(t_map *map, t_chunk *chunk)
{
	t_chunk	*next;
	t_chunk	*current;

	next = (t_chunk*)chunk->next_chunk;
	while (next != NULL && next->available == TRUE)
	{
		chunk->data_size += ALIGN_UP_SIZE(sizeof(t_chunk), MAL_ALIGN)
			+ next->data_size;
		chunk->next_chunk = next->next_chunk;
		next = (t_chunk*)chunk->next_chunk;
	}
	current = map->first_chunk;
	while (current != NULL && (t_chunk*)current->next_chunk != chunk)
		current = (t_chunk*)current->next_chunk;
	if (current != NULL && current->available == TRUE)
	{
		current->data_size += ALIGN_UP_SIZE(sizeof(t_chunk), MAL_ALIGN)
			+ chunk->data_size;
		current->next_chunk = chunk->next_chunk;
	}
}

t_chunk *add_chunk(t_map *map, size_t size_data){
    t_chunk *last_chunk = get_last_chunk(map);

    void *end_of_last_data = (char*)last_chunk->ptr_data + last_chunk->data_size;

    void *chunk_addr = ALIGN_UP_PTR(end_of_last_data, MAL_ALIGN);

    last_chunk->next_chunk = init_chunk(map, chunk_addr, size_data);
    return (t_chunk*)chunk_addr;
}


t_chunk	*get_chunk(t_chunk *chunk, t_map *first_map, size_t size){
	t_map	*current_map;

	current_map = first_map;
	while (chunk == NULL){
		current_map = find_map(current_map, size);
		if (current_map == NULL){
			current_map = new_map(first_map, size);
			if (current_map == NULL)
				return (NULL);
			init_chunk(current_map, current_map->first_chunk, size);
		}
		chunk = exist_free_chunk(current_map->first_chunk, size);
		if (chunk == NULL)
			chunk = add_chunk(current_map, size);
	}
	return (chunk);
}

