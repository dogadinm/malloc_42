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
								ALIGN_UP_SIZE(sizeof(t_chunk), MAL_ALIGN) +
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
	new_chunk = (t_chunk *)(ptr_data - ALIGN_UP_SIZE(sizeof(t_chunk), MAL_ALIGN));
	if (new_chunk != NULL){
		copy_data_size(ptr_data, chunk->ptr_data,
										new_size, chunk->data_size);
		free(chunk->ptr_data);
	}
	return (new_chunk);
}

static void	split_chunk(t_chunk *chunk, size_t old_size, size_t new_size)
{
	size_t	header_size;
	void	*free_addr;
	void	*end_boundary;
	size_t	free_data_size;
	t_chunk	*free_chunk;

	header_size = ALIGN_UP_SIZE(sizeof(t_chunk), MAL_ALIGN);
	free_addr = ALIGN_UP_PTR((char*)chunk->ptr_data + new_size, MAL_ALIGN);
	if (chunk->next_chunk != NULL)
		end_boundary = chunk->next_chunk;
	else
		end_boundary = ALIGN_UP_PTR((char*)chunk->ptr_data + old_size, MAL_ALIGN);
	if ((char*)end_boundary - (char*)free_addr <= (long)header_size)
		return ;
	free_data_size = (size_t)((char*)end_boundary - (char*)free_addr) - header_size;
	free_chunk = init_chunk((t_map*)chunk->parent_map, free_addr, free_data_size);
	free_chunk->next_chunk = chunk->next_chunk;
	chunk->next_chunk = free_chunk;
}

t_chunk	*resize_allocation(t_chunk *chunk, size_t new_size){
	t_chunk	*new_chunk;
	size_t	old_size;

	new_chunk = NULL;
	if (chunk->data_size >= new_size){
		old_size = chunk->data_size;
		chunk->data_size = new_size;
		split_chunk(chunk, old_size, new_size);
		new_chunk = chunk;
	}
	else if (chunk_fusion(chunk, new_size) == TRUE){
		t_chunk *next = (t_chunk*)chunk->next_chunk;
		chunk->data_size = chunk->data_size
			+ ALIGN_UP_SIZE(sizeof(t_chunk), MAL_ALIGN)
			+ next->data_size;
		chunk->next_chunk = next->next_chunk;
		if (chunk->next_chunk != NULL)
			((t_chunk*)chunk->next_chunk)->parent_map = chunk->parent_map;
		new_chunk = chunk;
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
	void	*result;

	if (ptr == NULL || g_first_addr == NULL)
		return (malloc(size));
	if (size == 0){
		free(ptr);
		return (NULL);
	}
	pthread_mutex_lock(&g_malloc_mutex);
	current_chunk = (t_chunk*)(ptr - ALIGN_UP_SIZE(sizeof(t_chunk), MAL_ALIGN));
	if (check_chunk_exist(current_chunk) == FALSE
		|| current_chunk->available == TRUE)
	{
		pthread_mutex_unlock(&g_malloc_mutex);
		return (NULL);
	}
	current_map = (t_map*)current_chunk->parent_map;
	if (check_type_map(current_map, size) == TRUE)
		new_chunk = resize_allocation(current_chunk, size);
	else
		new_chunk = new_allocation(current_chunk, size);
	result = (new_chunk != NULL) ? new_chunk->ptr_data : NULL;
	pthread_mutex_unlock(&g_malloc_mutex);
	return (result);
}