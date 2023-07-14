# **************************************************************************** #
#       TITLE                                                                  #
# **************************************************************************** #
NAME	= woody_woodpacker
OUTFILE	= woody

# **************************************************************************** #
#       COMMANDS                                                               #
# **************************************************************************** #
CC = cc

ASM		= nasm $(ASFLAGS)
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

STUB_FILE		= $(SRCS_DIR)/unpacker/stub.s

# **************************************************************************** #
#       INCLUDES                                                               #
# **************************************************************************** #
INCS 		= errors.h ww.h 
STUB_HDR	= stub.h

# **************************************************************************** #
#       OBJ                                                                    #
# **************************************************************************** #
OBJS_DIR		= objs/
OBJS			= $(addprefix $(OBJS_DIR), $(SRCS_FILES:.c=.o))

ASM_OBJS_DIR	= objs/
ASM_OBJS		= $(addprefix $(ASM_OBJS_DIR), $(ASM_SRCS_FILES:.s=.o))

STUB_OBJ		= $(STUB_FILE):.s=.o

# **************************************************************************** #
#       RULES                                                                  #
# **************************************************************************** #
$(ASM_OBJS_DIR)%.o : $(ASM_SRCS_DIR)%.s
	mkdir -p $(ASM_OBJS_DIR)
	$(ASM) $< -o $@

$(OBJS_DIR)%.o : $(SRCS_DIR)%.c $(INCS)
	mkdir -p $(OBJS_DIR)
	$(CC) -I. -c $(CFLAGS) $< -o $@

$(NAME) : $(OBJS) $(ASM_OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(ASM_OBJS) -o $(NAME)

all: generate_hex_stub $(NAME)

generate_hex_stub : $(STUB_FILE)
	echo "unsigned char _stub[] = {" > $(STUB_HDR)
	nasm -f bin $(STUB_FILE) -o $(STUB_OBJ)
	xxd -i < $(STUB_OBJ) >> $(STUB_HDR)
	echo "};" >> $(STUB_HDR)

debug: CFLAGS += -g3 -DDEBUG
debug: $(NAME)

clean:
	rm -rf $(OBJS_DIR) $(ASM_OBJS_DIR)

fclean: clean
	rm -f $(NAME) $(OUTFILE) $(STUB_HDR)

re: fclean all

# **************************************************************************** #
#       PHONY                                                                  #
# **************************************************************************** #
.PHONY: all clean fclean re debug
