# **************************************************************************** #
#       TITLE                                                                  #
# **************************************************************************** #
NAME	= woody_woodpacker

# **************************************************************************** #
#       COMMANDS                                                               #
# **************************************************************************** #
CC = cc

ASM		= nasm
ASFLAGS	= -f elf64

# **************************************************************************** #
#       FLAGS                                                                  #
# **************************************************************************** #
CFLAGS = -Wall -Wextra -g #-Werror

# **************************************************************************** #
#       SOURCES                                                                #
# **************************************************************************** #
SRCS_DIR	= srcs/
SRCS_FILES	= $(notdir $(wildcard $(SRCS_DIR)*.c))
SRCS		= $(addprefix $(SRCS_DIR), $(SRCS_FILES)%.c)

ASM_SRCS_DIR	= srcs/
ASM_SRCS_FILES	= $(notdir $(wildcard $(ASM_SRCS_DIR)*.s))
ASM_SRCS		= $(addprefix $(ASM_SRCS_DIR), $(ASM_SRCS_FILES)%.s)

STUB_SRCS_DIR	= $(SRCS_DIR)unpacker/
STUB_SRCS_FILES	= stub.s
STUB_SRCS		= $(addprefix $(STUB_SRCS_DIR), $(STUB_SRCS_FILES))

# **************************************************************************** #
#       INCLUDES                                                               #
# **************************************************************************** #
INCS 		= errors.h ww.h

# **************************************************************************** #
#       OBJ                                                                    #
# **************************************************************************** #
OBJS_DIR		= objs/
OBJS			= $(addprefix $(OBJS_DIR), $(SRCS_FILES:.c=.o))

ASM_OBJS_DIR	= $(OBJS_DIR)
ASM_OBJS		= $(addprefix $(ASM_OBJS_DIR), $(ASM_SRCS_FILES:.s=.o))

STUB_OBJS_DIR	= $(OBJS_DIR)
STUB_OBJS		= $(addprefix $(STUB_OBJS_DIR), $(STUB_SRCS_FILES:.s=.o))

# **************************************************************************** #
#       RULES                                                                  #
# **************************************************************************** #
$(ASM_OBJS_DIR)%.o : $(ASM_SRCS_DIR)%.s
	mkdir -p $(ASM_OBJS_DIR)
	$(ASM) $(ASFLAGS) $< -o $@

$(OBJS_DIR)%.o : $(SRCS_DIR)%.c $(STUB_SRCS) $(INCS)
	$(CC) $(CFLAGS) -I. -c $< -o $@ -DSTUB=\"`/usr/bin/hexdump -ve '"\\\x" 1/1 "%02x"' $(STUB_OBJS)`\"

$(NAME) : $(STUB_OBJS) $(ASM_OBJS) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(ASM_OBJS) -o $(NAME)

$(STUB_OBJS) : $(STUB_SRCS)
	mkdir -p $(STUB_OBJS_DIR)
	$(ASM) -f bin $< -o $@

all: $(NAME)

debug: CFLAGS += -g3 -DDEBUG
debug: $(NAME)

clean:
	rm -rf $(OBJS_DIR) $(ASM_OBJS_DIR)

fclean: clean
	rm -f $(NAME) $(OUTFILE)

re: fclean all

# **************************************************************************** #
#       PHONY                                                                  #
# **************************************************************************** #
.PHONY: all clean fclean re debug
