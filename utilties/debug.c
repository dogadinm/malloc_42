#include "malloc.h"
#include <stdlib.h>

typedef struct s_debug_opts {
	t_bool	verbose;
	t_bool	scribble;
	t_bool	pre_scribble;
}	t_debug_opts;

static t_debug_opts	s_debug = {FALSE, FALSE, FALSE};

void	init_debug_env(void)
{
	s_debug.verbose      = (getenv("MALLOC_VERBOSE")      != NULL) ? TRUE : FALSE;
	s_debug.scribble     = (getenv("MALLOC_SCRIBBLE")     != NULL) ? TRUE : FALSE;
	s_debug.pre_scribble = (getenv("MALLOC_PRE_SCRIBBLE") != NULL) ? TRUE : FALSE;
}

static void	write_size(size_t n)
{
	char	buf[21];
	int		len;

	len = 0;
	if (n == 0)
	{
		ft_putchar('0');
		return ;
	}
	while (n > 0)
	{
		buf[len++] = '0' + (n % 10);
		n /= 10;
	}
	while (len-- > 0)
		ft_putchar(buf[len]);
}

void	debug_malloc(void *ptr, size_t size)
{
	if (!s_debug.verbose)
		return ;
	ft_putstr("[MALLOC] malloc(");
	write_size(size);
	ft_putstr(") = ");
	ft_puthexa((uint64_t)(uintptr_t)ptr);
	ft_putchar('\n');
}

void	debug_free(void *ptr)
{
	if (!s_debug.verbose)
		return ;
	ft_putstr("[MALLOC] free(");
	ft_puthexa((uint64_t)(uintptr_t)ptr);
	ft_putstr(")\n");
}

void	debug_realloc(void *old_ptr, size_t size, void *new_ptr)
{
	if (!s_debug.verbose)
		return ;
	ft_putstr("[MALLOC] realloc(");
	ft_puthexa((uint64_t)(uintptr_t)old_ptr);
	ft_putstr(", ");
	write_size(size);
	ft_putstr(") = ");
	ft_puthexa((uint64_t)(uintptr_t)new_ptr);
	ft_putchar('\n');
}

void	debug_scribble_free(void *ptr, size_t size)
{
	if (!s_debug.scribble)
		return ;
	ft_memset(ptr, 0x55, size);
}

void	debug_scribble_alloc(void *ptr, size_t size)
{
	if (!s_debug.pre_scribble)
		return ;
	ft_memset(ptr, 0xAA, size);
}
