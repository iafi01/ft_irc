NAME = ircserv

SRCS = main.cpp srcs/Channel.cpp srcs/Client.cpp srcs/Server.cpp

CC = clang++
CFLAGS = -Wall -Wextra -Werror -std=c++98 
RM = rm -f
OBJS = $(SRCS:.cpp=.o)

#  clang++ -Wall -Wextra -Werror -std=c++98 main.cpp srcs/*.cpp
#make &> file.txt
#sed '/Applications*/d' file.txt && sed '/operator<<*/d' file.txt

all:		$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) $(SRCS) -o $(NAME) $(OBJS)

clean:		
			$(RM) $(OBJS)

fclean:		clean
			$(RM) $(NAME)

re:			fclean $(NAME)

.PHONY:		all clean fclean re