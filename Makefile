# **************************************************************************** #
#       TITLE                                                                  #
# **************************************************************************** #
NAME = woody_woodpacker

# **************************************************************************** #
#       COMMANDS                                                               #
# **************************************************************************** #
CC = cc

# **************************************************************************** #
#       FLAGS                                                                  #
# **************************************************************************** #
CFLAGS = -Wall -Wextra -Werror

# **************************************************************************** #
#       SOURCES                                                                #
# **************************************************************************** #
SRCS = main.c \

# **************************************************************************** #
#       RULES                                                                  #
# **************************************************************************** #
OBJS = $(addprefix objs/,$(SRCS:.c=.o))
OBJ_DIRS = $(sort $(dir $(OBJS)))

objs/%.o : %.c | $(OBJ_DIRS)
	$(CC) -I. -c $(CFLAGS) $< -o $@

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIRS):
	mkdir -p $@

all: $(NAME)

debug: CFLAGS += -g3 -DDEBUG
debug: $(NAME)

clean:
	rm -rf objs

fclean: clean
	rm -f $(NAME)

re: fclean all

# **************************************************************************** #
#       PHONY                                                                  #
# **************************************************************************** #
.PHONY: all clean fclean re test debug
