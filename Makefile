# **************************************************************************** #
#       TITLE                                                                  #
# **************************************************************************** #
NAME				= woody_woodpacker
OUTFILE				= woody

# **************************************************************************** #
#       COMMANDS                                                               #
# **************************************************************************** #
CC					= cc
ASM					= nasm
ASOBJS_FLAGS		= -f elf64
ASFLAGS				= -f bin

# **************************************************************************** #
#       FLAGS                                                                  #
# **************************************************************************** #
CFLAGS	= -Wall -Wextra -Werror

# **************************************************************************** #
#       SOURCES                                                                #
# **************************************************************************** #
SRCS_DIR			= srcs/
SRCS_FILES			= $(notdir $(wildcard $(SRCS_DIR)*.c))

ASM_SRCS_DIR		= srcs/
ASM_SRCS_FILES		= $(notdir $(wildcard $(ASM_SRCS_DIR)*.s))

STUB_SRCS_DIR		= $(SRCS_DIR)unpacker/
STUB_SRCS_FILES		= $(notdir $(wildcard $(STUB_SRCS_DIR)*.s))

# **************************************************************************** #
#       INCLUDES                                                               #
# **************************************************************************** #
INCS 				= errors.h ww.h $(STUB_HDRS)
# Eeach one of these headers contains a different shellcode.
# Both headers are created, but only one will be used, according to the commandline options
STUB_HDRS			= stub.h stub_virus.h

# **************************************************************************** #
#       OBJ                                                                    #
# **************************************************************************** #
OBJS_DIR			= objs/
OBJS				= $(addprefix $(OBJS_DIR), $(SRCS_FILES:.c=.o))

ASM_OBJS_DIR		= $(OBJS_DIR)
ASM_OBJS			= $(addprefix $(ASM_OBJS_DIR), $(ASM_SRCS_FILES:.s=.o))

STUB_OBJS_DIR		= $(OBJS_DIR)
STUB_OBJS			= $(addprefix $(STUB_OBJS_DIR), $(STUB_SRCS_FILES:.s=.o))

# **************************************************************************** #
#       RULES                                                                  #
# **************************************************************************** #

.PHONY: all run debug clean fclean re debug

all: $(STUB_OBJS) $(NAME)

$(ASM_OBJS_DIR)%.o: $(ASM_SRCS_DIR)%.s
	mkdir -p $(ASM_OBJS_DIR)
	$(ASM) $(ASOBJS_FLAGS) $< -o $@

$(OBJS_DIR)%.o: $(SRCS_DIR)%.c $(INCS)
	mkdir -p $(OBJS_DIR)
	$(CC) -I. -c $(CFLAGS) $< -o $@

$(NAME): $(OBJS) $(ASM_OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(ASM_OBJS) -o $(NAME)

# Generate the shellcode in hex format and inject it as a global variable value
#  in a header file
$(STUB_OBJS_DIR)%.o: $(STUB_SRCS_DIR)%.s
	mkdir -p $(OBJS_DIR)
	echo "unsigned char g_$(basename $(notdir $<))[] = {" > $(basename $(notdir $<)).h
	$(ASM) $(ASFLAGS) $< -o $@
	xxd -i < $(basename $@).o >> $(basename $(notdir $<)).h
	echo "};" >> $(basename $(notdir $<)).h

# A quick test that runs the compilation + packer + packed file wirh valgrind
run: re
	valgrind ./woody_woodpacker resources/64bit-sample -i=p -s=d && ./woody

debug: CFLAGS += -g3 -DDEBUG
debug: $(NAME)

clean:
	rm -rf $(OBJS_DIR) $(ASM_OBJS_DIR)

fclean: clean
	rm -f $(NAME) $(OUTFILE) $(STUB_HDRS)

re: fclean all
