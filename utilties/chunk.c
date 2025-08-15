#include "malloc.h"

// static void *align_ptr(void *ptr, size_t alignment)
// {
//     uintptr_t p = (uintptr_t)ptr;
//     uintptr_t aligned = (p + alignment - 1) & ~(alignment - 1);
//     return (void *)aligned;
// }

// t_chunk *init_chunk(t_map *map, void *chunk_ptr, size_t data_size) {
//     t_chunk *chunk;

//     chunk = (t_chunk*)chunk_ptr;
//     chunk->parent_map = map;
//     chunk->next_chunk = NULL;
//     chunk->available = TRUE;
//     chunk->data_size = data_size;
//     chunk->ptr_data = align_ptr(chunk_ptr + sizeof(t_chunk), 2 * sizeof(size_t));
//     return (chunk);
// }

t_chunk *init_chunk(t_map *map, void *chunk_ptr, size_t data_size){
    t_chunk *chunk = (t_chunk*)chunk_ptr;

    chunk->parent_map = map;
    chunk->next_chunk = NULL;
    chunk->available  = TRUE;
    chunk->data_size  = data_size;
    chunk->ptr_data = ALIGN_UP_PTR((char*)chunk_ptr + sizeof(t_chunk), MAL_ALIGN);
    return chunk;
}

// t_chunk	*init_chunk(t_map *map, void *chunk_ptr, size_t data_size){
// 	t_chunk	*chunk;

// 	chunk = (t_chunk*)chunk_ptr;
// 	chunk->parent_map = map;
// 	chunk->next_chunk = NULL;
// 	chunk->available = TRUE;
// 	chunk->data_size = data_size;
// 	chunk->ptr_data = chunk_ptr + sizeof(t_chunk);
// 	return (chunk);
// }

void	set_chunk(t_chunk *chunk, size_t data_size){
	chunk->data_size = data_size;
	chunk->available = FALSE;
}

void	release_chunk(t_chunk *chunk){
	chunk->available = TRUE;
}

t_chunk *add_chunk(t_map *map, size_t size_data){
    t_chunk *last_chunk = get_last_chunk(map);

    void *end_of_last_data = (char*)last_chunk->ptr_data + last_chunk->data_size;

    void *chunk_addr = ALIGN_UP_PTR(end_of_last_data, MAL_ALIGN);

    last_chunk->next_chunk = init_chunk(map, chunk_addr, size_data);
    return (t_chunk*)chunk_addr;
}


// t_chunk	*add_chunk(t_map *map, size_t size_data){
// 	t_chunk *chunk_addr;
// 	t_chunk *last_chunk;

// 	last_chunk = get_last_chunk(map);
// 	chunk_addr = (void*)last_chunk + sizeof(t_chunk) + last_chunk->data_size;
// 	last_chunk->next_chunk = init_chunk(map, chunk_addr, size_data);
// 	return (chunk_addr);
// }

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
		if (chunk == NULL){
			if (available_map_space(current_map, size) == TRUE)
				chunk = add_chunk(current_map, size);
			else
				current_map = current_map->map_next;
		}
	}
	return (chunk);
}

