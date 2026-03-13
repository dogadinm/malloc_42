NAME = libft.a

CC = cc
CFLAGS = -Wall -Wextra -Werror -I ../inc

SRC = ft_putstr.c \
	ft_putnbr.c \
	ft_putchar.c \
	ft_puthexa.c \
	ft_bzero.c \
	ft_memset.c

OBJ = $(SRC:.c=.o)

.PHONY: clean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	ar rcs $(NAME) $(OBJ)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
