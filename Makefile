NAME = ircserv

SRCS = main.cpp srcs/Channel.cpp srcs/Client.cpp srcs/Server.cpp

CC = clang++
CFLAGS = -Wall -Wextra -Werror -std=98
RM = rm -f
OBJS = $(SRCS:.cpp=.o)

all:		$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

clean:		
			$(RM) $(OBJS)

fclean:		clean
			$(RM) $(NAME)

re:			fclean $(NAME)

.PHONY:		all clean fclean re