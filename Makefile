########################
#### VARIABLES #########
########################

NAME		 	= blackstar
CC			 	= gcc
RM			 	= rm -rf

INCLUDES 	 	= -I./include/
LINK_FLAG    	=
CFLAGS 		 	= -Wall -Wextra -O2 -static -fPIC -Wshadow -Wdouble-promotion \
				  -Wformat=2 -Wformat-truncation=2 -Wundef -fno-common \
				  -Wfloat-equal -Wcast-align -Wstrict-prototypes \
				  -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return \
				  -Wunreachable-code -Wstack-usage=15000 \
				  -Wno-stringop-truncation -Wno-strict-overflow -Wno-discarded-qualifiers \
				  -Wno-unused-variable # -g3
PATTERN 	 	= .c
OBJPATTERN  	= .o
SRC_DIR 		= ./sources
OBJECT_DIR 		= ./objects

vpath %$(PATTERN) $(SRC_DIR)

MAIN		 	= main.c
SRC 		 	= elf_utils.c setup.c payload.c

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
	@echo "Compiling $@"
	@$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) $(LINK_FLAG)

$(NAME): $(OBJ)
	@$(CC) -o $(NAME) $^ $(CFLAGS) $(INCLUDES) $(LINK_FLAG)
	@strip --strip-all $(NAME)
	@echo "[*** COMPILATION SUCCESSFUL ***]"

pack: $(NAME)
	@upx --best $(NAME)
	@./.cleanup $(NAME)
	@echo "[*** CLEANED $(NAME) ***]"

clean:
	@$(RM) objects

fclean: clean
	@$(RM) $(NAME)
	@echo "[*** CLEAN ***]"

re: fclean all

.PHONY: all clean fclean re
