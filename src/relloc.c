#include "malloc.h"


void	*copy_data_size(void *dst, const void *src, size_t dst_size, size_t src_size){
	char		*char_dst;
	const char	*char_src;
	size_t		i;

	char_src = src;
	char_dst = dst;
	i = 0;
	while (i < src_size && i < dst_size){
		char_dst[i] = char_src[i];
		i++;
	}
	return (dst);
}

t_bool	chunk_fusion(t_chunk *chunk, size_t new_size){
	t_chunk	*chunk_next;
	size_t	available_space;

	available_space = 0;
	if (chunk != NULL){
		chunk_next = (t_chunk*)chunk->next_chunk;
		if (chunk_next != NULL && chunk_next->available == TRUE){
			available_space = (chunk->data_size + 
                                sizeof(t_chunk) +
								chunk_next->data_size);

			if (available_space >= new_size)
				return (TRUE);
		}
	}
	return (FALSE);
}

t_chunk	*new_allocation(t_chunk *chunk, size_t new_size){
	t_chunk	*new_chunk;
	void	*ptr_data;

	ptr_data = NULL;
	ptr_data = malloc(new_size);
	if (ptr_data == NULL)
		return (NULL);
	new_chunk = (t_chunk *)(ptr_data - sizeof(t_chunk));
	if (new_chunk != NULL){
		copy_data_size(new_chunk->ptr_data, chunk->ptr_data,
										new_chunk->data_size, chunk->data_size);
		free(chunk->ptr_data);
	}
	return (new_chunk);
}

t_chunk	*resize_allocation(t_chunk *chunk, size_t new_size){
	t_chunk	*new_chunk;

	new_chunk = NULL;
	if (chunk->data_size >= new_size){
		chunk->data_size = new_size;
		new_chunk = chunk;
	}
	else if (chunk_fusion(chunk, new_size) == TRUE){
		chunk->data_size = new_size;
		new_chunk = chunk;
		if ((t_chunk*)chunk->next_chunk != NULL)
			new_chunk->next_chunk = ((t_chunk*)chunk->next_chunk)->next_chunk;
	}
	else{
		new_chunk = new_allocation(chunk, new_size);
	}
	return (new_chunk);
}

void	*realloc(void *ptr, size_t size){
	t_chunk	*current_chunk;
	t_chunk	*new_chunk;
	t_map	*current_map;

	if (ptr == NULL || g_first_addr == NULL)
		return (malloc(size));
	current_chunk = (t_chunk*)(ptr - sizeof(t_chunk));
	if (check_chunk_exist(current_chunk) == FALSE)
		return (NULL);
	if (current_chunk->available == TRUE)
		return (NULL);
	current_map = (t_map*)current_chunk->parent_map;
	if (check_type_map(current_map, size) == TRUE)
		new_chunk = resize_allocation(current_chunk, size);
	else
		new_chunk = new_allocation(current_chunk, size);
	if (new_chunk == NULL)
		return (NULL);
	return (new_chunk->ptr_data);
}