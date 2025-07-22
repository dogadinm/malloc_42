#include "malloc.h"

t_bool	available_map_space(t_map *map, size_t size_data_chunk){
	t_chunk	*last_chunk;
	void	*max_addr;
	void	*next_addr;

	last_chunk = get_last_chunk(map);
	max_addr = map->map_origin + map->total_size;
	next_addr = (void*)last_chunk + sizeof(t_chunk)
							+ last_chunk->data_size + sizeof(t_chunk)
							+ size_data_chunk;
	if (next_addr > max_addr)
		return (FALSE);
	return (TRUE);
}

t_bool	check_type_map(t_map *map, size_t size){
	if (map && map->type){
		if (map->type == 'T' && size <= TINY_CHUNK)
			return (TRUE);
		else if (map->type == 'S'
						&& size > TINY_CHUNK
						&& size <= SMALL_CHUNK)
			return (TRUE);
		else if (map->type == 'L' && size > SMALL_CHUNK)
			return (TRUE);
		else
			return (FALSE);
	}
	return (FALSE);
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

size_t	number_of_pages(size_t page_size, size_t chunk_size){
	size_t	page_numbers;
	size_t	chunks_numbers;

	page_numbers = 0;
	chunks_numbers = 0;

	if (chunk_size > 0 && page_size > 0){
		while (chunks_numbers < MIN_CHUNKS_PER_MAP){
			page_numbers++;
			chunks_numbers = page_numbers * page_size / chunk_size;
		}
	}
	return (page_numbers);
}

void	*allocate_memory(size_t size){
	void *ptr_map;

	ptr_map = mmap(0, size, PROT_READ | PROT_WRITE,MAP_ANON | MAP_PRIVATE, -1, 0);
	if (ptr_map == MAP_FAILED)
		return (NULL);

	return (ptr_map);
}

size_t	calculate_map_size(size_t size){
    size_t	page_numbers;
	size_t	map_size;
	size_t	page_size;

    page_size = getpagesize();
	page_numbers = number_of_pages(page_size, sizeof(t_chunk) + size);

	map_size = page_numbers * page_size;
	return (map_size);
}