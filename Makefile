# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/04/08 13:35:54 by opernod           #+#    #+#              #
#    Updated: 2026/04/20 17:43:53 by opernod          ###   ########lyon.fr    #
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

CC = cc
CFLAGS = -Wall -Wextra -Werror -std=c89 -pthread -MMD -MP

SRC_DIR = src
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(COLOR_GREEN)✅ $(NAME) compiled successfully!$(COLOR_RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "$(COLOR_CYAN)⚙️  Compiling: $<$(COLOR_RESET)"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

-include $(DEPS)

run : all
	@echo "$(COLOR_MAGENTA)🚀 Running $(NAME)...$(COLOR_RESET)"
	./$(NAME) 10 10 10 10 10 1 1 fifo

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(COLOR_YELLOW)🧹 Object files cleaned.$(COLOR_RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(COLOR_RED)🗑️  Executable removed.$(COLOR_RESET)"

re: fclean all

tester: re
	@echo "\033[34mBasic tests :\033[0m"
	@echo "🚀 Press Enter to run : ./$(NAME) 5 800 200 200 200 5 10 edf"; read dummy; ./$(NAME) 5 800 200 200 200 5 10 edf || true
	@echo "\033[34mBasic tests :\033[0m"
	@echo "🚀 Press Enter to run : ./$(NAME) 60 1000 200 100 100 5 200 fifo"; read dummy; ./$(NAME) 60 1000 200 100 100 5 200 fifo || true
	@echo "\n\033[34mStress tests :\033[0m"
	@echo "🚀 Press Enter to run : ./$(NAME) 199 1 0 0 0 8 1 edf"; read dummy; ./$(NAME) 199 1 0 0 0 8 1 edf || true
	@echo "\n\033[34mStress tests :\033[0m"
	@echo "🚀 Press Enter to run : ./$(NAME) 20 1 1 1 0 12 0 edf"; read dummy; ./$(NAME) 20 1 1 1 0 12 0 edf || true
	@echo "\n\033[34mStress tests :\033[0m"
	@echo "🚀 Press Enter to run : ./$(NAME) 200 1000000 1 1 0 12 10 fifo"; read dummy; ./$(NAME) 200 1000000 1 1 0 12 10 fifo || true
	@echo "\n\033[34mStress tests :\033[0m"
	@echo "🚀 Press Enter to run : ./$(NAME) 200 1000000 1 1 0 4 10 fifo"; read dummy; ./$(NAME) 200 1000000 1 1 0 4 10 fifo || true
	@echo "\n\033[34mBurnout for sure :\033[0m"
	@echo "🚀 Press Enter to run : ./$(NAME) 1 800 200 200 200 5 0 fifo"; read dummy; ./$(NAME) 1 800 200 200 200 5 0 fifo || true
	@echo "\n\033[34mBurnout for sure :\033[0m"
	@echo "🚀 Press Enter to run : ./$(NAME) 2 200 500 100 100 5 0 fifo"; read dummy; ./$(NAME) 2 200 500 100 100 5 0 fifo || true
	@echo "\n\033[34mBurnout for sure :\033[0m"
	@echo "🚀 Press Enter to run : ./$(NAME) 2 500 200 50 50 5 400 fifo"; read dummy; ./$(NAME) 2 500 200 50 50 5 400 fifo || true
	@echo "\n\033[34mBurnout for sure :\033[0m"
	@echo "🚀 Press Enter to run : ./$(NAME) 3 100 50 50 50 10 0 fifo"; read dummy; ./$(NAME) 3 100 50 50 50 10 0 fifo || true
	@echo "\n\033[34mNormal termination :\033[0m"
	@echo "🚀 Press Enter to run : ./$(NAME) 4 1000 200 100 100 5 0 fifo"; read dummy; ./$(NAME) 4 1000 200 100 100 5 0 fifo || true
	@echo "\n\033[34mNormal termination :\033[0m"
	@echo "🚀 Press Enter to run : ./$(NAME) 2 800 200 100 100 3 0 fifo"; read dummy; ./$(NAME) 2 800 200 100 100 3 0 fifo || true
	@echo "\n\033[34mTimestamp :\033[0m"
	@echo "🚀 Press Enter to run : ./$(NAME) 2 1000 200 100 100 10 0 fifo"; read dummy; ./$(NAME) 2 1000 200 100 100 10 0 fifo || true


.PHONY: all clean fclean re run tester
