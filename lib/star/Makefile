########################
#### VARIABLES #########
########################

NAME		 	= libstar.a
CC			 	= gcc
AR 				= ar rc
RM			 	= rm -rf

INCLUDES 	 	=
LINK_FLAG    	=
CFLAGS 		 	= -Wall -Wextra -fPIC -Wshadow -Wdouble-promotion \
				  -Wformat=2 -Wformat-truncation=2 -Wundef -fno-common \
				  -Wfloat-equal -Wcast-align -Wstrict-prototypes \
				  -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return \
				  -Wunreachable-code -Wstack-usage=15000 \
				  -Wno-stringop-truncation -Wno-strict-overflow -Wno-discarded-qualifiers \
				  -Wno-unused-variable -I.
PROD_FLAGS 		= -static -s -O2
DEBUG_FLAGS 	= -g3 -g

PATTERN 	 	= .c
OBJPATTERN  	= .o
SRC_DIR 		= ./sources
OBJECT_DIR 		= ./objects

vpath %$(PATTERN) $(SRC_DIR)

MAIN		 	=
SRC 		 	= decrypt.c crypt.c utils.c crypters.c
HEADER 			= blackstar.h

DOCS_DIR 		= ./docs

########################
#### COMPILATION #######
########################

COMBINED		= $(SRC) $(MAIN)
OBJ 	 		= $(patsubst %$(PATTERN), $(OBJECT_DIR)/%$(OBJPATTERN), $(COMBINED))

.PHONY: all clean fclean re directories doc

all: directories $(NAME)

$(OBJECT_DIR):
	@mkdir -p $@

directories: | $(OBJECT_DIR)

$(OBJECT_DIR)/%$(OBJPATTERN) : %$(PATTERN)
	@echo "Compiling $@"
	@$(CC) -o $@ -c $< $(CFLAGS) $(PROD_FLAGS) $(INCLUDES) $(LINK_FLAG)

$(NAME): $(OBJ)
	@cp $(HEADER) ../../include
	@$(AR) $(NAME) $(OBJ)
	@cp $(NAME) ../../
	@echo "[*** LIB: COMPILATION SUCCESSFUL ***]"

clean:
	@$(RM) objects

fclean: clean
	@$(RM) ../../include/$(HEADER)
	@$(RM) ../../$(NAME)
	@$(RM) $(NAME)
	@make --no-print-directory ./docs/ clean
	@echo "[*** LIB: CLEAN ***]"

re: fclean all

doc:
	@make --no-print-directory ./docs

