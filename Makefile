NAME	= computorv1
GCC		= gcc
FLAGS	= -Wall -Wextra -Werror #-g3 -fsanitize=address

MAND_FILES	= 	calculate.c			\
				main.c				\
				math.c				\
				parser.c			\
				printers.c			\
				result_formatting.c	\
				utils.c

OBJ_FILES	= $(MAND_FILES:.c=.o)

OBJ_DIR = ./objects/

OBJ = $(addprefix $(OBJ_DIR), $(OBJ_FILES))

all: obj $(NAME)

obj:
	@mkdir -p $(OBJ_DIR)
$(OBJ_DIR)%.o:$(MAND_DIR)%.c
	@$(GCC) $(FLAGS) -o $@ -c $<

$(NAME): $(OBJ)
	@echo "\033[0;33mCompilation started\033[0;31m"
	@$(GCC) $(OBJ) $(FLAGS) -o $(NAME)
	@echo "\033[0;32mCompilation successful. File \033[1;32m$(NAME)\033[0;32m generated!\033[0;37m"

clean:
	@echo "\033[0;34mCleaning started\033[0m"
	@rm -Rf $(OBJ_DIR)
	@echo "\033[0;36mCleaning done. All objects were removed.\033[0m"

fclean:		clean
	@rm -f $(NAME)
	@echo "\033[0;36mExecutable \033[1;36m$(NAME)\033[0;36m removed!\033[0m"

re:			fclean all

.PHONY:		all clean fclean re