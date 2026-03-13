ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
NAME_LINK = libft_malloc.so

PATH_INC = ./inc

INCLUDES = -I $(PATH_INC)

CC = cc

ifeq ($(DEBUG),yes)
	CFLAGS = $(INCLUDES) -g -fPIC
	LDFLAGS = -shared -lpthread
else
	CFLAGS = -Wall -Wextra -Werror $(INCLUDES) -fPIC
	LDFLAGS = -shared -lpthread
endif

LIBS = -L libft/ -lft

SRC = ./src/malloc.c \
	./src/relloc.c \
	./src/free.c \
	./utilties/delete_chunk.c \
	./utilties/chunk.c \
	./utilties/map.c \
	./utilties/utilities_chunk.c \
	./utilties/utilities_map.c \
	./utilties/show_alloc_mem.c \
	./utilties/show_alloc_mem_ex.c \
	./utilties/debug.c \

OBJ = $(SRC:.c=.o)

.PHONY: clean fclean re test

all: $(NAME)

$(NAME): $(OBJ)
	make -C libft/
	$(CC) $(OBJ) -o $(NAME) $(LIBS) $(LDFLAGS)
	ln -sf $(NAME) $(NAME_LINK)

clean:
	rm -f $(OBJ)

fclean: clean
	make fclean -C libft/
	rm -f $(NAME)
	rm -f $(NAME_LINK)

re: fclean all