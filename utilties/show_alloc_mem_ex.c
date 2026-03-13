#include "malloc.h"

#define HEX_ROW 16

static void	put_hex_byte(unsigned char b)
{
	const char	*hex = "0123456789abcdef";

	ft_putchar(hex[b >> 4]);
	ft_putchar(hex[b & 0xF]);
}

static void	hex_dump(void *start, size_t size)
{
	unsigned char	*data;
	size_t			row;
	size_t			col;
	size_t			row_len;

	data = (unsigned char*)start;
	row = 0;
	while (row < size)
	{
		row_len = size - row;
		if (row_len > HEX_ROW)
			row_len = HEX_ROW;

		/* address */
		ft_putstr("  ");
		ft_puthexa((uint64_t)(data + row));
		ft_putstr(": ");

		/* hex bytes — first 8 */
		col = 0;
		while (col < HEX_ROW)
		{
			if (col == 8)
				ft_putchar(' ');
			if (col < row_len)
				put_hex_byte(data[row + col]);
			else
				ft_putstr("  ");
			ft_putchar(' ');
			col++;
		}

		ft_putstr(" |");

		/* ascii */
		col = 0;
		while (col < row_len)
		{
			unsigned char c = data[row + col];
			ft_putchar((c >= 32 && c < 127) ? (char)c : '.');
			col++;
		}
		/* padding if last row is short */
		col = row_len;
		while (col < HEX_ROW)
		{
			ft_putchar(' ');
			col++;
		}
		ft_putstr("|\n");

		row += row_len;
	}
}

static void	show_ex_block(t_chunk *chunk)
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
			hex_dump(chunk->ptr_data, chunk->data_size);
		}
		chunk = (t_chunk*)chunk->next_chunk;
	}
}

static void	show_ex_zone(t_map *map)
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
		show_ex_block(current_map->first_chunk);
		current_map = (t_map*)current_map->map_next;
	}
}

void	show_alloc_mem_ex(void)
{
	t_map		*map;
	uint64_t	total;
	t_map		*current_map;
	t_chunk		*chunk;

	map = (t_map*)g_first_addr;
	show_ex_zone(map);

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
	ft_putstr("Total : ");
	ft_putnbr(total);
	ft_putstr(" bytes\n");
}
