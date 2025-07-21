
#include "malloc.h"

void	*ft_memset(void *b, int c, size_t len){
	size_t	counter;
	char	*str;

	str = (char *)b;
	if (!str)
		return (NULL);
	if (len == 0)
		return (str);
	counter = 0;
	while (counter < len){
		str[counter] = (char)c;
		counter++;
	}
	return (b);
}