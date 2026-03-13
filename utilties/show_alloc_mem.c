#include "malloc.h"

static void	show_memory_block(t_chunk *chunk)
{
	while (chunk != NULL)
	{
		if (chunk->available == FALSE)
		{
			ft_puthexa((uint64_t)chunk->ptr_data);
			ft_putstr(" - ");
			ft_puthexa((uint64_t)chunk->ptr_data + (uint64_t)chunk->data_size);
			ft_putstr(" : ");
			ft_putnbr(chunk->data_size);
			ft_putstr(" bytes\n");
		}
		chunk = (t_chunk*)chunk->next_chunk;
	}
}

static void	show_memory_zone(t_map *map)
{
	t_map	*current_map;

	current_map = map;
	while (current_map != NULL)
	{
		if (current_map->type == TINY)
			ft_putstr("TINY : ");
		else if (current_map->type == SMALL)
			ft_putstr("SMALL : ");
		else
			ft_putstr("LARGE : ");
		ft_puthexa((uint64_t)current_map->map_origin);
		ft_putchar('\n');
		show_memory_block(current_map->first_chunk);
		current_map = (t_map*)current_map->map_next;
	}
}

static uint64_t	count_total(t_map *map)
{
	uint64_t	total;
	t_map		*current_map;
	t_chunk		*chunk;

	total = 0;
	current_map = map;
	while (current_map != NULL)
	{
		chunk = current_map->first_chunk;
		while (chunk != NULL)
		{
			if (chunk->available == FALSE)
				total += chunk->data_size;
			chunk = (t_chunk*)chunk->next_chunk;
		}
		current_map = (t_map*)current_map->map_next;
	}
	return (total);
}

void	show_alloc_mem(void)
{
	t_map		*map;
	uint64_t	total;

	map = (t_map*)g_first_addr;
	show_memory_zone(map);
	total = count_total(map);
	ft_putstr("Total : ");
	ft_putnbr(total);
	ft_putstr(" bytes\n");
}
