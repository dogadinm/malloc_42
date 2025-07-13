#include "malloc.h"

void	display_octets(t_chunk *chunk)
{
	size_t size;

	size = chunk->data_size;
	ft_putnbr(size);
	ft_putstr(" octets");
}

void	show_memory_block(void *addr)
{
	t_chunk *chunk;

	chunk = (t_chunk*)addr;
	while (chunk != NULL)
	{
		ft_puthexa((uint64_t)chunk->ptr_data);
		ft_putstr(" - ");
		ft_puthexa((uint64_t)chunk->ptr_data + (uint64_t)chunk->data_size);
		ft_putstr(" : ");
		display_octets(chunk);
		ft_putchar('\n');
		chunk = chunk->next_chunk;
	}
}

void	show_memory_zone(t_map *map)
{
	t_map	*current_map;

	current_map = map;
	while (current_map != NULL)
	{
		printf("map type: %c\n", current_map->type);
		if (current_map->type == 'T')
			ft_putstr("TINY : ");
		else if (current_map->type == 'S')
			ft_putstr("SMALL : ");
		else
			ft_putstr("LARGE : ");
		ft_puthexa((uint64_t)current_map->map_origin);
		ft_putchar('\n');
		show_memory_block(current_map->first_chunk);
		current_map = current_map->map_next;
	}
}

void	show_total(t_map *map)
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
			total += chunk->data_size;
			chunk = chunk->next_chunk;
		}
		current_map = current_map->map_next;
	}
	ft_putstr("Total : ");
	ft_putnbr(total);
	ft_putstr(" octets\n");
}

void	show_alloc_mem(void)
{
	t_map *map;

	map = (t_map*)g_first_addr;
	show_memory_zone(map);
	show_total(map);
}