# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/04/08 13:35:54 by opernod           #+#    #+#              #
#    Updated: 2026/04/21 16:05:46 by opernod          ###   ########lyon.fr    #
#                                                                              #
# **************************************************************************** #

NAME = codexion

# Colors
COLOR_RESET = \033[0m
COLOR_CYAN = \033[36m
COLOR_GREEN = \033[32m
COLOR_RED = \033[31m
COLOR_YELLOW = \033[33m
COLOR_MAGENTA = \033[35m
COLOR_BLUE = \033[34m

CC = cc
CFLAGS = -Wall -Wextra -Werror -std=c89 -pthread -MMD -MP

SRC_DIR = src
OBJ_DIR = obj
ARGS = 10 10 10 10 10 1 1 fifo
valgrind_args = valgrind --leak-check=full --track-origins=yes
helgrind_args = valgrind --tool=helgrind -s

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(COLOR_GREEN)$(NAME) compiled successfully!$(COLOR_RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "$(COLOR_CYAN) Compiling: $<$(COLOR_RESET)"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

-include $(DEPS)

run : all
	@echo "$(COLOR_MAGENTA)Running $(NAME)...$(COLOR_RESET)"
	./$(NAME) $(ARGS)

valgrind: all
	@echo "$(COLOR_MAGENTA)Running $(NAME) with Valgrind...$(COLOR_RESET)"
	$(valgrind_args) ./$(NAME) $(ARGS)

helgrind: all
	@echo "$(COLOR_MAGENTA)Running $(NAME) with Helgrind...$(COLOR_RESET)"
	$(helgrind_args) ./$(NAME) $(ARGS)
clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(COLOR_YELLOW)Object files cleaned.$(COLOR_RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(COLOR_RED)Executable removed.$(COLOR_RESET)"

re: fclean all

define RUN_TESTS
	@echo "\033[34mBasic tests :\033[0m"
	@echo "Press Enter to run : $(1) 5 800 200 200 200 5 10 edf"; read dummy; $(1) 5 800 200 200 200 5 10 edf || true
	@echo "\033[34mBasic tests :\033[0m"
	@echo "Press Enter to run : $(1) 60 1000 200 100 100 5 200 fifo"; read dummy; $(1) 60 1000 200 100 100 5 200 fifo || true
	@echo ""; echo "\033[34mStress tests :\033[0m"
	@echo "Press Enter to run : $(1) 199 1 0 0 0 8 1 edf"; read dummy; $(1) 199 1 0 0 0 8 1 edf || true
	@echo ""; echo "\033[34mStress tests :\033[0m"
	@echo "Press Enter to run : $(1) 20 1 1 1 0 12 0 edf"; read dummy; $(1) 20 1 1 1 0 12 0 edf || true
	@echo ""; echo "\033[34mStress tests :\033[0m"
	@echo "Press Enter to run : $(1) 200 1000000 1 1 0 12 10 fifo"; read dummy; $(1) 200 1000000 1 1 0 12 10 fifo || true
	@echo ""; echo "\033[34mStress tests :\033[0m"
	@echo "Press Enter to run : $(1) 200 1000000 1 1 0 4 10 fifo"; read dummy; $(1) 200 1000000 1 1 0 4 10 fifo || true
	@echo ""; echo "\033[34mBurnout for sure :\033[0m"
	@echo "Press Enter to run : $(1) 1 800 200 200 200 5 0 fifo"; read dummy; $(1) 1 800 200 200 200 5 0 fifo || true
	@echo ""; echo "\033[34mBurnout for sure :\033[0m"
	@echo "Press Enter to run : $(1) 2 200 500 100 100 5 0 fifo"; read dummy; $(1) 2 200 500 100 100 5 0 fifo || true
	@echo ""; echo "\033[34mBurnout for sure :\033[0m"
	@echo "Press Enter to run : $(1) 2 500 200 50 50 5 400 fifo"; read dummy; $(1) 2 500 200 50 50 5 400 fifo || true
	@echo ""; echo "\033[34mBurnout for sure :\033[0m"
	@echo "Press Enter to run : $(1) 3 100 50 50 50 10 0 fifo"; read dummy; $(1) 3 100 50 50 50 10 0 fifo || true
	@echo ""; echo "\033[34mNormal termination :\033[0m"
	@echo "Press Enter to run : $(1) 4 1000 200 100 100 5 0 fifo"; read dummy; $(1) 4 1000 200 100 100 5 0 fifo || true
	@echo ""; echo "\033[34mNormal termination :\033[0m"
	@echo "Press Enter to run : $(1) 2 800 200 100 100 3 0 fifo"; read dummy; $(1) 2 800 200 100 100 3 0 fifo || true
	@echo ""; echo "\033[34mTimestamp :\033[0m"
	@echo "Press Enter to run : $(1) 2 1000 200 100 100 10 0 fifo"; read dummy; $(1) 2 1000 200 100 100 10 0 fifo || true
endef

tester: all
	$(call RUN_TESTS,./$(NAME))

tester_val: all
	$(call RUN_TESTS,$(valgrind_args) ./$(NAME))

tester_hell: all
	$(call RUN_TESTS,$(helgrind_args) ./$(NAME))

.PHONY: all clean fclean re run tester tester_val tester_hell valgrind helgrind