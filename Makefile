NAME		=	ircserv

SRC			=	src/main.cpp \
				src/Server.cpp \
				src/Channel.cpp \
				src/Client.cpp \
				src/createMsgs.cpp \
				src/connectionMessages.cpp \
				src/kick.cpp \
				src/privmsg.cpp \
				src/mode.cpp \

OBJ			=	$(SRC:.cpp=.o)

CC			=	c++

RM			=	rm -f

CFLAGS		=	-Wextra -Wall -Werror 

$(NAME):	$(OBJ)
			$(CC) $(CFLAGS) -g -o $(NAME) $(OBJ)

all:		$(NAME)

clean:
			${RM} $(OBJ)

fclean: 	clean
			${RM} $(NAME) ${OBJ}

re:			fclean all

.PHONY:		all clean fclean re