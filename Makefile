ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
NAME_LINK = libft_malloc.so

# PATH_SRC = ./src/

PATH_INC = ./inc

# PATH_INC_LIBFT = ./libft/includes/

# INCLUDES = -I $(PATH_INC) -I $(PATH_INC_LIBFT)
INCLUDES = -I $(PATH_INC)

CC = cc

ifeq ($(DEBUG),yes)
	CFLAGS = $(INCLUDES) -g -fPIC
	LDFLAGS = -shared
else
	CFLAGS = -Wall -Wextra -Werror $(INCLUDES) -fPIC
	LDFLAGS = -shared
endif

# LIBS = -L libft/ -lft

#_____________FILES____________#

SRC = ./src/malloc.c \
	./src/relloc.c \
	./src/free.c \
	./utilties/delete_chunk.c \
	./utilties/chunk.c \
	./utilties/map.c \
	./utilties/utilities_chunk.c \
	./utilties/utilities_map.c \
	./utilties/show_alloc_mem.c \
	./libft/ft_putstr.c \
	./libft/ft_putnbr.c \
	./libft/ft_putchar.c \
	./libft/ft_puthexa.c \
	./libft/ft_bzero.c \
	./libft/ft_memset.c \



OBJ = $(SRC:.c=.o)

#____________RULES_____________#

.PHONY: clean fclean re test

all: $(NAME)

$(NAME): $(OBJ)
	# make -C libft/
	# $(CC) $(OBJ) -o $(NAME) $(LIBS) $(LDFLAGS)
	$(CC) $(OBJ) -o $(NAME) $(LDFLAGS)
	ln -s $(NAME) $(NAME_LINK)

#____CLEAN____#

clean:
	rm -f $(OBJ)

#___FCLEAN___#

fclean: clean
	rm -f $(NAME)
	rm -f $(NAME_LINK)

#____RE____#

re: fclean all