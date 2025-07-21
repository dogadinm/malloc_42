#include "malloc.h"

void	ft_putstr(char const *s){
	unsigned int i;

	i = 0;
	while (s[i])
		ft_putchar(s[i++]);
}