#include "malloc.h"
#include <stdint.h>

void	ft_puthexa(uint64_t nb){
	char	hex_revert[16];
	int		hex[16];
	int		i;

	ft_bzero(hex_revert, 16);
	ft_bzero(hex, 16);
	i = 0;
	while (nb){
		hex[i] = nb % 16;
		if (hex[i] >= 10)
			hex_revert[i] = hex[i] + 'A' - 10;
		else
			hex_revert[i] = hex[i] + '0';
		nb /= 16;
		i++;
	}
	ft_putstr("0x");
	while (i >= 0){
		ft_putchar(hex_revert[i]);
		i--;
	}
}