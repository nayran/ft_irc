NAME = ircserv

SRCS = src/main.cpp 			\
	   src/serverClass.cpp		\
	   src/userClass.cpp		\
	   src/commandClass.cpp		\
	   src/channelClass.cpp

OBJS = $(SRCS:.cpp=.o)

FLAGS = -Wall -Werror -Wextra -std=c++98

all: $(NAME)

$(NAME): $(OBJS)
	c++ $(FLAGS) $(OBJS) -o $(NAME)
	# valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ircserv 6667 asd

%.o: %.cpp
	@c++ -c $(FLAGS) $< -o $@

clean:
	@rm -rf $(OBJS)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean
.PRECIOUS: ircserv
