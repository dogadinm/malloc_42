#include "malloc.h"

extern void	free(void *ptr)
{
	t_chunk	*chunk;
	t_map	*map;

	if (ptr != NULL && g_first_addr != NULL){
		chunk = (t_chunk*)(ptr - sizeof(t_chunk));
		if (check_chunk_exist(chunk) == TRUE){
			if (chunk->available == TRUE)
				return ;

			map = (t_map*)chunk->parent_map;
            release_chunk(chunk);
			release_empty_zone(map);
		}
	}
}