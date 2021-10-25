########################
#### VARIABLES #########
########################

NAME		 	= blackstar_crypter
CC			 	= gcc
RM			 	= rm -rf

INCLUDES 	 	= -I./include/
LINK_FLAG    	= -L ../ -lstar

DEBUG_FLAGS 	= -g3 -g
CFLAGS 		 	= -Wall -Wextra -fPIC -Wshadow $(DEBUG_FLAGS) # -O2


PATTERN 	 	= .c
OBJPATTERN  	= .o
SRC_DIR 		= ./sources
OBJECT_DIR 		= ./objects

LIB_PATH 		= ../lib/star
LIB_HEADER 		= blackstar.h

vpath %$(PATTERN) $(SRC_DIR)

MAIN		 	= main.c
SRC 		 	=

########################
#### COMPILATION #######
########################

COMBINED		= $(SRC) $(MAIN)
OBJ 	 		= $(patsubst %$(PATTERN), $(OBJECT_DIR)/%$(OBJPATTERN), $(COMBINED))

all: directories $(NAME)

$(OBJECT_DIR):
	@mkdir -p $@

directories: | $(OBJECT_DIR)

$(OBJECT_DIR)/%$(OBJPATTERN) : %$(PATTERN)
	@make --no-print-directory  -C $(LIB_PATH) >/dev/null
	@cp ../include/$(LIB_HEADER) ./include/.
	@echo "Compiling $@"
	@$(CC) -o $@ -c $< $(CFLAGS) $(DEBUG_FLAGS) $(INCLUDES) $(LINK_FLAG)

$(NAME): $(OBJ)
	@$(CC) -o $(NAME) $^ $(CFLAGS) $(OBF_FLAGS) $(INCLUDES) $(LINK_FLAG)
	@echo "[*** CRYPTER: COMPILATION SUCCESSFUL ***]"

clean:
	@$(RM) objects

fclean: clean
	@$(RM) $(NAME)
	@$(RM) include/$(LIB_HEADER)
	@echo "[*** CRYPTER: CLEAN ***]"

re: fclean all

.PHONY: all clean fclean re
