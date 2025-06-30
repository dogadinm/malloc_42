#ifndef MALLOC_H
# define MALLOC_H

# include <sys/mman.h>
# include <unist.h>
# include <stdint.h>

# define TINY 'T'
# define SMALL 'S'
# define LARGE 'L'

# define TINY_CHUNK 1024
# define SMALL_CHUNK 1024 * 32
# define MIN_CHUNKS_PER_MAP 100

# define FALSE 0
# define TRUE 1

typedef unsigned int	t_bool;

void			*g_first_addr;

typedef struct s_chunk {
	void         *parent_map;
	void         *next_chunk;
	size_t        data_size;
	unsigned int  available;
	void         *ptr_data;
} t_chunk;

typedef struct s_map {
	t_chunk*   first_chunk;
	void*      map_origin;
	void*      map_next;
	void*      map_prev;
	size_t     total_size;
	char       type;
} t_map;

/* map init */
t_map 	*create_map(size_t size)
void	init_map(void *first_zone_addr, size_t map_size, char type)
t_chunk	*init_chunk(t_map *map, void *chunk_ptr, size_t data_size)

t_map	*find_map(t_map *first_map, size_t size)

t_chunk	*add_chunk(t_map *map, size_t size_data)

t_bool	available_chunk_space(t_chunk *chunk, size_t data_size)
t_chunk	*exist_free_chunk(t_chunk *chunk, size_t data_size)

t_chunk	*last_chunk(t_map *map)
t_bool	available_map_space(t_map *map, size_t size_data_chunk)
void	set_chunk(t_chunk *chunk, size_t data_size)
