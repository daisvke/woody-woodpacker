# **************************************************************************** #
#       TITLE                                                                  #
# **************************************************************************** #
NAME = woody_woodpacker
OUTFILE = woody

# **************************************************************************** #
#       COMMANDS                                                               #
# **************************************************************************** #
CC = cc

# **************************************************************************** #
#       FLAGS                                                                  #
# **************************************************************************** #
CFLAGS = -Wall -Wextra -g #-Werror

# **************************************************************************** #
#       SOURCES                                                                #
# **************************************************************************** #
SRCS = \
		main.c \
		syscall_impl.c \
		cryptography.c \
		process_data.c \
		utils.c \
		mapping.c \

# **************************************************************************** #
#       INCLUDES                                                                #
# **************************************************************************** #
INCS = \
		errors.h \
		ww.h

# **************************************************************************** #
#       RULES                                                                  #
# **************************************************************************** #
OBJS = $(addprefix objs/,$(SRCS:.c=.o))
OBJ_DIRS = $(sort $(dir $(OBJS)))

objs/%.o : %.c $(INCS) | $(OBJ_DIRS)
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
	rm -f $(NAME) $(OUTFILE)

re: fclean all

# **************************************************************************** #
#       PHONY                                                                  #
# **************************************************************************** #
.PHONY: all clean fclean re debug
