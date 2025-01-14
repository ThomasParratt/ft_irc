NAME		=	ircserv

SRC			=	src/main.cpp \
				src/join.cpp \
				src/Server.cpp \
				src/Channel.cpp \
				src/Client.cpp \
				src/initializeMsg.cpp \
				src/makeSelectAndRunCommand.cpp \
				src/makeMessages.cpp \
				src/connectionMessages.cpp \
				src/kick.cpp \
				src/invite.cpp \
				src/handleUserInput.cpp \
				src/part.cpp \
				src/topic.cpp \
				src/privmsg.cpp \
				src/mode.cpp \
				src/channelUtils.cpp \
				src/joinChecks.cpp \

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