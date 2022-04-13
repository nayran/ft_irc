NAME = ircserv

SRCS = src/main.cpp 			\
	   src/serverClass.cpp		\
	   src/usersClass.cpp		\
	   src/commandClass.cpp

OBJS = $(SRCS:.cpp=.o)

FLAGS = -Wall -Werror -Wextra -std=c++98 -fsanitize=address

all: $(NAME)

$(NAME): $(OBJS)
	@c++ $(FLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	@c++ -c $(FLAGS) $< -o $@

clean:
	@rm -rf $(OBJS)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean
.PRECIOUS: ircserv
