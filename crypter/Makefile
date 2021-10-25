########################
#### VARIABLES #########
########################

NAME		 	= blackstar_crypter
CC			 	= gcc
RM			 	= rm -rf

INCLUDES 	 	= -I./include/
LINK_FLAG    	=

DEBUG_FLAGS 	= -g3 -g
CFLAGS 		 	= -Wall -Wextra -fPIC -Wshadow $(DEBUG_FLAGS) # -O2


PATTERN 	 	= .c
OBJPATTERN  	= .o
SRC_DIR 		= ./sources
OBJECT_DIR 		= ./objects

vpath %$(PATTERN) $(SRC_DIR)

MAIN		 	= main.c
SRC 		 	= crypt.c

########################
#### COMPILATION #######
########################

COMBINED		= $(SRC) $(MAIN)
OBJ 	 		= $(patsubst %$(PATTERN), $(OBJECT_DIR)/%$(OBJPATTERN), $(COMBINED))

all: directories $(NAME)

debug: directories dbg

dbg: $(OBJ)
	@$(CC) -o $(NAME) $^ $(CFLAGS) $(DEBUG_FLAGS) $(INCLUDES) $(LINK_FLAG)
	@echo "[*** COMPILATION SUCCESSFUL ## DEBUG MODE ***]"

$(OBJECT_DIR):
	@mkdir -p $@

directories: | $(OBJECT_DIR)

$(OBJECT_DIR)/%$(OBJPATTERN) : %$(PATTERN)
	@echo "Compiling $@"
	@$(CC) -o $@ -c $< $(CFLAGS) $(DEBUG_FLAGS) $(INCLUDES) $(LINK_FLAG)

$(NAME): $(OBJ)
	@$(CC) -o $(NAME) $^ $(CFLAGS) $(OBF_FLAGS) $(INCLUDES) $(LINK_FLAG)
	@echo "[*** CRYPTER: COMPILATION SUCCESSFUL ***]"

clean:
	@$(RM) objects

fclean: clean
	@$(RM) $(NAME)
	@echo "[*** CRYPTER: CLEAN ***]"

re: fclean all

.PHONY: all clean fclean re
