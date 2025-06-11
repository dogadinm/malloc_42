#ifndef MALLOC_H
# define MALLOC_H

# include <sys/mman.h>
# include <unist.h>
# include <stdint.h>

# define TINY 'T'
# define SMALL 'S'
# define LARGE 'L'

# define TINY_BLOCK 1024
# define SMALL_BLOCK 1024 * 32
# define MIN_CHUNKS_PER_MAP 100

# define FALSE 0
# define TRUE 1

typedef unsigned int	t_bool;

void			*g_first_addr;

typedef struct s_chunk {
	void         *parent_map;
	void         *next_chunk;
	size_t        payload_size;
	unsigned int  available;
	void         *ptr_data;
} t_chunk;

typedef struct s_map {
	t_chunk*   first_chunk;
	void*      map_origin;
	void*      map_next;
	void*      map_prev;
	size_t     total_size;
	char       category;
} t_map;