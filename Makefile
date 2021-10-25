########################
#### VARIABLES #########
########################

NAME		 	= blackstar
CC			 	= gcc
RM			 	= rm -rf

INCLUDES 	 	= -I./include/
LINK_FLAG    	=
CFLAGS 		 	= -Wall -Wextra -fPIC -Wshadow -Wdouble-promotion \
				  -Wformat=2 -Wformat-truncation=2 -Wundef -fno-common \
				  -Wfloat-equal -Wcast-align -Wstrict-prototypes \
				  -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return \
				  -Wunreachable-code -Wstack-usage=15000 \
				  -Wno-stringop-truncation -Wno-strict-overflow -Wno-discarded-qualifiers \
				  -Wno-unused-variable
PROD_FLAGS 		= -static -s -O2
DEBUG_FLAGS 	= -g3 -g

PATTERN 	 	= .c
OBJPATTERN  	= .o
SRC_DIR 		= ./sources
OBJECT_DIR 		= ./objects

CRYPTER_PATH 	= ./crypter
CRYPTER_BIN 	= blackstar_crypter

vpath %$(PATTERN) $(SRC_DIR)

MAIN		 	= main.c
SRC 		 	= elf_utils.c setup.c payload.c

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
	@make --no-print-directory -C $(CRYPTER_PATH)
	@cp ./$(CRYPTER_PATH)/$(CRYPTER_BIN) .
	@$(CC) -o $(NAME) $^ $(CFLAGS) $(DEBUG_FLAGS) $(INCLUDES) $(LINK_FLAG)
	@echo "[** RUNNING CRYPTER **]"
	@./$(CRYPTER_BIN) "Some key" ".banshee" ".st_peter" $(NAME)
	@echo "[*** COMPILATION SUCCESSFUL ***]"

clean:
	@make --no-print-directory clean -C $(CRYPTER_PATH)
	@rm -f $(CRYPTER_BIN)
	@$(RM) objects

fclean: clean
	@make --no-print-directory fclean -C $(CRYPTER_PATH)
	@$(RM) $(NAME)
	@echo "[*** CLEAN ***]"

re: fclean all

.PHONY: all clean fclean re
