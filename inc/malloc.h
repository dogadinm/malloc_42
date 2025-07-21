#ifndef MALLOC_H
#define MALLOC_H

# include <sys/mman.h>
# include <unistd.h>
# include <stdint.h>
# include <stdio.h>

# define TINY 'T'
# define SMALL 'S'
# define LARGE 'L'

# define TINY_CHUNK 1024
# define SMALL_CHUNK 1024 * 32
# define MIN_CHUNKS_PER_MAP 100

# define FALSE 0
# define TRUE 1

typedef unsigned int	t_bool;

extern void *g_first_addr;

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



/* map */
t_bool	available_map_space(t_map *map, size_t size_data_chunk);
t_map	*find_map(t_map *first_map, size_t size);
t_map	*new_map(t_map *map, size_t size_data);
t_map 	*init_first_map(size_t size);
size_t	calculate_map_size(size_t size);
void	*allocate_memory(size_t size);
void 	*malloc(size_t size);
void	*realloc(void *ptr, size_t size);
extern void	free(void *ptr);
void	release_empty_zone(t_map *map_origin);
t_bool	check_type_map(t_map *map, size_t size);
t_bool	check_chunk_exist(t_chunk *addr_chunk);
void	release_chunk(t_chunk *chunk);
void	show_alloc_mem(void);


/* chunk */
t_chunk	*init_chunk(t_map *map, void *chunk_ptr, size_t data_size);
t_chunk	*get_chunk(t_chunk *chunk, t_map *first_map, size_t size);
t_chunk	*exist_free_chunk(t_chunk *chunk, size_t data_size);
t_chunk	*get_last_chunk(t_map *map);
void	set_chunk(t_chunk *chunk, size_t data_size);


void	ft_putstr(char const *s);
void	ft_putnbr(int n);
void	ft_putchar(char c);
void	ft_puthexa(uint64_t nb);
void	ft_bzero(void *s, size_t n);
void	*ft_memset(void *b, int c, size_t len);

#endif