#include "malloc.h"

t_map *create_map(size_t size){
    t_map *ptr_map;

    if (size <= TINY_CHUNK):
        ptr_map = create_type_map(TINY_CHUNK, TINY);
    else if (size <= SMALL_CHUNK):
        ptr_map = create_type_map(SMALL_CHUNK, SMALL);
    else
        ptr_map = create_type_map(size, LARGE);

    return (ptr_map)
}


t_map *create_type_map(size_t size, char type){
    size_t map_size;
    void *ptr_map;

    if (type == LARGE){
        map_size = size * sizeof(t_chunk);
    }
    else{
        map_size = get_map_size(size);
    }
    ptr_map = get_ptr_map(map_size);
    if (ptr_zone != NULL){
		init_map(ptr_map, map_size, type);
	}
	return (ptr_zone);

}

size_t get_map_size(size_t size){
    size_t	page_numbers;
	size_t	allocation_size;
	size_t	page_size;

    page_size = getpagesize();
	page_numbers = get_number_of_pages(page_size, sizeof(t_chunk) + size);
	map_size = page_numbers * page_size;
	return (map_size);
}


size_t	get_number_of_pages(size_t page_size, size_t chunk_size)
{
	size_t	page_numbers;
	size_t	chunks_numbers;
    size_t  chunks_per_page;

	page_numbers = 0;
	chunks_numbers = 0;
    chunks_per_page = page_size / chunk_size;
	if (chunk_size > 0 && page_size > 0){
		while (chunks_numbers < MIN_CHUNKS_PER_MAP){
			page_numbers++;
			chunks_numbers = page_numbers * chunks_per_page;
		}
	}
	return (page_numbers);
}

void	*init_map(size_t size)
{
	void *ptr_map;

	ptr_map = mmap(0,
									allocation_size,
									PROT_READ | PROT_WRITE,
									MAP_ANON | MAP_PRIVATE,
									-1,
									0);
	if (ptr_map == MAP_FAILED){
		return (NULL);
	}
	return (ptr_map);
}