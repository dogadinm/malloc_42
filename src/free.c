#include "malloc.h"

void	free(void *ptr)
{
	t_chunk	*chunk;
	t_map	*map;

	if (ptr != NULL && g_first_addr != NULL){
		chunk = (t_chunk*)(ptr - ALIGN_UP_SIZE(sizeof(t_chunk), MAL_ALIGN));
		if (check_chunk_exist(chunk) == TRUE){
			if (chunk->available == TRUE)
				return ;

			map = (t_map*)chunk->parent_map;
			release_chunk(chunk);
			coalesce_chunk(map, chunk);
			release_empty_zone(map);
		}
	}
}